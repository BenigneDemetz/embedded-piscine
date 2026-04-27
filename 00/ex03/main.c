#include <avr/io.h>
#include <util/delay.h>

void switch_on_led(int direction, int value) {
    DDRB |= (1 << direction); //DDRB is the address to select input or output operation of DDBx qddress, here output - page 85 14.2
    PORTB |= (1 << value); //PORTB is the address to read tension from or write to the port. setting bit 0 at 1 will switch up led on PB0
}

void switch_off_led(int direction, int value) {
    DDRB |= (1 << direction);
    PORTB &= ~(1 << value); //if |= (0... switch every leds off
}

void switch_led(int direction, int value) {
    DDRB |= (1 << direction);
    PORTB ^= (1 << value); // with xor allows to swap the state
}

int main() {
    DDRB |= (1 << DDB0); //set pin led as write
    DDRD |= (0 << DDD2); // set pin switch DDD2 as read
    while (1) {

        unsigned char button_state = PIND & (1 << PIND2); // read state of the button PIND2 with & (1 << PIND2) is a mask used to isolate the PIND2 value

        if (button_state == 0) {
            switch_led(DDB0, PORTB0);
            do {
                button_state = PIND & (1 << PIND2);
            }
            while (button_state == 0); // wait for the button to be released
            _delay_ms(50); //little delay while releasing the button
        }
    }
    return 0;
}
