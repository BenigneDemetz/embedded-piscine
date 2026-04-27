/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeflers <adeflers@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 19:47:48 by adeflers          #+#    #+#             */
/*   Updated: 2026/04/25 14:01:02 by adeflers         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "embedded.h"

void	clear_lights()
{
	PORTB &= ~(1 << D1); // Turn off D1
	PORTB &= ~(1 << D2); // Turn off D2
	PORTB &= ~(1 << D3); // Turn off D3
	PORTB &= ~(1 << D4); // Turn off D4
	
	PORTD &= ~(1 << LED_RGB_BLUE); // Turn off blue LED_RGB_BLUE
	PORTD &= ~(1 << LED_RGB_RED); // Turn off red LED_RGB_RED
	PORTD &= ~(1 << LED_RGB_GREEN); // Turn off green LED_RGB_GREEN
}

void	update_leds(uint8_t mode)
{
	(mode & 0x01) ? (PORTB |= (1 << PB0)) : (PORTB &= ~(1 << PB0)); // D1
	(mode & 0x02) ? (PORTB |= (1 << PB1)) : (PORTB &= ~(1 << PB1)); // D2
	(mode & 0x04) ? (PORTB |= (1 << PB2)) : (PORTB &= ~(1 << PB2)); // D3
	(mode & 0x08) ? (PORTB |= (1 << PB4)) : (PORTB &= ~(1 << PB4)); // D4
}
