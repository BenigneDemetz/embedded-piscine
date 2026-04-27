#include "avr/io.h"
#include "util/twi.h"
#include "util/delay.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif
#define SCL_CLOCK 100000

void init_uart() {
    UBRR0H = 0;
    UBRR0L = 8; // 115200 baud rate with 16MHz clock, U2X0 enabled
    UCSR0B |= (1 << TXEN0); // Enable transmitter
    UCSR0B |= (1 << RXEN0); // Enable receiver
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits
    UCSR0C &= ~(1 << USBS0); // 1 stop bit

    UCSR0B |= (1 << RXCIE0); // Enable receive interrupt
    SREG |= (1 << 7); // flag for global interrupt
}

void uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = c; // Put data into buffer, sends the data
}

void uart_tx_string(const char *str) {
    while (*str) {
        uart_tx(*str++);
    }
}

void print_status(uint8_t status) {
    uart_tx_string("Status received: 0x");
    uart_tx((TWSR >> 4) + '0'); // Print TWI status code
    uart_tx((TWSR & 0x0F) + '0'); // Print TWI status code
    uart_tx_string("\r\n");
}

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
    print_status(TWSR);
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
    print_status(TWSR);
}

uint8_t i2c_read(uint8_t ack) {
	if (ack)
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // ACK
	else
		TWCR = (1 << TWINT) | (1 << TWEN); // NACK
    while (!(TWCR & (1 << TWINT)));
    print_status(TWSR);
    return TWDR;
}

uint8_t i2c_read_nack() {
	TWCR = (1 << TWINT) | (1 << TWEN); //check if nothing to read from slave
    while (!(TWCR & (1 << TWINT)));
    // return TWSR; //status code return
    print_status(TWSR);
    return TWDR;
}

void init_i2c() {
    TWBR = 72; // Set TWI clock to 100kHz (assuming 16MHz CPU) --> 16 + 2(72)*1 = 160. 16MHz/160 = 100kHz
    TWSR = 0;
}

void init() {
    init_uart();
    init_i2c();
}

uint8_t temp_connection_check(uint8_t slave_adr) { //check if slave is connected by trying to read the temperature)
    i2c_start();
    i2c_write(slave_adr | 1); // Send slave address with write bit to set the register pointer
    i2c_write(0x71); // Send slave address
    uint8_t data = i2c_read_nack(); // Read data from slave without sending ACK
    i2c_stop();
    return data;
    // if (data & (1 << 3)) {
    //     return 1; // Slave is connected
    // } else {
    //     return 0; // Slave is not connected
    // }
}

void init_temp(uint8_t slave_adr) {
    i2c_start();
    i2c_write(slave_adr);
    i2c_write(0xbe);
    i2c_write(0x08);
    i2c_write(0x00);
    _delay_ms(10); //wait for init to be done
    i2c_stop();
}

void trigger_measurement(uint8_t slave_adr) {
    i2c_start();
    i2c_write(slave_adr);
    i2c_write(0b10101100);
    i2c_write(0b00110011);
    i2c_write(0b00000000);
    i2c_stop();
}

void get_temp(uint8_t slave_adr, int8_t *buf) { //read slave temperature
    // debug 0xb6
    if (temp_connection_check(slave_adr) != 0x18) {
        init_temp(slave_adr);
    }
    _delay_ms(10);
    
    trigger_measurement(slave_adr);
    
    _delay_ms(80); //wait for measurement to be done
    i2c_start();
    i2c_write(slave_adr | 1); // Send slave address with read bit
    for(int i=0;i<7;i++) {
        // Send ACK for first 5 bytes, NACK for last (6th) byte
        uint8_t ack = (i < 6) ? 1 : 0;
        buf[i] = i2c_read(ack);
    }
    i2c_stop();
}

void print_hex_value(char c) {
    // Print the high nibble
    char high_nibble = (c >> 4) & 0x0F;
    if (high_nibble < 10) {
        uart_tx(high_nibble + '0');
    } else {
        uart_tx(high_nibble - 10 + 'A');
    }
    // Print the low nibble
    char low_nibble = c & 0x0F;
    if (low_nibble < 10) {
        uart_tx(low_nibble + '0');
    } else {
        uart_tx(low_nibble - 10 + 'A');
    }

}

int main() {
    int8_t buf[7];

    init();
    _delay_ms(200); //wait for measurement to be done
    init_temp(0b01110000);
    _delay_ms(200); //wait for measurement to be done

    while (1) {
        get_temp(0b01110000, buf);
        for (int i = 0; i < 7; i++) {
            print_hex_value(buf[i]);
            uart_tx(' ');
        }
        uart_tx_string("\r\n");
        _delay_ms(2000); //wait for measurement to be done
    }
}
