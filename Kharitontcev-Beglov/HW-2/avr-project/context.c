#include "context.h"
void getcontext(context_t *context) {
    /* loading context address r24-r25 --- pointer to the context. See GCC-AVR calling convention */ \
    SAVECONTEXT("",
                "mov r30, r24\n"
                "mov r31, r25\n"
    )
    __asm__ __volatile__ ("ret\n");
}


void setcontext(const context_t *context) {
    RESTORE_CONTEXT("mov r28, r24\n"
                    "mov r29, r25\n"
    )
    __asm__ __volatile__("ret\n");
}