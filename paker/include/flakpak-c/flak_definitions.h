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
// File: [flak_definitions.h]
//
// Description: Defines the FLK file format structure and constants for use in
//              the flakpak application, including magic numbers, versioning,
//              and data structures for file headers and entries.
//				Also includes other data type definitions used across the application.
//
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 11.10.2025
// Version: 1.1.0
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <stdint.h> - C Standard Library
//
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAKPAK_FLK_DEFINITION_H
#define FLAKPAK_FLK_DEFINITION_H

#include <stdint.h>


// Define a macro for packed structures to ensure no padding is added by the compiler (compiler-specific)
#if defined(_MSC_VER)
	#define FLAK_PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#elif defined(__GNUC__) || defined(__clang__)
	#define FLAK_PACKED_STRUCT(name) struct __attribute__((packed)) name
#else
	#define FLAK_PACKED_STRUCT(name) struct name
#endif


#define FLK_MAX_HEADER_ENTRIES 256		// Maximum number of entries in the FLK file
#define FLK_MAX_FILE_PATH_LENGTH 128	// Maximum length for file paths
#define FLK_MAX_FILE_SIZE (1ULL << 30)  // Maximum file size (1 GB)

typedef enum {
	FLK_FLAG_NONE = 0, 				// 0000
	FLK_FLAG_COMPRESSED	= 1 << 0,	// 0001
	FLK_FLAG_ENCRYPTED = 1 << 1,	// 0010
} FLK_file_flags;

// FLK entry structure
typedef struct FLK_entry FLK_entry_t;
FLAK_PACKED_STRUCT(FLK_entry) {
	char file_path[FLK_MAX_FILE_PATH_LENGTH];	// File path (null-terminated string)
	uint64_t offset;							// Offset of the file data in the FLK file
	uint64_t base_size;							// Original size of the file before compression/encryption
	uint64_t packed_size;						// Size of the file after compression/encryption
};

// FLK file header structure
typedef struct FLK_header FLK_header_t;
FLAK_PACKED_STRUCT(FLK_header) {
	char magic[4];									// 'F', 'L', 'K', '\0'
	uint8_t	version;			 					// Version of the FLK format
	uint32_t salt_lenght;							// Length of the global salt (0 if no salt)
	uint32_t content_version;						// User-defined content version
	uint32_t entry_count;							// Actual number of entries used
	FLK_entry_t	entries[FLK_MAX_HEADER_ENTRIES];	// Fixed-size array of entries
	FLK_file_flags flags;							// Flags for compression/encryption
};

// Result structure for compression operations
typedef struct FLAK_COMPRESSION_RESULT {
	uint8_t* data; 				// Pointer to the compressed data
	size_t original_size; 		// Original size before compression
	size_t compressed_size;		// Size after compression
} FLAK_COMPRESSION_RESULT;

// Result structure for encryption operations
typedef struct FLAK_ENCRYPTION_RESULT {
	uint8_t* data; 		// Pointer to the encrypted data
	uint8_t	salt[16];	// 128-bit salt
	uint8_t	nonce[24];	// 192-bit nonce
	size_t data_size;	// Size of the encrypted data
} FLAK_ENCRYPTION_RESULT;


#endif // !FLAKPAK_FLK_DEFINITION_H