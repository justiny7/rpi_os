#include "syscall.h"
#include "lib.h"
#include "uart.h"
#include "fd.h"
#include "process.h"

#include <stddef.h>

static inline int sys_write(int fd, const void* buf, int len) {
    if (fd < 0 || fd >= MAX_FDS || !current_process->fd_table) {
        return EBADF;
    }

    File* f = current_process->fd_table[fd];
    if (!f || !f->vnode->ops->write) {
        return EBADF;
    }

    return f->vnode->ops->write(f, buf, len);
}
static inline int sys_writev(int fd, const struct iovec* iov, int iovcnt) {
    int total = 0;
    for (int i = 0; i < iovcnt; i++) {
        total += sys_write(fd, iov[i].iov_base, iov[i].iov_len);
    }
    return total;
}
static inline int sys_read(int fd, void* buf, int len) {
    if (fd < 0 || fd >= MAX_FDS || !current_process->fd_table) {
        return EBADF;
    }

    File* f = current_process->fd_table[fd];
    if (!f || !f->vnode->ops->read) {
        return EBADF;
    }

    return f->vnode->ops->read(f, buf, len);
}

static inline int arm_nr_set_tls(uint32_t tls_ptr) {
    // cp15, r13, c0, 3 = user RO thread ID reg
    asm volatile("mcr p15, 0, %0, c13, c0, 3" : : "r"(tls_ptr));
    return 0;
}
static inline int sys_exit(int code) {
    printk("Process %d exited with code %d\n", current_process->pid, code);

    proc_destroy(current_process);
    current_process = NULL;

    rpi_reset();
    return 0;
}

void swi_syscall_handler(TrapFrame* regs) {
    uint32_t syscall_num = regs->r[7];

    switch (syscall_num) {
        case LINUX_SYS_WRITE:
            regs->r[0] = sys_write(regs->r[0], (const void*) regs->r[1], regs->r[2]);
            break;
        case LINUX_SYS_WRITEV:
            regs->r[0] = sys_writev(regs->r[0],
                    (const struct iovec*) regs->r[1],
                    regs->r[2]);
            break;
        case LINUX_SYS_READ:
            regs->r[0] = sys_read(regs->r[0], (void*) regs->r[1], regs->r[2]);
            break;
        case LINUX_SYS_IOCTL :
            regs->r[0] = ENOTTY;
            break;
        case LINUX_SYS_EXIT_GROUP:
        case LINUX_SYS_EXIT:
            regs->r[0] = sys_exit(regs->r[0]);
            break;
        case LINUX_SYS_BRK:
            break;
        case LINUX_SYS_SET_TID_ADDR:
            regs->r[0] = current_process->pid; // current thread ID
            break;
        case ARM_NR_set_tls:
            regs->r[0] = arm_nr_set_tls(regs->r[0]);
            break;
        default:
            printk("WARNING: Unimplemented system call: %d\n", syscall_num);
            regs->r[0] = ENOSYS;
            break;
    }
}
