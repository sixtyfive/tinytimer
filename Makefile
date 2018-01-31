all: build fuse flash

build:
	pio run

fuse:
	# lfuse=0x62: 1MHz
	# lfuse=0xe2: 8MHz
	avrdude	-pattiny85 -cstk500v1 -P/dev/ttyUSB0 -b19200 -e -Uefuse:w:0xFF:m -Uhfuse:w:0xD7:m -Ulfuse:w:0x62:m

flash:
	avrdude -pattiny85 -cstk500v1 -P/dev/ttyUSB0 -b19200 -Uflash:w:.pioenvs/attiny85/firmware.hex:i
