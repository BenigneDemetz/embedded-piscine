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
    DDRD |= (0 << DDD2); // set pin switch DDD2 as read
    DDRD |= (0 << DDD4); // set pin switch DDD4 as read
    unsigned char value = 0;
    unsigned char display_value = value;
    while (1) {

        unsigned char button1_state = PIND & (1 << PIND2); // read state of the button PIND2 with & (1 << PIND2) is a mask used to isolate the PIND2 value
        unsigned char button2_state = PIND & (1 << PIND4); //read snd button
        switch_off_led(DDB0, PORTB0);
        switch_off_led(DDB1, PORTB1);
        switch_off_led(DDB2, PORTB2);
        switch_off_led(DDB4, PORTB4);
        display_value = value;
        if (value >= 8) {
            display_value += 8;
        }
        PORTB |= (display_value);

        if (button1_state == 0) {
            value++;
            if (value == 16)
                value = 0;
            do {
                button1_state = PIND & (1 << PIND2);
            }
            while (button1_state == 0); // wait for the button to be released
            _delay_ms(50); //little delay while releasing the button
        }


        else if (button2_state == 0) {
            if (value == 0)
                value = 16;
            value--;
            do {
                button2_state = PIND & (1 << PIND4);
            }
            while (button2_state == 0); // wait for the button to be released
            _delay_ms(50); //little delay while releasing the button
        }
    }
    return 0;
}

