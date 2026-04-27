#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void switch_on_rgb(int direction, int value) {
    DDRD |= (1 << direction); //DDRB is the address to select input or output operation of DDBx qddress, here output - page 85 14.2
    PORTD |= (1 << value); //PORTB is the address to read tension from or write to the port. setting bit 0 at 1 will switch up led on PB0
}

void switch_off_rgb(int direction, int value) {
    DDRD |= (1 << direction); //DDRB is the address to select input or output operation of DDBx qddress, here output - page 85 14.2
    PORTD &= ~(1 << value); //PORTB is the address to read tension from or write to the port. setting bit 0 at 1 will switch up led on PB0
}

void switch_for_1s_rgb(int direction, int value) {
    switch_on_rgb(direction, value);
    _delay_ms(1000);
    switch_off_rgb(direction, value);
}

int main() {
    while (1) {
        switch_for_1s_rgb(DDD5, PORTD5); // red
        switch_for_1s_rgb(DDD6, PORTD6); // green
        switch_for_1s_rgb(DDD3, PORTD3); // blue
    }
}
