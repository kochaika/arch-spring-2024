#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
// Based on: https://www.nongnu.org/simulavr/simple_ex.html
/* This port corresponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((volatile char *)0x20))

/* This port corresponds to the "-R 0x22,-" command line option. */
#define special_input_port  (*((volatile char *)0x22))

#define true 1
#define false 0
#define bool int

/* Poll the specified string out the debug port. */
void debug_puts(const char *str) {
    const char *c;

    for(c = str; *c; c++)
        special_output_port = *c;
}

void start_system_timer(void)
{
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

void inline __attribute__((always_inline)) *get_stack_pointer() { return (void*)SP; }
void inline __attribute__((always_inline)) *get_stack_end_pointer() { return (void*)RAMEND; }

void inline __attribute__((always_inline)) push_registers_to_stack() {
    asm volatile(
            "push r0\n"
            "push r1\n"
            "push r2\n"
            "push r3\n"
            "push r4\n"
            "push r5\n"
            "push r6\n"
            "push r7\n"
            "push r8\n"
            "push r9\n"
            "push r10\n"
            "push r11\n"
            "push r12\n"
            "push r13\n"
            "push r14\n"
            "push r15\n"
            "push r16\n"
            "push r17\n"
            "push r18\n"
            "push r19\n"
            "push r20\n"
            "push r21\n"
            "push r22\n"
            "push r23\n"
            "push r24\n"
            "push r25\n"
            "push r26\n"
            "push r27\n"
            "push r28\n"
            "push r29\n"
            "push r30\n"
            "push r31\n"
            );
}

void inline __attribute__((always_inline)) pop_registers_from_stack() {
    asm volatile(
            "pop r31\n"
            "pop r30\n"
            "pop r29\n"
            "pop r28\n"
            "pop r27\n"
            "pop r26\n"
            "pop r25\n"
            "pop r24\n"
            "pop r23\n"
            "pop r22\n"
            "pop r21\n"
            "pop r20\n"
            "pop r19\n"
            "pop r18\n"
            "pop r17\n"
            "pop r16\n"
            "pop r15\n"
            "pop r14\n"
            "pop r13\n"
            "pop r12\n"
            "pop r11\n"
            "pop r10\n"
            "pop r9\n"
            "pop r8\n"
            "pop r7\n"
            "pop r6\n"
            "pop r5\n"
            "pop r4\n"
            "pop r3\n"
            "pop r2\n"
            "pop r1\n"
            "pop r0\n"
            );
}

void inline __attribute__((always_inline)) save_state_to(void ** target) {
    push_registers_to_stack();
    void* sp = get_stack_pointer();
    void* spe = get_stack_end_pointer();
    *target = malloc(spe - sp);
    memcpy(*target, sp, spe - sp);
}

bool did_we_just_do_memcpy = false;

void inline __attribute__((always_inline)) set_state(void* source) {
    if (source != 0 && did_we_just_do_memcpy == false) {
        void *sp = get_stack_pointer();
        void *spe = get_stack_end_pointer();
        did_we_just_do_memcpy = true;
        memcpy(sp, source, spe - sp);
        free(source);
    }
    did_we_just_do_memcpy = false;
    free(source);
    pop_registers_from_stack();
}

void do_magic(void ** target, void* source) {
    save_state_to(target);
    set_state(source);
}

void** to_set_zero = 0;

bool temp_pause_flag = false; // just to collect context of next while

void* state_low = NULL;
void* state_main = NULL;

// some sort of bug in CLion or something idk
__attribute__((unused)) bool main_context_collected = false;

ISR(TIMER0_COMPA_vect){
    cli();
    if (PORTB == 1) {
        debug_puts("1\n");
    } else {
        debug_puts("0\n");
    }
    if (main_context_collected == false) {
        // skip first cycle manually to collect second cycle context
        if (temp_pause_flag == false) {
            temp_pause_flag = true;
            do_magic(&state_low, state_main);
        } else {
            temp_pause_flag = false;
            to_set_zero = &state_low;
            main_context_collected = true;
            do_magic(&state_main, state_low);
        }
    } else {
        void** not_zero;
        void** other;
        if (state_low != 0) {
            not_zero = &state_low;
            other = &state_main;
        } else {
            not_zero = &state_main;
            other = &state_low;
        }
        to_set_zero = not_zero;
        do_magic(other, *not_zero);
    }
    if (to_set_zero != 0) {
        *to_set_zero = 0;
    }
    sei();
}

static void low_task() {
    while (temp_pause_flag == false) {
        PORTB = 0;
    }
}

int main(){
    start_system_timer();
    low_task();
    while (true) {
        PORTB = 1;
    }
    return 0;
}
