#include <flakpak-c/zstd_compressor.h>
#include <flakpak-c/flak_arena.h>
#include <flakpak-c/flak_dynamic_buffer.h>

#include <stdlib.h>
#include <string.h>

#include <zstd/zstd.h>
#include <microlog/ulog.h>


FLAK_COMPRESSION_RESULT FLAK_zstd_compress_data(const char* in_file_name, const uint8_t* in_data, size_t in_data_size, int in_comp_level) {
	FLAK_COMPRESSION_RESULT compression_result = { 0 };

	// Initialize ZSTD compression context
	ZSTD_CCtx* cctx = ZSTD_createCCtx();
	size_t ret = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, in_comp_level);
	if (ZSTD_isError(ret)) {
		ZSTD_freeCCtx(cctx);
		ulog_error("ZSTD: Error setting compression level for file %s: %s", in_file_name, ZSTD_getErrorName(ret));
		return compression_result;
	}

	// Use dynamic buffer instead of repeated realloc
	FLAK_dynamic_buffer_t output_buf;
	if (!dynamic_buffer_init(&output_buf, FLAK_INITIAL_BUFFER_CAPACITY)) {
		ulog_error("ZSTD: Failed to initialize output buffer");
		ZSTD_freeCCtx(cctx);
		return compression_result;
	}
	// Buffers for input and output
	const size_t in_chunk_size = ZSTD_CStreamInSize();
	const size_t out_chunk_size = ZSTD_CStreamOutSize();

	uint8_t* out_buffer = (uint8_t*)malloc(out_chunk_size);
	if (!out_buffer) {
		dynamic_buffer_free(&output_buf);
		ZSTD_freeCCtx(cctx);
		return compression_result;
	}

	ZSTD_inBuffer input = { in_data, in_data_size, 0 };

	while (input.pos < input.size) {
		ZSTD_outBuffer output = { out_buffer, out_chunk_size, 0 };

		size_t ret = ZSTD_compressStream2(cctx, &output, &input, ZSTD_e_continue);
		if (ZSTD_isError(ret)) {
			ulog_error("ZSTD: Compression error: %s", ZSTD_getErrorName(ret));
			free(out_buffer);
			dynamic_buffer_free(&output_buf);
			ZSTD_freeCCtx(cctx);
			return compression_result;
		}

		if (!dynamic_buffer_append(&output_buf, out_buffer, output.pos)) {
			ulog_error("ZSTD: Buffer append failed");
			free(out_buffer);
			dynamic_buffer_free(&output_buf);
			ZSTD_freeCCtx(cctx);
			return compression_result;
		}
	}

	// Finalize compression
	ZSTD_outBuffer output = { out_buffer, out_chunk_size, 0 };
	size_t ret_end = 0;
	do {
		output.pos = 0;
		ret_end = ZSTD_compressStream2(cctx, &output, &input, ZSTD_e_end);
		if (ZSTD_isError(ret_end)) {
			ulog_error("ZSTD: End stream error: %s", ZSTD_getErrorName(ret_end));
			free(out_buffer);
			dynamic_buffer_free(&output_buf);
			ZSTD_freeCCtx(cctx);
			return compression_result;
		}

		if (!dynamic_buffer_append(&output_buf, out_buffer, output.pos)) {
			ulog_error("ZSTD: Buffer append failed during finalization");
			free(out_buffer);
			dynamic_buffer_free(&output_buf);
			ZSTD_freeCCtx(cctx);
			return compression_result;
		}
	} while (ret_end != 0);

	free(out_buffer);
	ZSTD_freeCCtx(cctx);

	// Transfer ownership to result
	compression_result.data = output_buf.data;
	compression_result.original_size = in_data_size;
	compression_result.compressed_size = output_buf.size;

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
	uint8_t* decompressed_data = FLAK_memory_arena_allocate(arena, decompressed_size, FLAK_DEFAULT_ALIGNMENT);

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
