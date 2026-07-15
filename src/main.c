#include "multt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void eduardo(void *args) {
  char *input = (char *)args;
  printf("%s\n", input);
}

#define N 32

int main() {
  char prologs[N][20];

  init_tasks();
  for (int i = 0; i < N; i++) {
    snprintf(prologs[i], sizeof(prologs[i]), "Hello from task: %d", i);
    create_task(i, eduardo, prologs[i]);
  }
  schedule();
  printf("Hello from main!\n");

  return 0;
}
