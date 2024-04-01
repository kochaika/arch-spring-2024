build:
	avr-gcc -g -O2 -mmcu=atmega328 -o simple.elf simple.c

run:
	simulavr -d atmega328 -f simple.elf -W 0x20,- -R 0x22,- -T exit
