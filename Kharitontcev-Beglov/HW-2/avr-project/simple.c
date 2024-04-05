#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include <stdio.h>
#include "context.h"
// Based on: https://www.nongnu.org/simulavr/simple_ex.html
/* This port corresponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((volatile char *)0x20))

/* This port corresponds to the "-R 0x22,-" command line option. */
#define special_input_port  (*((volatile char *)0x22))

/* Poll the specified string out the debug port. */
void debug_puts(const char *str) {
    const char *c;

    for (c = str; *c; c++)
        special_output_port = *c;
}

void start_system_timer(void) {
    TCNT0 = 0;   // Value of counter
    OCR0A = 250;  // Divisor. 1ms on 4MHz, CTC mode
    TCCR0B = (1 << CS01) | (1 << CS00);// clkT0S/64 (From prescaler)
    TCCR0A = (1 << WGM01);
    TIMSK0 = (1 << OCIE0A);  // Timer/Counter0 Output Compare Match Interrupt Enable

    // button
    PCICR |= (1 << PCIE2);     // set PCIE0 to enable PCMSK0 scan
    PCMSK2 |= (1 << PCINT2);   // set PCINT0 to trigger an interrupt on state change

    sei();
}


static context_t contexts[2] = {};
static volatile context_t *current_task = contexts;
static volatile int current_task_num = 0;
char buffer[256];

static void switch_task(void) {
    /* commenting this (v) line leads to linker error :D */
    sprintf(buffer, "current task: %d. switching to: %d\n", current_task_num, (current_task_num == 0 ? 1 : 0));
    current_task_num = (current_task_num == 0 ? 1 : 0);
    current_task = &(contexts[current_task_num]);
}

static void print_b(void) {
    sprintf(buffer, "PORTB=%d\n", PORTB);
    debug_puts(buffer);
}

ISR(TIMER0_COMPA_vect, ISR_NAKED) {
    cli();
    print_b();
    switch_task();
    /* commenting this (v) line leads to linker error :D */
    sprintf(buffer, "Current task pointer: %p\n", current_task);
//    debug_puts(buffer);
    RESTORE_CONTEXT_GLOBAL_POINTER(current_task)
    asm volatile("reti\n");
}

static void call_fun_in_makecontext(const context_t *next, void (*f_p)()) {
//    sprintf(buffer, "executing: %p\n", f_p);
//    debug_puts(buffer);
    f_p();
    setcontext(next);
}

void makecontext(context_t *context, void *stack_pointer, const size_t stack_size, const context_t *successor,
                 void (*func_p)()) {
    uint16_t addr;
    uint8_t *p = (uint8_t *) &addr; // Yay Endians
    context->sp.ptr = ((uint8_t *) stack_pointer + stack_size - 1); // Independent stack in case we have any errors
    context->pc.ptr = (void *) call_fun_in_makecontext;
    addr = (uint16_t) successor;
    context->r[24] = p[0];
    context->r[25] = p[1];
    addr = (uint16_t) func_p;
    context->r[22] = p[0];
    context->r[23] = p[1];
}

static void low_task() {
//    debug_puts("Entered low_task\n");
    PORTB = 1;
}

static void low_task2() {
//    debug_puts("entered low_task2\n");
    PORTB = 2;
}


/* Main for test program.  Enter a string and echo it. */
int main() {
    uint8_t stack1[256], stack2[256];
    context_t dummy;
//    debug_puts("Started!\n");
    getcontext(&contexts[0]);
    getcontext(&contexts[1]);

    makecontext(&contexts[0], &stack1[0], 256, &dummy, low_task2);
    makecontext(&contexts[1], &stack2[0], 256, &dummy, low_task);
    getcontext(&dummy);
    start_system_timer();
    while (1);
//    volatile char in_char;
//
//    /* Output the prompt string */
//    debug_puts("\nPress any key and enter:\n> ");
//
//    /* Input one character but since line buffered, blocks until a CR. */
//    in_char = special_input_port;
//
//    /* Print the "what you entered:" message. */
//    debug_puts("\nYou entered: ");
//
//    /* now echo the rest of the characters */
//    do {
//        special_output_port = in_char;
//    } while ((in_char = special_input_port) != '\n');
//
//    special_output_port = '\n';
//    special_output_port = '\n';

    return 0;
}