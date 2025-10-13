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
// File: [flak_packer.h - flak_packer.c]
//
// Description: Implements the packing functionality for the flakpak
//              application, including file compression, encryption,
//              and FLK file format handling.
//
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 12.10.2025
// Version: 1.1.1
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <flakpak-c/flak_definitions.h>	 - flakpak API data types
//  - <flakpak-c/flak_pswd_definition.h> - flakpak API password definition stuff
//  - <flakpak-c/zstd_compressor.h>		 - flakpak API zstd compressor interface
//  - <flakpak-c/xccp20_encryptor.h>	 - flakpak API xccp20 encryptor interface
//
//	- <stdbool.h> - C Standard Library boolean type
// 
//  - <tinydir.h> - filesystem path handling library
// 
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAKPAK_PACKER_H
#define FLAKPAK_PACKER_H

#include <flakpak-c/flak_definitions.h>

#include <stdbool.h>


bool FLAK_pack_files(const char* in_dir_path, FLK_file_flags in_flags, int in_comp_level);

#endif // !FLAKPAK_PACKER_H