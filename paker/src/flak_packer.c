#include <flakpak-c/flak_packer.h>

#include <flakpak-c/flak_pswd_definition.h>
#include <flakpak-c/zstd_compressor.h>
#include <flakpak-c/xccp20_encryptor.h>
#include <flakpak-c/flak_arena.h>

#include <microlog/ulog.h>

#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <tinydir/tinydir.h>


// Count files recursively
static void count_files_recursive(const char* dir_path, size_t* count) {
    tinydir_dir dir;

    // Convert input path to TCHAR, since tinydir uses TCHAR and char in Windows is not UTF-8
    /// Thanks to Santiago Farall on explaining this issue -> https://github.com/elsantiF
    TCHAR tchar_in_dir_path[FLK_MAX_FILE_PATH_LENGTH] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, dir_path, -1, tchar_in_dir_path, FLK_MAX_FILE_PATH_LENGTH);

    if (tinydir_open(&dir, tchar_in_dir_path) == -1) {
        ulog_error("Failed to open directory: %s\n", dir_path);
		ulog_error("Make sure the directory exists and you have permission to read it.\n");
        return;
    }

    while (dir.has_next) {
        tinydir_file file;
        if (tinydir_readfile(&dir, &file) == -1) {
            ulog_error("Error reading file\n");
            tinydir_next(&dir);
            continue;
        }

        // Convert file.name to utf8 for comparing "." and ".."
        char name_utf8[FLK_MAX_FILE_PATH_LENGTH];
#ifdef _WIN32
        if (!tchar_to_utf8((TCHAR*)file.name, name_utf8, sizeof(name_utf8))) {
            ulog_warn("Name conversion failed\n");
            tinydir_next(&dir);
            continue;
        }
#else
        strncpy(name_utf8, (char*)file.name, sizeof(name_utf8));
        name_utf8[sizeof(name_utf8) - 1] = '\0';
#endif

        if (!file.is_dir) {
            (*count)++;
        }
        else if (strcmp(name_utf8, ".") != 0 && strcmp(name_utf8, "..") != 0) {
            // Convert file.path -> utf8 for recursion call
            char path_utf8[FLK_MAX_FILE_PATH_LENGTH];
#ifdef _WIN32
            if (!tchar_to_utf8((TCHAR*)file.path, path_utf8, sizeof(path_utf8))) {
                ulog_warn("Path conversion failed\n");
                tinydir_next(&dir);
                continue;
            }
#else
            strncpy(path_utf8, (char*)file.path, sizeof(path_utf8));
            path_utf8[sizeof(path_utf8) - 1] = '\0';
#endif
            count_files_recursive(path_utf8, count);
        }

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
}

static size_t count_files_in_directory(const char* in_dir_path) {
    size_t count = 0;
    count_files_recursive(in_dir_path, &count);
    return count;
}

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
        ulog_fatal("Memory allocation failed\n");
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

// Recursive helper for packing files
static void pack_directory_recursive(const char* in_base_dir, const char* in_current_dir,
    FLK_header_t* in_header, uint8_t** in_blobs, size_t* in_blob_sizes,
    uint64_t* in_current_offset, uint32_t* in_entry_index,
    FLK_file_flags in_flags, int in_comp_level, uint8_t** in_global_salt, size_t* in_global_salt_size)
{
    tinydir_dir dir;

    // Convert current path directory to TCHAR, since tinydir uses TCHAR and char in Windows is not UTF-8
    /// Thanks to Santiago Farall on explaining this issue -> https://github.com/elsantiF
    TCHAR tchar_dir[FLK_MAX_FILE_PATH_LENGTH] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, in_current_dir, -1, tchar_dir, FLK_MAX_FILE_PATH_LENGTH);

    if (tinydir_open(&dir, tchar_dir) == -1) {
        ulog_error("Failed to open directory: %s\n", in_current_dir);
        return;
    }

    while (dir.has_next) {
        tinydir_file file;
        if (tinydir_readfile(&dir, &file) != 0) {
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
        strncpy(path_utf8, (char*)file.path, sizeof(path_utf8));
        path_utf8[sizeof(path_utf8) - 1] = '\0';
        strncpy(name_utf8, (char*)file.name, sizeof(name_utf8));
        name_utf8[sizeof(name_utf8) - 1] = '\0';
#endif

        // Skip "." and ".."
        if (strcmp(name_utf8, ".") == 0 || strcmp(name_utf8, "..") == 0) {
            tinydir_next(&dir);
            continue;
        }

        if (file.is_dir) {
            // Recurse into subdirectory
            pack_directory_recursive(in_base_dir, path_utf8, in_header, in_blobs, in_blob_sizes,
                in_current_offset, in_entry_index, in_flags, in_comp_level,
                in_global_salt, in_global_salt_size);
        }
        else {
            // Process file
            char rel_path[FLK_MAX_FILE_PATH_LENGTH];
            if (!get_relative_path(in_base_dir, path_utf8, rel_path, sizeof(rel_path))) {
                ulog_warn("Failed to get relative path: %s\n", path_utf8);
                tinydir_next(&dir);
                continue;
            }

            struct stat st;
            if (stat(path_utf8, &st) == -1) {
                ulog_error("Failed to stat file: %s\n", path_utf8);
                tinydir_next(&dir);
                continue;
            }
            size_t file_size = (size_t)st.st_size;

            if (!validate_flk_constraints(rel_path, file_size)) {
                tinydir_next(&dir);
                continue;
            }

            ulog_info("Processing: %s\n", rel_path);

            size_t data_size = 0;
            uint8_t* data = read_file_data(path_utf8, &data_size);
            if (!data) {
				ulog_warn("Failed to read file data: %s\n", path_utf8);
                tinydir_next(&dir);
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
                    ulog_debug("Compressed %zu -> %zu bytes\n", data_size, processed_size);
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

                    if (*in_entry_index == 0 && *in_global_salt == NULL) {
                        *in_global_salt = (uint8_t*)malloc(16);
                        if (*in_global_salt) {
                            memcpy(*in_global_salt, enc_result.salt, 16);
                            *in_global_salt_size = 16;
                        }
                    }
                    ulog_debug("Encrypted to %zu bytes\n", processed_size);
                }
                else {
                    ulog_warn("Encryption failed\n");
                    free(processed_data);
                    tinydir_next(&dir);
                    continue;
                }
            }

            // Fill header entry
            strcpy(in_header->entries[*in_entry_index].file_path, rel_path);
            in_header->entries[*in_entry_index].offset = *in_current_offset;
            in_header->entries[*in_entry_index].base_size = base_size;
            in_header->entries[*in_entry_index].packed_size = processed_size;

            in_blobs[*in_entry_index] = processed_data;
            in_blob_sizes[*in_entry_index] = processed_size;
            *in_current_offset += processed_size;
            (*in_entry_index)++;
        }

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
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
    // Count files first
    size_t file_count = count_files_in_directory(in_dir_path);
    if (file_count == 0) {
        ulog_error("No files to pack\n");
        return false;
    }
    if (file_count > FLK_MAX_HEADER_ENTRIES) {
        ulog_error("Too many files. Max allowed: %d\n", FLK_MAX_HEADER_ENTRIES);
        return false;
    }
    ulog_info("Found %zu files to pack\n", file_count);

    size_t base_arena_size = sizeof(FLK_header_t) +
        (sizeof(uint8_t*) * file_count) +
        (sizeof(size_t) * file_count) +
        4096; // Extra padding for alignment
	FLAK_memory_arena_t* arena = FLAK_memory_arena_create(base_arena_size);
    if (!arena | !arena->base) {
        ulog_fatal("Failed to allocate memory arena\n");
        if (arena) free(arena);
        return false;
    }

    // Allocate header and blobs
    FLK_header_t* header = FLAK_memory_arena_allocate(arena, sizeof(FLK_header_t), FLK_DEFAULT_ALIGNMENT);
	uint8_t** blobs = FLAK_memory_arena_allocate(arena, sizeof(uint8_t*) * file_count, FLK_DEFAULT_ALIGNMENT);
	size_t* blob_sizes = FLAK_memory_arena_allocate(arena, sizeof(size_t) * file_count, FLK_DEFAULT_ALIGNMENT);
    if (!header || !blobs || !blob_sizes) {
        ulog_fatal("Memory allocation failed\n");
        FLAK_memory_arena_free(arena);
        free(arena);
        return false;
    }

    strcpy(header->magic, "FLK");
    header->version = 1;
    header->content_version = 1;
    header->flags = in_flags;

    uint32_t entry_index = 0;
    uint64_t current_offset = sizeof(FLK_header_t);
    uint8_t* global_salt = NULL;
    size_t global_salt_size = 0;

    // Recursive packing
    pack_directory_recursive(in_dir_path, in_dir_path, header, blobs, blob_sizes,
        &current_offset, &entry_index, in_flags, in_comp_level,
        &global_salt, &global_salt_size);

    header->entry_count = entry_index;
    header->salt_lenght = (uint8_t)global_salt_size;

    // Write output
    bool ok = write_flk_file(out_output_path, header, (const uint8_t**)blobs,
        blob_sizes, entry_index, global_salt, global_salt_size);

    // Cleanup
    for (uint32_t i = 0; i < entry_index; i++) {
        if (blobs[i]) free(blobs[i]);  // These are the processed file data
    }
    free(global_salt);
	FLAK_memory_arena_free(arena);
    free(arena);

    if (!ok) {
        ulog_error("Failed to write FLK file\n");
        FLAK_memory_arena_free(arena);
        free(arena);
        return false;
    }

    ulog_info("Successfully packed %u files to %s\n", entry_index, out_output_path);
    return true;
}
