#include <flakpak/flak_PathCompressor.hpp>


namespace flakpak::pathcom {
	std::string PathCompressor::CompressPath(const std::string& in_filePath) {
        std::string compressed = in_filePath;

        for (const auto& [original, replacement] : GetSubstitutions()) {
            size_t pos = 0;
            while ((pos = compressed.find(original, pos)) != std::string::npos) {
                compressed.replace(pos, original.length(), 1, replacement);
                pos += 1;
            }
        }

        return compressed;
	}
	std::string PathCompressor::DecompressPath(const std::string& in_compressedPath) {
        std::string decompressed = in_compressedPath;

        // Reverse the substitutions
        for (const auto& [original, replacement] : GetSubstitutions()) {
            size_t pos = 0;
            while ((pos = decompressed.find(replacement, pos)) != std::string::npos) {
                decompressed.replace(pos, 1, original);
                pos += original.length();
            }
        }

        return decompressed;
	}

	const std::vector<std::pair<std::string, char>>& PathCompressor::GetSubstitutions() {
        static const std::vector<std::pair<std::string, char>> substitutions = {
            // Directories
            {"textures/", '\x01'},
            {"sounds/", '\x02'},
            {"models/", '\x03'},
            {"shaders/", '\x04'},
            {"materials/", '\x06'},
            {"animations/", '\x07'},
            {"ui/", '\x08'},
            {"fonts/", '\x09'},
            {"config/", '\x0A'},
            {"levels/", '\x0B'},
            {"effects/", '\x0C'},

            // Texture file formats
            {".png", '\x0D'},
            {".dds", '\x0E'},

            // Sound file formats
            {".wav", '\x0F'},
            {".ogg", '\x10'},

            // Shader file formats
            {".geo", '\x11'},
            {".glsl", '\x12'},
            {".hlsl", '\x13'},
            {".vert", '\x14'},
            {".frag", '\x15'},
            {".comp", '\x16'},

            // Model file formats
            {".obj", '\x17'},
            {".gltf", '\x18'},
			{".glb", '\x19'},

        };

        return substitutions;
	}
} // namespace flakpak::pathcom