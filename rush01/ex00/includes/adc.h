#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

uint16_t read_adc10(uint8_t channel);
void init_adc();

#endif // ADC_H
