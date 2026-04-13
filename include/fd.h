#ifndef FD_H
#define FD_H

#include <stdint.h>

typedef struct File File;

typedef struct {
    int (*read)(File* file, void* buf, uint32_t len);
    int (*write)(File* file, const void* buf, uint32_t len);
    int (*ioctl)(File* file, int request, void* arg);
    int (*close)(File* file);
} FileOps;

typedef struct {
    FileOps* ops;
    void* data;
} VNode;

struct File {
    VNode* vnode;
    uint32_t offset;
    uint32_t flags;
    uint32_t ref_count;
};

enum {
    EBADF  = -9,
    ENOSYS = -38,
    ENOTTY = -25
};

enum {
    STDOUT = 1,
    STDERR = 2
};

extern VNode console_vnode;

#endif
