#pragma once
#ifndef MULTT_H
#define MULTT_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

void close_tasks(void);

// void create_task(uint32_t index, void (*task_function)(void *args), void
// *args);

// This way it can use any kind of function
#define create_task(index, fn, args)                                           \
  do {                                                                         \
    tasks[index].id = index;                                                   \
    tasks[index].state = READY;                                                \
    /* Start at the top of the stack*/                                         \
    uint64_t *sp = &tasks[index].stack[STACK_SIZE - 1];                        \
    /* x86_64 requires 16-byte stack alignment.*/                              \
    /* We align the pointer downwards to the nearest 16-byte boundary.*/       \
    sp = (uint64_t *)((uintptr_t)sp & ~0xF);                                   \
    /* Fake the stack frame expected by your x86_64 context_switch*/           \
    *(--sp) = (uint64_t)task_exit;                                             \
    *(--sp) = (uint64_t)fn;   /* RIP (The 'ret' instruction jumps here)*/      \
    *(--sp) = (uint64_t)args; /* RDI (First argument passed to the function)*/ \
    *(--sp) = 0;              /* RBP */                                        \
    *(--sp) = 0;              /* RBX */                                        \
    *(--sp) = 0;              /* R12 */                                        \
    *(--sp) = 0;              /* R13 */                                        \
    *(--sp) = 0;              /* R14 */                                        \
    *(--sp) = 0;              /* R15 */                                        \
    /*Save the final stack pointer into the TCB*/                              \
    tasks[index].stack_pointer = sp;                                           \
  } while (0)

__attribute__((naked)) void context_switch(void);

void schedule();

void task_exit(void);

#endif
