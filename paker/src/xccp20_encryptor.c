#include <flakpak-c/xccp20_encryptor.h>

#include <string.h>

#include <libsodium/sodium.h>
#include <microlog/ulog.h>

static void derive_key(const char* in_password, uint8_t* in_salt, unsigned char* out_key);

FLAK_ENCRYPTION_RESULT FLAK_xccp20_encrypt_data(const char* in_file_name, const uint8_t* in_data, size_t in_data_size, const char* in_password) {
	// Generate random salt for key derivation
	FLAK_ENCRYPTION_RESULT encryption_result = { 0 };
	randombytes_buf(encryption_result.salt, 16);

	// Derive key from password and salt
	unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
	derive_key(in_password, encryption_result.salt, key);

	randombytes_buf(encryption_result.nonce, crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);

	// Encrypt data
	/// TODO
	/// Allocate memory for ciphertext and handle encryption using memory arena or dynamic array
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

	/// TODO
	/// Reallocate memory for ciphertext and handle encryption using memory arena or dynamic array
	uint8_t* temp = (uint8_t*)realloc(ciphertext, ciphertext_len);
	if (temp == NULL) {
		ulog_fatal("XCCP20: Memory reallocation failed for file %s", in_file_name);
		free(ciphertext);
		memset(&encryption_result, 0, sizeof(FLAK_ENCRYPTION_RESULT));
		return encryption_result;
	}
	ciphertext = temp;

	// Prepend nonce to ciphertext for storage/transmission
	/// TODO
	/// Allocate memory for final encrypted data and handle using memory arena or dynamic array
	size_t nonce_size = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
	size_t total_size = nonce_size + ciphertext_len;

	encryption_result.data = (uint8_t*)malloc(total_size);
	if (encryption_result.data == NULL) {
		ulog_fatal("XCCP20: Memory allocation failed for final encrypted data for file %s", in_file_name);
		free(ciphertext);
		memset(&encryption_result, 0, sizeof(FLAK_ENCRYPTION_RESULT));
		return encryption_result;
	}

	// Copy nonce and ciphertext
	if (total_size >= nonce_size && total_size >= (nonce_size + ciphertext_len)) {
		memcpy(encryption_result.data, encryption_result.nonce, nonce_size);
		memcpy(encryption_result.data + nonce_size, ciphertext, ciphertext_len);
	}

	encryption_result.data_size = total_size;

	free(ciphertext);

	return encryption_result;
}

void derive_key(const char* in_password, uint8_t* in_salt, unsigned char* out_key) {
	int result = crypto_pwhash(
		out_key,
		crypto_aead_xchacha20poly1305_ietf_KEYBYTES,
		in_password, strlen(in_password),
		(unsigned char*)in_salt,
		crypto_pwhash_OPSLIMIT_MODERATE,
		crypto_pwhash_MEMLIMIT_MODERATE,
		crypto_pwhash_ALG_ARGON2ID13
	);
}
