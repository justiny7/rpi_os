#include "syscall.h"
#include "lib.h"
#include "uart.h"

void swi_syscall_handler(TrapFrame* regs) {
    uint32_t syscall_num = regs->r[7];

    switch (syscall_num) {
        case LINUX_SYS_WRITE: {
            int fd = regs->r[0];
            char* str = (char*) regs->r[1];
            int len = regs->r[2];

            if (fd == 1 || fd == 2) {
                for (int i = 0; i < len; i++) {
                    uart_putc(str[i]);
                }
                regs->r[0] = len;
            } else {
                // TODO: write to file
                regs->r[0] = -1;
            }
            break;
        }
        case LINUX_SYS_READ: {
            panic("unimplemented\n");
            break;
        }
        case LINUX_SYS_EXIT: {
            printk("Process %d exited with code %d\n", current_process->pid, regs->r[0]);

            // TODO: destroy process
            rpi_reset();
            break;
        }
        case LINUX_SYS_BRK: {
            break;
        }
        default: {
            printk("WARNING: Unimplemented system call: %d\n", syscall_num);
            regs->r[0] = -38;
            break;
        }
    }
}
