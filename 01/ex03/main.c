#include <avr/io.h>
#include <util/delay.h>


int main() {

    unsigned char button1_state;
    unsigned char button2_state;
    float period;
    float duty_cycle = 50;

    DDRD |= (0 << DDD2); // set pin switch DDD2 as read
    DDRD |= (0 << DDD4); // set pin switch DDD4 as read
    DDRB |= (1 << DDB1); //set led pin to write

    TCCR1A |= (1 << COM1A1); //Toggle OC1A on Compare Match
    TCCR1B |= (1 << WGM13); //set mode 8 PWM, Phase and Frequency Corrsct (table 16-4, p141)
    TCCR1B |= (1 << CS12); //prescaler set to 256 (table 16-5, p143)

    ICR1 = 31249;

    while (1) {
        period = duty_cycle / 100 * 31249;
        OCR1A = period; // 10% duty cycle

        button1_state = PIND & (1 << PIND2); // read state of the button PIND2 with & (1 << PIND2) is a mask used to isolate the PIND2 value
        button2_state = PIND & (1 << PIND4); //read snd button

        if (button1_state == 0) {
            if (duty_cycle < 100)
                duty_cycle += 10;

            do {
                button1_state = PIND & (1 << PIND2);
            }
            while (button1_state == 0); // wait for the button to be released
            _delay_ms(50); //little delay while releasing the button
        }

        if (button2_state == 0) {
            if (duty_cycle > 10)
                duty_cycle -= 10;

            do {
                button2_state = PIND & (1 << PIND4);
            }
            while (button2_state == 0); // wait for the button to be released
            _delay_ms(50); //little delay while releasing the button
        }
    }
}
