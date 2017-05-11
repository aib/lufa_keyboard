CC = avr-gcc
CP = cp
AVRDUDE = avrdude
AVRDUDE_OPTS =

ARCH = ARCH_AVR8
DEVICE = atmega32u4
PROGRAMMER = usbasp
PROGRAMMER_PORT = usb
F_CPU = 16000000
FUSE_H = 0b10011001
FUSE_L = 0b01011110

CFLAGS = -std=c99 -Wall -O2 -mmcu=$(DEVICE) -DF_CPU=$(F_CPU)UL -DF_USB=$(F_CPU)UL -DUSE_LUFA_CONFIG_HEADER -I.

.PHONY: avrdude-test
avrdude-test:
	$(AVRDUDE) $(AVRDUDE_OPTS) -p $(DEVICE) -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -v

.PHONY: program
program: main.hex
	$(AVRDUDE) $(AVRDUDE_OPTS) -p $(DEVICE) -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -U flash:w:$<:i

.PHONY: program-fuses
program-fuses:
	$(AVRDUDE) $(AVRDUDE_OPTS) -p $(DEVICE) -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

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

.PHONY: lufakey
lufakey: lufakey.hex
	$(CP) lufakey.hex main.hex
	avr-size main.hex

HIDClassDevice.o: LUFA/Drivers/USB/Class/Device/HIDClassDevice.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(@F) $^
USBController_AVR8.o: LUFA/Drivers/USB/Core/AVR8/USBController_AVR8.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(@F) $^
USBTask.o: LUFA/Drivers/USB/Core/USBTask.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(@F) $^
Endpoint_AVR8.o: LUFA/Drivers/USB/Core/AVR8/Endpoint_AVR8.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(@F) $^
EndpointStream_AVR8.o: LUFA/Drivers/USB/Core/AVR8/EndpointStream_AVR8.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(@F) $^
USBInterrupt_AVR8.o: LUFA/Drivers/USB/Core/AVR8/USBInterrupt_AVR8.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(@F) $^
DeviceStandardReq.o: LUFA/Drivers/USB/Core/DeviceStandardReq.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(@F) $^
Events.o: LUFA/Drivers/USB/Core/Events.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(@F) $^

lufakey.elf: lufakey.o descriptor.o HIDClassDevice.o USBController_AVR8.o USBTask.o Endpoint_AVR8.o EndpointStream_AVR8.o USBInterrupt_AVR8.o DeviceStandardReq.o Events.o
	$(CC) -s $(CFLAGS) -o $@ $^
