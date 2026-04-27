#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_timer0() {
    // TCCR0A |= (1 << WGM00); // mode 7 Fast PWM with OCR0A as TOP
    TCCR0A |= (1 << WGM01); // mode 7 Fast PWM with OCR0A as TOP
    // TCCR0B |= (1 << WGM02); // mode 7 Fast PWM with OCR0A as TOP
    TCCR0B |= (1 << CS01) | (1 << CS00); // prescaler de 64
    OCR0A = 125; // 249 ticks = 1 ms
    TIMSK0 |= (1 << OCIE0A); // enable interrupt
    TIFR0 |= (1 << OCF0A); // clear any pending interrupt
}

void init_timer1() {
    //mode 10 Fast PWM with ICR1 as TOP
    TCCR1B |= (1 << WGM13); 
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << CS10) | (1 << CS11); //prescaler de 64
    ICR1 = 999; // 3125 ticks = 100 ms
    OCR1A = 0; // 3125 ticks = 100 ms
    //0s = 0
    //500ms = 999
    //1s = 0
    TCCR1A |= (1 << COM1A1); // toggle OC1A on compare match
}

__attribute__((signal, used))
void TIMER0_COMPA_vect() {
    static int8_t ms = 1;
    OCR1A += ms; // Increment or decrement OCR1A by 1
    if (OCR1A >= ICR1) {
        ms *= -1; // Toggle between 1 and -1
    }
    if (OCR1A <= 0) {
        ms *= -1; // Toggle between 1 and -1
    }
}

int main() {
    DDRB |= (1 << DDB1);
    SREG |= (1 << 7);
    init_timer0();
    init_timer1();
    
    while (1) {
    }
}
