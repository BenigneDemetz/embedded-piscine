/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeflers <adeflers@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 19:50:10 by adeflers          #+#    #+#             */
/*   Updated: 2026/04/25 14:02:09 by adeflers         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "embedded.h"

// timer0 set to control the debounce of the button SW1, with an interrupt every 50ms to check if the button is still pressed
void	timer0_init()
{
	TCNT0 = 0; // Initialize timer0 counter to 0
	TCCR0A |= (1 << WGM01); // Set CTC mode
	TCCR0B = 0;
	OCR0A = 250; // Set the compare match value for a debounce time of ~16ms

	TIMSK0 |= (1 << OCIE0A); // Enable interrupt for timer 0
}

// external interrupt set to detect the button SW1 press, with an interrupt on the falling edge (when the button is pressed)
void buttons_interrupt_init(void)
{
	// SW1 is connected to PD2, which corresponds to INT0
    EICRA = (1 << ISC01); // Configure INT0 to trigger on the falling edge (when the button is pressed)
    EIFR = (1 << INTF0);  // initial clear any pending interrupt on INT0
    EIMSK |= (1 << INT0); // int0 interrupt enabled
	
	// SW2 is connected to PD4, which corresponds to PCINT20
	PCICR |= (1 << PCIE2); // Enable pin change interrupt for PCINT20 (PD4)
	PCMSK2 |= (1 << PCINT20); // Configure pin change interrupt for PD4 (SW2)
}

// initialisation of the ports for the LEDs and the button
void	init_setup()
{
	DDRB |= (1 << D1); // Set D1 as an output
	DDRB |= (1 << D2); // Set D2 as an output
	DDRB |= (1 << D3); // Set D3 as an output
	DDRB |= (1 << D4); // Set D4 as an output
	
	// Set RGB LED pins as outputs
	DDRD |= (1 << LED_RGB_BLUE);
	DDRD |= (1 << LED_RGB_RED);
	DDRD |= (1 << LED_RGB_GREEN);

	DDRD &= ~(1 << PD2); // Set SW1 as an input
	PORTD |= (1 << PD2); // Enable pull-up resistor on SW1
	
	DDRD &= ~(1 << PD4); // Set SW2 as an input
	PORTD |= (1 << PD4); // Enable pull-up resistor on SW2
}
