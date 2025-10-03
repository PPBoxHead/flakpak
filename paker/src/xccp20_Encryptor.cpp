#include <flakpak/xccp20_Encryptor.hpp>

#include <libsodium/sodium.h>
#include <iostream>


using namespace flakpak::data_types;

namespace flakpak::encryption::xccp20 {
	// Public methods
	// ---------------------------------------------------------------------------
	FLK_ENCRYPTION_RESULT XChaCha20Poly1305Encryptor::EncryptData(const std::vector<uint8_t>& in_data,
		const std::string& in_password) {
		// Generate random salt for key derivation
		std::vector<uint8_t> salt(16);
		randombytes_buf(salt.data(), salt.size());

		// Derive key from password and salt
		unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
		DeriveKey(in_password, salt, key);

		// Generate random nonce
		std::vector<uint8_t> nonce(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
		randombytes_buf(nonce.data(), nonce.size());

		// Encrypt data
		std::vector<uint8_t> ciphertext(in_data.size() + crypto_aead_xchacha20poly1305_ietf_ABYTES);
		unsigned long long ciphertextLen = 0;

		int result = crypto_aead_xchacha20poly1305_ietf_encrypt(
			ciphertext.data(), &ciphertextLen,
			in_data.data(), in_data.size(),
			nullptr, 0, nullptr,
			nonce.data(), key
		);
		if (result != 0) {
			/// TODO
			/// Handle encryption error 
			/// Output to console and close encryption process
			std::cout << "Error: Encryption failed.\n";
			return {};
		}

		ciphertext.resize(ciphertextLen);

		// Prepend nonce to ciphertext for storage/transmission
		std::vector<uint8_t> encryptedData;
		encryptedData.reserve(nonce.size() + ciphertext.size());
		encryptedData.insert(encryptedData.end(), nonce.begin(), nonce.end());
		encryptedData.insert(encryptedData.end(), ciphertext.begin(), ciphertext.end());

		FLK_ENCRYPTION_RESULT encryptionResult;
		encryptionResult.data = std::move(encryptedData);
		encryptionResult.salt = std::move(salt);
		encryptionResult.nonce = std::move(nonce);

		return encryptionResult;
	}

	std::vector<uint8_t> XChaCha20Poly1305Encryptor::DecryptData(const std::vector<uint8_t>& in_encryptedData,
		const std::vector<uint8_t>& in_salt,
		const std::string& in_password) {
		if (in_encryptedData.size() < crypto_aead_xchacha20poly1305_ietf_NPUBBYTES + crypto_aead_xchacha20poly1305_ietf_ABYTES) {
			/// TODO
			/// Handle error: Encrypted data too short
			/// Output to console and close decryption process
			std::cout << "Error: Encrypted data too short.\n";
			return std::vector<uint8_t>();
		}

		// Derive key from password and salt
		unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
		DeriveKey(in_password, in_salt, key);

		// Extract nonce and ciphertext
		const unsigned char* nonce = in_encryptedData.data();
		const unsigned char* ciphertext = in_encryptedData.data() + crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
		size_t ciphertextLen = in_encryptedData.size() - crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
		
		// Decrypt
		std::vector<uint8_t> decryptedData(ciphertextLen - crypto_aead_xchacha20poly1305_ietf_ABYTES);
		unsigned long long decryptedLen = 0;
		int result = crypto_aead_xchacha20poly1305_ietf_decrypt(
			decryptedData.data(), &decryptedLen,
			nullptr,
			ciphertext, ciphertextLen,
			nullptr, 0,
			nonce, key
		);

		if (result != 0) {
			/// TODO
			/// Handle decryption error (e.g., authentication failure)
			/// Output to console and close decryption process
			std::cout << "Error: Decryption failed or data is tampered.\n";
			return std::vector<uint8_t>();
		}

		decryptedData.resize(decryptedLen);
		return decryptedData;
	}

	size_t XChaCha20Poly1305Encryptor::GetNonceSize() const {
		return crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
	}
	size_t XChaCha20Poly1305Encryptor::GetMacSize() const {
		return crypto_aead_xchacha20poly1305_ietf_ABYTES;
	}

	// Private methods
	// ---------------------------------------------------------------------------
	void XChaCha20Poly1305Encryptor::DeriveKey(const std::string& in_password,
		const std::vector<uint8_t>& in_salt, unsigned char* out_key)
	{
		int result = crypto_pwhash(
			out_key,
			crypto_aead_xchacha20poly1305_ietf_KEYBYTES,
			in_password.c_str(), in_password.size(),
			in_salt.data(),
			crypto_pwhash_OPSLIMIT_MODERATE,
			crypto_pwhash_MEMLIMIT_MODERATE,
			crypto_pwhash_ALG_ARGON2ID13
		);
	}

} // namespace flakpak::encryption