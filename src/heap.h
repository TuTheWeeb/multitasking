#pragma once
#ifndef HEAP_DEFS
#define HEAP_DEFS

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum _HEAP_STATE { FREE, INUSE } _HEAP_STATE;

typedef struct HEAP_BLOCK {
  struct HEAP_BLOCK *halfs;
  size_t size;
  void *mem;
  _HEAP_STATE state;
} HEAP_BLOCK;

typedef struct _HEAP_ARENA {
  void *mem;
  size_t size;
  size_t pos;
} _HEAP_ARENA;

#ifndef _MIN_HEAP_SIZE
#define _MIN_HEAP_SIZE 1024
#endif

// Use 64 bytes as crop since the use of memory by the structure is 32, so the
// max size is block(n) = 32 * (pow(2, log2(HEAP_SIZE - log2(HEAP_CROP))) - 1)
#ifndef _MIN_HEAP_CROP
#define _MIN_HEAP_CROP 64
#endif

// 8-byte aligment macro
#define ALIGNMENT 8
#define ALIGN_UP(x) (((x) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

int8_t _heap_arena_internal_open(size_t mem, _HEAP_ARENA *arena);
void _heap_arena_internal_close(_HEAP_ARENA *arena);
void *_heap_arena_internal_alloc(_HEAP_ARENA *arena, size_t mem);
size_t heap_structure_size(size_t mem);
HEAP_BLOCK *create_heap_nodes(void);
void init_heap(HEAP_BLOCK *node, size_t size);
HEAP_BLOCK *traverse(HEAP_BLOCK *node, size_t size);
HEAP_BLOCK *alloc_mem(size_t size);
void free_mem(HEAP_BLOCK *block);
void open_heap(size_t reserve);
void close_heap(void);

#endif
