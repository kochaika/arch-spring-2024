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


typedef struct {
//    uint8_t sreg;
    uint8_t gp_registers[32];
    uint8_t *stack;

} dump_t;

void dump_stack_and_registers(dump_t *dest) {

// DEBUG
//    char buffer [33];
//    itoa(SP, buffer, 10);
//    debug_puts(buffer);
// DEBUG


    uint8_t *sp = (uint8_t *) SP;
    for (uint8_t addr = 0; sp + addr < (uint8_t *) RAMEND; addr++) {
        uint8_t value = *(uint8_t * )(sp + addr);

        dest->stack[addr] = value;
    }

//    uint8_t regValue;
//    for (uint8_t i = 0; i < 32; i++) {
//        asm("mov %0, r%1" : "=r" (regValue) : "r" (i));

//        dest->gp_registers[i] = *((uint8_t *)(0x00) + i);
//    }
//
//    uint8_t * registers = dest->gp_registers;
//    memcpy(registers, (uint8_t *) 0, 32 * sizeof(uint8_t));
}

void restore_stack_and_registers(dump_t *src) {
    uint8_t *sp = (uint8_t *) SP;
    for (uint8_t addr = 0; sp + addr < (uint8_t *) RAMEND; addr++) {
        *(uint8_t * )(sp + addr) = src->stack[addr];
    }

//    for (uint8_t i = 0; i < 32; i++) {
//        *((uint8_t *)(0x00) + i) = src->gp_registers[i];
//    memcpy((uint8_t *) 0, src->gp_registers, 32 * sizeof(uint8_t));
//    }

}

dump_t dump1;
dump_t dump2;



ISR(TIMER0_COMPA_vect){
        cli();
        dump1.stack = (uint8_t*)malloc(RAMEND-SP);




        dump_stack_and_registers(&dump1);

        free(dump1.stack);

//        restore_stack_and_registers(&dump1);


        debug_puts("Interrupt!\n");
//        exit(0);

        sei();
}

void low_task(void) {
    while (wh) { // TODO why?
        PORTB = 0;
    }
}


int main() {
    start_system_timer();
    low_task();
    while (1) {
        PORTB = 1;
    }
    return 0;
}