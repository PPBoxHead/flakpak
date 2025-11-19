#include <flakpak-c/xccp20_encryptor.h>
#include <flakpak-c/flak_arena.h>

#include <string.h>

#include <libsodium/sodium.h>
#include <microlog/ulog.h>

static bool derive_key(const char* in_password, uint8_t* in_salt, unsigned char* out_key);

FLAK_ENCRYPTION_RESULT FLAK_xccp20_encrypt_data(const char* in_file_name, const uint8_t* in_data, size_t in_data_size, const char* in_password) {
	// Generate random salt for key derivation
	FLAK_ENCRYPTION_RESULT encryption_result = { 0 };
	const size_t salt_size = 16;
	randombytes_buf(encryption_result.salt, salt_size);

	// Derive key from password and salt
	unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
	if (!derive_key(in_password, encryption_result.salt, key)) {
		ulog_error("XCCP20: Key derivation failed for file %s", in_file_name);
		memset(&encryption_result, 0, sizeof(FLAK_ENCRYPTION_RESULT));
		return encryption_result;
	}

	randombytes_buf(encryption_result.nonce, crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);

	// Encrypt data
	uint8_t* ciphertext = (uint8_t*)malloc(in_data_size + crypto_aead_xchacha20poly1305_ietf_ABYTES);
	if (ciphertext == NULL) {
		ulog_fatal("XCCP20: Memory allocation failed for file %s", in_file_name);
		memset(&encryption_result, 0, sizeof(FLAK_ENCRYPTION_RESULT));
		return encryption_result;
	}

	unsigned long long ciphertext_len = 0;

	int result = crypto_aead_xchacha20poly1305_ietf_encrypt(
		ciphertext, &ciphertext_len,
		in_data, in_data_size,
		NULL, 0, NULL,
		encryption_result.nonce, key
	);
	if (result != 0) {
		ulog_error("XCCP20: Encryption failed for file %s", in_file_name);
		free(ciphertext);
		memset(&encryption_result, 0, sizeof(FLAK_ENCRYPTION_RESULT));
		return encryption_result;
	}

	// shrink ciphertext buffer if needed
	uint8_t* temp = (uint8_t*)realloc(ciphertext, (size_t)ciphertext_len);
	if (temp != NULL) {
		ciphertext = temp;
	}

	// Build output: [salt | nonce | ciphertext]
	size_t nonce_size = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
	size_t total_size = salt_size + nonce_size + (size_t)ciphertext_len;

	encryption_result.data = (uint8_t*)malloc(total_size);
	if (encryption_result.data == NULL) {
		ulog_fatal("XCCP20: Memory allocation failed for final encrypted data for file %s", in_file_name);
		free(ciphertext);
		sodium_memzero(key, sizeof key);
		memset(&encryption_result, 0, sizeof(FLAK_ENCRYPTION_RESULT));
		return encryption_result;
	}

	// Copy salt, nonce and ciphertext
	if (encryption_result.data != NULL && total_size >= (salt_size + nonce_size + ciphertext_len)) {
		memcpy(encryption_result.data, encryption_result.salt, salt_size);
		memcpy(encryption_result.data + salt_size, encryption_result.nonce, nonce_size);
		memcpy(encryption_result.data + salt_size + nonce_size, ciphertext, ciphertext_len);
		encryption_result.data_size = total_size;
	} else {
		ulog_fatal("XCCP20: Buffer overrun risk detected for file %s", in_file_name);
		free(ciphertext);
		sodium_memzero(key, sizeof key);
		memset(&encryption_result, 0, sizeof(FLAK_ENCRYPTION_RESULT));
		return encryption_result;
	}

	// Clean up
	sodium_memzero(key, sizeof key);
	free(ciphertext);

	return encryption_result;
}

FLAK_DECRYPTION_RESULT FLAK_xccp20_decrypt_data(const char* in_file_name, const uint8_t* in_data, size_t in_data_size, const char* in_password) {
	FLAK_DECRYPTION_RESULT decryption_result = { 0 };

	const size_t salt_size = 16;
	const size_t nonce_size = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
	const size_t min_size = salt_size + nonce_size + crypto_aead_xchacha20poly1305_ietf_ABYTES;

	if (in_data_size < min_size) {
		ulog_error("XCCP20: Encrypted data too short for file %s", in_file_name);
		return decryption_result;
	}

	// Extract salt, nonce and ciphertext from input
	const unsigned char* salt = in_data;
	const unsigned char* nonce = in_data + salt_size;
	const unsigned char* ciphertext = in_data + salt_size + nonce_size;
	size_t ciphertext_len = in_data_size - salt_size - nonce_size;

	// Derive key from provided salt
	unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
	if (!derive_key(in_password, (uint8_t*)salt, key)) {
		ulog_error("XCCP20: Key derivation failed for file %s", in_file_name);
		sodium_memzero(key, sizeof key);
		return decryption_result;
	}

	if (ciphertext_len < crypto_aead_xchacha20poly1305_ietf_ABYTES) {
		ulog_error("XCCP20: Ciphertext too short for file %s", in_file_name);
		sodium_memzero(key, sizeof key);
		return decryption_result;
	}

	size_t decrypted_size = ciphertext_len - crypto_aead_xchacha20poly1305_ietf_ABYTES;
	uint8_t* decrypted_data = (uint8_t*)malloc(decrypted_size);
	if (decrypted_data == NULL) {
		ulog_fatal("XCCP20: Memory allocation failed for decryption for file %s", in_file_name);
		sodium_memzero(key, sizeof key);
		return decryption_result;
	}

	unsigned long long decrypted_len = 0;
	int result = crypto_aead_xchacha20poly1305_ietf_decrypt(
		decrypted_data, &decrypted_len,
		NULL,
		ciphertext, ciphertext_len,
		NULL, 0,
		nonce, key
	);

	// Always zero key material after use
	sodium_memzero(key, sizeof key);

	if (result != 0) {
		ulog_error("XCCP20: Decryption failed or data is tampered for file %s", in_file_name);
		free(decrypted_data);
		return decryption_result;
	}

	decryption_result.data = decrypted_data;
	decryption_result.data_size = decrypted_len;

	return decryption_result;
}

bool derive_key(const char* in_password, uint8_t* in_salt, unsigned char* out_key) {
	if (!in_password || !in_salt || !out_key) {
		ulog_error("Invalid parameters to derive_key");
		return false;
	}

	size_t password_len = strlen(in_password);
	if (password_len == 0) {
		ulog_error("Empty password provided");
		return false;
	}

	int result = crypto_pwhash(
		out_key,
		crypto_aead_xchacha20poly1305_ietf_KEYBYTES,
		in_password, strlen(in_password),
		(unsigned char*)in_salt,
		crypto_pwhash_OPSLIMIT_MODERATE,
		crypto_pwhash_MEMLIMIT_MODERATE,
		crypto_pwhash_ALG_ARGON2ID13
	);

	if (result != 0) {
		ulog_error("Key derivation failed with code %d", result);
		return false;
	}

	return true;
}
