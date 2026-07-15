#include "heap.h"
// Ignored because it's used entirely by the library
static HEAP_BLOCK _HEAP_HEAD = {0};
static _HEAP_ARENA _HEAP_ARENA_INTERNAL = {0};

size_t heap_structure_size(size_t mem) {
  size_t max_leaves = mem / _MIN_HEAP_CROP;
  if (max_leaves == 0)
    max_leaves = 1; // Prevent 0-sized trees
  size_t total_nodes = (max_leaves * 2) - 1;

  return sizeof(HEAP_BLOCK) * total_nodes;
}

int8_t _heap_arena_internal_open(size_t mem, _HEAP_ARENA *arena) {
  size_t structure_size = heap_structure_size(mem);
  size_t total_mem = structure_size + mem;

  arena->mem = (void *)malloc(total_mem);
  if (arena->mem == NULL) {
    return -1;
  }

  arena->size = total_mem;
  arena->pos = 0;

  return 0;
}

void _heap_arena_internal_close(_HEAP_ARENA *arena) {
  free(arena->mem);
  arena->mem = NULL;
  arena->pos = 0;
  arena->size = 0;
}

void *_heap_arena_internal_alloc(_HEAP_ARENA *arena, size_t mem) {
  size_t aligned_mem = ALIGN_UP(mem);

  if (arena->pos + aligned_mem > arena->size) {
    return NULL;
  }

  void *offset = (uint8_t *)arena->mem + (arena->pos);
  arena->pos += aligned_mem;

  return offset;
}

void open_heap(size_t reserve) {
  if (reserve == 0)
    return;
  size_t alloc_size = reserve * _MIN_HEAP_SIZE;
  _heap_arena_internal_open(alloc_size, &_HEAP_ARENA_INTERNAL);
  _HEAP_HEAD.size = alloc_size; // Reserve at least DEFAULT_SIZE
  _HEAP_HEAD.mem =
      _heap_arena_internal_alloc(&_HEAP_ARENA_INTERNAL, alloc_size);
  _HEAP_HEAD.halfs = NULL;
  _HEAP_HEAD.state = FREE;

  init_heap(&_HEAP_HEAD, _MIN_HEAP_CROP);
}

void close_heap(void) { _heap_arena_internal_close(&_HEAP_ARENA_INTERNAL); }

HEAP_BLOCK *alloc_mem(size_t size) {
  if (size < _MIN_HEAP_CROP) {
    size = _MIN_HEAP_CROP;
  }
  HEAP_BLOCK *node = traverse(&_HEAP_HEAD, size);
  if (node != NULL)
    return node;

  return NULL;
}

void free_mem(HEAP_BLOCK *block) { block->state = FREE; }

HEAP_BLOCK *create_heap_nodes(void) {
  HEAP_BLOCK *nodes = (HEAP_BLOCK *)_heap_arena_internal_alloc(
      &_HEAP_ARENA_INTERNAL, sizeof(HEAP_BLOCK) * 2);

  nodes[0].halfs = NULL;
  nodes[0].state = FREE;
  nodes[1].halfs = NULL;
  nodes[1].state = FREE;

  return nodes;
}

void init_heap(HEAP_BLOCK *node, size_t size) {
  if (node == NULL)
    return;

  if (node->size > size) {
    node->halfs = create_heap_nodes();
    size_t n_size = node->size / 2;

    node->halfs[0].mem = node->mem;
    node->halfs[0].size = n_size;

    node->halfs[1].mem = ((uint8_t *)node->mem) + n_size;
    node->halfs[1].size = n_size;

    init_heap(&node->halfs[0], size);
    init_heap(&node->halfs[1], size);
  }
}

HEAP_BLOCK *traverse(HEAP_BLOCK *node, size_t size) {
  if (node == NULL)
    return NULL;

  if (node->size < size)
    return NULL;

  if (node->halfs != NULL) {
    HEAP_BLOCK *temp_node = traverse(&node->halfs[0], size);
    if (temp_node != NULL) {
      return temp_node;
    }

    temp_node = traverse(&node->halfs[1], size);
    if (temp_node != NULL) {
      return temp_node;
    }
  }

  if (node->state == FREE) {
    node->state = INUSE;
    return node;
  }

  return NULL;
}
