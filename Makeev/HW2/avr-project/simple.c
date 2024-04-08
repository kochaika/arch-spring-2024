#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// Based on: https://www.nongnu.org/simulavr/simple_ex.html
/* This port corresponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((volatile char *)0x20))

/* This port corresponds to the "-R 0x22,-" command line option. */
#define special_input_port  (*((volatile char *)0x22))

/* Poll the specified string out the debug port. */
void debug_puts(const char *str) {
    const char *c;

    for(c = str; *c; c++)
        special_output_port = *c;
}


#define REG_NUM 32

typedef struct {
    uint8_t* stack;
    uint8_t registers[REG_NUM];
} state_t;

state_t state1, state2;

void save_stack(uint8_t* stack){
    uint8_t *sp = (uint8_t *)SP;
    uint8_t* ramend = (uint8_t*)RAMEND;
    stack = (uint8_t*)malloc(ramend - sp);
    memcpy(stack, sp, ramend - sp);
}


void restore_stack(uint8_t* stack){
    uint8_t *sp = (uint8_t *)SP;
    uint8_t* ramend = (uint8_t*)RAMEND;
    memcpy(sp, stack, ramend - sp);
    free(stack);
}



void save_registers(uint8_t* registers){
    memcpy(registers, (uint8_t*)0, REG_NUM * sizeof(uint8_t));
}

void restore_registers(uint8_t* registers){
    memcpy((uint8_t*)0, registers, REG_NUM * sizeof(uint8_t));
}


void save_state(state_t* state){
    save_stack(state->stack);
    save_registers(state->registers);
}

void restore_state(state_t* state){
    restore_stack(state->stack);
    restore_registers(state->registers);
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

#define MAIN_TASK 1
#define LOW_TASK 0

uint8_t low_task_flag = 1;
uint8_t current_context = LOW_TASK;
uint8_t first_switch = 1;

static void low_task() {
    while (low_task_flag) {
        PORTB = 0;
    }
}


ISR(TIMER0_COMPA_vect){
    cli();

    if (current_context == LOW_TASK) {
        debug_puts("switching to MAIN_TASK\n");
        current_context = MAIN_TASK;
        low_task_flag = 0;
        save_state(&state1);
        if (!first_switch)
            restore_state(&state2);
    } else if (current_context == MAIN_TASK) {
        debug_puts("switching to LOW_TASK\n");
        current_context = LOW_TASK;
        low_task_flag = 1;
        save_state(&state2);
        restore_state(&state1);
    }

    first_switch = 0;
    sei();
}

int main() {
    start_system_timer();
    low_task();

    while (1) {
        PORTB = 1;
    }

    return 0;
}