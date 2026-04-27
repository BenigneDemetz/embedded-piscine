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

void uart_recv_init() {
    UBRR0H = 0;
    UBRR0L = 8; // 115200 baud rate with 16MHz clock, U2X0 enabled
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits
    UCSR0C &= ~(1 << USBS0); // 1 stop bit
    UCSR0B |= (1 << RXEN0); // Enable receiver
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

void switch_off_rgb(int id) {
    DDRD &= ~(1 << id);
}

void switch_on_rgb(int id) {
    DDRD |= (1 << id);
}

void init_rgb() {
    //GREEN PD6
    DDRD |= (1 << DDD6);
    // Mode Fast PWM
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    // Sortie non inversée
    TCCR0A |= (1 << COM0A1);
    // Prescaler = 64
    TCCR0B |= (1 << CS01) | (1 << CS00);
    OCR0A = 0;

    //RED PD5
    // Mode Fast PWM
    DDRD |= (1 << DDD5);
    // Sortie non inversée
    TCCR0A |= (1 << COM0B1);
    OCR0B = 0;

    //BLUE PD3
    DDRD |= (1 << DDD3);
    // Mode Fast PWM
    TCCR2A |= (1 << WGM20) | (1 << WGM21);
    // Sortie non inversée
    TCCR2A |= (1 << COM2B1);
    // Prescaler = 64
    TCCR2B |= (1 << CS21) | (1 << CS20);
    OCR2B = 0;
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    if (r == 0)
        switch_off_rgb(DDD5);
    else
        switch_on_rgb(DDD5);
    if (g == 0)
        switch_off_rgb(DDD6);
    else
        switch_on_rgb(DDD6);
    if (b == 0)
        switch_off_rgb(DDD3);
    else
        switch_on_rgb(DDD3);

    OCR0B = r;
    OCR0A = g;
    OCR2B = b;
}

void wheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    }
    else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    }
    else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

void light_up_leds(uint8_t adc_value) {
    uint8_t led_count = adc_value / (float) 255 * 100;
    DDRB |= (DDB0 | DDB1 | DDB2 | DDB4); // Set leds as output
    PORTB &= ~(0b00010111); //turn off all leds
    // adc_value = adc_value >> 4; // keep only the 4 most significant bits
    // PORTB |= (adc_value & 7); // turn on status LED (PB4)
    // PORTB |= ((adc_value & 0b00001000) << 1); // turn on status LED (PB4)
	
	wheel(adc_value);
	// light_up_rgb(led_count);
    
    // Light up LEDs from PB0 to PB2 based on gauge level
    if (led_count >= 25) PORTB |= (1 << PB0);
    if (led_count >= 50) PORTB |= (1 << PB1);
    if (led_count >= 75) PORTB |= (1 << PB2);
    if (led_count >= 100) PORTB |= (1 << PB4);
}

int main() {

    uart_init();
    init_potentiometer();
	init_rgb();

    while (1) {
        uint8_t adc_value = read_adc8(0); // Read ADC value // read for potentiometer
        light_up_leds(adc_value);
    }
}

// 1100
// 1101
// 1110
// 1111
