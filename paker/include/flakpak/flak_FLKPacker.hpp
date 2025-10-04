// ===========================================================================
// 
// flakpak - File Archiver and Compressor CL Application
//
// Copyright (C) 2025 SACRAROSSA
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// ---------------------------------------------------------------------------
// File: [flak_FLKPacker.hpp - flak_FLKPacker.cpp]
//
// Description: Implements the packing functionality for the flakpak
//              application, including file compression, encryption,
//              and FLK file format handling.
//
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 29.09.2025
// Version: 1.0.3
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <flakpak/flak_FLKDefinition.hpp>		 - flakpak API
//  - <flakpak/zstd_Compressor.hpp>			 - flakpak API
//  - <flakpak/xccp20_Encryptor.hpp>		 - flakpak API
//  - <flakpak/flak_PasswordHandler.hpp>	 - flakpak API
//	- <flakpak/flak_PathCompressor.hpp>		 - flakpak API
// 
//  - <filesystem>   - C++ Standard Library
//  - <cstring>      - C++ Standard Library
//  - <memory>		 - C++ Standard Library
//  - <iostream>	 - C++ Standard Library
//  - <fstream>		 - C++ Standard Library
//
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAK_FLK_PACKER_HPP
#define FLAK_FLK_PACKER_HPP

#include <flakpak/flak_FLKDefinition.hpp>

#include <filesystem>
#include <cstring>


namespace flakpak {
	class FLKPacker {
	public:
		FLKPacker() = default;
		~FLKPacker() = default;

		// Packs all the files in the specified directory into an FLK file
		// with no compression and no encryption
		static bool PackUncompressedAndUnencrypted(std::filesystem::path in_dirPath, std::filesystem::path in_outPath);
		// Packs all the files in the specified directory into an FLK file
		// with compression and no encryption
		static bool PackCompressedAndUnencrypted(std::filesystem::path in_dirPath, std::filesystem::path in_outPath, int in_compressionLevel);
		// Packs all the files in the specified directory into an FLK file
		// with no compression and with encryption
		static bool PackUncompressedAndEncrypted(std::filesystem::path in_dirPath, std::filesystem::path in_outPath);
		// Packs all the files in the specified directory into an FLK file
		// with compression and with encryption
		static bool PackCompressedAndEncrypted(std::filesystem::path in_dirPath, std::filesystem::path in_outPath, int in_compressionLevel);

	private:
		static std::vector<uint8_t> ReadFileData(const std::filesystem::path& in_filePath);
		static size_t CountFilesInDirectory(const std::filesystem::path& in_dirPath);

		// Validates file path length and file size constraints in the FLK format
		static bool ValidateFLKConstraints(const std::string& in_relPath, size_t in_fileSize);

		static bool WriteFLKFile(const std::filesystem::path& in_outPath, data_types::FLKHeader* in_header, const std::vector<std::vector<uint8_t>>& in_fileBlobs, const std::vector<uint8_t>& in_globalSalt);

		static void OptimizePathPadding(char* out_entryPath, const std::string& in_actualPath);

		static void OptimizeUnusedEntries(data_types::FLKHeader* in_header, uint32_t in_actualCount);

	}; // class FLKPacker
}

#endif // !FLAK_FLK_PACKER_HPP