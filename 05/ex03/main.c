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

uint16_t read_adc10(uint8_t channel) {
    // Select ADC channel (0-7)
    ADMUX |= (channel & 0x0F); // select channel
    ADMUX |= (1 << REFS0); // AVcc reference, ADC0 input
    ADCH = 0;
    ADCL = 0;
    ADCSRA |= (1 << ADSC); // Start ADC conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    //clear the channel selection for the next read
    ADMUX &= ~(channel & 0x0F); // clear previous channel selection
    return ADC; // Read ADC value
}

void send_int(int16_t value) {
    if (value < 0) {
        uart_tx('-'); // Send a negative sign if the value is negative
        value = -value; // Make the value positive for further processing
    }
    if (value >= 1000) {
        uart_tx(((value / 1000) % 10) + '0'); // Send the thousands digit
    }
    if (value >= 100) {
        uart_tx(((value / 100) % 10) + '0'); // Send the hundreds digit
    }
    if (value >= 10) {
        uart_tx(((value / 10) % 10) + '0'); // Send the tens digit
    }
    uart_tx((value % 10) + '0'); // Send the ones digit
}

int16_t read_temp() {
    ADCSRA |= (1 << ADPS0); // Prescaler of 128
    ADCSRA |= (1 << ADPS1); // Prescaler of 128
    ADCSRA |= (1 << ADPS2); // Prescaler of 128
    ADMUX |= (1 << MUX3); // select channel
    ADMUX |= (1 << REFS0) | (1 << REFS1); // select internal 1.1V ref
    ADCH = 0;
    ADCL = 0;
    ADCSRA |= (1 << ADSC); // Start ADC conversion
    ADCSRA |= (1 << ADEN); // enable adc
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    
    return ADC-242-45; // Read ADC value

    // return (ADC - 324) * 100 / 122; // Read ADC value
    // return ADC;
}

int main() {

    uart_init();
    // init_potentiometer();

    while (1) {
        _delay_ms(20);
        int16_t adc_value = read_temp(); // Read ADC value // read for potentiometer
        //send in int
        send_int(adc_value);
        // uart_tx(','); // Send a newline character
        // uart_tx(' '); // Send a newline character
        // adc_value = read_adc10(1); // Read ADC value // read for LDR, light sensor
        // //send in hexadecimal
        // send_int(adc_value);
        // uart_tx(','); // Send a newline character
        // uart_tx(' '); // Send a newline character
        // adc_value = read_adc10(2); // Read ADC value // read for temperature sensor
        // //send in hexadecimal
        // send_int(adc_value);
        uart_tx('\n'); // Send a newline character
        uart_tx('\r'); // Send a newline character


    }
}
