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
// File: [flak_pswd_definition.h]
// 
// Description: Provides password encoding and decoding utilities for flakpak,
//				including obfuscated password segments and functions to reconstruct the
//				original password using simple XOR and ADD ciphers.
// 
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 11.10.2025
// Version: 1.1.0
// 
// ---------------------------------------------------------------------------
// Dependencies:
//  - <stdint.h> - C Standard Library
//  - <string.h> - C Standard Library
// 
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAKPAK_PSWD_DEFINITION_H
#define FLAKPAK_PSWD_DEFINITION_H

#include <stdint.h>
#include <string.h>


// Encoded segments of the password
#define FLAK_PSWD_A 0x5A
#define FLAK_PSWD_B 0xC3
#define FLAK_PSWD_C 0x17
#define FLAK_PSWD_D 0x9F

// I only gonna say this is a Lorem Ipsum text I generated on the internet, encoded with a mix of XOR and ADD ciphers xd
static const uint8_t FLAK_PSWD_SEG_1[8] = {
	(uint8_t)('L' ^ FLAK_PSWD_A), (uint8_t)('o' ^ FLAK_PSWD_A),
	(uint8_t)('r' ^ FLAK_PSWD_A), (uint8_t)('e' ^ FLAK_PSWD_A),
	(uint8_t)('m' ^ FLAK_PSWD_A), (uint8_t)(' ' ^ FLAK_PSWD_A),
	(uint8_t)('i' ^ FLAK_PSWD_A), (uint8_t)('p' ^ FLAK_PSWD_A)
};
static const uint8_t FLAK_PSWD_SEG_2[8] = {
	(uint8_t)('s' + FLAK_PSWD_B), (uint8_t)('u' + FLAK_PSWD_B),
	(uint8_t)('m' + FLAK_PSWD_B), (uint8_t)(' ' + FLAK_PSWD_B),
	(uint8_t)('d' + FLAK_PSWD_B), (uint8_t)('o' + FLAK_PSWD_B),
	(uint8_t)('l' + FLAK_PSWD_B), (uint8_t)('o' + FLAK_PSWD_B)
};
static const uint8_t FLAK_PSWD_SEG_3[8] = {
	(uint8_t)('r' ^ FLAK_PSWD_C), (uint8_t)(' ' ^ FLAK_PSWD_C),
	(uint8_t)('s' ^ FLAK_PSWD_C), (uint8_t)('i' ^ FLAK_PSWD_C),
	(uint8_t)('t' ^ FLAK_PSWD_C), (uint8_t)(' ' ^ FLAK_PSWD_C),
	(uint8_t)('a' ^ FLAK_PSWD_C), (uint8_t)('m' ^ FLAK_PSWD_C)
};
static const uint8_t FLAK_PSWD_SEG_4[8] = {
	(uint8_t)('e' + FLAK_PSWD_D), (uint8_t)('t' + FLAK_PSWD_D),
	(uint8_t)(',' + FLAK_PSWD_D), (uint8_t)(' ' + FLAK_PSWD_D),
	(uint8_t)('c' + FLAK_PSWD_D), (uint8_t)('o' + FLAK_PSWD_D),
	(uint8_t)('n' + FLAK_PSWD_D), (uint8_t)('s' + FLAK_PSWD_D)
};
static const uint8_t FLAK_PSWD_SEG_5[8] = {
	(uint8_t)('e' ^ FLAK_PSWD_A), (uint8_t)('c' ^ FLAK_PSWD_A),
	(uint8_t)('t' ^ FLAK_PSWD_A), (uint8_t)('e' ^ FLAK_PSWD_A),
	(uint8_t)('t' ^ FLAK_PSWD_A), (uint8_t)('u' ^ FLAK_PSWD_A),
	(uint8_t)('r' ^ FLAK_PSWD_A), (uint8_t)(' ' ^ FLAK_PSWD_A)
};
static const uint8_t FLAK_PSWD_SEG_6[8] = {
	(uint8_t)('a' + FLAK_PSWD_B), (uint8_t)('d' + FLAK_PSWD_B),
	(uint8_t)('i' + FLAK_PSWD_B), (uint8_t)('p' + FLAK_PSWD_B),
	(uint8_t)('i' + FLAK_PSWD_B), (uint8_t)('s' + FLAK_PSWD_B),
	(uint8_t)('c' + FLAK_PSWD_B), (uint8_t)('i' + FLAK_PSWD_B)
};
static const uint8_t FLAK_PSWD_SEG_7[8] = {
	(uint8_t)('n' ^ FLAK_PSWD_C), (uint8_t)('g' ^ FLAK_PSWD_C),
	(uint8_t)(' ' ^ FLAK_PSWD_C), (uint8_t)('e' ^ FLAK_PSWD_C),
	(uint8_t)('l' ^ FLAK_PSWD_C), (uint8_t)('i' ^ FLAK_PSWD_C),
	(uint8_t)('t' ^ FLAK_PSWD_C), (uint8_t)('.' ^ FLAK_PSWD_C)
};
static const uint8_t FLAK_PSWD_SEG_8[8] = {
	(uint8_t)(' ' + FLAK_PSWD_D), (uint8_t)('D' + FLAK_PSWD_D),
	(uint8_t)('u' + FLAK_PSWD_D), (uint8_t)('i' + FLAK_PSWD_D),
	(uint8_t)('s' + FLAK_PSWD_D), (uint8_t)(' ' + FLAK_PSWD_D),
	(uint8_t)('v' + FLAK_PSWD_D), (uint8_t)('u' + FLAK_PSWD_D)
};
static const uint8_t FLAK_PSWD_SEG_9[8] = {
	(uint8_t)('l' ^ FLAK_PSWD_A), (uint8_t)('p' ^ FLAK_PSWD_A),
	(uint8_t)('u' ^ FLAK_PSWD_A), (uint8_t)('t' ^ FLAK_PSWD_A),
	(uint8_t)('a' ^ FLAK_PSWD_A), (uint8_t)('t' ^ FLAK_PSWD_A),
	(uint8_t)('e' ^ FLAK_PSWD_A), (uint8_t)(' ' ^ FLAK_PSWD_A)
};
static const uint8_t FLAK_PSWD_SEG_10[8] = {
	(uint8_t)('m' + FLAK_PSWD_B), (uint8_t)('i' + FLAK_PSWD_B),
	(uint8_t)(' ' + FLAK_PSWD_B), (uint8_t)('r' + FLAK_PSWD_B),
	(uint8_t)('i' + FLAK_PSWD_B), (uint8_t)('s' + FLAK_PSWD_B),
	(uint8_t)('u' + FLAK_PSWD_B), (uint8_t)('s' + FLAK_PSWD_B)
};
static const uint8_t FLAK_PSWD_SEG_11[8] = {
	(uint8_t)(',' ^ FLAK_PSWD_C), (uint8_t)(' ' ^ FLAK_PSWD_C),
	(uint8_t)('i' ^ FLAK_PSWD_C), (uint8_t)('d' ^ FLAK_PSWD_C),
	(uint8_t)(' ' ^ FLAK_PSWD_C), (uint8_t)('o' ^ FLAK_PSWD_C),
	(uint8_t)('r' ^ FLAK_PSWD_C), (uint8_t)('n' ^ FLAK_PSWD_C)
};
static const uint8_t FLAK_PSWD_SEG_12[8] = {
	(uint8_t)('a' + FLAK_PSWD_D), (uint8_t)('r' + FLAK_PSWD_D),
	(uint8_t)('e' + FLAK_PSWD_D), (uint8_t)(' ' + FLAK_PSWD_D),
	(uint8_t)('l' + FLAK_PSWD_D), (uint8_t)('i' + FLAK_PSWD_D),
	(uint8_t)('g' + FLAK_PSWD_D), (uint8_t)('u' + FLAK_PSWD_D)
};
static const uint8_t FLAK_PSWD_SEG_13[8] = {
	(uint8_t)('l' ^ FLAK_PSWD_A), (uint8_t)('a' ^ FLAK_PSWD_A),
	(uint8_t)(' ' ^ FLAK_PSWD_A), (uint8_t)('i' ^ FLAK_PSWD_A),
	(uint8_t)('a' ^ FLAK_PSWD_A), (uint8_t)('c' ^ FLAK_PSWD_A),
	(uint8_t)('u' ^ FLAK_PSWD_A), (uint8_t)('l' ^ FLAK_PSWD_A)
};
static const uint8_t FLAK_PSWD_SEG_14[8] = {
	(uint8_t)('i' + FLAK_PSWD_B), (uint8_t)('s' + FLAK_PSWD_B),
	(uint8_t)(' ' + FLAK_PSWD_B), (uint8_t)('v' + FLAK_PSWD_B),
	(uint8_t)('e' + FLAK_PSWD_B), (uint8_t)('n' + FLAK_PSWD_B),
	(uint8_t)('e' + FLAK_PSWD_B), (uint8_t)('n' + FLAK_PSWD_B)
};
static const uint8_t FLAK_PSWD_SEG_15[8] = {
	(uint8_t)('a' ^ FLAK_PSWD_C), (uint8_t)('t' ^ FLAK_PSWD_C),
	(uint8_t)('i' ^ FLAK_PSWD_C), (uint8_t)('s' ^ FLAK_PSWD_C),
	(uint8_t)('.' ^ FLAK_PSWD_C), (uint8_t)(' ' ^ FLAK_PSWD_C),
	(uint8_t)('D' ^ FLAK_PSWD_C), (uint8_t)('o' ^ FLAK_PSWD_C)
};
static const uint8_t FLAK_PSWD_SEG_16[8] = {
	(uint8_t)('n' + FLAK_PSWD_D), (uint8_t)('e' + FLAK_PSWD_D),
	(uint8_t)('c' + FLAK_PSWD_D), (uint8_t)(' ' + FLAK_PSWD_D),
	(uint8_t)('i' + FLAK_PSWD_D), (uint8_t)('n' + FLAK_PSWD_D),
	(uint8_t)(' ' + FLAK_PSWD_D), (uint8_t)('p' + FLAK_PSWD_D)
};
static const uint8_t FLAK_PSWD_SEG_17[8] = {
	(uint8_t)('l' ^ FLAK_PSWD_A), (uint8_t)('a' ^ FLAK_PSWD_A),
	(uint8_t)('c' ^ FLAK_PSWD_A), (uint8_t)('e' ^ FLAK_PSWD_A),
	(uint8_t)('r' ^ FLAK_PSWD_A), (uint8_t)('a' ^ FLAK_PSWD_A),
	(uint8_t)('t' ^ FLAK_PSWD_A), (uint8_t)(' ' ^ FLAK_PSWD_A)
};
static const uint8_t FLAK_PSWD_SEG_18[8] = {
	(uint8_t)('e' + FLAK_PSWD_B), (uint8_t)('n' + FLAK_PSWD_B),
	(uint8_t)('i' + FLAK_PSWD_B), (uint8_t)('m' + FLAK_PSWD_B),
	(uint8_t)('.' + FLAK_PSWD_B), (uint8_t)(' ' + FLAK_PSWD_B),
	(uint8_t)('S' + FLAK_PSWD_B), (uint8_t)('u' + FLAK_PSWD_B)
};
static const uint8_t FLAK_PSWD_SEG_19[8] = {
	(uint8_t)('s' ^ FLAK_PSWD_C), (uint8_t)('p' ^ FLAK_PSWD_C),
	(uint8_t)('e' ^ FLAK_PSWD_C), (uint8_t)('n' ^ FLAK_PSWD_C),
	(uint8_t)('d' ^ FLAK_PSWD_C), (uint8_t)('i' ^ FLAK_PSWD_C),
	(uint8_t)('s' ^ FLAK_PSWD_C), (uint8_t)('s' ^ FLAK_PSWD_C)
};
static const uint8_t FLAK_PSWD_SEG_20[8] = {
	(uint8_t)('e' + FLAK_PSWD_D), (uint8_t)(' ' + FLAK_PSWD_D),
	(uint8_t)('f' + FLAK_PSWD_D), (uint8_t)('r' + FLAK_PSWD_D),
	(uint8_t)('i' + FLAK_PSWD_D), (uint8_t)('n' + FLAK_PSWD_D),
	(uint8_t)('g' + FLAK_PSWD_D), (uint8_t)('i' + FLAK_PSWD_D)
};
static const uint8_t FLAK_PSWD_SEG_21[8] = {
	(uint8_t)('l' ^ FLAK_PSWD_A), (uint8_t)('l' ^ FLAK_PSWD_A),
	(uint8_t)('a' ^ FLAK_PSWD_A), (uint8_t)(' ' ^ FLAK_PSWD_A),
	(uint8_t)('q' ^ FLAK_PSWD_A), (uint8_t)('u' ^ FLAK_PSWD_A),
	(uint8_t)('a' ^ FLAK_PSWD_A), (uint8_t)('m' ^ FLAK_PSWD_A)
};
static const uint8_t FLAK_PSWD_SEG_22[8] = {
	(uint8_t)(' ' + FLAK_PSWD_B), (uint8_t)('d' + FLAK_PSWD_B),
	(uint8_t)('i' + FLAK_PSWD_B), (uint8_t)('a' + FLAK_PSWD_B),
	(uint8_t)('m' + FLAK_PSWD_B), (uint8_t)(',' + FLAK_PSWD_B),
	(uint8_t)(' ' + FLAK_PSWD_B), (uint8_t)('v' + FLAK_PSWD_B)
};
static const uint8_t FLAK_PSWD_SEG_23[8] = {
	(uint8_t)('i' ^ FLAK_PSWD_C), (uint8_t)('t' ^ FLAK_PSWD_C),
	(uint8_t)('a' ^ FLAK_PSWD_C), (uint8_t)('e' ^ FLAK_PSWD_C),
	(uint8_t)(' ' ^ FLAK_PSWD_C), (uint8_t)('p' ^ FLAK_PSWD_C),
	(uint8_t)('o' ^ FLAK_PSWD_C), (uint8_t)('r' ^ FLAK_PSWD_C)
};
static const uint8_t FLAK_PSWD_SEG_24[8] = {
	(uint8_t)('t' + FLAK_PSWD_D), (uint8_t)('i' + FLAK_PSWD_D),
	(uint8_t)('t' + FLAK_PSWD_D), (uint8_t)('o' + FLAK_PSWD_D),
	(uint8_t)('r' + FLAK_PSWD_D), (uint8_t)(' ' + FLAK_PSWD_D),
	(uint8_t)('s' + FLAK_PSWD_D), (uint8_t)('e' + FLAK_PSWD_D)
};
static const uint8_t FLAK_PSWD_SEG_25[8] = {
	(uint8_t)('m' ^ FLAK_PSWD_A), (uint8_t)(' ' ^ FLAK_PSWD_A),
	(uint8_t)('c' ^ FLAK_PSWD_A), (uint8_t)('o' ^ FLAK_PSWD_A),
	(uint8_t)('n' ^ FLAK_PSWD_A), (uint8_t)('d' ^ FLAK_PSWD_A),
	(uint8_t)('i' ^ FLAK_PSWD_A), (uint8_t)('m' ^ FLAK_PSWD_A)
};
static const uint8_t FLAK_PSWD_SEG_26[8] = {
	(uint8_t)('e' + FLAK_PSWD_B), (uint8_t)('n' + FLAK_PSWD_B),
	(uint8_t)('t' + FLAK_PSWD_B), (uint8_t)('u' + FLAK_PSWD_B),
	(uint8_t)('m' + FLAK_PSWD_B), (uint8_t)(' ' + FLAK_PSWD_B),
	(uint8_t)('e' + FLAK_PSWD_B), (uint8_t)('u' + FLAK_PSWD_B)
};
static const uint8_t FLAK_PSWD_SEG_27[8] = {
	(uint8_t)('.' ^ FLAK_PSWD_C), (uint8_t)(' ' ^ FLAK_PSWD_C),
	(uint8_t)('P' ^ FLAK_PSWD_C), (uint8_t)('r' ^ FLAK_PSWD_C),
	(uint8_t)('a' ^ FLAK_PSWD_C), (uint8_t)('e' ^ FLAK_PSWD_C),
	(uint8_t)('s' ^ FLAK_PSWD_C), (uint8_t)('e' ^ FLAK_PSWD_C)
};
static const uint8_t FLAK_PSWD_SEG_28[8] = {
	(uint8_t)('n' + FLAK_PSWD_D), (uint8_t)('t' + FLAK_PSWD_D),
	(uint8_t)(' ' + FLAK_PSWD_D), (uint8_t)('v' + FLAK_PSWD_D),
	(uint8_t)('o' + FLAK_PSWD_D), (uint8_t)('l' + FLAK_PSWD_D),
	(uint8_t)('u' + FLAK_PSWD_D), (uint8_t)('t' + FLAK_PSWD_D)
};
static const uint8_t FLAK_PSWD_SEG_29[8] = {
	(uint8_t)('p' ^ FLAK_PSWD_A), (uint8_t)('a' ^ FLAK_PSWD_A),
	(uint8_t)('t' ^ FLAK_PSWD_A), (uint8_t)(' ' ^ FLAK_PSWD_A),
	(uint8_t)('n' ^ FLAK_PSWD_A), (uint8_t)('e' ^ FLAK_PSWD_A),
	(uint8_t)('c' ^ FLAK_PSWD_A), (uint8_t)(' ' ^ FLAK_PSWD_A)
};
static const uint8_t FLAK_PSWD_SEG_30[8] = {
	(uint8_t)('d' + FLAK_PSWD_B), (uint8_t)('o' + FLAK_PSWD_B),
	(uint8_t)('l' + FLAK_PSWD_B), (uint8_t)('o' + FLAK_PSWD_B),
	(uint8_t)('r' + FLAK_PSWD_B), (uint8_t)(' ' + FLAK_PSWD_B),
	(uint8_t)('q' + FLAK_PSWD_B), (uint8_t)('u' + FLAK_PSWD_B)
};
static const uint8_t FLAK_PSWD_SEG_31[8] = {
	(uint8_t)('i' ^ FLAK_PSWD_C), (uint8_t)('s' ^ FLAK_PSWD_C),
	(uint8_t)(' ' ^ FLAK_PSWD_C), (uint8_t)('r' ^ FLAK_PSWD_C),
	(uint8_t)('u' ^ FLAK_PSWD_C), (uint8_t)('t' ^ FLAK_PSWD_C),
	(uint8_t)('r' ^ FLAK_PSWD_C), (uint8_t)('u' ^ FLAK_PSWD_C)
};
static const uint8_t FLAK_PSWD_SEG_32[8] = {
	(uint8_t)('m' + FLAK_PSWD_D), (uint8_t)(' ' + FLAK_PSWD_D),
	(uint8_t)('o' + FLAK_PSWD_D), (uint8_t)('d' + FLAK_PSWD_D),
	(uint8_t)('i' + FLAK_PSWD_D), (uint8_t)('o' + FLAK_PSWD_D),
	(uint8_t)('.' + FLAK_PSWD_D), (uint8_t)('\0' + FLAK_PSWD_D)
};

// Function to decode a segment using XOR cipher
static inline void FLAK_decode_xor(const uint8_t* in_segment, uint8_t in_key, char* out_segment) {
	for (size_t i = 0; i < 8; ++i) {
		out_segment[i] = (char)(in_segment[i] ^ in_key);
	}
	out_segment[8] = '\0'; // Null-terminate the string
}
// Helper functions to decode a segment directly into password buffer using XOR cipher
static inline void FLAK_decode_xor_to_buffer(const uint8_t* in_segment, uint8_t in_key, char* out_dst, size_t* inout_offset) {
	char temp[9];
	FLAK_decode_xor(in_segment, in_key, temp);
	memcpy(out_dst + *inout_offset, temp, 8);
	*inout_offset += 8;
}
// Function to decode a segment using ADD cipher
static inline void FLAK_decode_add(const uint8_t* in_segment, uint8_t in_key, char* out_segment) {
	for (size_t i = 0; i < 8; ++i) {
		out_segment[i] = (char)(in_segment[i] - in_key);
	}
	out_segment[8] = '\0'; // Null-terminate the string
}
// Helper functions to decode a segment directly into password buffer using ADD cipher
static inline void FLAK_decode_add_to_buffer(const uint8_t* in_segment, uint8_t in_key, char* out_dst, size_t* inout_offset) {
	char temp[9];
	FLAK_decode_add(in_segment, in_key, temp);
	memcpy(out_dst + *inout_offset, temp, 8);
	*inout_offset += 8;
}

char* FLAK_get_password(void) {
	static char password[257]; // 32 segments * 8 bytes + null
	size_t offset = 0;
	
	// Writting this was a pain in the ass. Hope I NEVER have to come back here lol
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_1, FLAK_PSWD_A, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_2, FLAK_PSWD_B, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_3, FLAK_PSWD_C, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_4, FLAK_PSWD_D, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_5, FLAK_PSWD_A, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_6, FLAK_PSWD_B, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_7, FLAK_PSWD_C, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_8, FLAK_PSWD_D, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_9, FLAK_PSWD_A, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_10, FLAK_PSWD_B, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_11, FLAK_PSWD_C, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_12, FLAK_PSWD_D, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_13, FLAK_PSWD_A, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_14, FLAK_PSWD_B, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_15, FLAK_PSWD_C, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_16, FLAK_PSWD_D, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_17, FLAK_PSWD_A, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_18, FLAK_PSWD_B, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_19, FLAK_PSWD_C, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_20, FLAK_PSWD_D, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_21, FLAK_PSWD_A, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_22, FLAK_PSWD_B, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_23, FLAK_PSWD_C, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_24, FLAK_PSWD_D, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_25, FLAK_PSWD_A, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_26, FLAK_PSWD_B, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_27, FLAK_PSWD_C, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_28, FLAK_PSWD_D, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_29, FLAK_PSWD_A, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_30, FLAK_PSWD_B, password, &offset);
	FLAK_decode_xor_to_buffer(FLAK_PSWD_SEG_31, FLAK_PSWD_C, password, &offset);
	FLAK_decode_add_to_buffer(FLAK_PSWD_SEG_32, FLAK_PSWD_D, password, &offset);

	password[offset] = '\0'; // Null-terminate the final password
	return password;
}

#endif // !FLAKPAK_PSWD_DEFINITION_H