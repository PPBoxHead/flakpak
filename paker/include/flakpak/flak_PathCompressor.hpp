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
//
// File: [flak_PathCompressor.hpp - flak_PathCompressor.hpp]
//
// Description: Implements path compression and decompression functionalities
//              for the flakpak application, optimizing file paths using
//              predefined substitutions.
//
// Author: Emanuel Favaro
// Date: 04.10.2025
// Version: 1.0.0
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <string>  - C++ Standard Library
//  - <vector>   - C++ Standard Library
//
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAK_PATH_COMPRESSOR_HPP
#define FLAK_PATH_COMPRESSOR_HPP

#include <string>
#include <vector>


namespace flakpak::pathcom {
	class PathCompressor {
	public:
		static std::string CompressPath(const std::string& in_filePath);
		static std::string DecompressPath(const std::string& in_compressedPath);

	private:
		static const std::vector<std::pair<std::string, char>>& GetSubstitutions();

	}; // class PathCompressor
} // namespace flakpak::pathcom

#endif // FLAK_FLK_PACKER_HPP