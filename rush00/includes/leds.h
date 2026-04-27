#include "avr/io.h"
#include "util/delay.h"

#ifndef F_CPU
	#define F_CPU 16000000
#endif
#ifndef MASTER
	#define MASTER 0
#endif
#define SCL_CLOCK 100000L

#define EXPANDER01_ADDR 0x40 // 1st expander address 0100 0000
#define EXPANDER02_ADDR 0x42 // 2nd expander address 0100 0010
#define SLAVE_ADDR 0x50 // slave address 0101 0000
#define RED 0b00000100
#define YELLOW 0b00000010
#define GREEN 0b00000001

void leds_init(void);

void expander_set_leds(uint8_t expander_addr, uint8_t value);

void TWI_start(void);
void TWI_stop(void);
void TWI_write(uint8_t data);


uint8_t expander_read_PORT0(uint8_t expander_adr);

uint8_t expander_read_switch(uint8_t expander_adr);
