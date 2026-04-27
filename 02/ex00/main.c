#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#ifndef F_CPU
    #define F_CPU 16000000
#endif
#ifndef UART_BAUDRATE
    #define UART_BAUDRATE 115200
#endif

void uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = c; // Put data into buffer, sends the data
}

void uart_tx_string(const char *str) {
    while (*str) {
        uart_tx(*str++);
    }
}

void uart_init() {
    float baud_setting = (F_CPU / (float) (16UL * UART_BAUDRATE)) - 1;

    if (baud_setting - (int)baud_setting > 0.5) //math rounding
        baud_setting = (int)baud_setting + 1;
    UBRR0H = (int) baud_setting >> 8;
    UBRR0L = (int) baud_setting & 0xFF;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits, 1 stop bit
}

int main() {
    uart_init();
    while (1) {
        uart_tx('Z');   // Send 'Z' 
        _delay_ms(1000); // wait 1s
    }
}
