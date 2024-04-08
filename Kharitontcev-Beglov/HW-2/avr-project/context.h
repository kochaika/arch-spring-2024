#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdint.h>

#define SAVECONTEXT(presave_code, get_address_to_z)     \
__asm__ __volatile__( \
    /* Push old z register, before copying the context to it */ \
    "push r30\n" \
    "push r31\n" \
    \
    /* sreg pushing */ \
    "push r0\n" /* pushing old r0 */ \
    "in r30, __SREG__\n" /* status register to r30 */   \
    "\n" presave_code "\n" \
    "push r30\n" /* pushing sreg */ \
    "\n" get_address_to_z "\n" \
    \
    /* save sreg */ \
    "pop r0\n" \
    "st z+, r0\n" \
    /* restore r0 */ \
    "pop r0\n" \
    \
    "st z+, r0\n" \
    "st z+, r1\n" \
    "st z+, r2\n" \
    "st z+, r3\n" \
    "st z+, r4\n" \
    "st z+, r5\n" \
    "st z+, r6\n" \
    "st z+, r7\n" \
    "st z+, r8\n" \
    "st z+, r9\n" \
    "st z+, r10\n" \
    "st z+, r11\n" \
    "st z+, r12\n" \
    "st z+, r13\n" \
    "st z+, r14\n" \
    "st z+, r15\n" \
    "st z+, r16\n" \
    "st z+, r17\n" \
    "st z+, r18\n" \
    "st z+, r19\n" \
    "st z+, r20\n" \
    "st z+, r21\n" \
    "st z+, r22\n" \
    "st z+, r23\n" \
    "st z+, r24\n" \
    "st z+, r25\n" \
    "st z+, r26\n" \
    "st z+, r27\n" \
    "st z+, r28\n" /* saving y-low */ \
    "st z+, r29\n" /* and y-high, so... */ \
    \
    "mov r28, r30\n" /* we can save z in y (y already written) */ \
    "mov r29, r31\n" \
    \
    "pop r31\n" \
    "pop r30\n" \
    "st y+, r30\n" \
    "st y+, r31\n" /* saving z */ \
    /* removing PC from the stack to save it */ \
    "pop r30\n" \
    "pop r31\n" \
    "st y+, r31\n" \
    "st y+, r30\n" \
    "in r26, __SP_L__\n" \
    "in r27, __SP_H__\n" \
    "st y+, r26\n" \
    "st y, r27\n" \
    /* push pc to return to the correct frame */ \
    "push r31\n" \
    "push r30\n" \
    /* in that case I have corrupted some registers. */ \
    "mov r30, r28\n" /* back to Z */ \
    "mov r31, r29\n" \
    "in r28, __SREG__\n" /* Save SREG, since we will do some subtractions */ \
    "sbiw r30, 9\n" /* back to r26 in memory */ \
    "out __SREG__, r28\n" /* restoring SREG */ \
    "ld r26, Z+\n" \
    "ld r27, Z+\n" \
    "ld r28, Z+\n" \
    "ld r29, Z+\n" \
    "push r28\n" /* save Y to swap it with Z */ \
    "push r29\n" \
    "mov r28, r30\n" \
    "mov r29, r31\n" \
    "ld r30, Y+\n" \
    "ld r31, Y+\n" \
    "pop r29\n" /* Original Y */ \
    "pop r28\n" \
    );
#define RESTORE_CONTEXT(get_address_to_y)     __asm__ __volatile__( \
    "\n" get_address_to_y "\n" \
    /* Then we need to offset structure pointer to the end of the structure to restore the sp */ \
    "adiw r28, 36\n" \
    "ld r0, Y\n" \
    "out __SP_H__, r0\n" \
    "ld r0, -Y\n" \
    "out __SP_L__, r0\n" \
    /* returning PC to the right position (for the ABI) */ \
    "ld r1, -Y\n" \
    "ld r0, -Y\n" \
    "push r0\n" \
    "push r1\n" \
    /* loading original Z and saving it to stack, thus we can load all other registers and load Z to original position */ \
    "ld r31, -Y\n" \
    "ld r30, -Y\n" \
    "push r31\n" \
    "push r30\n" \
    "mov r30, r28\n" \
    "mov r31, r29\n" \
    "ld r29, -Z\n" \
    "ld r28, -Z\n" \
    "ld r27, -Z\n" \
    "ld r26, -Z\n" \
    "ld r25, -Z\n" \
    "ld r24, -Z\n" \
    "ld r23, -Z\n" \
    "ld r22, -Z\n" \
    "ld r21, -Z\n" \
    "ld r20, -Z\n" \
    "ld r19, -Z\n" \
    "ld r18, -Z\n" \
    "ld r17, -Z\n" \
    "ld r16, -Z\n" \
    "ld r15, -Z\n" \
    "ld r14, -Z\n" \
    "ld r13, -Z\n" \
    "ld r12, -Z\n" \
    "ld r11, -Z\n" \
    "ld r10, -Z\n" \
    "ld r9, -Z\n" \
    "ld r8, -Z\n" \
    "ld r7, -Z\n" \
    "ld r6, -Z\n" \
    "ld r5, -Z\n" \
    "ld r4, -Z\n" \
    "ld r3, -Z\n" \
    "ld r2, -Z\n" \
    "ld r1, -Z\n" \
    "ld r0, -Z\n" \
    /* restore sreg */ \
    "push r0\n" \
    "ld r0, -Z\n" \
    "out __SREG__, r0\n" \
    "pop r0\n" \
    /* restore original Z */ \
    "pop r30\n" \
    "pop r31\n" \
);

#define RESTORE_CONTEXT_GLOBAL_POINTER(global_pointer) \
    RESTORE_CONTEXT(                                   \
    "lds YL, "#global_pointer"\n" \
    "lds YH, "#global_pointer" + 1\n" \
)

typedef union {
    struct {
        uint8_t low;
        uint8_t high;
    } part; // to ensure the size of the void* pointer
    void *ptr;
} register_pointer;

typedef struct {
    uint8_t sreg; // status register, offset: 0
    uint8_t r[32]; // registers, offset: 1
    register_pointer pc; // offset: 33
    register_pointer sp; // offset: 35
} context_t;

void getcontext(context_t *context) __attribute__ ((naked)); // For that function we don't need epilogue and prologue.
void setcontext(const context_t *context) __attribute__ ((naked));

#endif