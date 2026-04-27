#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


__attribute__((signal))
void TIMER1_COMPA_vect() {
    // Timer expired, re-enable external interrupt
    // PORTB ^= (1 << PORTB2); // Turn off status LED
    EIMSK |= (1 << INT0); // Re-enable external interrupt
    TIMSK1 &= ~(1 << OCIE1A); // Disable timer interrupt until next button press
    EIFR |= (1 << INTF0); // Clear any pending external interrupt
}

__attribute__((signal))
void INT0_vect() {
    static uint8_t previous_state = 0;
    // Toggler la LED
    if (previous_state == 0)
        PORTB ^= (1 << PORTB0);

    previous_state = !previous_state;
    
    // Activer le debouncing
    TCNT1 = 0;
    OCR1A = 3125; //50ms debounce
    TIFR1 |= (1 << OCF1A);
    TIMSK1 |= (1 << OCIE1A);
    
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12);
    
    EIMSK &= ~(1 << INT0);  // Désactiver INT0 pendant debounce
}

int main() {
    DDRB |= (1 << DDB0); // LED 1 (PB0)
    DDRD &= ~(1 << DDD2); // Button input (PD2)
    PORTD |= (1 << PORTD2); // Enable pull-up resistor on button pin

    EIMSK |= (1 << INT0); // Enable interrupt INT0 (switch)
    // EICRA |= (1 << ISC01); // Falling edge
    EICRA |= (1 << ISC00); // Falling edge
    SREG |= (1 << 7); // Global Interrupt Enable
    
    while (1) {
    }
}