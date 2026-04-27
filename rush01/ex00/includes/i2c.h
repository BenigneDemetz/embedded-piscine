#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "embedded.h"

void init_i2c(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(uint8_t data);
uint8_t i2c_read(uint8_t ack);
uint8_t i2c_read_nack();
void expander_set(uint8_t expander_addr, uint8_t value, uint8_t port);
void init_expander();
uint8_t expander_read_PORT(uint8_t port);
uint8_t i2c_read_switch();
uint8_t slave_read(uint8_t slave_adr, uint8_t reg_addr);
void expander_set_leds(uint8_t expander_addr, uint8_t value);

#endif // I2C_H
