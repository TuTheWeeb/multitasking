#include "multt.h"

TCB_t main_task = {};
TCB_t *tasks;
TCB_t *current_task = &main_task;
TCB_t *next_task = NULL;
uint8_t *_TASKS_MEM;

static uint32_t current_task_index = 0;

void init_tasks(void) {
  tasks = (TCB_t *)calloc(MAX_STACKS, sizeof(TCB_t));
  for (size_t i = 0; i < MAX_STACKS; i++) {
    tasks[i].stack = (uint64_t *)malloc(STACK_SIZE);
  }
}

void close_tasks(void) {
  for (size_t i = 0; i < MAX_STACKS; i++) {
    free(tasks[i].stack);
  }

  free(tasks);
}

void task_exit(void) {
  current_task->state = EMPTY;
  schedule();
}

/*
void create_task(uint32_t index, void (*task_function)(void *args),
                 void *args) {
  tasks[index].id = index;
  tasks[index].state = READY;

  // Start at the top of the stack
  uint64_t *sp = &tasks[index].stack[STACK_SIZE - 1];

  // x86_64 requires 16-byte stack alignment.
  // We align the pointer downwards to the nearest 16-byte boundary.
  sp = (uint64_t *)((uintptr_t)sp & ~0xF);

  // Fake the stack frame expected by your x86_64 context_switch
  *(--sp) = (uint64_t)task_exit;
  *(--sp) = (uint64_t)task_function; // RIP (The 'ret' instruction jumps here)
  *(--sp) = (uint64_t)args; // RDI (First argument passed to the function)
  *(--sp) = 0;              // RBP
  *(--sp) = 0;              // RBX
  *(--sp) = 0;              // R12
  *(--sp) = 0;              // R13
  *(--sp) = 0;              // R14
  *(--sp) = 0;              // R15

  // Save the final stack pointer into the TCB
  tasks[index].stack_pointer = sp;
}*/

__attribute__((naked)) void context_switch(void) {
  // __asm__ volatile ensures the compiler doesn't optimize this code away
  __asm__ volatile(
      // Save current context
      // On x86_64 (System V ABI), we only need to save the "callee-saved"
      // registers. The return address (RIP) is already pushed to the stack by
      // the 'call' instruction.
      "pushq %rdi \n"
      "pushq %rbp \n"
      "pushq %rbx \n"
      "pushq %r12 \n"
      "pushq %r13 \n"
      "pushq %r14 \n"
      "pushq %r15 \n"

      // Save current stack pointer to current_task
      // Using %rip-relative addressing which is required for 64-bit binaries
      "movq current_task(%rip), %rax \n" // Load pointer to current TCB into RAX
      "movq %rsp, (%rax) \n" // Store CPU RSP into TCB->stack_pointer

      // Switch current_task = next_task
      "movq next_task(%rip), %rcx \n"    // Load pointer to next TCB into RCX
      "movq %rcx, current_task(%rip) \n" // current_task = next_task

      // Load new stack pointer
      "movq (%rcx), %rsp \n" // Load new RSP from next TCB->stack_pointer

      // Restore new context
      // Must be popped in the exact reverse order they were pushed
      "popq %r15 \n"
      "popq %r14 \n"
      "popq %r13 \n"
      "popq %r12 \n"
      "popq %rbx \n"
      "popq %rbp \n"
      "popq %rdi \n"

      // Return
      // 'ret' pops the top value of the stack into the Instruction Pointer
      // (RIP)
      "ret \n");
}

void schedule() {
  uint32_t start_index = current_task_index;

  do {
    current_task_index++;
    if (current_task_index >= MAX_STACKS) {
      current_task_index = 0;
    }

    if (current_task_index == start_index &&
        tasks[current_task_index].state == EMPTY) {
      if (current_task != &main_task) {
        next_task = &main_task;
        context_switch();
      }
      return;
    }
  } while (tasks[current_task_index].state == EMPTY);

  next_task = &tasks[current_task_index];

  if (current_task != next_task) {
    context_switch();
  }
}
