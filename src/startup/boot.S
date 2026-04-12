#include "armv6_asm.h"
#include "armv6_cache_asm.h"

.section ".bss"
.align 4
    // 16KB for SVC stack
    .space 16384 
svc_stack_top:

    // 4KB for IRQ stack
    .space 4096
irq_stack_top:

    // 4KB for abort stack
    .space 4096
abt_stack_top:

    // 4KB for undef stack
    .space 4096
und_stack_top:


.section ".text.boot"
.global _start
_start:
    // enable access to cp10/cp11 (RMW set bits 20-23 in CPACR)
    mrc p15, 0, r0, c1, c0, 2
    orr r0, r0, #(0xF << 20)
    mcr p15, 0, r0, c1, c0, 2
    
    PREFETCH_FLUSH(r0)

    // turn on VFP (bit 30)
    mov r0, #0x40000000
    vmsr fpexc, r0

    ldr sp, =0x8000

    // initialize virtual memory
    bl l1_page_table_init
    bl mmu_sync_ptes
    bl mmu_init
    bl mmu_enable

    ldr pc, =higher_half_entry

higher_half_entry:
    // initialize stacks
    ldr sp, =svc_stack_top

    cps #IRQ_MODE
    ldr sp, =irq_stack_top

    cps #ABORT_MODE
    ldr sp, =abt_stack_top

    cps #UNDEF_MODE
    ldr sp, =und_stack_top

    cps #SUPER_MODE

    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    mov r2, #0

    // clear bss
bss_loop:
    cmp r0, r1
    bge bss_done
    str r2, [r0], #4
    b bss_loop

bss_done:
    // load interrupt table
    ldr r0, =_interrupt_table
    mcr p15, 0, r0, c12, c0, 0
    
    DSB(r0)
    PREFETCH_FLUSH(r0)

    // clear page 0 in L1 PT
    ldr r0, =__l1_page_table_start__
    mov r1, #0
    str r1, [r0]
    bl mmu_sync_ptes

    // jump to C initialization
    b cstart


.p2align 5
.global swi_ptr
.global irq_ptr
.global _interrupt_table
_interrupt_table:
    ldr pc, reset_ptr
    ldr pc, undef_ptr
    ldr pc, swi_ptr
    ldr pc, prefetch_ptr
    ldr pc, data_abort_ptr
    ldr pc, reset_ptr
    ldr pc, irq_ptr
    ldr pc, fiq_ptr

reset_ptr:      .word reset_vector
undef_ptr:      .word undefined_instruction_vector
swi_ptr:        .word swi_asm
prefetch_ptr:   .word prefetch_abort_vector
data_abort_ptr: .word data_abort_asm
irq_ptr:        .word interrupt_vector
fiq_ptr:        .word fast_interrupt_vector

reset_vector:
    bl rpi_reset

data_abort_asm:
    // adjust LR to pc that crashed
    sub lr, lr, #8

    push {r0-r3, r12, lr}
    mov r0, lr

    // read dfsr (crash reason) and dfar (crash address)
    mrc p15, 0, r1, c5, c0, 0
    mrc p15, 0, r2, c6, c0, 0

    b data_abort_vector

swi_asm:
    // subtract sizeof(TrapFrame) + store all registers
    sub sp, sp, #68
    stmia sp, {r0-r12}^
    add r0, sp, #52
    stmia r0, {sp, lr}^
    str lr, [sp, #60] // user lr = SVC pc
    mrs r0, spsr
    str r0, [sp, #64]

    mov r0, sp
    bl swi_syscall_handler

    // restore registers
    ldr r0, [sp, #64]
    msr spsr_cxsf, r0
    ldr lr, [sp, #60]
    add r0, sp, #52
    ldmia r0, {sp, lr}^
    ldmia sp, {r0-r12}^
    add sp, sp, #68

    movs pc, lr
