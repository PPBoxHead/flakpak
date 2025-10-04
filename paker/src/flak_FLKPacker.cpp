#include <flakpak/flak_FLKPacker.hpp>

#include <flakpak/zstd_Compressor.hpp>
#include <flakpak/xccp20_Encryptor.hpp>
#include <flakpak/flak_PasswordHandler.hpp>
#include <flakpak/flak_PathCompressor.hpp>

#include <memory>
#include <iostream>
#include <fstream>

using namespace flakpak::data_types;

namespace flakpak {
	bool FLKPacker::PackUncompressedAndUnencrypted(std::filesystem::path in_dirPath, std::filesystem::path in_outPath) {
		// Validate input directory
        if (!std::filesystem::exists(in_dirPath) || !std::filesystem::is_directory(in_dirPath)) {
            /// TODO
			/// Handle error: invalid input directory
			/// Output to console
			std::cout << "Error: Invalid input directory.\n";
            return false;
        }

        // Count files
        size_t fileCount = CountFilesInDirectory(in_dirPath);
        /// TODO
        /// If debug flag enabled output to console the file count

        if (fileCount > MAX_FLK_HEADER_ENTRIES) {
			/// TODO
			/// Handle error: too many files
            /// Output to console
			std::cout << "Error: Too many files in directory. Maximum allowed is " << MAX_FLK_HEADER_ENTRIES << ".\n";
            return false;
        }

        // Initialize header
        auto header = std::make_unique<data_types::FLKHeader>();

        std::vector<std::vector<uint8_t>> blobs;
        size_t entryIndex = 0;

        // Process files
        for (auto& entry : std::filesystem::recursive_directory_iterator(in_dirPath)) {
            if (!std::filesystem::is_regular_file(entry)) continue;

            std::filesystem::path rel = std::filesystem::relative(entry.path(), in_dirPath);
            std::string relPathStr = rel.string();
            auto fileSize = std::filesystem::file_size(entry.path());

            if (!ValidateFLKConstraints(relPathStr, fileSize)) {
                return false;
            }

            /// TODO
			/// If debug flag enabled output to console the file being processed
            std::cout << "Processing: " << relPathStr << "\n";

            try {
                // Read file data directly
                auto fileData = ReadFileData(entry.path());

                // Fill entry
                data_types::FLKEntry& flkEntry = header->entries[entryIndex];
                std::strncpy(flkEntry.path, relPathStr.c_str(), MAX_FILE_PATH_LENGTH - 1);
                flkEntry.path[MAX_FILE_PATH_LENGTH - 1] = '\0';
                flkEntry.baseSize = fileSize;
                flkEntry.packedSize = fileSize;

                blobs.push_back(std::move(fileData));
                entryIndex++;
            }
            catch (const std::exception& ex) {
				/// TODO
				/// Handle error: file processing failed
				/// Output to console
                std::cerr << "Error processing file " << relPathStr << ": " << ex.what() << "\n";
                return false;
            }
        }

        OptimizeUnusedEntries(header.get(), static_cast<uint32_t>(entryIndex));

        header->entryCount = static_cast<uint32_t>(entryIndex);

        // Calculate offsets
        uint64_t currentOffset = sizeof(data_types::FLKHeader);
        for (uint32_t i = 0; i < header->entryCount; i++) {
            header->entries[i].offset = currentOffset;
            currentOffset += blobs[i].size();
        }

        // Generate output path
        std::filesystem::path outputPath = in_outPath;

        // Write file
        std::vector<uint8_t> emptySalt;
        if (!WriteFLKFile(outputPath, header.get(), blobs, emptySalt)) {
            return false;
        }

		/// TODO
		/// If debug flag enabled output to console the summary
        std::cout << "Successfully packed " << header->entryCount << " files to " << outputPath.string() << "\n";
		return true;
	}
    bool FLKPacker::PackCompressedAndUnencrypted(std::filesystem::path in_dirPath, std::filesystem::path in_outPath, int in_compressionLevel) {
        // Validate input directory
        if (!std::filesystem::exists(in_dirPath) || !std::filesystem::is_directory(in_dirPath)) {
            /// TODO
            /// Handle error: invalid input directory
            /// Output to console
            return false;
        }

        // Count files
        size_t fileCount = CountFilesInDirectory(in_dirPath);
        /// TODO
        /// If debug flag enabled output to console the file count

        if (fileCount > MAX_FLK_HEADER_ENTRIES) {
            /// TODO
            /// Handle error: too many files
            /// Output to console
            return false;
        }

        // Initialize header
        auto header = std::make_unique<data_types::FLKHeader>();

		// Initialize compression library
        auto compressor = std::make_unique<compression::zstd::ZstdCompressor>();

        std::vector<std::vector<uint8_t>> blobs;
        size_t entryIndex = 0;

        // Process files
        for (auto& entry : std::filesystem::recursive_directory_iterator(in_dirPath)) {
            if (!std::filesystem::is_regular_file(entry)) continue;

            std::filesystem::path rel = std::filesystem::relative(entry.path(), in_dirPath);
            std::string relPathStr = rel.string();
            auto fileSize = std::filesystem::file_size(entry.path());

            std::string compressedPath = pathcom::PathCompressor::CompressPath(relPathStr);

            if (compressedPath.length() >= MAX_FILE_PATH_LENGTH) {
                std::cerr << "Error: Compressed path too long: " << relPathStr
                    << " -> " << compressedPath << " (" << compressedPath.length() << " chars)\n";
                return false;
            }

            if (!ValidateFLKConstraints(relPathStr, fileSize)) {
                return false;
            }

            /// TODO
            /// If debug flag enabled output to console the file being processed
            std::cout << "Processing: " << relPathStr << "\n";

            try {
				// Compress the data in place to save memory
                auto compressionResult = compressor->CompressData(entry.path(), in_compressionLevel);

                // Fill entry
                FLKEntry& flkEntry = header->entries[entryIndex];
                
				OptimizePathPadding(flkEntry.path, compressedPath);
                flkEntry.baseSize = compressionResult.originalSize;
                flkEntry.packedSize = compressionResult.data.size();

                blobs.push_back(std::move(compressionResult.data));
                entryIndex++;
            }
            catch (const std::exception& ex) {
                /// TODO
                /// Handle error: file processing failed
                /// Output to console
                std::cerr << "Error processing file " << relPathStr << ": " << ex.what() << "\n";
                return false;
            }
        }

        OptimizeUnusedEntries(header.get(), static_cast<uint32_t>(entryIndex));

        header->entryCount = static_cast<uint32_t>(entryIndex);

        // Calculate offsets
        uint64_t currentOffset = sizeof(data_types::FLKHeader);
        for (uint32_t i = 0; i < header->entryCount; i++) {
            header->entries[i].offset = currentOffset;
            currentOffset += blobs[i].size();
        }

        // Generate output path
        std::filesystem::path outputPath = in_outPath;

        // Write file
        std::vector<uint8_t> emptySalt;
        if (!WriteFLKFile(outputPath, header.get(), blobs, emptySalt)) {
            return false;
        }

        /// TODO
        /// If debug flag enabled output to console the summary
        std::cout << "Successfully packed " << header->entryCount << " files to " << outputPath.string() << "\n";
        return true;
    }

    bool FLKPacker::PackUncompressedAndEncrypted(std::filesystem::path in_dirPath, std::filesystem::path in_outPath) {
        // Validate input directory
        if (!std::filesystem::exists(in_dirPath) || !std::filesystem::is_directory(in_dirPath)) {
            /// TODO
            /// Handle error: invalid input directory
            /// Output to console
            return false;
        }

        // Count files
        size_t fileCount = CountFilesInDirectory(in_dirPath);
        /// TODO
        /// If debug flag enabled output to console the file count

        if (fileCount > MAX_FLK_HEADER_ENTRIES) {
            /// TODO
            /// Handle error: too many files
            /// Output to console
            return false;
        }

        // Initialize header
        auto header = std::make_unique<data_types::FLKHeader>();

		// Initialize encryption library
        auto encryptor = std::make_unique<encryption::xccp20::XChaCha20Poly1305Encryptor>();

        std::vector<std::vector<uint8_t>> blobs;
        std::vector<uint8_t> globalSalt;
        bool firstFile = true;
        size_t entryIndex = 0;

        // Process files
        for (auto& entry : std::filesystem::recursive_directory_iterator(in_dirPath)) {
            if (!std::filesystem::is_regular_file(entry)) continue;

            std::filesystem::path rel = std::filesystem::relative(entry.path(), in_dirPath);
            std::string relPathStr = rel.string();
            auto fileSize = std::filesystem::file_size(entry.path());

            if (!ValidateFLKConstraints(relPathStr, fileSize)) {
                return false;
            }

            /// TODO
            /// If debug flag enabled output to console the file being processed
            std::cout << "Processing: " << relPathStr << "\n";

            try {
				// Read file data directly and encrypt
                auto fileData = ReadFileData(entry.path());
                auto encryptionResult = encryptor->EncryptData(fileData, encryption::GetPassword());

				// Store the salt from the first file as global salt
                if (firstFile) {
                    globalSalt = encryptionResult.salt;
                    firstFile = false;
                }

                // Fill entry
                FLKEntry& flkEntry = header->entries[entryIndex];
                std::strncpy(flkEntry.path, relPathStr.c_str(), MAX_FILE_PATH_LENGTH - 1);
                flkEntry.path[MAX_FILE_PATH_LENGTH - 1] = '\0';
                flkEntry.baseSize = fileSize;
                flkEntry.packedSize = encryptionResult.data.size();

                blobs.push_back(std::move(encryptionResult.data));
                entryIndex++;
            }
            catch (const std::exception& ex) {
                /// TODO
                /// Handle error: file processing failed
                /// Output to console
                std::cerr << "Error processing file " << relPathStr << ": " << ex.what() << "\n";
                return false;
            }
        }

        OptimizeUnusedEntries(header.get(), static_cast<uint32_t>(entryIndex));

        header->entryCount = static_cast<uint32_t>(entryIndex);
        header->saltLen = static_cast<uint32_t>(globalSalt.size());

        // Calculate offsets
        uint64_t currentOffset = sizeof(data_types::FLKHeader);
        for (uint32_t i = 0; i < header->entryCount; i++) {
            header->entries[i].offset = currentOffset;
            currentOffset += blobs[i].size();
        }

        // Generate output path
        std::filesystem::path outputPath = in_outPath;

        // Write file
        std::vector<uint8_t> emptySalt;
        if (!WriteFLKFile(outputPath, header.get(), blobs, emptySalt)) {
            return false;
        }

        /// TODO
        /// If debug flag enabled output to console the summary
        std::cout << "Successfully packed " << header->entryCount << " files to " << outputPath.string() << "\n";
        return true;
    }

    bool FLKPacker::PackCompressedAndEncrypted(std::filesystem::path in_dirPath, std::filesystem::path in_outPath, int in_compressionLevel) {
        // Validate input directory
        if (!std::filesystem::exists(in_dirPath) || !std::filesystem::is_directory(in_dirPath)) {
            /// TODO
            /// Handle error: invalid input directory
            /// Output to console
            return false;
        }

        // Count files
        size_t fileCount = CountFilesInDirectory(in_dirPath);
        /// TODO
        /// If debug flag enabled output to console the file count

        if (fileCount > MAX_FLK_HEADER_ENTRIES) {
            /// TODO
            /// Handle error: too many files
            /// Output to console
            return false;
        }

        // Initialize header
        auto header = std::make_unique<data_types::FLKHeader>();

		// Initialize compression library
		auto compressor = std::make_unique<compression::zstd::ZstdCompressor>();
		// Initialize encryption library
		auto encryptor = std::make_unique<encryption::xccp20::XChaCha20Poly1305Encryptor>();

        std::vector<std::vector<uint8_t>> blobs;
        std::vector<uint8_t> globalSalt;
        bool firstFile = true;
        size_t entryIndex = 0;

        // Process files
        for (auto& entry : std::filesystem::recursive_directory_iterator(in_dirPath)) {
            if (!std::filesystem::is_regular_file(entry)) continue;

            std::filesystem::path rel = std::filesystem::relative(entry.path(), in_dirPath);
            std::string relPathStr = rel.string();
            auto fileSize = std::filesystem::file_size(entry.path());

            std::string compressedPath = pathcom::PathCompressor::CompressPath(relPathStr);

            if (compressedPath.length() >= MAX_FILE_PATH_LENGTH) {
                std::cerr << "Error: Compressed path too long: " << relPathStr
                    << " -> " << compressedPath << " (" << compressedPath.length() << " chars)\n";
                return false;
            }

            if (!ValidateFLKConstraints(relPathStr, fileSize)) {
                return false;
            }

            /// TODO
            /// If debug flag enabled output to console the file being processed
            std::cout << "Processing: " << relPathStr << " -> " << compressedPath << " (saved " << (relPathStr.length() - compressedPath.length()) << " bytes)\n";

            try {
				// Compress and then encrypt the data in place to save memory
                auto compressionResult = compressor->CompressData(entry.path(), in_compressionLevel);
                auto encryptionResult = encryptor->EncryptData(compressionResult.data, encryption::GetPassword());

				// Store the salt from the first file as global salt
                if (firstFile) {
                    globalSalt = encryptionResult.salt;
                    firstFile = false;
                }

				// Fill entry
                FLKEntry& flkEntry = header->entries[entryIndex];

                OptimizePathPadding(flkEntry.path, compressedPath);
                //std::strncpy(flkEntry.path, relPathStr.c_str(), MAX_FILE_PATH_LENGTH - 1);
                //flkEntry.path[MAX_FILE_PATH_LENGTH - 1] = '\0';
                
                flkEntry.baseSize = compressionResult.originalSize;
                flkEntry.packedSize = encryptionResult.data.size();

                blobs.push_back(std::move(encryptionResult.data));
                entryIndex++;
            }
            catch (const std::exception& ex) {
                /// TODO
                /// Handle error: file processing failed
                /// Output to console
                std::cerr << "Error processing file " << relPathStr << ": " << ex.what() << "\n";
                return false;
            }
        }

        OptimizeUnusedEntries(header.get(), static_cast<uint32_t>(entryIndex));

        header->entryCount = static_cast<uint32_t>(entryIndex);
        header->saltLen = static_cast<uint32_t>(globalSalt.size());

        // Calculate offsets
        uint64_t currentOffset = sizeof(data_types::FLKHeader);
        for (uint32_t i = 0; i < header->entryCount; i++) {
            header->entries[i].offset = currentOffset;
            currentOffset += blobs[i].size();
        }

        // Generate output path
        std::filesystem::path outputPath = in_outPath;

        // Write file
        std::vector<uint8_t> emptySalt;
        if (!WriteFLKFile(outputPath, header.get(), blobs, emptySalt)) {
            return false;
        }

        /// TODO
        /// If debug flag enabled output to console the summary
        std::cout << "Successfully packed " << header->entryCount << " files to " << outputPath.string() << "\n";
        return true;
    }

    std::vector<uint8_t> FLKPacker::ReadFileData(const std::filesystem::path& in_filePath) {
        std::ifstream file(in_filePath, std::ios::binary | std::ios::ate);
        if (!file) {
            /// TODO Change this to output to console instead
			std::cout << "Failed to open file: " << in_filePath.string() << "\n";
        }

        auto fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> data(fileSize);
        file.read(reinterpret_cast<char*>(data.data()), fileSize);

        if (!file) {
            /// TODO Change this to output to console instead
			std::cout << "Failed to read file: " << in_filePath.string() << "\n";
        }

        return data;
    }
    size_t FLKPacker::CountFilesInDirectory(const std::filesystem::path& in_dirPath) {
        size_t count = 0;
        for (auto& entry : std::filesystem::recursive_directory_iterator(in_dirPath)) {
            if (std::filesystem::is_regular_file(entry)) {
                count++;
            }
        }
        return count;
    }

    bool FLKPacker::ValidateFLKConstraints(const std::string& in_relPath, size_t in_fileSize) {
        if (in_relPath.length() >= MAX_FILE_PATH_LENGTH) {
            /// TODO
			/// Handle error: file path too long
            /// Output to console
			std::cout << "Error: File path too long: " << in_relPath << "\n";
            return false;
        }

        // Check file size constraint
        if (in_fileSize > MAX_FILE_SIZE) {
            /// TODO
			/// Handle error: file too large
            /// Output to console
			std::cout << "Error: File too large: " << in_relPath << "\n";
            return false;
        }

		return true;
    }

    bool FLKPacker::WriteFLKFile(const std::filesystem::path& in_outPath,
        data_types::FLKHeader* in_header,
        const std::vector<std::vector<uint8_t>>& in_fileBlobs,
        const std::vector<uint8_t>& in_globalSalt) {

        std::ofstream out(in_outPath, std::ios::binary);
        if (!out) {
            /// TODO
			/// Handle error: failed to create output file
			/// Output to console
			std::cout << "Error: Failed to create output file: " << in_outPath.string() << "\n";
            return false;
        }

        // Write header
        out.write(reinterpret_cast<const char*>(in_header), sizeof(data_types::FLKHeader));
        if (!out.good()) {
            /// TODO
			/// Handle error: failed to write header
			/// Output to console
			std::cout << "Error: Failed to write header to file: " << in_outPath.string() << "\n";
            return false;
        }

        // Write salt if present
        if (!in_globalSalt.empty()) {
            out.write(reinterpret_cast<const char*>(in_globalSalt.data()), in_globalSalt.size());
            if (!out.good()) {
                /// TODO
				/// Handle error: failed to write salt
				/// Output to console
				std::cout << "Error: Failed to write salt to file: " << in_outPath.string() << "\n";
                return false;
            }
        }

        // Write data blobs
        for (size_t i = 0; i < in_fileBlobs.size(); i++) {
            out.write(reinterpret_cast<const char*>(in_fileBlobs[i].data()), in_fileBlobs[i].size());
            if (!out.good()) {
                /// TODO
				/// Handle error: failed to write blob data
				/// Output to console
				std::cout << "Error: Failed to write blob data to file: " << in_outPath.string() << "\n";
                return false;
            }
        }

        out.flush();
        return out.good();
    }

    void FLKPacker::OptimizePathPadding(char* out_entryPath, const std::string& in_actualPath) {
        // Clear with pattern first
        std::fill(out_entryPath, out_entryPath + MAX_FILE_PATH_LENGTH, FLK_PADDING_PATTERN);

        // Copy actual path
        size_t copyLen = std::min(in_actualPath.length(),
            static_cast<size_t>(MAX_FILE_PATH_LENGTH - 1));
        std::memcpy(out_entryPath, in_actualPath.c_str(), copyLen);
        out_entryPath[copyLen] = '\0';

        // Fill remaining with decreasing pattern
        for (size_t i = copyLen + 1; i < MAX_FILE_PATH_LENGTH; ++i) {
            out_entryPath[i] = static_cast<char>(FLK_PADDING_PATTERN - (i % 8));
        }
    }

    void FLKPacker::OptimizeUnusedEntries(flakpak::data_types::FLKHeader* in_header, uint32_t in_actualCount) {
        for (uint32_t i = in_actualCount; i < flakpak::MAX_FLK_HEADER_ENTRIES; i++) {
            auto& entry = in_header->entries[i];

            // Use compression-friendly pattern
            std::fill(entry.path, entry.path + MAX_FILE_PATH_LENGTH, FLK_PADDING_PATTERN);
            entry.path[MAX_FILE_PATH_LENGTH - 1] = '\0';

            entry.offset = FLK_PADDING_PATTERN_64;
            entry.baseSize = FLK_PADDING_PATTERN_64;
            entry.packedSize = FLK_PADDING_PATTERN_64;
        }
    }

} // namespace flakpak