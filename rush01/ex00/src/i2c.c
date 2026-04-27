#include "i2c.h"

void i2c_init(void)
{
    //TWBR = (F_CPU/SCL-16)/2*Prescaler
	//TWBR = (16000000/100000-16)/2*1 = 72
	TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2; // Set bit rate register for 100kHz SCL frequency
	TWSR = 0; // prescaler = 1 for the bitrate generator frequency
}

void i2c_start(void) // start communication as master and tries START condition
{
	TWCR = (1<<TWINT)|(1<<TWSTA); // TWINT cleared by one will start TWI op, TWSTA start condition, TWEN enable TWI
	TWCR |= (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

void i2c_stop(void) // stop communication as master
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); // TWINT cleared by one will start TWI op, TWEN enable TWI, TWSTO send stop condition
	while (TWCR & (1<<TWSTO)); // Wait until stop condition is executed on bus. TWSTO is cleared by hardware when bus released
}

void i2c_write(uint8_t data) //send data to slave
{
	TWDR = data; // Load data into TWDR register, data to send
	TWCR = (1<<TWINT)|(1<<TWEN); // TWINT cleared by a ONE. clear it starts TWI operations,
	//TWEN enable TWI, hardware will start transmission of data
	while (!(TWCR & (1<<TWINT))); //once communication is done, TWINT is set again by hardware, we can continue
}

uint8_t i2c_read(uint8_t ack) {
	if (ack)
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // ACK
	else
		TWCR = (1 << TWINT) | (1 << TWEN); // NACK
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t i2c_read_nack() {
	TWCR = (1 << TWINT) | (1 << TWEN); //check if nothing to read from slave
    while (!(TWCR & (1 << TWINT)));
    // return TWSR; //status code return
    return TWDR;
}

void expander_set(uint8_t expander_addr, uint8_t value, uint8_t port)
{
	i2c_start();
	i2c_write(expander_addr); // write
	i2c_write(port); // output port
	i2c_write(value);
	i2c_stop();
}

void init_expander()
{
	i2c_start();
	i2c_write(EXPANDER_ADR << 1); // write
	i2c_write(0x06); // config port 0
	i2c_write(0b00000001); // only switch as input
	i2c_stop();

	i2c_start(); // 7 segments
	i2c_write(EXPANDER_ADR << 1); // write
	i2c_write(0x07); // config port 1
	i2c_write(0b00000000); // only switch as input
	i2c_stop();
	expander_set(EXPANDER_ADR, 0b10001111, 2); // All LEDs off (active low) //PORTB equivalent
	expander_set(EXPANDER_ADR, 0b00000000, 3); // All LEDs off (active low) //PORTB equivalent
	// expander_set(EXPANDER_ADR, 0b00001111, 2); // All LEDs off (active low) //PORTB equivalent
}

uint8_t expander_read_PORT(uint8_t port) { //read switch status from expander
	i2c_start();
	i2c_write(EXPANDER_ADR << 1); // Send slave address with write bit to set the register pointer
	i2c_write(port); // Set register pointer to PORT0
	
	i2c_start();
	i2c_write(EXPANDER_ADR << 1 | 1); // Send slave address with read bit
	uint8_t data = i2c_read_nack(); // Read data from slave without sending ACK
	i2c_stop();
    
	return data;
}

uint8_t i2c_read_switch() {
    return (expander_read_PORT(0) & 0x01); // Return the state of the switch (bit 1 of PORT0)
}

void init_i2c() {
    TWBR = 72; // Set TWI clock to 100kHz (assuming 16MHz CPU) --> 16 + 2(72)*1 = 160. 16MHz/160 = 100kHz
    TWSR = 0; // on est directement à 100kHz, pas de prédivision
}

uint8_t slave_read(uint8_t slave_adr, uint8_t reg_addr) { //read switch status from slave
	i2c_start();
	i2c_write(slave_adr); // Send slave address with write bit to set the register pointer
	i2c_write(reg_addr); // Set register pointer
	
	i2c_start();
	i2c_write(slave_adr | 1); // Send slave address with read bit
	uint8_t data = i2c_read_nack(); // Read data from slave without sending ACK
	i2c_stop();
	return data;
}

void expander_set_leds(uint8_t expander_addr, uint8_t value)
{
	i2c_start();
	i2c_write(expander_addr); // write
	i2c_write(2); // output port 0
	i2c_write(value);
	i2c_stop();
}