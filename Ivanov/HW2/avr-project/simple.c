#include <assert.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <string.h>
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

uint8_t *s1 = 0, *s2 = 0;

uint8_t cur_ctx = 0;
uint8_t first_switch = 1;

void change_stack(uint8_t **stack1, uint8_t *stack2) {
  asm volatile("push r0  \n\t"
               "push r1  \n\t"
               "push r2  \n\t"
               "push r3  \n\t"
               "push r4  \n\t"
               "push r5  \n\t"
               "push r6  \n\t"
               "push r7  \n\t"
               "push r8  \n\t"
               "push r9  \n\t"
               "push r10 \n\t"
               "push r11 \n\t"
               "push r12 \n\t"
               "push r13 \n\t"
               "push r14 \n\t"
               "push r15 \n\t"
               "push r16 \n\t"
               "push r17 \n\t"
               "push r18 \n\t"
               "push r19 \n\t"
               "push r20 \n\t"
               "push r21 \n\t"
               "push r22 \n\t"
               "push r23 \n\t"
               "push r24 \n\t"
               "push r25 \n\t"
               "push r26 \n\t"
               "push r27 \n\t"
               "push r28 \n\t"
               "push r29 \n\t"
               "push r30 \n\t"
               "push r31 \n\t");
  uint8_t *sp = (uint8_t *)SP;
  uint16_t sz = (uint8_t *)RAMEND - sp;
  *stack1 = (uint8_t *)malloc(sz);
  assert(*stack1 != 0);
  uint8_t *stp = *stack1;
  for (uint16_t i = 0; i < sz; i++) {
    stp[i] = sp[i];
  }
  if (s2 != 0) {
    // not the 1st switch
    sp = (uint8_t *)SP;
    sz = (uint8_t *)RAMEND - sp;
    for (uint16_t i = 0; i < sz; i++) {
      sp[i] = stack2[i];
    }
    free(stack2);
  }
  asm volatile("pop r31  \n\t"
               "pop r30  \n\t"
               "pop r29  \n\t"
               "pop r28  \n\t"
               "pop r27  \n\t"
               "pop r26  \n\t"
               "pop r25  \n\t"
               "pop r24  \n\t"
               "pop r23  \n\t"
               "pop r22  \n\t"
               "pop r21  \n\t"
               "pop r20  \n\t"
               "pop r19  \n\t"
               "pop r18  \n\t"
               "pop r17  \n\t"
               "pop r16  \n\t"
               "pop r15  \n\t"
               "pop r14  \n\t"
               "pop r13  \n\t"
               "pop r12  \n\t"
               "pop r11  \n\t"
               "pop r10  \n\t"
               "pop r9   \n\t"
               "pop r8   \n\t"
               "pop r7   \n\t"
               "pop r6   \n\t"
               "pop r5   \n\t"
               "pop r4   \n\t"
               "pop r3   \n\t"
               "pop r2   \n\t"
               "pop r1   \n\t"
               "pop r0   \n\t");
}

ISR(TIMER0_COMPA_vect) {
  cli();
  if (PORTB == 1)
    debug_puts("1\n");
  if (PORTB == 0)
    debug_puts("0\n");
  if (cur_ctx == 0) {
    cur_ctx = 1;
    wh = 1;
    change_stack(&s1, s2);
  } else {
    cur_ctx = 0;
    wh = 1;
    change_stack(&s2, s1);
  }
  if (first_switch) {
    first_switch = 0;
    wh = 0;
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
