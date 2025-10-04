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
// File: [flak_FLKDefinition.hpp]
//
// Description: Defines the FLK file format structure and constants for use in
//              the flakpak application, including magic numbers, versioning,
//              and data structures for file headers and entries.
//				Also includes other data type definitions used across the application.
//
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 15.09.2025
// Version: 1.0.1
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <cstdint> - C++ Standard Library
//  - <array>   - C++ Standard Library
//  - <string>  - C++ Standard Library
//  - <vector>   - C++ Standard Library
//
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAK_FLK_DEFINITION_HPP
#define FLAK_FLK_DEFINITION_HPP

#include <cstdint>
#include <array>
#include <string>
#include <vector>


namespace flakpak {
	static constexpr size_t MAX_FLK_HEADER_ENTRIES = 256;	// Maximum number of entries in the FLK file
	static constexpr size_t MAX_FILE_PATH_LENGTH = 128;		// Maximum length for file paths
	static constexpr size_t MAX_FILE_SIZE = 1ULL << 30;		// Maximum file size (1 GB)

	static constexpr uint8_t FLK_PADDING_PATTERN = 0xCC;
	static constexpr uint64_t FLK_PADDING_PATTERN_64 = 0xCCCCCCCCCCCCCCCC;
}

namespace flakpak::data_types {
#pragma pack(push, 1) // Ensure no padding is added by the compiler
	struct FLKEntry {
		char path[MAX_FILE_PATH_LENGTH] {};		// File path (null-terminated string)
		uint64_t offset { 0 };					// Offset of the file data in the FLK file
		uint64_t baseSize { 0 };				// Original size of the file before compression/encryption
		uint64_t packedSize { 0 };				// Size of the file after compression/encryption

	}; // FLKEntry

	struct FLKHeader {
		std::array<char, 4> magic { {'F', 'L', 'K', '\0'} };			// Magic number to identify FLK files
		uint8_t version { 1 };											// FLK file format version
		uint16_t reserved{ 0xABCD }; 									// Reserved for future use
		uint32_t saltLen { 0 };											// Length of the global salt (0 if no salt)
		uint32_t contentVersion { 0 };									// User-defined content version
		uint32_t entryCount { 0 };										// Actual number of entries used
		std::array<FLKEntry, MAX_FLK_HEADER_ENTRIES> entries {};		// Fixed-size array of entries

	}; // FLKHeader
#pragma pack(pop)

	// Result structure for compression operations
	struct FLK_COMPRESSION_RESULT {
		std::vector<uint8_t> data{};
		size_t originalSize{};
		size_t compressedSize{};

	}; // FLK_COMPRESSION_RESULT

	// Result structure for encryption operations
	struct FLK_ENCRYPTION_RESULT {
		std::vector<uint8_t> data;
		std::vector<uint8_t> salt;  // For key derivation
		std::vector<uint8_t> nonce; // Per-file nonce (extracted from data for interface clarity)

	}; // FLK_ENCRYPTION_RESULT

} // namespace flakpak::data_types

#endif // !FLAK_FLK_DEFINITION_HPP