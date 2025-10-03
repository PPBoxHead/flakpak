#include <flakpak/zstd_Compressor.hpp>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <zstd/zstd.h>


using namespace flakpak::data_types;

namespace flakpak::compression::zstd {
	FLK_COMPRESSION_RESULT ZstdCompressor::CompressData(const std::filesystem::path& in_path,
		int in_compressionLevel) {
		// Open the file to compress
		std::ifstream file(in_path, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			/// TODO
			/// Failed to open file error 
			/// Output to console
			std::cout << "Error: Failed to open file: " << in_path << "\n";
			return {};
		}

		// Get original file size
		size_t originalSize = file.tellg();
		file.seekg(0, std::ios::beg);

		const size_t chunkSize = ZSTD_CStreamInSize();
		const size_t outChunkSize = ZSTD_CStreamOutSize();


		// Initialize ZSTD compression context
		ZSTD_CCtx* cctx = ZSTD_createCCtx();
		size_t ret = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, in_compressionLevel);
		if (ZSTD_isError(ret)) {
			ZSTD_freeCCtx(cctx);
			/// TODO
			/// Handle error setting compression level
			/// Output to console
			std::cout << "Error: Failed to set compression level.\n";
			return {};
		}

		// Buffers for input and output
		std::vector<uint8_t> compressed;
		std::vector<char> inBuffer(chunkSize);
		std::vector<char> outBuffer(outChunkSize);

		ZSTD_inBuffer input = { nullptr, 0, 0 };
		ZSTD_outBuffer output = { outBuffer.data(), outChunkSize, 0 };

		size_t totalBytesRead = 0;
		std::string filename = in_path.filename().string();

		// Read and compress the file in chunks
		while (file) {
			file.read(inBuffer.data(), chunkSize);
			size_t bytesRead = file.gcount();
			if (bytesRead == 0) break;  // End of file

			totalBytesRead += bytesRead;
			input.src = inBuffer.data();
			input.size = bytesRead;
			input.pos = 0;
			while (input.pos < input.size) {
				output.pos = 0;
				size_t ret = ZSTD_compressStream(cctx, &output, &input);
				if (ZSTD_isError(ret)) {
					ZSTD_freeCCtx(cctx);
					/// TODO
					/// Handle compression error
					/// Output to console
					std::cout << "Error: Compression failed for file: " << filename << "\n";
					return {};
				}
				compressed.insert(compressed.end(), outBuffer.data(), outBuffer.data() + output.pos);
				// Update progress display here if needed
			}
		}

		// End the stream
		output.pos = 0;
		size_t ret2;
		do {
			ret2 = ZSTD_endStream(cctx, &output);
			if (ZSTD_isError(ret2)) {
				ZSTD_freeCCtx(cctx);
				/// TODO
				/// Handle end stream error
				/// Output to console
				std::cout << "Error: Failed to finalize compression for file: " << filename << "\n";
				return {};
			}
			compressed.insert(compressed.end(), outBuffer.data(), outBuffer.data() + output.pos);
			output.pos = 0;
		} while (ret2 != 0);

		ZSTD_freeCCtx(cctx);
		file.close();

		// Return the compressed data and sizes
		FLK_COMPRESSION_RESULT compressionResult;
		compressionResult.data = std::move(compressed);
		compressionResult.originalSize = originalSize;
		compressionResult.compressedSize = compressionResult.data.size();

		return compressionResult;
	}

	std::vector<uint8_t> ZstdCompressor::DecompressData(const std::vector<uint8_t>& in_data,
		size_t in_originalSize) {
		// Buffer to hold decompressed data
		std::vector<uint8_t> decompressedData(in_originalSize);

		size_t const dSize = ZSTD_decompress(decompressedData.data(), in_originalSize,
			in_data.data(), in_data.size());

		if (ZSTD_isError(dSize)) {
			/// TODO
			/// Handle decompression error
			/// Output to console
			return std::vector<uint8_t>();
		}

		decompressedData.resize(dSize);

		return decompressedData;
	}

} // namespace flakpak::compression::zstd