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
// File: [flak_arena.h - flak_arena.c]
//
// Description: Header file for a simple implementation of memory arena allocator used in flakpak.
// 				Provides functions for creating, allocating from, and destroying memory arenas.
// 
// Author: \x45\x6D\x61\x6E\x75\x65\x6C\x20\x46\x61\x76\x61\x72\x6F
// Date: 17.10.2025
// Version: 1.0.0
//
// ---------------------------------------------------------------------------
// Dependencies:
//  - <stdlib.h> - C Standard Library
//  - <stdint.h> - C Standard Library
//
// Notes:
//  - [Any important implementation notes]
//  - [Known issues or limitations]
//  - [Performance considerations]
//
// ===========================================================================
#ifndef FLAKPAK_ARENA_ALLOCATOR_H
#define FLAKPAK_ARENA_ALLOCATOR_H

#include <stdlib.h>
#include <stdint.h>

#include <microlog/ulog.h>


#define FLK_DEFAULT_ALIGNMENT sizeof(void*)


typedef struct FLAK_memory_arena {
    uint8_t* base;
    size_t size;
    size_t offset;
} FLAK_memory_arena_t;

static FLAK_memory_arena_t* FLAK_memory_arena_create(size_t in_size) {
    FLAK_memory_arena_t* arena = (FLAK_memory_arena_t*)malloc(sizeof(FLAK_memory_arena_t));
    if (!arena) return NULL;
    arena->base = (uint8_t*)malloc(in_size);
    if (!arena->base) {
        free(arena);
        return NULL;
    }
    arena->size = in_size;
    arena->offset = 0;

	ulog_trace("Created memory arena of size %zu bytes\n", in_size);
    return arena;
}

void* FLAK_memory_arena_allocate(FLAK_memory_arena_t* in_arena, size_t in_size, size_t in_align);
void FLAK_memory_arena_reset(FLAK_memory_arena_t* in_arena);
void FLAK_memory_arena_free(FLAK_memory_arena_t* in_arena);

#endif // !FLAKPAK_ARENA_ALLOCATOR_H