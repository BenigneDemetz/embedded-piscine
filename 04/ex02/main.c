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
    // Toggler la LED
    PORTB ^= (1 << PORTB0);
    
    // Activer le debouncing
    TCNT1 = 0;
    OCR1A = 3125;
    TIFR1 |= (1 << OCF1A);
    TIMSK1 |= (1 << OCIE1A);
    
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS10) | (1 << CS12);
    
    EIMSK &= ~(1 << INT0);  // Désactiver INT0 pendant debounce
}

int main() {
    DDRB |= (1 << DDB0); // LED 1 (PB0)
    DDRD &= ~(1 << DDD2); // Button input (PD2)
    PORTD |= (1 << PORTD2); // Enable pull-up resistor on button pin

    EIMSK |= (1 << INT0); // Enable interrupt INT0 (switch)
    EICRA |= (1 << ISC01); // Falling edge
    EICRA &= ~(1 << ISC00); // Falling edge
    SREG |= (1 << 7); // Global Interrupt Enable
    
    while (1) {
    }
}