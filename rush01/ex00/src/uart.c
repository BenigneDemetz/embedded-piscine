#include "embedded.h"

void init_uart() {
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

char uart_rx() {
    while (!(UCSR0A & (1 << RXC0))); // Wait for data to be received
    return UDR0; // Get and return received data from buffer
}

void uart_tx_string(const char *str) {
    while (*str) {
        uart_tx(*str++);
    }
}


void uart_received() {
    static char buffer[100]; // Buffer to store received string
    char received = uart_rx(); // Read the received byte
    uart_tx(received); // Echo the received byte back to the sender
    if (received == '\r') { // Check for carriage return to end the string
        uart_tx_string("\r\n"); // Echo newline for better formatting
        if (is_date(buffer)) {
            // If the received string is a valid date, set the RTC
            uint8_t hours, minutes, seconds, days, months, years;
            parse_date(buffer, &hours, &minutes, &seconds, &days, &months, &years);
            write_rtc(hours, minutes, seconds, days, months, years);
        }
        buffer[0] = '\0'; // Clear the buffer for the next string
    } else {
        // Shift the buffer to the left and add the new character at the end
        int i = 0;
        while (buffer[i] != '\0' && i < 99) {
            i++;
        }
        buffer[i] = received; // Add the received character to the buffer
        buffer[i + 1] = '\0'; // Null-terminate the string
    }
}

__attribute__((signal)) void USART_RX_vect() {
    receive_uart = 1;
}
