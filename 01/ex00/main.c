#include <avr/io.h>
#include <util/delay.h>

void switch_on_led(int value) {
    PORTB |= (1 << value); //PORTB is the address to read tension from or write to the port. setting bit 0 at 1 will switch up led on PB0
}

void switch_off_led(int value) {
    PORTB &= ~(1 << value); //if |= (0... switch every leds off
}

void switch_led(int value) {
    PORTB ^= (1 << value); // with xor allows to swap the state
}

void delay(unsigned int ms) {
    unsigned int i = 0;
    while (i < ms) {
        for (int j = 0; j < 5000; j++) {
            DDRB = DDRB; //ne fait rien, juste perte de temps
        }
        i++;
    }
}

int main() {
    DDRB |= (1 << DDB1); //set led pin to write
    while (1) {
        switch_led(PORTB1); //switch state of led
        delay(250);
        // _delay_ms(500);
    }
}

