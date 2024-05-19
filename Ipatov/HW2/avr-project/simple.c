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

// Homework starts here
#define REG_NUM 32

typedef struct {
	uint8_t* stack;
	uint8_t registers[REG_NUM];
} context;

context context_1, context_2;


uint8_t switch_occurred = 0;
static void low_task() {
	while (!switch_occurred) {
		PORTB = 0;
	}
}

void save_stack(uint8_t* stack){
	uint8_t *sp = (uint8_t *)SP;
	uint16_t size = (uint8_t*)RAMEND - sp;
	stack = (uint8_t*)malloc(size);
	memcpy(stack, sp, size);
}


void restore_stack(uint8_t* stack){
	uint8_t *sp = (uint8_t *)SP;
	uint16_t size = (uint8_t*)RAMEND - sp;
	memcpy(sp, stack, size);
	free(stack);
}

void save_registers(uint8_t* registers){
	memcpy(registers, (uint8_t*)0, REG_NUM * sizeof(uint8_t));
}

void restore_registers(uint8_t* registers){
	memcpy((uint8_t*)0, registers, REG_NUM * sizeof(uint8_t));
}

void save_context(context* current_context){
	save_stack(current_context->stack);
	save_registers(current_context->registers);
}

void restore_context(context* new_context){
	restore_stack(new_context->stack);
	restore_registers(new_context->registers);
}

uint8_t *s1 = 0, *s2 = 0;

uint8_t context_id = 0;

void change_context(context *current_context, context *new_context) {
	save_context(current_context);
	if (switch_occurred) {
		restore_context(new_context);
	}
}

ISR(TIMER0_COMPA_vect) {
	cli();
	if (PORTB == 1) {
		debug_puts("1\n");
	}
	if (PORTB == 0) {
		debug_puts("0\n");
	}
	if (context_id == 0) {
		context_id = 1;
		change_context(&context_1, &context_2);
	} else {
		context_id = 0;
		change_context(&context_2, &context_1);
	}
	if (!switch_occurred) {
		switch_occurred = 1;
	}
	sei();
}

static void high_task() {
	while (1) {
		PORTB = 1;
	}	
}

int main() {
	start_system_timer();
	low_task();
	high_task();
	return 0;
}
