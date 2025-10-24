#include <flakpak-c/flak_packer.h>

#include <flakpak-c/flak_pswd_definition.h>
#include <flakpak-c/zstd_compressor.h>
#include <flakpak-c/xccp20_encryptor.h>
#include <flakpak-c/flak_arena.h>

#include <microlog/ulog.h>

#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <tinydir/tinydir.h>


typedef struct FLAKFS_file_info {
    char path[FLK_MAX_FILE_PATH_LENGTH];
    size_t size;
} FLAKFS_file_info_t;

typedef struct FLAK_file_list {
    FLAKFS_file_info_t* files;
    size_t count;
    size_t capacity;
} FLAKFS_file_list_t;

static bool file_list_init(FLAKFS_file_list_t* list, size_t initial_capacity) {
    list->files = (FLAKFS_file_info_t*)malloc(sizeof(FLAKFS_file_info_t) * initial_capacity);
    if (!list->files) return false;
    list->count = 0;
    list->capacity = initial_capacity;
    return true;
}

static bool file_list_add(FLAKFS_file_list_t* list, const char* path, size_t size) {
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        FLAKFS_file_info_t* new_files = (FLAKFS_file_info_t*)realloc(
            list->files, sizeof(FLAKFS_file_info_t) * new_capacity);
        if (!new_files) return false;
        list->files = new_files;
        list->capacity = new_capacity;
    }

    strncpy(list->files[list->count].path, path, FLK_MAX_FILE_PATH_LENGTH - 1);
    list->files[list->count].path[FLK_MAX_FILE_PATH_LENGTH - 1] = '\0';
    list->files[list->count].size = size;
    list->count++;
    return true;
}

static void file_list_free(FLAKFS_file_list_t* list) {
    free(list->files);
    list->files = NULL;
    list->count = 0;
    list->capacity = 0;
}

static bool collect_files_recursive(const char* in_base_dir, const char* in_current_dir, FLAKFS_file_list_t* in_file_list);

static uint8_t* read_file_data(const char* in_file_path, size_t* out_size) {
    FILE* f = fopen(in_file_path, "rb");
    if (!f) {
        ulog_error("Failed to open file: %s\n", in_file_path);
        *out_size = 0;
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size <= 0) {
        ulog_error("Invalid file size\n");
        fclose(f);
        *out_size = 0;
        return NULL;
    }

    // This is arbitrary data
    uint8_t* data = (uint8_t*)malloc(file_size);
    if (!data) {
        ulog_fatal("Memory allocation failed");
		ulog_fatal("Arena out of memory while reading %s\n", in_file_path);
        fclose(f);
        *out_size = 0;
        return NULL;
    }

    size_t bytes_read = fread(data, 1, file_size, f);
    fclose(f);

    if (bytes_read != (size_t)file_size) {
        ulog_fatal("Failed to read entire file: %s\n", in_file_path);
        free(data);
        *out_size = 0;
        return NULL;
    }

    *out_size = file_size;
    return data;
}

// Simple subpath-based relative path
static bool get_relative_path(const char* in_base, const char* in_full, char* out_rel, size_t out_size) {
    if (!in_base || !in_full || !out_rel) return false;

    size_t base_len = strlen(in_base);
    size_t full_len = strlen(in_full);
    if (base_len > full_len) return false;

    // Compare ignoring slash direction (and case on Windows)
    for (size_t i = 0; i < base_len; i++) {
        char a = in_base[i];
        char b = in_full[i];
        if (a == '/' || a == '\\') a = PATH_SEP;
        if (b == '/' || b == '\\') b = PATH_SEP;
        if (!PATH_EQ(a, b))
            return false;
    }

    const char* rel_start = in_full + base_len;

    // Skip separator if present
    if (*rel_start == '/' || *rel_start == '\\')
        rel_start++;

    size_t rel_len = strlen(rel_start);
    if (rel_len >= out_size)
        return false;

    memcpy(out_rel, rel_start, rel_len + 1);
    return true;
}

static bool validate_flk_constraints(const char* rel_path, size_t file_size) {
    if (strlen(rel_path) >= FLK_MAX_FILE_PATH_LENGTH) {
		ulog_error("File path too long: %s\n", rel_path);
        return false;
    }

    if (file_size > FLK_MAX_FILE_SIZE) {
		ulog_error("File too large: %s\n", rel_path);
        return false;
    }

    return true;
}

static bool write_flk_file(const char* in_out_path, FLK_header_t* in_header,
    const uint8_t** in_file_blobs, const size_t* in_file_blob_sizes,
    uint32_t in_blob_count, const uint8_t* in_global_salt, size_t in_global_salt_size) {
    
    FILE* f = fopen(in_out_path, "wb");
    if (!f) {
        ulog_error("Failed to create output file: %s\n", in_out_path);
        return false;
    }

    // Write header
    size_t header_size = sizeof(FLK_header_t);
    if (fwrite(in_header, 1, header_size, f) != header_size) {
        ulog_error("Failed to write header to file: %s\n", in_out_path);
        fclose(f);
        return false;
    }

    // Write global salt if present
    if (in_global_salt && in_global_salt_size > 0) {
        if (fwrite(in_global_salt, 1, in_global_salt_size, f) != in_global_salt_size) {
            ulog_error("Failed to write global salt to file: %s\n", in_out_path);
            fclose(f);
            return false;
        }
    }

    // Write file blobs
    for (uint32_t i = 0; i < in_blob_count; i++) {
        if (fwrite(in_file_blobs[i], 1, in_file_blob_sizes[i], f) != in_file_blob_sizes[i]) {
            ulog_error("Failed to write file blob %u to file: %s\n", i, in_out_path);
            fclose(f);
            return false;
        }
    }

    fclose(f);
	return true;
}


bool FLAK_pack_files(const char* in_dir_path, const char* out_output_path,
    FLK_file_flags in_flags, int in_comp_level) {
    // STEP 1: Collect all files in a single pass
    FLAKFS_file_list_t file_list;
    if (!file_list_init(&file_list, 64)) {
        ulog_error("Failed to initialize file list\n");
        return false;
    }

    ulog_info("Scanning directory: %s\n", in_dir_path);
    if (!collect_files_recursive(in_dir_path, in_dir_path, &file_list)) {
        ulog_error("Failed to collect files\n");
        file_list_free(&file_list);
        return false;
    }

    if (file_list.count == 0) {
        ulog_error("No files to pack\n");
        file_list_free(&file_list);
        return false;
    }

    if (file_list.count > FLK_MAX_HEADER_ENTRIES) {
        ulog_error("Too many files. Max allowed: %d\n", FLK_MAX_HEADER_ENTRIES);
        file_list_free(&file_list);
        return false;
    }

    ulog_info("Found %zu files to pack\n", file_list.count);

    // STEP 2: Allocate memory for packing
    size_t base_arena_size = sizeof(FLK_header_t) +
        (sizeof(uint8_t*) * file_list.count) +
        (sizeof(size_t) * file_list.count) +
        4096;

    FLAK_memory_arena_t* arena = FLAK_memory_arena_create(base_arena_size);
    if (!arena || !arena->base) {
        ulog_fatal("Failed to allocate memory arena\n");
        if (arena) free(arena);
        file_list_free(&file_list);
        return false;
    }

    FLK_header_t* header = FLAK_memory_arena_allocate(arena, sizeof(FLK_header_t), FLAK_DEFAULT_ALIGNMENT);
    uint8_t** blobs = FLAK_memory_arena_allocate(arena, sizeof(uint8_t*) * file_list.count, FLAK_DEFAULT_ALIGNMENT);
    size_t* blob_sizes = FLAK_memory_arena_allocate(arena, sizeof(size_t) * file_list.count, FLAK_DEFAULT_ALIGNMENT);

    if (!header || !blobs || !blob_sizes) {
        ulog_fatal("Memory allocation failed\n");
        FLAK_memory_arena_free(arena);
        free(arena);
        file_list_free(&file_list);
        return false;
    }
    memset(header, 0, sizeof(FLK_header_t));

    // Initialize header
    strcpy(header->magic, "FLK");
    header->version = 1;
    header->content_version = 1;
    header->flags = in_flags;

    uint64_t current_offset = sizeof(FLK_header_t);

    // If encryption is enabled, account for global salt space
    if (in_flags & FLK_FLAG_ENCRYPTED) {
        current_offset += 16; // Global salt size
    }

    uint8_t* global_salt = NULL;
    size_t global_salt_size = 0;

    // STEP 3: Process each file from the collected list
    uint32_t entry_index = 0;
    for (size_t i = 0; i < file_list.count; i++) {
        const char* full_path = file_list.files[i].path;
        size_t file_size = file_list.files[i].size;

        // Get relative path
        char rel_path[FLK_MAX_FILE_PATH_LENGTH];
        if (!get_relative_path(in_dir_path, full_path, rel_path, sizeof(rel_path))) {
            ulog_warn("Failed to get relative path: %s\n", full_path);
            continue;
        }

        // Validate constraints
        if (!validate_flk_constraints(rel_path, file_size)) {
            continue;
        }

        ulog_info("Processing: %s", rel_path);

        // Read file data
        size_t data_size = 0;
        uint8_t* data = read_file_data(full_path, &data_size);
        if (!data) {
            ulog_warn("Failed to read file data: %s\n", full_path);
            continue;
        }

        uint8_t* processed_data = data;
        size_t processed_size = data_size;
        uint64_t base_size = data_size;

        // Compression
        if (in_flags & FLK_FLAG_COMPRESSED) {
            FLAK_COMPRESSION_RESULT comp_result = FLAK_zstd_compress_data(
                rel_path, data, data_size, in_comp_level);
            if (comp_result.data) {
                free(data);
                processed_data = comp_result.data;
                processed_size = comp_result.compressed_size;
                ulog_debug("Compressed %zu -> %zu bytes", data_size, processed_size);
            }
            else {
                ulog_warn("Compression failed, using uncompressed\n");
            }
        }

        // Encryption
        if (in_flags & FLK_FLAG_ENCRYPTED) {
            FLAK_ENCRYPTION_RESULT enc_result = FLAK_xccp20_encrypt_data(
                rel_path, processed_data, processed_size, FLAK_get_password());
            if (enc_result.data) {
                if (processed_data != data) free(processed_data);
                else free(data);

                processed_data = enc_result.data;
                processed_size = enc_result.data_size;

                // Store global salt from first file
                if (entry_index == 0 && global_salt == NULL) {
                    global_salt = (uint8_t*)malloc(16);
                    if (global_salt) {
                        memcpy(global_salt, enc_result.salt, 16);
                        global_salt_size = 16;
                    }
                }
                ulog_debug("Encrypted to %zu bytes\n", processed_size);
            }
            else {
                ulog_warn("Encryption failed\n");
                free(processed_data);
                continue;
            }
        }

        // Fill header entry
        strcpy(header->entries[entry_index].file_path, rel_path);
        header->entries[entry_index].offset = current_offset;
        header->entries[entry_index].base_size = base_size;
        header->entries[entry_index].packed_size = processed_size;

        blobs[entry_index] = processed_data;
        blob_sizes[entry_index] = processed_size;
        current_offset += processed_size;
        entry_index++;
    }

    // Done with file list
    file_list_free(&file_list);

    // STEP 4: Write output file
    header->entry_count = entry_index;
    header->salt_lenght = (uint8_t)global_salt_size;

    bool ok = write_flk_file(out_output_path, header, (const uint8_t**)blobs,
        blob_sizes, entry_index, global_salt, global_salt_size);

    // STEP 5: Cleanup
    for (uint32_t i = 0; i < entry_index; i++) {
        if (blobs[i]) free(blobs[i]);
    }
    free(global_salt);

    if (!ok) {
        ulog_error("Failed to write FLK file\n");
        FLAK_memory_arena_free(arena);
        free(arena);
        return false;
    }

    FLAK_memory_arena_free(arena);
    free(arena);

    ulog_info("Successfully packed %u files to %s\n", entry_index, out_output_path);
    return true;
}

bool collect_files_recursive(const char* in_base_dir, const char* in_current_dir, FLAKFS_file_list_t* in_file_list) {
	tinydir_dir dir;

#ifdef _WIN32
    // Convert input path to TCHAR, since tinydir uses TCHAR and char in Windows is not UTF-8
    /// Thanks to Santiago Farall on explaining this issue -> https://github.com/elsantiF
    TCHAR tchar_in_dir_path[FLK_MAX_FILE_PATH_LENGTH];
    MultiByteToWideChar(CP_UTF8, 0, in_current_dir, -1, tchar_in_dir_path, FLK_MAX_FILE_PATH_LENGTH);
#else
    const char* tchar_in_dir_path = in_current_dir;
#endif

    if (tinydir_open(&dir, tchar_in_dir_path) == -1) {
        ulog_error("Failed to open directory: %s", in_current_dir);
        return false;
    }

    bool success = true;
    while (dir.has_next) {
        tinydir_file file;
        if (tinydir_readfile(&dir, &file) == -1) {
            tinydir_next(&dir);
            continue;
        }

        char path_utf8[FLK_MAX_FILE_PATH_LENGTH];
        char name_utf8[FLK_MAX_FILE_PATH_LENGTH];

#ifdef _WIN32
        if (!tchar_to_utf8((TCHAR*)file.path, path_utf8, sizeof(path_utf8)) ||
            !tchar_to_utf8((TCHAR*)file.name, name_utf8, sizeof(name_utf8))) {
            tinydir_next(&dir);
            continue;
        }
#else
        strncpy(path_utf8, file.path, sizeof(path_utf8) - 1);
        strncpy(name_utf8, file.name, sizeof(name_utf8) - 1);
        path_utf8[sizeof(path_utf8) - 1] = '\0';
        name_utf8[sizeof(name_utf8) - 1] = '\0';
#endif

        if (strcmp(name_utf8, ".") == 0 || strcmp(name_utf8, "..") == 0) {
            tinydir_next(&dir);
            continue;
        }

        if (file.is_dir) {
            // Recurse
            if (!collect_files_recursive(in_base_dir, path_utf8, in_file_list)) {
                success = false;
                break;
            }
        }
        else {
            // Add file
            struct stat st;
            if (stat(path_utf8, &st) != -1) {
                if (!file_list_add(in_file_list, path_utf8, (size_t)st.st_size)) {
                    ulog_error("Failed to add file to list: %s", path_utf8);
                    success = false;
                    break;
                }
            }
        }

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
    return success;
}
