#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void switch_on_rgb(int direction, int value) {
    PORTD = (value << 3);
}

void switch_off_rgb(int direction, int value) {
    PORTD &= ~(value << 3); //PORTB is the address to read tension from or write to the port. setting bit 0 at 1 will switch up led on PB0
}

void switch_for_1s_rgb(int direction, int value) {
    switch_on_rgb(direction, value);
    _delay_ms(1000);
    switch_off_rgb(direction, value);
}

int main() {
    // GR B
    // 0100 4
    // 1000 8
    // 0001 1
    // 1100 12
    // 1001 9
    // 0101 5
    // 1101 13
    DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6); // set PD3, PD5 and PD6 as output
    uint8_t liste[] = {4, 8, 1, 12, 9, 5, 13};
    while (1) {
        for (int i = 0; i < sizeof(liste)/sizeof(uint8_t); i++) {
            switch_for_1s_rgb(liste[i], liste[i]); // green
        }
    }
}
