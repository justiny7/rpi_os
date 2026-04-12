#ifndef SYSCALL_H
#define SYSCALL_H

#include "process.h"

enum {
    LINUX_SYS_EXIT   = 1,
    LINUX_SYS_READ   = 3,
    LINUX_SYS_WRITE  = 4,
    LINUX_SYS_BRK    = 45,
};

void swi_syscall_handler(TrapFrame* regs);

#endif
