#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void uart_init() {
    UBRR0H = 0;
    UBRR0L = 8; // 115200 baud rate with 16MHz clock, U2X0 enabled
    UCSR0B = (1 << TXEN0); // Enable transmitter
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits
    UCSR0C &= ~(1 << USBS0); // 1 stop bit
}

void uart_recv_init() {
    UBRR0H = 0;
    UBRR0L = 8; // 115200 baud rate with 16MHz clock, U2X0 enabled
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits
    UCSR0C &= ~(1 << USBS0); // 1 stop bit
    UCSR0B |= (1 << RXEN0); // Enable receiver
    UCSR0B |= (1 << RXCIE0); // Enable receive complete interrupt
    SREG |= (1 << 7); // flag for Global Interrupt
}

void uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = c; // Put data into buffer, sends the data
}

void uart_printstr(const char* str) {
    while (*str) {
        uart_tx(*str++);
    }
}

char uart_rx() {
    while (!(UCSR0A & (1 << RXC0))); // Wait for data to be received
    return UDR0; // Get and return received data from buffer
}

void uart_resend() {
    uart_tx(uart_rx()); // Echo the received character back
}

__attribute__((signal))
void USART_RX_vect() {
    uart_resend();
}

void connection() {
    static char state = 0; // 0: waiting for username, 1: waiting for password, 2: connected
    char *username = "chipMaster";
    char *password = "password";
    if (state == 0) {
        uart_printstr("Enter username and password");
        uart_printstr("Username :");
    }
}

int main() {

    uart_init();
    uart_recv_init();
    connection();
    while (1) {}
}
