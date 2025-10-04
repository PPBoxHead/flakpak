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
// File: [flak_PasswordHandler.hpp]
// 
// Description: Provides password encoding and decoding utilities for flakpak,
//				including obfuscated password segments and functions to reconstruct the
//				original password using simple XOR and ADD ciphers.
// 
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 26.08.2025
// Version: 1.0.1
// 
// ---------------------------------------------------------------------------
// Dependencies:
//  - <string> - C++ Standard Library
//  - <array>  - C++ Standard Library
// 
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAK_PASSWORD_HANDLER_HPP
#define FLAK_PASSWORD_HANDLER_HPP

#include <string>
#include <array>


namespace flakpak::encryption {
	// Encoded segments of the password
	constexpr uint8_t FLAK_PSWD_A = 0x5A;
	constexpr uint8_t FLAK_PSWD_B = 0xC3;
	constexpr uint8_t FLAK_PSWD_C = 0x17;
	constexpr uint8_t FLAK_PSWD_D = 0x9F;

	// I only gonna say this is a Lorem Ipsum text encoded with a mix of XOR and ADD ciphers xd
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_1 = {
		static_cast<uint8_t>('L' ^ FLAK_PSWD_A), static_cast<uint8_t>('o' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('r' ^ FLAK_PSWD_A), static_cast<uint8_t>('e' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('m' ^ FLAK_PSWD_A), static_cast<uint8_t>(' ' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('i' ^ FLAK_PSWD_A), static_cast<uint8_t>('p' ^ FLAK_PSWD_A)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_2 = {
		static_cast<uint8_t>('s' + FLAK_PSWD_B), static_cast<uint8_t>('u' + FLAK_PSWD_B),
		static_cast<uint8_t>('m' + FLAK_PSWD_B), static_cast<uint8_t>(' ' + FLAK_PSWD_B),
		static_cast<uint8_t>('d' + FLAK_PSWD_B), static_cast<uint8_t>('o' + FLAK_PSWD_B),
		static_cast<uint8_t>('l' + FLAK_PSWD_B), static_cast<uint8_t>('o' + FLAK_PSWD_B)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_3 = {
		static_cast<uint8_t>('r' ^ FLAK_PSWD_C), static_cast<uint8_t>(' ' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('s' ^ FLAK_PSWD_C), static_cast<uint8_t>('i' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('t' ^ FLAK_PSWD_C), static_cast<uint8_t>(' ' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('a' ^ FLAK_PSWD_C), static_cast<uint8_t>('m' ^ FLAK_PSWD_C)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_4 = {
		static_cast<uint8_t>('e' + FLAK_PSWD_D), static_cast<uint8_t>('t' + FLAK_PSWD_D),
		static_cast<uint8_t>(',' + FLAK_PSWD_D), static_cast<uint8_t>(' ' + FLAK_PSWD_D),
		static_cast<uint8_t>('c' + FLAK_PSWD_D), static_cast<uint8_t>('o' + FLAK_PSWD_D),
		static_cast<uint8_t>('n' + FLAK_PSWD_D), static_cast<uint8_t>('s' + FLAK_PSWD_D)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_5 = {
		static_cast<uint8_t>('e' ^ FLAK_PSWD_A), static_cast<uint8_t>('c' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('t' ^ FLAK_PSWD_A), static_cast<uint8_t>('e' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('t' ^ FLAK_PSWD_A), static_cast<uint8_t>('u' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('r' ^ FLAK_PSWD_A), static_cast<uint8_t>(' ' ^ FLAK_PSWD_A)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_6 = {
		static_cast<uint8_t>('a' + FLAK_PSWD_B), static_cast<uint8_t>('d' + FLAK_PSWD_B),
		static_cast<uint8_t>('i' + FLAK_PSWD_B), static_cast<uint8_t>('p' + FLAK_PSWD_B),
		static_cast<uint8_t>('i' + FLAK_PSWD_B), static_cast<uint8_t>('s' + FLAK_PSWD_B),
		static_cast<uint8_t>('c' + FLAK_PSWD_B), static_cast<uint8_t>('i' + FLAK_PSWD_B)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_7 = {
		static_cast<uint8_t>('n' ^ FLAK_PSWD_C), static_cast<uint8_t>('g' ^ FLAK_PSWD_C),
		static_cast<uint8_t>(' ' ^ FLAK_PSWD_C), static_cast<uint8_t>('e' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('l' ^ FLAK_PSWD_C), static_cast<uint8_t>('i' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('t' ^ FLAK_PSWD_C), static_cast<uint8_t>('.' ^ FLAK_PSWD_C)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_8 = {
		static_cast<uint8_t>(' ' + FLAK_PSWD_D), static_cast<uint8_t>('D' + FLAK_PSWD_D),
		static_cast<uint8_t>('u' + FLAK_PSWD_D), static_cast<uint8_t>('i' + FLAK_PSWD_D),
		static_cast<uint8_t>('s' + FLAK_PSWD_D), static_cast<uint8_t>(' ' + FLAK_PSWD_D),
		static_cast<uint8_t>('v' + FLAK_PSWD_D), static_cast<uint8_t>('u' + FLAK_PSWD_D)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_9 = {
		static_cast<uint8_t>('l' ^ FLAK_PSWD_A), static_cast<uint8_t>('p' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('u' ^ FLAK_PSWD_A), static_cast<uint8_t>('t' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('a' ^ FLAK_PSWD_A), static_cast<uint8_t>('t' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('e' ^ FLAK_PSWD_A), static_cast<uint8_t>(' ' ^ FLAK_PSWD_A)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_10 = {
		static_cast<uint8_t>('m' + FLAK_PSWD_B), static_cast<uint8_t>('i' + FLAK_PSWD_B),
		static_cast<uint8_t>(' ' + FLAK_PSWD_B), static_cast<uint8_t>('r' + FLAK_PSWD_B),
		static_cast<uint8_t>('i' + FLAK_PSWD_B), static_cast<uint8_t>('s' + FLAK_PSWD_B),
		static_cast<uint8_t>('u' + FLAK_PSWD_B), static_cast<uint8_t>('s' + FLAK_PSWD_B)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_11 = {
		static_cast<uint8_t>(',' ^ FLAK_PSWD_C), static_cast<uint8_t>(' ' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('i' ^ FLAK_PSWD_C), static_cast<uint8_t>('d' ^ FLAK_PSWD_C),
		static_cast<uint8_t>(' ' ^ FLAK_PSWD_C), static_cast<uint8_t>('o' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('r' ^ FLAK_PSWD_C), static_cast<uint8_t>('n' ^ FLAK_PSWD_C)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_12 = {
		static_cast<uint8_t>('a' + FLAK_PSWD_D), static_cast<uint8_t>('r' + FLAK_PSWD_D),
		static_cast<uint8_t>('e' + FLAK_PSWD_D), static_cast<uint8_t>(' ' + FLAK_PSWD_D),
		static_cast<uint8_t>('l' + FLAK_PSWD_D), static_cast<uint8_t>('i' + FLAK_PSWD_D),
		static_cast<uint8_t>('g' + FLAK_PSWD_D), static_cast<uint8_t>('u' + FLAK_PSWD_D)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_13 = {
		static_cast<uint8_t>('l' ^ FLAK_PSWD_A), static_cast<uint8_t>('a' ^ FLAK_PSWD_A),
		static_cast<uint8_t>(' ' ^ FLAK_PSWD_A), static_cast<uint8_t>('i' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('a' ^ FLAK_PSWD_A), static_cast<uint8_t>('c' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('u' ^ FLAK_PSWD_A), static_cast<uint8_t>('l' ^ FLAK_PSWD_A)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_14 = {
		static_cast<uint8_t>('i' + FLAK_PSWD_B), static_cast<uint8_t>('s' + FLAK_PSWD_B),
		static_cast<uint8_t>(' ' + FLAK_PSWD_B), static_cast<uint8_t>('v' + FLAK_PSWD_B),
		static_cast<uint8_t>('e' + FLAK_PSWD_B), static_cast<uint8_t>('n' + FLAK_PSWD_B),
		static_cast<uint8_t>('e' + FLAK_PSWD_B), static_cast<uint8_t>('n' + FLAK_PSWD_B)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_15 = {
		static_cast<uint8_t>('a' ^ FLAK_PSWD_C), static_cast<uint8_t>('t' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('i' ^ FLAK_PSWD_C), static_cast<uint8_t>('s' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('.' ^ FLAK_PSWD_C), static_cast<uint8_t>(' ' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('D' ^ FLAK_PSWD_C), static_cast<uint8_t>('o' ^ FLAK_PSWD_C)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_16 = {
		static_cast<uint8_t>('n' + FLAK_PSWD_D), static_cast<uint8_t>('e' + FLAK_PSWD_D),
		static_cast<uint8_t>('c' + FLAK_PSWD_D), static_cast<uint8_t>(' ' + FLAK_PSWD_D),
		static_cast<uint8_t>('i' + FLAK_PSWD_D), static_cast<uint8_t>('n' + FLAK_PSWD_D),
		static_cast<uint8_t>(' ' + FLAK_PSWD_D), static_cast<uint8_t>('p' + FLAK_PSWD_D)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_17 = {
		static_cast<uint8_t>('l' ^ FLAK_PSWD_A), static_cast<uint8_t>('a' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('c' ^ FLAK_PSWD_A), static_cast<uint8_t>('e' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('r' ^ FLAK_PSWD_A), static_cast<uint8_t>('a' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('t' ^ FLAK_PSWD_A), static_cast<uint8_t>(' ' ^ FLAK_PSWD_A)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_18 = {
		static_cast<uint8_t>('e' + FLAK_PSWD_B), static_cast<uint8_t>('n' + FLAK_PSWD_B),
		static_cast<uint8_t>('i' + FLAK_PSWD_B), static_cast<uint8_t>('m' + FLAK_PSWD_B),
		static_cast<uint8_t>('.' + FLAK_PSWD_B), static_cast<uint8_t>(' ' + FLAK_PSWD_B),
		static_cast<uint8_t>('S' + FLAK_PSWD_B), static_cast<uint8_t>('u' + FLAK_PSWD_B)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_19 = {
		static_cast<uint8_t>('s' ^ FLAK_PSWD_C), static_cast<uint8_t>('p' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('e' ^ FLAK_PSWD_C), static_cast<uint8_t>('n' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('d' ^ FLAK_PSWD_C), static_cast<uint8_t>('i' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('s' ^ FLAK_PSWD_C), static_cast<uint8_t>('s' ^ FLAK_PSWD_C)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_20 = {
		static_cast<uint8_t>('e' + FLAK_PSWD_D), static_cast<uint8_t>(' ' + FLAK_PSWD_D),
		static_cast<uint8_t>('f' + FLAK_PSWD_D), static_cast<uint8_t>('r' + FLAK_PSWD_D),
		static_cast<uint8_t>('i' + FLAK_PSWD_D), static_cast<uint8_t>('n' + FLAK_PSWD_D),
		static_cast<uint8_t>('g' + FLAK_PSWD_D), static_cast<uint8_t>('i' + FLAK_PSWD_D)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_21 = {
		static_cast<uint8_t>('l' ^ FLAK_PSWD_A), static_cast<uint8_t>('l' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('a' ^ FLAK_PSWD_A), static_cast<uint8_t>(' ' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('q' ^ FLAK_PSWD_A), static_cast<uint8_t>('u' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('a' ^ FLAK_PSWD_A), static_cast<uint8_t>('m' ^ FLAK_PSWD_A)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_22 = {
		static_cast<uint8_t>(' ' + FLAK_PSWD_B), static_cast<uint8_t>('d' + FLAK_PSWD_B),
		static_cast<uint8_t>('i' + FLAK_PSWD_B), static_cast<uint8_t>('a' + FLAK_PSWD_B),
		static_cast<uint8_t>('m' + FLAK_PSWD_B), static_cast<uint8_t>(',' + FLAK_PSWD_B),
		static_cast<uint8_t>(' ' + FLAK_PSWD_B), static_cast<uint8_t>('v' + FLAK_PSWD_B)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_23 = {
		static_cast<uint8_t>('i' ^ FLAK_PSWD_C), static_cast<uint8_t>('t' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('a' ^ FLAK_PSWD_C), static_cast<uint8_t>('e' ^ FLAK_PSWD_C),
		static_cast<uint8_t>(' ' ^ FLAK_PSWD_C), static_cast<uint8_t>('p' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('o' ^ FLAK_PSWD_C), static_cast<uint8_t>('r' ^ FLAK_PSWD_C)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_24 = {
		static_cast<uint8_t>('t' + FLAK_PSWD_D), static_cast<uint8_t>('i' + FLAK_PSWD_D),
		static_cast<uint8_t>('t' + FLAK_PSWD_D), static_cast<uint8_t>('o' + FLAK_PSWD_D),
		static_cast<uint8_t>('r' + FLAK_PSWD_D), static_cast<uint8_t>(' ' + FLAK_PSWD_D),
		static_cast<uint8_t>('s' + FLAK_PSWD_D), static_cast<uint8_t>('e' + FLAK_PSWD_D)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_25 = {
		static_cast<uint8_t>('m' ^ FLAK_PSWD_A), static_cast<uint8_t>(' ' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('c' ^ FLAK_PSWD_A), static_cast<uint8_t>('o' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('n' ^ FLAK_PSWD_A), static_cast<uint8_t>('d' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('i' ^ FLAK_PSWD_A), static_cast<uint8_t>('m' ^ FLAK_PSWD_A)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_26 = {
		static_cast<uint8_t>('e' + FLAK_PSWD_B), static_cast<uint8_t>('n' + FLAK_PSWD_B),
		static_cast<uint8_t>('t' + FLAK_PSWD_B), static_cast<uint8_t>('u' + FLAK_PSWD_B),
		static_cast<uint8_t>('m' + FLAK_PSWD_B), static_cast<uint8_t>(' ' + FLAK_PSWD_B),
		static_cast<uint8_t>('e' + FLAK_PSWD_B), static_cast<uint8_t>('u' + FLAK_PSWD_B)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_27 = {
		static_cast<uint8_t>('.' ^ FLAK_PSWD_C), static_cast<uint8_t>(' ' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('P' ^ FLAK_PSWD_C), static_cast<uint8_t>('r' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('a' ^ FLAK_PSWD_C), static_cast<uint8_t>('e' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('s' ^ FLAK_PSWD_C), static_cast<uint8_t>('e' ^ FLAK_PSWD_C)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_28 = {
		static_cast<uint8_t>('n' + FLAK_PSWD_D), static_cast<uint8_t>('t' + FLAK_PSWD_D),
		static_cast<uint8_t>(' ' + FLAK_PSWD_D), static_cast<uint8_t>('v' + FLAK_PSWD_D),
		static_cast<uint8_t>('o' + FLAK_PSWD_D), static_cast<uint8_t>('l' + FLAK_PSWD_D),
		static_cast<uint8_t>('u' + FLAK_PSWD_D), static_cast<uint8_t>('t' + FLAK_PSWD_D)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_29 = {
		static_cast<uint8_t>('p' ^ FLAK_PSWD_A), static_cast<uint8_t>('a' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('t' ^ FLAK_PSWD_A), static_cast<uint8_t>(' ' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('n' ^ FLAK_PSWD_A), static_cast<uint8_t>('e' ^ FLAK_PSWD_A),
		static_cast<uint8_t>('c' ^ FLAK_PSWD_A), static_cast<uint8_t>(' ' ^ FLAK_PSWD_A)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_30 = {
		static_cast<uint8_t>('d' + FLAK_PSWD_B), static_cast<uint8_t>('o' + FLAK_PSWD_B),
		static_cast<uint8_t>('l' + FLAK_PSWD_B), static_cast<uint8_t>('o' + FLAK_PSWD_B),
		static_cast<uint8_t>('r' + FLAK_PSWD_B), static_cast<uint8_t>(' ' + FLAK_PSWD_B),
		static_cast<uint8_t>('q' + FLAK_PSWD_B), static_cast<uint8_t>('u' + FLAK_PSWD_B)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_31 = {
		static_cast<uint8_t>('i' ^ FLAK_PSWD_C), static_cast<uint8_t>('s' ^ FLAK_PSWD_C),
		static_cast<uint8_t>(' ' ^ FLAK_PSWD_C), static_cast<uint8_t>('r' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('u' ^ FLAK_PSWD_C), static_cast<uint8_t>('t' ^ FLAK_PSWD_C),
		static_cast<uint8_t>('r' ^ FLAK_PSWD_C), static_cast<uint8_t>('u' ^ FLAK_PSWD_C)
	};
	constexpr std::array<uint8_t, 8> FLAK_PSWD_SEG_32 = {
		static_cast<uint8_t>('m' + FLAK_PSWD_D), static_cast<uint8_t>(' ' + FLAK_PSWD_D),
		static_cast<uint8_t>('o' + FLAK_PSWD_D), static_cast<uint8_t>('d' + FLAK_PSWD_D),
		static_cast<uint8_t>('i' + FLAK_PSWD_D), static_cast<uint8_t>('o' + FLAK_PSWD_D),
		static_cast<uint8_t>('.' + FLAK_PSWD_D), static_cast<uint8_t>('\0' + FLAK_PSWD_D)
	};

	// Decoding using XOR and ADD ciphers
	template <size_t N>
	inline std::string DecodeXOR(const std::array<uint8_t, N>& in_a, uint8_t in_k) {
		std::string s;
		s.reserve(N);

		for (auto b : in_a) {
			s.push_back(static_cast<char>(b ^ in_k));
		}
		return s;
	}
	template <size_t N>
	inline std::string DecodeADD(const std::array<uint8_t, N>& in_a, uint8_t in_k) {
		std::string s;
		s.reserve(N);

		for (auto b : in_a) {
			s.push_back(static_cast<char>(b - in_k));
		}

		return s;
	}

	// Function to get the full decoded password
	inline std::string GetPassword() {
		std::string p; p.reserve(256);

		p += DecodeXOR(FLAK_PSWD_SEG_1, FLAK_PSWD_A);
		p += DecodeADD(FLAK_PSWD_SEG_2, FLAK_PSWD_B);
		p += DecodeXOR(FLAK_PSWD_SEG_3, FLAK_PSWD_C);
		p += DecodeADD(FLAK_PSWD_SEG_4, FLAK_PSWD_D);
		p += DecodeXOR(FLAK_PSWD_SEG_5, FLAK_PSWD_A);
		p += DecodeADD(FLAK_PSWD_SEG_6, FLAK_PSWD_B);
		p += DecodeXOR(FLAK_PSWD_SEG_7, FLAK_PSWD_C);
		p += DecodeADD(FLAK_PSWD_SEG_8, FLAK_PSWD_D);
		p += DecodeXOR(FLAK_PSWD_SEG_9, FLAK_PSWD_A);
		p += DecodeADD(FLAK_PSWD_SEG_10, FLAK_PSWD_B);
		p += DecodeXOR(FLAK_PSWD_SEG_11, FLAK_PSWD_C);
		p += DecodeADD(FLAK_PSWD_SEG_12, FLAK_PSWD_D);
		p += DecodeXOR(FLAK_PSWD_SEG_13, FLAK_PSWD_A);
		p += DecodeADD(FLAK_PSWD_SEG_14, FLAK_PSWD_B);
		p += DecodeXOR(FLAK_PSWD_SEG_15, FLAK_PSWD_C);
		p += DecodeADD(FLAK_PSWD_SEG_16, FLAK_PSWD_D);
		p += DecodeXOR(FLAK_PSWD_SEG_17, FLAK_PSWD_A);
		p += DecodeADD(FLAK_PSWD_SEG_18, FLAK_PSWD_B);
		p += DecodeXOR(FLAK_PSWD_SEG_19, FLAK_PSWD_C);
		p += DecodeADD(FLAK_PSWD_SEG_20, FLAK_PSWD_D);
		p += DecodeXOR(FLAK_PSWD_SEG_21, FLAK_PSWD_A);
		p += DecodeADD(FLAK_PSWD_SEG_22, FLAK_PSWD_B);
		p += DecodeXOR(FLAK_PSWD_SEG_23, FLAK_PSWD_C);
		p += DecodeADD(FLAK_PSWD_SEG_24, FLAK_PSWD_D);
		p += DecodeXOR(FLAK_PSWD_SEG_25, FLAK_PSWD_A);
		p += DecodeADD(FLAK_PSWD_SEG_26, FLAK_PSWD_B);
		p += DecodeXOR(FLAK_PSWD_SEG_27, FLAK_PSWD_C);
		p += DecodeADD(FLAK_PSWD_SEG_28, FLAK_PSWD_D);
		p += DecodeXOR(FLAK_PSWD_SEG_29, FLAK_PSWD_A);
		p += DecodeADD(FLAK_PSWD_SEG_30, FLAK_PSWD_B);
		p += DecodeXOR(FLAK_PSWD_SEG_31, FLAK_PSWD_C);
		p += DecodeADD(FLAK_PSWD_SEG_32, FLAK_PSWD_D);

		return p;
	}

} // namespace flakpak::encryption

#endif // FLAK_PASSWORD_HANDLER_HPP