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
// File: [zstd_Compressor.hpp - zstd_Compressor.cpp]
// 
// Description: Interface for zstd compression algorithm
// 
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 30.08.2025
// Version: 1.1.0
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <flak_FLKDefinition.hpp> - flakpak API data types
//
//  - <vector>     - C++ Standard Library
//  - <filesystem> - C++ Standard Library
//  - <cstdint>	   - C++ Standard Library
//  - <iostream>   - C++ Standard Library
//  - <fstream>	   - C++ Standard Library
//  - <iomanip>	   - C++ Standard Library
//
//  - <zstd> - Zstandard compression library
// 
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAK_ZSTD_COMPRESSOR_HPP
#define FLAK_ZSTD_COMPRESSOR_HPP

#include <flakpak/flak_FLKDefinition.hpp>

#include <vector>
#include <filesystem>
#include <cstdint>


namespace flakpak::compression::zstd {
	class ZstdCompressor final {
	public:
		ZstdCompressor() = default;
		~ZstdCompressor() = default;

		// Compress file at given path with specified compression level
		//    @param in_path					- Path to the file to compress
		//	  @param in_compressionLevel		- Compression level (default is 3)
		//    
		//    @return FLK_COMPRESSION_RESULT	- structure containing compressed data and sizes
		data_types::FLK_COMPRESSION_RESULT CompressData(const std::filesystem::path& in_path,
			int in_compressionLevel = 3);
		// Decompress data to its original form
		//    @param in_data				 - Data to decompress
		//	  @param in_originalSize		 - Original size of the data (required for some algorithms)
		//	  
		//    @return std::vector<uint8_t>	 - Decompressed data
		std::vector<uint8_t> DecompressData(const std::vector<uint8_t>& in_data,
			size_t in_originalSize);

	}; // class ZstdCompressor final

} // namespace flakpak::compression::zstd

#endif // !FLAK_ZSTD_COMPRESSOR_HPP