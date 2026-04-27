/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   embedded.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeflers <adeflers@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 13:59:19 by adeflers          #+#    #+#             */
/*   Updated: 2026/04/25 13:59:19 by adeflers         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EMBEDDED_H
# define EMBEDDED_H


# include <avr/io.h>
# include <util/delay.h>
# include <stdint.h>
# include <avr/interrupt.h>
#include "7seg.h"
#include "adc.h"
#include "i2c.h"
#include "sensors.h"
#include "uart.h"
#include "rtc.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif
#ifndef SCL_CLOCK
#define SCL_CLOCK 100000
#endif

extern volatile uint8_t mode;
extern volatile int8_t state;
extern volatile uint8_t launch_animation;
extern volatile int16_t val_7seg[4]; //save number to display for each 7seg
extern volatile int8_t receive_uart;
extern volatile uint8_t switch_state; // 1 means not pressed, 0 means pressed

# define D1				PB0
# define D2				PB1
# define D3				PB2
# define D4				PB4

# define LED_RGB_BLUE	PD3
# define LED_RGB_RED	PD5
# define LED_RGB_GREEN	PD6

#define EXPANDER_ADR 0x20 // I2C address of the expander
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;


void	init_setup();
void	timer0_init();
void	buttons_interrupt_init();
void	clear_lights();
void	update_leds(uint8_t mode);
void    check_switchs();
void	spi_init();
void	spi_transmit(uint8_t data);
void	update_3_leds(RGB_t led1, RGB_t led2, RGB_t led3);

void	mode4();
void	mode5();

#endif //EMBEDDED_H
