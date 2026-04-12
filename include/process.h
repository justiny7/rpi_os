#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define PROC_HEAP_START 0x10000000
#define PROC_STACK_START 0xB0000000
#define PROC_CODE_START 0x8000

typedef struct {
    uint32_t r[13];
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t cpsr;
} TrapFrame;

typedef enum {
    PROC_READY,
    PROC_RUNNING,
    PROC_BLOCKED,
    PROC_DEAD
} ProcessState;

typedef struct {
    uint32_t pid;
    ProcessState state;
    TrapFrame context;

    uint32_t l1_pt_paddr;

    uint32_t heap_start;
    uint32_t prog_break;
} Process;

extern Process* current_process;
Process* proc_create(const char* filename, uint32_t entry_point);
void proc_run(TrapFrame* context, uint32_t l1_pt_vaddr);

#endif
