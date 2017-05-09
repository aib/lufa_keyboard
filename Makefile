CC = avr-gcc
CP = cp

DEVICE = atmega32u4
PROGRAMMER = usbasp
PROGRAMMER_PORT = usb
F_CPU = 16000000
FUSE_H = 0b10011001
FUSE_L = 0b01011110

CFLAGS = -Wall -O2 -mmcu=$(DEVICE) -DF_CPU=$(F_CPU)UL

.PHONY: avrdude-test
avrdude-test:
	avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -v

.PHONY: program
program: main.hex
	avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -U flash:w:$<:i

.PHONY: program-fuses
program-fuses:
	avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

.PHONY: clean
clean:
	$(RM) *.o *.elf *.hex

%.elf: %.o
	$(CC) -s $(CFLAGS) -o $@ $^

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $^ $@

.PHONY: blink
blink: blink.hex
	$(CP) blink.hex main.hex
	avr-size main.hex
