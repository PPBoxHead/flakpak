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
// File: [xccp20_encryptor.h - xccp20_encryptor.c]
// 
// Description: Interface for XChaCha20-Poly1305 encryption method
// 
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 12.10.2025
// Version: 1.2.3
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <flak_definitions.h> - flakpak API data types
//  - <flak_arena.h>       - flakpak API memory arena allocator
//
//  - <string.h>  - C Standard Library
// 
//  - <sodium.h> - For XChaCha20-Poly1305 encryption and Argon2 key derivation
// 
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAKPAK_XCPP20_ENCRYPTOR_H
#define FLAKPAK_XCPP20_ENCRYPTOR_H

#include <flakpak-c/flak_definitions.h>


FLAK_ENCRYPTION_RESULT FLAK_xccp20_encrypt_data(const char* in_file_name, const uint8_t* in_data, size_t in_data_size, const char* in_password);

FLAK_DECRYPTION_RESULT FLAK_xccp20_decrypt_data(const char* in_file_name, const uint8_t* in_data, size_t in_data_size, const char* in_password);

#endif // !FLAKPAK_XCPP20_ENCRYPTOR_H

