#include <flakpak/flak_PasswordHandler.hpp>
#include <flakpak/flak_FLKDefinition.hpp>
#include <flakpak/flak_FLKPacker.hpp>

#include <flakpak/zstd_Compressor.hpp>
#include <flakpak/xccp20_Encryptor.hpp>

#include <CLI11/CLI11.hpp>

// STD C++ Libraries
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <iomanip>
#include <memory>


namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    // --- CLI11 SETUP ---
    CLI::App app{ "PAK file packer tool" };

    fs::path inputDir;
    fs::path outPath;
    int compressionLevel = 3;
    uint32_t contentVersion = 0;
    bool useCompression = false;
    bool useEncryption = false;

    app.add_option("input_dir", inputDir, "Input directory to pack")
        ->required()->check(CLI::ExistingDirectory);

    app.add_option("output", outPath, "Output .flk file")->required();

    app.add_option("-c,--compression", compressionLevel,
        "Compression level (1-22 for Zstd)")->default_val(3);

    app.add_flag("--compress", useCompression, "Enable compression");
    app.add_flag("--encrypt", useEncryption, "Enable encryption");

    app.add_option("--content-version", contentVersion,
        "Custom content version number")->default_val(0);

    CLI11_PARSE(app, argc, argv);

    // Call appropriate packing method based on flags
    bool success = false;

    if (!useCompression && !useEncryption) {
        std::cout << "Mode: Uncompressed + Unencrypted\n";
        success = flakpak::FLKPacker::PackUncompressedAndUnencrypted(inputDir, outPath);
    }
    else if (useCompression && !useEncryption) {
        std::cout << "Mode: Compressed + Unencrypted (level " << compressionLevel << ")\n";
        success = flakpak::FLKPacker::PackCompressedAndUnencrypted(inputDir, outPath, compressionLevel);
    }
    else if (!useCompression && useEncryption) {
        std::cout << "Mode: Uncompressed + Encrypted\n";
        success = flakpak::FLKPacker::PackUncompressedAndEncrypted(inputDir, outPath);
    }
    else { // useCompression && useEncryption
        std::cout << "Mode: Compressed + Encrypted (level " << compressionLevel << ")\n";
        success = flakpak::FLKPacker::PackCompressedAndEncrypted(inputDir, outPath, compressionLevel);
    }

    if (!success) {
        std::cerr << "Packing failed!\n";
        return 1;
    }

    std::cout << "Packing completed successfully!\n";
    exit(EXIT_SUCCESS);
}