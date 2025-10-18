#include <flakpak-c/flak_arena.h>


void* FLAK_memory_arena_allocate(FLAK_memory_arena_t* in_arena, size_t in_size, size_t in_align) {
	uintptr_t current = (uintptr_t)in_arena->base + in_arena->offset;
	uintptr_t aligned = (current + (in_align - 1)) & ~(uintptr_t)(in_align - 1);
	size_t padding = aligned - current;
	if (in_arena->offset + padding + in_size > in_arena->size) {
		ulog_error("Out of memory in arena (requested %zu bytes, available %zu bytes)\n", in_size, in_arena->size - in_arena->offset);
		return NULL; // Out of space
	}
	in_arena->offset += padding + in_size;
	return (void*)aligned;
}

void FLAK_memory_arena_reset(FLAK_memory_arena_t* in_arena) {
	in_arena->offset = 0;
}

void FLAK_memory_arena_free(FLAK_memory_arena_t* in_arena) {
	free(in_arena->base);
	in_arena->base = NULL;
	in_arena->size = in_arena->offset = 0;

	ulog_trace("Freed memory arena data\n");
}
