#include <avr/io.h>

int main() {
    DDRB |= (1 << DDB1); //set led pin to write
    TCCR1A |= (1 << COM1A1); //Toggle OC1A on Compare Match
    TCCR1B |= (1 << WGM13); //set mode 8 PWM, Phase and Frequency Corrsct (table 16-4, p141)
    TCCR1B |= (1 << CS12); //prescaler set to 256 (table 16-5, p143)
    float duty_cycle = 10;
    float period = duty_cycle / 100 * 31249;
    ICR1 = 31249;
    OCR1A = period; // 10% duty cycle

    while (1) {}
}
