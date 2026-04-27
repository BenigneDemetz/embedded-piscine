#include <avr/io.h>

int main() {
    DDRB |= (1 << DDB1); //set led pin to write
    TCCR1B |= (1 << WGM12); // set timer to CTC mode, so it resets when it reaches the value in OCR1A p142
    TCCR1B |= (1 << CS12); // p143 set prescaler at 256
    OCR1A = 31249; //    F_CPU / (256 * 2 * 1) - 1 = 31249
    TCCR1A |= (1 << COM1A0); // toggle OC1A on compare match, which will toggle the LED connected to OC1A (PB1)

    while (1) {}
}
