#include <avr/io.h>

void switch_on_led(int direction, int value) {
    DDRB |= (1 << direction); //DDRB is the address to select input or output operation of DDBx qddress, here output - page 85 14.2
    PORTB |= (1 << value); //PORTB is the address to read tension from or write to the port. setting bit 0 at 1 will switch up led on PB0
}

int main() {
    switch_on_led(DDB0, PORTB0);
    return 0;
}

// page 84 atmega registres
// page 100 DDRB & PORTB
// pinx read only
