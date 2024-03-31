#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
// Based on: https://www.nongnu.org/simulavr/simple_ex.html
/* This port corresponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((volatile char *)0x20))

/* This port corresponds to the "-R 0x22,-" command line option. */
#define special_input_port (*((volatile char *)0x22))

/* Poll the specified string out the debug port. */
void debug_puts(const char *str) {
  const char *c;

  for (c = str; *c; c++)
    special_output_port = *c;
}

void start_system_timer(void) {
  TCNT0 = 0;                          // Value of counter
  OCR0A = 250;                        // Divisor. 1ms on 4MHz, CTC mode
  TCCR0B = (1 << CS01) | (1 << CS00); // clkT0S/64 (From prescaler)
  TCCR0A = (1 << WGM01);
  TIMSK0 =
      (1 << OCIE0A); // Timer/Counter0 Output Compare Match Interrupt Enable

  // button
  PCICR |= (1 << PCIE2);   // set PCIE0 to enable PCMSK0 scan
  PCMSK2 |= (1 << PCINT2); // set PCINT0 to trigger an interrupt on state change

  sei();
}

uint8_t wh = 1;
static void low_task() {
  while (wh) {
    PORTB = 0;
  }
}

static void low_task2() {
  while (1) {
    PORTB = 1;
  }
}

#define STACK_SIZE 14
uint8_t stack_task1[STACK_SIZE];
uint8_t stack_task2[STACK_SIZE];

uint8_t cur_ctx = 0;
uint8_t first_switch = 1;

volatile void save_stack(uint8_t *dest) {
  uint8_t *sp = SP;
  for (int i = 0; i < STACK_SIZE; i++) {
    dest[i] = *(sp + i);
  }
}

volatile void restore_stack(uint8_t *src) {
  uint8_t *sp = SP;
  for (int i = 0; i < STACK_SIZE; i++) {
    *(sp + i) = src[i];
  }
}

ISR(TIMER0_COMPA_vect) {
  cli();
  if (PORTB == 1)
    debug_puts("1\n");
  if (PORTB == 0)
    debug_puts("0\n");
  if (first_switch) {
    cur_ctx = 1;
    first_switch = 0;
    wh = 0;
    save_stack(stack_task1);
  } else if (cur_ctx == 0) {
    cur_ctx = 1;
    wh = 1;
    save_stack(stack_task1);
    restore_stack(stack_task2);
  } else {
    cur_ctx = 0;
    wh = 1;
    save_stack(stack_task2);
    restore_stack(stack_task1);
  }
  // debug_puts("Interrupt!\n");
  sei();
}

/* Main for test program.  Enter a string and echo it. */
int main() {
  start_system_timer();
  low_task();
  while (1) {
    PORTB = 1;
  }

  return 0;
}