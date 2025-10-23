#include <flakpak-c/zstd_compressor.h>
#include <flakpak-c/flak_arena.h>

#include <stdlib.h>
#include <string.h>

#include <zstd/zstd.h>
#include <microlog/ulog.h>


// Append function
static bool append_compressed(uint8_t** compressed_data, size_t* compressed_size,
	void* src, size_t src_size, const char* file_name) {
	/// TODO
	/// Append output.src[0..output.pos] to compressed_data using a memory arena or dynamic array in C
	size_t prev_size = *compressed_data ? *compressed_size : 0;
	uint8_t* temp = (uint8_t*)realloc(*compressed_data, prev_size + src_size);
	if (!temp) {
		free(*compressed_data);
		ulog_fatal("ZSTD: Memory allocation error during compression for file %s", file_name);
		return false;
	}
	*compressed_data = temp;
	memcpy(*compressed_data + prev_size, src, src_size);
	*compressed_size += src_size;
	return true;
}


FLAK_COMPRESSION_RESULT FLAK_zstd_compress_data(const char* in_file_name, const uint8_t* in_data, size_t in_data_size, int in_comp_level) {
	FLAK_COMPRESSION_RESULT compression_result = { 0 };

	const size_t out_chunk_size = ZSTD_CStreamOutSize();
	const size_t chunk_size = ZSTD_CStreamInSize();
	// Initialize ZSTD compression context
	ZSTD_CCtx* cctx = ZSTD_createCCtx();
	size_t ret = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, in_comp_level);
	if (ZSTD_isError(ret)) {
		ZSTD_freeCCtx(cctx);
		ulog_error("ZSTD: Error setting compression level for file %s: %s", in_file_name, ZSTD_getErrorName(ret));
		return compression_result;
	}

	// Buffers for input and output
	uint8_t* compressed_data = NULL;
	size_t compressed_size = 0;
	char* in_buffer = (char*)malloc(chunk_size);
	char* out_buffer = (char*)malloc(out_chunk_size);

	ZSTD_inBuffer zstd_input = { NULL, 0, 0 };
	ZSTD_outBuffer zstd_output = { out_buffer, out_chunk_size, 0 };

	size_t total_bytes_read = 0;
	while (total_bytes_read < in_data_size) {
		size_t to_read = (in_data_size - total_bytes_read) < chunk_size ? (in_data_size - total_bytes_read) : chunk_size;
		zstd_input.src = in_data + total_bytes_read;
		zstd_input.size = to_read;
		zstd_input.pos = 0;
		total_bytes_read += to_read;
		while (zstd_input.pos < zstd_input.size) {
			zstd_output.pos = 0;
			size_t ret = ZSTD_compressStream(cctx, &zstd_output, &zstd_input);
			if (ZSTD_isError(ret)) {
				ZSTD_freeCCtx(cctx);
				ulog_error("ZSTD: Compression error for file %s: %s", in_file_name, ZSTD_getErrorName(ret));
				return compression_result;
			}
			if (!append_compressed(&compressed_data, &compressed_size, zstd_output.dst, zstd_output.pos, in_file_name)) {
				return compression_result;
			}
		}
	}

	// End the stream
	zstd_output.pos = 0;
	size_t ret2;
	do {
		ret2 = ZSTD_endStream(cctx, &zstd_output);
		if (ZSTD_isError(ret2)) {
			ZSTD_freeCCtx(cctx);
			ulog_error("ZSTD: End stream error for file %s: %s", in_file_name, ZSTD_getErrorName(ret2));
			return compression_result;
		}
		if (!append_compressed(&compressed_data, &compressed_size, zstd_output.dst, zstd_output.pos, in_file_name)) {
			return compression_result;
		}
	} while (ret2 != 0);

	ZSTD_freeCCtx(cctx);

	// Return the compressed data and {sizes
	compression_result.data = compressed_data;
	compression_result.original_size = in_data_size;
	compression_result.compressed_size = compressed_size;

	return compression_result;
}

FLAK_DECOMPRESSION_RESULT FLAK_zstd_decompress_data(const char* in_file_name, const uint8_t* in_data, size_t in_data_size) {
	FLAK_DECOMPRESSION_RESULT decompression_result = { 0 };
	// Get decompressed (original) size
	size_t decompressed_size = ZSTD_getFrameContentSize(in_data, in_data_size);
	if (decompressed_size == ZSTD_CONTENTSIZE_ERROR) {
		ulog_error("ZSTD: Not a valid compressed frame for file %s", in_file_name);
		return decompression_result;
	}
	else if (decompressed_size == ZSTD_CONTENTSIZE_UNKNOWN) {
		ulog_error("ZSTD: Original size unknown for file %s", in_file_name);
		return decompression_result;
	}
	// Allocate memory with decompressed data as base size
	FLAK_memory_arena_t* arena = FLAK_memory_arena_create(decompressed_size);
	uint8_t* decompressed_data = FLAK_memory_arena_allocate(arena, decompressed_size, FLK_DEFAULT_ALIGNMENT);

	// Perform decompression
	size_t d_size = ZSTD_decompress(decompressed_data, decompressed_size, in_data, in_data_size);
	if (ZSTD_isError(d_size)) {
		ulog_error("ZSTD: Decompression error for file %s: %s",
			in_file_name, ZSTD_getErrorName(d_size));
		FLAK_memory_arena_free(arena);
		free(arena);
		return decompression_result;
	}

	// Optional: shrink allocation if actual size < expected size
	if (d_size < decompressed_size) {
		uint8_t* temp = (uint8_t*)realloc(decompressed_data, d_size);
		if (temp)
			decompressed_data = temp;
	}

	decompression_result.data = decompressed_data;
	decompression_result.data_size = d_size;

	FLAK_memory_arena_free(arena);
	free(arena);

	return decompression_result;
}
