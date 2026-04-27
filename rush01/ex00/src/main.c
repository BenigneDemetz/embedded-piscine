#include "avr/io.h"
#include "util/delay.h"
#include "embedded.h"


volatile uint8_t switch_state = 1; // 1 means not pressed, 0 means pressed
volatile int16_t val_7seg[4] = {0, 0, 0, 0}; //save number to display for each 7seg
volatile uint8_t mode = 0;
volatile int8_t state = 0;
volatile uint8_t launch_animation = 1;
volatile int8_t receive_uart = 0;
// volatile uint8_t state_7seg = [4]; //save if 7seg is switched on or not

void print_status(uint8_t status) {
    uart_tx_string("Status received: 0x");
    uart_tx((TWSR >> 4) + '0'); // Print TWI status code
    uart_tx((TWSR & 0x0F) + '0'); // Print TWI status code
    uart_tx_string("\r\n");
}

void init_timer2() {
    TCCR2A |= (1 << WGM21); // CTC mode
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // Prescaler 1024
    // TCCR2B |= (1 << CS20); // Prescaler 1
    TIMSK2 |= (1 << OCIE2A); // Enable timer compare interrupt
    OCR2A = 16;
}

void init_timer1() {
    TCNT1 = 0; // Initialize timer counter to 0
    TCCR1A = 0; // CTC mode
    TCCR1B |= (1 << WGM12); // CTC mode
    TCCR1B |= (1 << CS12); // Prescaler 1024
    TCCR1B |= (1 << CS10); // Prescaler 1024
    TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt
    // OCR1A = (uint32_t) 50 * 16000 / 1024; // Set compare value for 50ms delay
    OCR1A = 15625; // Set compare value for 50ms delay
}

void init() {
    init_uart();
    init_i2c();
    init_expander();
	init_setup();
	buttons_interrupt_init();
	timer0_init();
    init_timer1();
    init_adc();
    // init_timer2();
}

void animation() {
    if (state == 0) {
        PORTB |= (1 << PORTB0); // Turn on LED on PB0 D1
        PORTB |= (1 << PORTB1); // Turn off LED on PB1 D2
        PORTB |= (1 << PORTB2); // Turn off LED on PB2 D3
        PORTB |= (1 << PORTB4); // Turn off LED on PB3 D4
        val_7seg[0] = 0b11111111; // Display the least significant digit on the 7-segment display
        val_7seg[1] = 0b11111111; // Display the second digit on the 7-segment display
        val_7seg[2] = 0b11111111; // Display the third digit on the 7-segment display
        val_7seg[3] = 0b11111111; // Display the fourth digit on the 7-segment display
    } else if (state == 3) {
        PORTB &= ~(1 << PORTB0); // Turn on LED on PB0 D1
        PORTB &= ~(1 << PORTB1); // Turn off LED on PB1 D2
        PORTB &= ~(1 << PORTB2); // Turn off LED on PB2 D3
        PORTB &= ~(1 << PORTB4); // Turn off LED on PB3 D4
        val_7seg[0] = 0b00000000; // Display the least significant digit on the 7-segment display
        val_7seg[1] = 0b00000000; // Display the second digit on the 7-segment display
        val_7seg[2] = 0b00000000; // Display the third digit on the 7-segment display
        val_7seg[3] = 0b00000000; // Display the fourth digit on the 7-segment display
    }
}

__attribute__((signal))
void TIMER2_COMPA_vect() {
}

__attribute__((signal))
void TIMER1_COMPA_vect() {
    // switch_state = i2c_read_switch();
	if (launch_animation)
		animation();
	state = (state > 2) ? 0 : (state + 1);
}

int main()
{
    init();

    // int_to_7seg(12); // Display on the 7-segment display
    SREG |= (1 << 7); // Global Interrupt Enable
    uint8_t switchs_last[3] = {1, 1, 1}; // Last state of switches (initialized to not pressed)
    float temp, humi; //read temperature and humidity from slave
    get_temp(0b01110000, &temp, &humi);
    _delay_ms(200); //wait for measurement to be done
    while (1) {
        check_switchs();
        for (int i = 0; i < 4; i++) { // display for 7seg
            display_one_seg(i, val_7seg[i]);
            // switch_off_7seg();
        }
        if (launch_animation)
            continue;
        if (receive_uart) {
            uart_received();
            receive_uart = 0;
        }
        switch (mode) {
            case 0:
                int_to_7seg(read_adc10(0)); // Read ADC value for potentiometer
                break;
            case 1:
                int_to_7seg(read_adc10(1)); // Read ADC value for LDR, light sensor
                break;
            case 2:
                int_to_7seg(read_adc10(2)); // Read ADC value for temperature sensor
                break;
            case 3:
			    mode5();
                PORTD &= ~(1 << LED_RGB_RED); // Turn on blue LED_RGB_RED
                PORTD &= ~(1 << LED_RGB_GREEN); // Turn off blue LED_RGB_GREEN
                PORTD &= ~(1 << LED_RGB_BLUE); // Turn off blue LED_RGB_BLUE
                int_to_7seg(read_internal_temp()); // Read internal temperature value
                break;
            case 4: //display -42-
                mode4();
                break;
            case 5: //display temperature
			    mode5();
                if (!state)
                    trigger_measurement(0b01110000);
                if (state == 2)
                    get_temp(0b01110000, &temp, &humi);
                    PORTD &= ~(1 << LED_RGB_RED); // Turn on blue LED_RGB_RED
                    PORTD &= ~(1 << LED_RGB_GREEN); // Turn off blue LED_RGB_GREEN
                    PORTD &= ~(1 << LED_RGB_BLUE); // Turn off blue LED_RGB_BLUE
                    int_to_7seg(temp);
                break;
            case 6: //display temp in fahrenheit
                if (!state)
                    trigger_measurement(0b01110000);
                if (state == 2)
                    get_temp(0b01110000, &temp, &humi);
                    PORTD &= ~(1 << LED_RGB_RED); // Turn on blue LED_RGB_RED
                    PORTD &= ~(1 << LED_RGB_GREEN); // Turn off blue LED_RGB_GREEN
                    PORTD &= ~(1 << LED_RGB_BLUE); // Turn off blue LED_RGB_BLUE
                    int_to_7seg(temp);
                int_to_7seg(temp*1.8+32);
                break;
            case 7: //display humidity
                if (!state)
                    trigger_measurement(0b01110000);
                if (state == 2)
                    get_temp(0b01110000, &temp, &humi);
                    PORTD &= ~(1 << LED_RGB_RED); // Turn on blue LED_RGB_RED
                    PORTD &= ~(1 << LED_RGB_GREEN); // Turn off blue LED_RGB_GREEN
                    PORTD &= ~(1 << LED_RGB_BLUE); // Turn off blue LED_RGB_BLUE
                    int_to_7seg(temp);
                int_to_7seg(humi);
                break;
            case 8: //display heure + minutes
                uint16_t hm, dm, year;
                read_rtc(&hm, &dm, &year);
                int_to_7seg(hm);
                break;
            case 9: //display jour + mois
                read_rtc(&hm, &dm, &year);
                int_to_7seg(dm);
                break;
            case 10: //display année
                read_rtc(&hm, &dm, &year);
                int_to_7seg(year);
                break;
            default:
                break;
        }

		// uart_tx_string("state : ");
		// uart_tx(state + '0');
		// uart_tx_string(" mode : ");
		// uart_tx(mode + '0');
		// uart_tx_string(" launch_animation : ");
		// uart_tx(launch_animation + '0');
		// uart_tx_string("\r\n");
    }
}

//spi 3 leds
//rtc
//date
//3 leds light up on switch

