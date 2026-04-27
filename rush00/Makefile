COMPILE = avr-gcc
FLAGS = -mmcu=atmega328p -Os
FILES = *.c
BIN = main.bin
HEX = main.hex
F_CPU = 16000000
BAUD = 115200
MASTER = 1


all: hex1 flash1 hex2 flash2


hex1:
	$(COMPILE) $(FLAGS) -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(BAUD) -DMASTER -o $(BIN) $(FILES)
	avr-objcopy -O ihex $(BIN) $(HEX)

flash1:
	avrdude -c arduino -p m328p -P /dev/ttyUSB3 -b $(BAUD) -U flash:w:$(HEX)

hex2:
	$(COMPILE) $(FLAGS) -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(BAUD) -o $(BIN) $(FILES)
	avr-objcopy -O ihex $(BIN) $(HEX)

flash2:
	avrdude -c arduino -p m328p -P /dev/ttyUSB4 -b $(BAUD) -U flash:w:$(HEX)


clean:
	rm -f $(BIN) $(HEX)
