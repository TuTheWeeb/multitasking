#include "heap.h"
#include "macros.h"
#include "multt.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  void *args;
  void *sizes;
} IT;

void eduardo(void *args) {
  if (!args)
    return;

  uint32_t a = *(uint32_t *)args;

  char *input = *(char **)((char *)args + sizeof(uint32_t));
  printf("id: %d, str: %s\n", a, input);
}

#define ADD(x)                                                                 \
  memcpy((char *)space + dis, &x, sizeof(x));                                  \
  dis += sizeof(x)

#define SIZEOF_OP(x) sizeof(x)
#define SUMMON_TASK(index, fn, ...)                                            \
  do {                                                                         \
    void *space = NULL;                                                        \
    __VA_OPT__(space = malloc(MAP(SIZEOF_OP, +, UNWRAP(__VA_ARGS__))));        \
    size_t dis = 0;                                                            \
    MAP(ADD, ;, UNWRAP(__VA_ARGS__));                                          \
                                                                               \
    create_task(index, fn, space);                                             \
  } while (0);

#define N 32

int main() {
  open_heap(8);
  HEAP_BLOCK *allocation = alloc_mem(600);

  printf("HEAP_BLOCK size: %lu\n", sizeof(HEAP_BLOCK));

  if (allocation != NULL) {
    uint32_t *numbers = (uint32_t *)allocation->mem;
    numbers[0] = 42;
    printf("number is: %d, numbers size: %lu\n", numbers[0], allocation->size);
    free_mem(allocation);
  }

  close_heap();
  /*char prologs[N][20];

  init_tasks();
  for (uint32_t i = 0; i < N; i++) {
    snprintf(prologs[i], sizeof(prologs[i]), "Hello from task: %d", i);
    SUMMON_TASK(i, eduardo, i, prologs[i]);
  }
  schedule();

  close_tasks();
  printf("Hello from main!\n");

  uint32_t a = 32;
  double b = 64.0;*/

  // printf("size: %lu\n", MAP(SIZEOF_OP, +, a, b));
  /*void *space[] = {&a, &b};
  uint32_t *c = (uint32_t *)space[0];
  double *d = (double *)space[1];*/
  // printf("%d\n", *c);
  // printf("%lf\n", *d);

  // free(space);

  return 0;
}
