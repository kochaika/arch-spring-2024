build:
	avr-gcc -g -O2 -mmcu=atmega328 -o simple.elf simple.c

run:
	simulavr -d atmega328 -f simple.elf -W 0x20,- -R 0x22,- -m 50000000

test : clean build
	simulavr -d atmega328 -f simple.elf -W 0x20,- -R 0x22,- -m 50000000 | head -n -2  > test/001.result
	diff test/001.result test/001.expected
	echo "OK: 001"

clean:
	rm -f *.elf test/*.result 

.PHONY: test
