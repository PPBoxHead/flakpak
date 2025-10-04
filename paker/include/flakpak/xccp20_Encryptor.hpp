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
// File: [xccp20_Encryptor.hpp - xccp20_Encryptor.cpp]
// 
// Description: Interface for XChaCha20-Poly1305 encryption method
// 
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 30.08.2025
// Version: 1.1.0
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <flak_FLKDefinition.hpp> - flakpak API data types
// 
//	- <iostream> - C++ Standard Library
//  - <vector>  - C++ Standard Library
//  - <cstdint> - C++ Standard Library
//  - <string>	- C++ Standard Library
// 
//  - <libsodium> - For XChaCha20-Poly1305 encryption and Argon2 key derivation
// 
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAK_XCPP20_ENCRYPTOR_HPP
#define FLAK_XCPP20_ENCRYPTOR_HPP

#include <flakpak/flak_FLKDefinition.hpp>

#include <vector>
#include <cstdint>
#include <string>


namespace flakpak::encryption::xccp20 {
	class XChaCha20Poly1305Encryptor final {
	public:
		XChaCha20Poly1305Encryptor() = default;
		~XChaCha20Poly1305Encryptor() = default;

		// Encrypts the input data using the provided password
		//    @param in_data		 - The plaintext data to encrypt
		//	  @param in_password	 - The password used for encryption
		//	  
		//    @return FLK_ENCRYPTION_RESULT - structure containing encrypted data and salt
		data_types::FLK_ENCRYPTION_RESULT EncryptData(const std::vector<uint8_t>& in_data,
			const std::string& in_password);
		// Decrypts the input encrypted data using the provided password and salt
		//    @param in_encryptedData	 - The encrypted data to decrypt
		//	  @param in_salt			 - The salt used for key derivation
		//	  @param in_password		 - The password used for decryption
		//    
		//	  @return std::vector<uint8_t> - The decrypted plaintext data
		std::vector<uint8_t> DecryptData(const std::vector<uint8_t>& in_encryptedData,
			const std::vector<uint8_t>& salt,
			const std::string& in_password);

		[[nodiscard]] size_t GetNonceSize() const;
		[[nodiscard]] size_t GetMacSize() const;

	private:
		// Derives a key from the given password and salt using Argon2id
		//    @param in_password	 - The password from which to derive the key
		//	  @param in_salt		 - The salt used in the key derivation process
		//    @param out_key		 - The output buffer where the derived key will be stored
		void DeriveKey(const std::string& in_password,
			const std::vector<uint8_t>& in_salt,
			unsigned char* out_key);

	}; // class XChaCha20Poly1305Encryptor final

} // namespace flakpak::encryption::xccp20

#endif // !FLAK_XCPP20_ENCRYPTOR_HPP