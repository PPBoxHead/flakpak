#include <vfspp/VFS.h>
#include "FLKFileSystem.h"

#include <iostream>
#include <vector>

// stb_image for loading textures
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// libsodium for encryption (if using encrypted FLK files)
#include <libsodium/sodium.h>

using namespace vfspp;

struct Texture {
    unsigned char* data = nullptr;
    int width = 0;
    int height = 0;
    int channels = 0;

    ~Texture() {
        if (data) {
            stbi_image_free(data);
        }
    }
};

// Load texture from FLK virtual filesystem
bool LoadTextureFromVFS(VirtualFileSystem& vfs, const std::string& texturePath, Texture& outTexture) {
    // Open file from virtual filesystem
    IFilePtr file = vfs.OpenFile(FileInfo(texturePath), IFile::FileMode::Read);
    if (!file || !file->IsOpened()) {
        std::cerr << "Failed to open file: " << texturePath << std::endl;
        return false;
    }

    // Read entire file into memory
    uint64_t fileSize = file->Size();
    std::vector<uint8_t> fileData;
    fileData.resize(fileSize);

    uint64_t bytesRead = file->Read(fileData.data(), fileSize);
    file->Close();

    if (bytesRead != fileSize) {
        std::cerr << "Failed to read complete file: " << texturePath << std::endl;
        return false;
    }

    // Load image using stb_image from memory
    outTexture.data = stbi_load_from_memory(
        fileData.data(),
        static_cast<int>(fileData.size()),
        &outTexture.width,
        &outTexture.height,
        &outTexture.channels,
        0  // desired_channels (0 = keep original)
    );

    if (!outTexture.data) {
        std::cerr << "Failed to decode image: " << texturePath << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
        return false;
    }

    std::cout << "Loaded texture: " << texturePath << std::endl;
    std::cout << "  Size: " << outTexture.width << "x" << outTexture.height << std::endl;
    std::cout << "  Channels: " << outTexture.channels << std::endl;

    return true;
}

int main(int argc, char** argv) {
    // Initialize libsodium (required if FLK file is encrypted)
    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium" << std::endl;
        return 1;
    }

    std::cout << "Initializing Virtual File System..." << std::endl;

    // Create and mount FLK filesystem
    FLKFileSystemPtr flkFS = std::make_shared<FLKFileSystem>("resources_FS_C22EN.flk");
    flkFS->Initialize();

    if (!flkFS->IsInitialized()) {
        std::cerr << "Failed to initialize FLK filesystem" << std::endl;
        return 1;
    }

    // Create virtual filesystem and mount FLK at root
    VirtualFileSystem vfs;
    vfs.AddFileSystem("/", flkFS);

    std::cout << "FLK filesystem mounted successfully" << std::endl;

    // List all files in the archive
    std::cout << "\nFiles in archive:" << std::endl;
    const auto& fileList = flkFS->FileList();
    for (const auto& [path, file] : fileList) {
        std::cout << "  " << path << " (" << file->Size() << " bytes)" << std::endl;
    }

    // Load a texture
    std::cout << "\n--- Loading Texture ---" << std::endl;
    Texture heroTexture;
    if (!LoadTextureFromVFS(vfs, "/textures/gpoo_tex/gpoo_01_tex.png", heroTexture)) {
        std::cerr << "Failed to load hero texture" << std::endl;
        return 1;
    }

    // Example: Access pixel data
    if (heroTexture.data) {
        std::cout << "\nFirst pixel RGB values: ";
        std::cout << "R=" << (int)heroTexture.data[0] << " ";
        std::cout << "G=" << (int)heroTexture.data[1] << " ";
        std::cout << "B=" << (int)heroTexture.data[2] << " ";
		std::cout << "A=" << (heroTexture.channels == 4 ? (int)heroTexture.data[3] : 1) << std::endl;
    }

    // Example: Calculate memory usage
    size_t totalTextureMemory = 0;
    if (heroTexture.data) {
        totalTextureMemory += heroTexture.width * heroTexture.height * heroTexture.channels;
    }

    std::cout << "\nTotal texture memory: " << totalTextureMemory << " bytes" << std::endl;

    // Textures will be automatically freed by Texture destructor
    std::cout << "\nProgram completed successfully" << std::endl;

    return 0;
}