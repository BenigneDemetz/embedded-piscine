#ifndef SEVEN_SEG_H
# define SEVEN_SEG_H

#include "i2c.h"
#include "embedded.h"

void display_one_seg(uint8_t num, uint8_t value);
void int_to_7seg(int16_t value);
void switch_off_7seg();

#endif //7_SEG_H
