/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buttons.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeflers <adeflers@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 15:36:44 by adeflers          #+#    #+#             */
/*   Updated: 2026/04/25 15:36:44 by adeflers         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "embedded.h"

// ce que l on fait qd le bouton SW1 est pressé : on incremente le mode
void	__vector_1(void) __attribute__ ((signal, used, externally_visible));
void	__vector_1(void)
{
    /* TIMSK1 &= ~(0 << OCIE1A); // disable timer compare interrupt */
	EIMSK &= ~(1 << INT0); // Disable INT0 interrupt to prevent multiple triggers while the button is pressed
	TCNT0 = 0; // reset timer0 counter to 0 when the button is pressed
	TCCR0B |= (1 << CS02) | (1 << CS00); // Start timer0 with a prescaler of 1024 to debounce the button
	
	if (!launch_animation)
		mode = (mode == 10) ? 0 : (mode + 1); // Increment mode, wrap around to 0 if it goes above 10
	state = 0; // reset state to 0 when the mode is changed
	TCNT1 = 0; // reset timer1 counter to 0 when the mode is activated
	launch_animation = 0;
	update_leds(mode);
}

// ce que l on fait qd le bouton SW2 est pressé : on decremente le mode
void	__vector_5(void) __attribute__ ((signal, used, externally_visible));
void	__vector_5(void)
{
    /* TIMSK1 &= ~(0 << OCIE1A); // disable timer compare interrupt */
	if (!(PIND & (1 << PD4)) && (PCMSK2 & (1 << PCINT20))) // Check if SW2 is pressed and its interrupt is enabled
	{
		PCMSK2 &= ~(1 << PCINT20); // Disable pin change interrupt for PD4 to prevent multiple triggers while the button is pressed
		TCNT0 = 0; // reset timer1 counter to 0 when the button is pressed
		TCCR0B |= (1 << CS02) | (1 << CS00); // Start timer1 with a prescaler of 1024 to debounce the button
		
		if (!launch_animation)
			mode = (mode == 0) ? 10 : (mode - 1); // Decrement mode, wrap around to 10 if it goes below 0
		state = 0; // reset state to 0 when the mode is changed
		TCNT1 = 0; // reset timer1 counter to 0 when the mode is activated
		
	}
	launch_animation = 0;
	update_leds(mode);
}

// ce que l on fait qd le timer0 atteint la valeur de comparaison : on vérifie si le bouton SW1 est toujours pressé pour éviter les rebonds, et si le bouton est relâché depuis au moins 2 checks (~30ms), on réactive l'interruption INT0 pour permettre de nouveaux appuis sur le bouton
void	__vector_14(void) __attribute__ ((signal, used, externally_visible));
void	__vector_14(void)
{	
	static uint8_t count1 = 0;
    static uint8_t count2 = 0;

    // Check SW1 (PD2)
    if (PIND & (1 << PD2))
	{
        if (count1 < 2)
			count1++;
		else
			count1 = 0;
    }

    // Check SW2 (PD4)
    if (PIND & (1 << PD4))
	{
        if (count2 < 2)
			count2++;
		else
			count2 = 0;
    }

    // Réactivation INDÉPENDANTE
    if (count1 >= 2)
	{
        EIFR |= (1 << INTF0);
        EIMSK |= (1 << INT0);
    }
    if (count2 >= 2)
	{
        PCIFR |= (1 << PCIF2);
        PCMSK2 |= (1 << PCINT20);
    }

    // On arrête le timer si les deux sont relâchés (ou si l'un est déjà actif)
    // Pour éviter le blocage, on vérifie si les interruptions sont revenues à l'état initial
    if ((EIMSK & (1 << INT0)) && (PCMSK2 & (1 << PCINT20)))
	{
        TCCR0B = 0;
        count1 = 0;
        count2 = 0;
    }
}

void check_switchs() {
    switch_state = i2c_read_switch();
    uint8_t current_value = 255;
    if (PIND & (1 << PD2)) { // Check if SW1 is pressed
        //set to one the four left bits
        current_value |= (1 << 3); // Write segment value to display the number
    }
    else {
        current_value &= ~(1 << 3); // Write segment value to display the number
    }
    if (PIND & (1 << PD4)) { // Check if SW2 is pressed
        current_value |= (1 << 2); // Write segment value to display the number
    }
    else {
        current_value &= ~(1 << 2); // Write segment value to display the number
    }
    if (switch_state) { // Check if SW3 is pressed
        current_value |= (1 << 1); // Write segment value to display the number
    }
    else {
        current_value &= ~(1 << 1); // Write segment value to display the number
    }
    // uart_tx_string("current value: ");
    // for (int i = 7; i >= 0; i--) {
    //     uart_tx((current_value & (1 << i)) ? '1' : '0');
    // }
    // uart_tx_string("\r\n");

    i2c_start();
    i2c_write(EXPANDER_ADR << 1); // Write to expander
    i2c_write(2); // Set register pointer to output port for the display
    i2c_write(current_value); // Write segment value to display the number
    i2c_stop();
}
