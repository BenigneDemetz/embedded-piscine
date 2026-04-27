#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void uart_init() {
    UBRR0H = 0;
    UBRR0L = 8; // 115200 baud rate with 16MHz clock, U2X0 enabled
    UCSR0B = (1 << TXEN0); // Enable receiver and transmitter
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits
    UCSR0C &= ~(1 << USBS0); // 1 stop bit
}

void uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = c; // Put data into buffer, sends the data
}

void init_potentiometer() {
    // Configure ADC
    
    ADCSRA = (1 << ADPS0); // Prescaler of 128
    ADCSRA = (1 << ADPS1); // Prescaler of 128
    ADCSRA = (1 << ADPS2); // Prescaler of 128
    ADCSRA |= (1 << ADEN); // Enable ADC
}

uint8_t read_adc8(uint8_t channel) {
    // Select ADC channel (0-7)
    ADMUX |= (channel & 0x0F); // AVcc reference, left adjust, select channel
    ADMUX |= (1 << ADLAR); // Left adjust result for 8-bit resolution
    ADMUX |= (1 << REFS0); // AVcc reference, ADC0 input
    ADCH = 0;
    ADCL = 0;
    ADCSRA |= (1 << ADSC); // Start ADC conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    //clear the channel selection for the next read
    ADMUX &= ~(channel & 0x0F); // clear previous channel selection
    return ADCH; // Read ADC value (8-bit)
}

void send_hexa(uint8_t value) {
    // uart_tx('0'); // Send the high nibble as a character
    // uart_tx('x'); // Send the high nibble as a character
    if ((value >> 4) > 9) {
        uart_tx((value >> 4) - 10 + 'a'); // Send the high nibble as a character
    } else {
        uart_tx((value >> 4) + '0'); // Send the high nibble as a character
    }
    if ((value & 0x0F) > 9) {
        uart_tx((value & 0x0F) - 10 + 'a'); // Send the low nibble as a character
    } else {
        uart_tx((value & 0x0F) + '0'); // Send the low nibble as a character
    }
}

int main() {

    uart_init();
    init_potentiometer();

    while (1) {
        _delay_ms(20);
        uint16_t adc_value = read_adc8(0); // Read ADC value // read for potentiometer
        //send in hexadecimal
        send_hexa(adc_value);
        uart_tx(','); // Send a newline character
        uart_tx(' '); // Send a newline character
        adc_value = read_adc8(1); // Read ADC value // read for LDR, light sensor
        //send in hexadecimal
        send_hexa(adc_value);
        uart_tx(','); // Send a newline character
        uart_tx(' '); // Send a newline character
        adc_value = read_adc8(2); // Read ADC value // read for temperature sensor
        //send in hexadecimal
        send_hexa(adc_value);
        uart_tx('\n'); // Send a newline character
        uart_tx('\r'); // Send a newline character


    }
}
