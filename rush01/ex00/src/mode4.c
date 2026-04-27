/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode4.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeflers <adeflers@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 17:01:33 by adeflers          #+#    #+#             */
/*   Updated: 2026/04/25 17:01:33 by adeflers         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "embedded.h"

void	mode4()
{
	int_to_7seg(4420);
	val_7seg[0] = 0b01000000;
	val_7seg[3] = 0b01000000;
	switch (state)
	{
		case 0:
			PORTD |= (1 << LED_RGB_RED); // Turn on blue LED_RGB_RED
			PORTD &= ~(1 << LED_RGB_GREEN); // Turn off blue LED_RGB_GREEN
			PORTD &= ~(1 << LED_RGB_BLUE); // Turn off blue LED_RGB_BLUE
			update_3_leds((RGB_t){255, 0, 0}, (RGB_t){255, 0, 0}, (RGB_t){255, 0, 0}); // Red */	
			break;
		case 1:
			PORTD |= (1 << LED_RGB_GREEN); // Turn on blue LED_RGB_GREEN
			PORTD &= ~(1 << LED_RGB_RED); // Turn off blue LED_RGB_RED
			PORTD &= ~(1 << LED_RGB_BLUE); // Turn off blue LED_RGB_BLUE
			update_3_leds((RGB_t){0, 255, 0}, (RGB_t){0, 255, 0}, (RGB_t){0, 255, 0}); // Green */
			break;
		case 2:
			PORTD |= (1 << LED_RGB_BLUE); // Turn on blue LED_RGB_BLUE
			PORTD &= ~(1 << LED_RGB_RED); // Turn off blue LED_RGB_RED
			PORTD &= ~(1 << LED_RGB_GREEN); // Turn off blue LED_RGB_GREEN
			update_3_leds((RGB_t){0, 0, 255}, (RGB_t){0, 0, 255}, (RGB_t){0, 0, 255}); // Blue */
			break;
	}
	if (state > 2)
		state = 0;
}

void spi_init(void)
{
    // 1. Configurer MOSI, SCK et SS en SORTIE
    // SS (PB2) doit impérativement être en sortie pour le mode Master
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2); // MOSI, SCK, SS en sortie.



    // 2. Activer le SPI, mode Master, définir la vitesse
    // SPE  : SPI Enable
    // MSTR : Master Mode
    // SPR0 : Vitesse (fosc/16). Avec 16MHz, ça donne 1MHz, très stable pour les LEDs.
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void spi_transmit(uint8_t data)
{
    // Charger la donnée dans le registre
    SPDR = data;

    // Attendre que la transmission soit terminée (le flag SPIF passe à 1)
    while (!(SPSR & (1 << SPIF)));
}

void update_3_leds(RGB_t led1, RGB_t led2, RGB_t led3) {

	// activer SPI pour contrôler les LEDs APA102
	spi_init();


    // 1. START FRAME (4 octets à 0)
    for (uint8_t i = 0; i < 4; i++) spi_transmit(0x00);

    // 2. LED 1
    spi_transmit(0xEF); // Luminosité Max (0xE0 | 31)
    spi_transmit(led1.b);
    spi_transmit(led1.g);
    spi_transmit(led1.r);

    // 3. LED 2
    spi_transmit(0xEF); // Luminosité Max (0xE0 | 31)
    spi_transmit(led2.b);
    spi_transmit(led2.g);
    spi_transmit(led2.r);

    // 4. LED 3
    spi_transmit(0xEF);
    spi_transmit(led3.b);
    spi_transmit(led3.g);
    spi_transmit(led3.r);

    // 5. END FRAME 
    // Pour APA102, on envoie (N/2) bits de 1 supplémentaires. 
    // Pour 3 LEDs, 4 octets a 1.
    for (uint8_t i = 0; i < 4; i++) spi_transmit(0xFF);

	SPCR &= ~(1 << SPE); // Disable SPI to manage conflict on D4
	DDRB |= (1 << PB2); // Set PB2 as output to control D4
}
void	mode5()
{
	PORTD &= ~(1 << LED_RGB_RED); // Turn off blue LED_RGB_RED
	PORTD &= ~(1 << LED_RGB_GREEN); // Turn off blue LED_RGB_GREEN
	PORTD &= ~(1 << LED_RGB_BLUE); // Turn off blue LED_RGB_BLUE
	update_3_leds((RGB_t){0, 0, 0}, (RGB_t){0, 0, 0}, (RGB_t){0, 0, 0}); // Off */
}


