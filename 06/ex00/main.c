#include "avr/io.h"
#include "util/twi.h"
#include "util/delay.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif
#define SCL_CLOCK 100000

void uart_init() {
    UBRR0H = 0;
    UBRR0L = 8; // 115200 baud rate with 16MHz clock, U2X0 enabled
    UCSR0B |= (1 << TXEN0); // Enable transmitter
    UCSR0B |= (1 << RXEN0); // Enable receiver
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits
    UCSR0C &= ~(1 << USBS0); // 1 stop bit
    
    UCSR0B |= (1 << RXCIE0); // Enable receive interrupt
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
}

void init() {
    uart_init();
    i2c_init();
}

int main() {
    init();
    SREG |= (1 << 7); // flag for global interrupt
    _delay_ms(40); //wait for measurement to be done

    while (1) {
        i2c_start();
        i2c_stop();
        _delay_ms(200);
    }
}
