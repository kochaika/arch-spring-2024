all: build asm
build:
	avr-gcc -g -O2 -mmcu=atmega328 -o simple.elf simple.c context.c
run:
	simulavr -d atmega328 -f simple.elf -W 0x20,- -R 0x22,- -m 50000000
asm:
	avr-gcc -g -O2 -mmcu=atmega328 -S -o simple.s simple.c
	avr-gcc -g -O2 -mmcu=atmega328 -S -o context.s context.c

debug:
	simulavr -d atmega328 -f simple.elf -W 0x20,- -R 0x22,- -g

test:
	$(MAKE) build
	simulavr -d atmega328 -f simple.elf -W 0x20,- -R 0x22,- -m 50000000 | grep 'PORTB=' > run-out
	diff run-out test_output
	printf "\033[0;32mPassed✓\033[0m\n"

clean:
	rm -rf *.elf *.s run-out