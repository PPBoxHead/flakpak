// ===========================================================================
// 
// flakpak - File Archiver and Compressor CL Application
//
// Copyright (C) 2025 SACRAROSSA
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// ---------------------------------------------------------------------------
// File: [flak_dynamic_buffer.h]
//
// Description: Signle header file for a simple implementation of dynamic buffer used in flakpak.
// 				Provides functions for creating, appending to, and destroying dynamic buffers.
// 
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 23.10.2025
// Version: 1.0.0
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <stdlib.h> - C Standard Library
//  - <stdint.h> - C Standard Library
//  - <string.h> - C Standard Library
//
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAKPAK_DYNAMIC_BUFFER_H
#define FLAKPAK_DYNAMIC_BUFFER_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define FLAK_INITIAL_BUFFER_CAPACITY (64 * 1024)  // 64KB initial


typedef struct FLAK_dynamic_buffer {
	uint8_t* data;
	size_t size;      // Current used size
	size_t capacity;  // Total allocated capacity
} FLAK_dynamic_buffer_t;


static bool dynamic_buffer_init(FLAK_dynamic_buffer_t* in_buffer, size_t in_initial_capacity) {
	in_buffer->data = (uint8_t*)malloc(in_initial_capacity);
	if (!in_buffer->data) return false;
	in_buffer->size = 0;
	in_buffer->capacity = in_initial_capacity;
	return true;
}

static bool dynamic_buffer_append(FLAK_dynamic_buffer_t* in_buffer, const void* in_src, size_t in_src_size) {
	// Check if we need to grow
	if (in_buffer->size + in_src_size > in_buffer->capacity) {
		// Grow by 1.5x or enough to fit new data, whichever is larger
		size_t new_capacity = in_buffer->capacity + (in_buffer->capacity / 2);
		if (new_capacity < in_buffer->size + in_src_size) {
			new_capacity = in_buffer->size + in_src_size;
		}
		uint8_t* new_data = (uint8_t*)realloc(in_buffer->data, new_capacity);
		if (!new_data) return false;
		in_buffer->data = new_data;
		in_buffer->capacity = new_capacity;
	}
	memcpy(in_buffer->data + in_buffer->size, in_src, in_src_size);
	in_buffer->size += in_src_size;
	return true;
}

static void dynamic_buffer_free(FLAK_dynamic_buffer_t* in_buffer) {
	free(in_buffer->data);
	in_buffer->data = NULL;
	in_buffer->size = 0;
	in_buffer->capacity = 0;
}


#endif // !FLAKPAK_DYNAMIC_BUFFER_H