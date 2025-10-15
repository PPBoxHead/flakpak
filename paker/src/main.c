#include <stdio.h>
#include <stdlib.h>

#include <argtable3/argtable3.h>
#include <microlog/ulog.h>

#include <flakpak-c/flak_packer.h>

struct arg_str* input_dir;
struct arg_str* output_file;
struct arg_lit* compression_enabled;
struct arg_lit* encryption_enabled;
struct arg_int* compression_level;
struct arg_int* content_version;
struct arg_end* end;

int main (int argc, char* argv[]) {

	void* argtable[] = {
		input_dir			= arg_str1("i", "input", "<input_dir>", "Input directory folder to pack"),
		output_file			= arg_str1("o", "output", "<output>", "Output .flk file"),
		compression_enabled = arg_lit0("c", "compress-enabled", "Enable .flk data compression"),
		encryption_enabled	= arg_lit0("e", "encrypt-enabled", "Enable .flk data encryption using XChaCha20-Poly1305 encryption and Argon2 key derivation"),
		compression_level	= arg_int0("l", "compress-level", "<level>", "Compression level (1-22 for Zstd, default: 3)"),
		content_version		= arg_int0("v", "content-version", "<ver>", "Custom content version number"),
		end = arg_end(20)
	};

	// Set default values
	compression_level->ival[0] = 3; // Default compression level
	content_version->ival[0] = 1; // Default content version


	int nerrors = arg_parse(argc, argv, argtable);
	if (nerrors != 0) {
		ulog_error("Error parsing command line arguments");
		arg_print_errors(stderr, end, argv[0]);

		ulog_info("Usage: %s", argv[0]);
		arg_print_syntax(stderr, argtable, "\n");

		ulog_info("Options:");
		arg_print_glossary(stderr, argtable, "  %-25s %s\n");

		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		exit(EXIT_FAILURE);
	}

	// Example access to argument values
	const char* inputDir = input_dir->sval[0];
	const char* outputPath = output_file->sval[0];
	int compLevel = compression_level->count > 0 ? compression_level->ival[0] : 3;
	bool useCompression = compression_enabled->count > 0;
	bool useEncryption = encryption_enabled->count > 0;
	int version = content_version->count > 0 ? content_version->ival[0] : 1;

	bool result = FLAK_pack_files(inputDir, outputPath,(FLK_file_flags)((useCompression ? FLK_FLAG_COMPRESSED : 0) | (useEncryption ? FLK_FLAG_ENCRYPTED : 0) | (version << 16)), compLevel);
	if (!result) {
		ulog_error("Packing failed");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		exit(EXIT_FAILURE);
	}

	ulog_info("Input Directory: %s", inputDir);
	ulog_info("Output File: %s", outputPath);
	ulog_info("Compression Level: %d", compLevel);
	ulog_info("Compression Enabled: %s", useCompression ? "true" : "false");
	ulog_info("Encryption Enabled: %s", useEncryption ? "true" : "false");
	ulog_info("Content Version: %d", version);

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	ulog_cleanup();
	exit(EXIT_SUCCESS);
}