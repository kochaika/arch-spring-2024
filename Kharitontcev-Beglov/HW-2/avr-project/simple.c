#include <avr/io.h>
#include <avr/interrupt.h>
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

ISR(TIMER0_COMPA_vect){
    cli();
    debug_puts("Interrupt!\n");
    sei();
}

/* Main for test program.  Enter a string and echo it. */
int main() {
//    start_system_timer();
//    while(1){
//
//    }
    volatile char in_char;

    /* Output the prompt string */
    debug_puts("\nPress any key and enter:\n> ");

    /* Input one character but since line buffered, blocks until a CR. */
    in_char = special_input_port;

    /* Print the "what you entered:" message. */
    debug_puts("\nYou entered: ");

    /* now echo the rest of the characters */
    do {
        special_output_port = in_char;
    } while((in_char = special_input_port) != '\n');

    special_output_port = '\n';
    special_output_port = '\n';

    return 0;
}