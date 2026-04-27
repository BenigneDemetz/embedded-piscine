#include "includes/leds.h"

#include <avr/io.h>

void TWI_init(void)
{
	//TWBR = (F_CPU/SCL-16)/2*Prescaler
	//TWBR = (16000000/100000-16)/2*1 = 72
	TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2; // Set bit rate register for 100kHz SCL frequency
	TWSR = 0; // prescaler = 1 for the bitrate generator frequency
}

void TWI_start(void) // start communication as master and tries START condition
{
	TWCR = (1<<TWINT)|(1<<TWSTA); // TWINT cleared by one will start TWI op, TWSTA start condition, TWEN enable TWI
	TWCR |= (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

void TWI_stop(void) // stop communication as master
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); // TWINT cleared by one will start TWI op, TWEN enable TWI, TWSTO send stop condition
	while (TWCR & (1<<TWSTO)); // Wait until stop condition is executed on bus. TWSTO is cleared by hardware when bus released
}

void TWI_write(uint8_t data) //send data to slave
{
	TWDR = data; // Load data into TWDR register, data to send
	TWCR = (1<<TWINT)|(1<<TWEN); // TWINT cleared by a ONE. clear it starts TWI operations,
	//TWEN enable TWI, hardware will start transmission of data
	while (!(TWCR & (1<<TWINT))); //once communication is done, TWINT is set again by hardware, we can continue
}

uint8_t TWI_read_ack(void) // read from slave and request more data (ACK)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA); // TWINT cleared by a ONE. clear it starts TWI operations, TWEN enable TWI, TWEA to receive more data from slave
	while (!(TWCR & (1<<TWINT))); //wait for send on bus
	return TWDR; //return read data
}

uint8_t TWI_read_nack() {
	TWCR = (1 << TWINT) | (1 << TWEN); //check if nothing to read from slave
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t expander_read_PORT0(uint8_t expander_adr) { //read switch status from expander
	TWI_start();
	TWI_write(expander_adr); // Send slave address with write bit to set the register pointer
	TWI_write(0x00); // Set register pointer to PORT0
	
	TWI_start();
	TWI_write(expander_adr | 1); // Send slave address with read bit
	uint8_t data = TWI_read_nack(); // Read data from slave without sending ACK
	TWI_stop();
	return data;
}

void expander_init(uint8_t expander_addr)
{
	TWI_start();
	TWI_write(expander_addr); // write
	TWI_write(0x06); // config port 0
	TWI_write(0b00000001); // only switch as input
	TWI_stop();
}

void expander_set_leds(uint8_t expander_addr, uint8_t value)
{
	TWI_start();
	TWI_write(expander_addr); // write
	TWI_write(2); // output port 0
	TWI_write(value);
	TWI_stop();
}

void leds_init(void)
{
	TWI_init(); //init I2C communication with speed
	expander_init(EXPANDER01_ADDR); //DDRB equivalent
	expander_init(EXPANDER02_ADDR);
	expander_set_leds(EXPANDER01_ADDR, 0b00001110); // All LEDs off (active low) //PORTB equivalent
	expander_set_leds(EXPANDER02_ADDR, 0b00001110); // All LEDs off (active low)
}
