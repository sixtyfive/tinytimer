# Makefile für das Projekt MFV-Wählscheibe
# (Schaltung zum Einbau in alte Telefone,
# ermöglicht Tonwahl trotz Fingerlochscheibe
# sowie programmierbare Kurzwahl
# (10 Nummern à 30 Ziffern)
# Bei ATtiny25 sollte auf 15 oder 20 Ziffern gestutzt werden

PROJECT = clocktimer
DEVICE  = attiny25
TOOLPREFIX = /opt/arduino/hardware/tools/avr
COMPILE = $(TOOLPREFIX)/bin/avr-gcc -Os -mmcu=$(DEVICE) -Wall
AVROBJCOPY = $(TOOLPREFIX)/bin/avr-objcopy
AVRSIZE = $(TOOLPREFIX)/bin/avr-size
AVROBJDUMP = $(TOOLPREFIX)/bin/avr-objdump
AVRDUDE = $(TOOLPREFIX)/bin/avrdude -C$(TOOLPREFIX)/etc/avrdude.conf -v -pattiny25 -cstk500v1 -P/dev/ttyUSB0 -b19200

all: $(PROJECT).hex size disasm

disasm: $(PROJECT).lst

$(PROJECT).elf: mfv2.c
	$(COMPILE) -o $@ $<

%.hex: $(PROJECT).elf
	$(AVROBJCOPY) -j .text -j .data -O ihex $< $@  # Ohne fuses und signatur - fuses werden im avrdude-Aufruf mitgegeben

size: $(PROJECT).elf
	@echo
	@$(AVRSIZE) -C --mcu=$(DEVICE) $<

%.lst: $(PROJECT).elf
	$(AVROBJDUMP) -d $< > $@

.PHONY: clean program fuse flash
clean:
	-rm -rf $(PROJECT).elf $(PROJECT).lst $(PROJECT).hex

flash: $(PROJECT).hex
	$(AVRDUDE) -U lfuse:w:0x5F:m -U hfuse:w:0xD5:m flash:w:$(PROJECT).hex:i # Mit fuses! (s.o.)
