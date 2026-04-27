COMPILE = avr-gcc
FLAGS = -mmcu=atmega328p -Os
FILES = main.c
BIN = main.bin
HEX = main.hex
F_CPU = 16000000
BAUD = 115200


all: hex flash

hex:
	$(COMPILE) $(FLAGS) -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(BAUD) -o $(BIN) $(FILES)
	avr-objcopy -O ihex $(BIN) $(HEX)

flash:
	avrdude -c arduino -p m328p -P /dev/ttyUSB* -b $(BAUD) -U flash:w:$(HEX)

clean:
	rm -f $(BIN) $(HEX)
