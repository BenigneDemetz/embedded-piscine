#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void switch_off_rgb(int id) {
    DDRD &= ~(1 << id);
}

void switch_on_rgb(int id) {
    DDRD |= (1 << id);
}

void init_rgb() {
    //GREEN PD6
    DDRD |= (1 << DDD6);
    // Mode Fast PWM
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    // Sortie non inversée
    TCCR0A |= (1 << COM0A1);
    // Prescaler = 64
    TCCR0B |= (1 << CS01) | (1 << CS00);
    OCR0A = 0;

    //RED PD5
    // Mode Fast PWM
    DDRD |= (1 << DDD5);
    // Sortie non inversée
    TCCR0A |= (1 << COM0B1);
    OCR0B = 0;

    //BLUE PD3
    DDRD |= (1 << DDD3);
    // Mode Fast PWM
    TCCR2A |= (1 << WGM20) | (1 << WGM21);
    // Sortie non inversée
    TCCR2A |= (1 << COM2B1);
    // Prescaler = 64
    TCCR2B |= (1 << CS21) | (1 << CS20);
    OCR2B = 0;
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    if (r == 0)
        switch_off_rgb(DDD5);
    else
        switch_on_rgb(DDD5);
    if (g == 0)
        switch_off_rgb(DDD6);
    else
        switch_on_rgb(DDD6);
    if (b == 0)
        switch_off_rgb(DDD3);
    else
        switch_on_rgb(DDD3);

    OCR0B = r;
    OCR0A = g;
    OCR2B = b;
}

void wheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    }
    else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    }
    else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

int main() {
    init_rgb();
    while (1) {
        for (int i = 0; i < 256; i++) {
            wheel(i);
            _delay_ms(10);
        }
    }
}
