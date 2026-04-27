#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void uart_init() {
    UBRR0H = 0;
    UBRR0L = 8; // 115200 baud rate with 16MHz clock, U2X0 enabled
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits
    UCSR0C &= ~(1 << USBS0); // 1 stop bit
}

void timer1_init() {
    TCCR1B |= (1 << WGM12); // CTC mode
    TCCR1B |= (1 << CS12);        // Prescaler 1024
    TCCR1B |= (1 << CS10);        // Prescaler 1024
    OCR1A = 31249; // 1 Hz interrupt frequency with 16MHz clock
    TIMSK1 |= (1 << OCIE1A); //enable compare with OCR1A
    TIFR1 &= ~(1 << OCF1A); //set interrupt flag to 0
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

__attribute__((signal))
void TIMER1_COMPA_vect() {
    uart_printstr("Hello World!\n\r");
}

int main() {

    uart_init();
    timer1_init();

    while (1) {}
}
