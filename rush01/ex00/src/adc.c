#include "adc.h"

uint16_t read_adc10(uint8_t channel) {
    // Select ADC channel (0-7)
    ADMUX = 0;
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

void init_adc() {
    // Configure ADC
    
    ADCSRA = (1 << ADPS0); // Prescaler of 128
    ADCSRA = (1 << ADPS1); // Prescaler of 128
    ADCSRA = (1 << ADPS2); // Prescaler of 128
    ADCSRA |= (1 << ADEN); // Enable ADC
}