#pragma once
#ifndef MULTT_H
#define MULTT_H
#include <stdint.h>
#include <stdio.h>

#define STACK_SIZE 10 * 1024
#define MAX_STACKS 32

typedef enum { EMPTY = 0, READY = 1, RUNNING = 2 } TCB_State;

// Task control block
typedef struct {
  uint64_t *stack_pointer;
  // uint64_t stack[STACK_SIZE];
  uint64_t *stack;
  uint32_t id;
  TCB_State state;
} TCB_t;

extern TCB_t *tasks;
extern TCB_t *current_task;
extern TCB_t *next_task;
extern TCB_t main_task;

void init_tasks(void);

void create_task(uint32_t index, void (*task_function)(void *args), void *args);

__attribute__((naked)) void context_switch(void);

void schedule();

void task_exit(void);

#endif
