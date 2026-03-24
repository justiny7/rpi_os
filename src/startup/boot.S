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
    mrc p15, 0, r0, c1, c0, 2    // Read CPACR (Coprocessor Access Control Register)
    orr r0, r0, #(0xF << 20)     // Set bits 20-23 to 1 (Full access to CP10 and CP11)
    mcr p15, 0, r0, c1, c0, 2    // Write back to CPACR
    
    // Flush the instruction pipeline so the CPU knows it has access now
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 4    // (You can also use your PREFETCH_FLUSH macro here)

    // 2. Power on the VFP hardware
    mov r0, #0x40000000          // Bit 30 is the 'EN' (Enable) bit for the VFP
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

    // jump to C initialization
    b cstart


.p2align 5
.global _interrupt_table
_interrupt_table:
    ldr pc, =reset_vector
    ldr pc, =undefined_instruction_vector
    ldr pc, swi_handler_ptr
    ldr pc, prefetch_abort_handler_ptr
    ldr pc, =data_abort_handler_ptr
    ldr pc, =reset_vector
    ldr pc, irq_handler_ptr
    ldr pc, =fast_interrupt_vector

.global irq_handler_ptr
irq_handler_ptr: .word interrupt_vector

.global data_abort_handler_ptr
data_abort_handler_ptr: .word data_abort_vector

.global prefetch_abort_handler_ptr
prefetch_abort_handler_ptr: .word prefetch_abort_vector

.global swi_handler_ptr
swi_handler_ptr: .word software_interrupt_vector

reset_vector:
    bl rpi_reset
