#ifndef SENSORS_H
#define SENSORS_H

#include "i2c.h"
#include <avr/io.h>
#include <util/delay.h>

int16_t read_internal_temp();
void trigger_measurement(uint8_t slave_adr);
int32_t get_temp(uint8_t slave_adr, float *temp, float *humidity);

#endif // SENSORS_H
