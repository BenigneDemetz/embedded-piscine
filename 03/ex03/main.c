#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void switch_off_rgb(int id) {
    DDRD &= ~(1 << id);
}

void switch_on_rgb(int id) {
    DDRD |= (1 << id);
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

void init_rgb() {

    //RED PD5
    // Mode Fast PWM
    DDRD |= (1 << DDD5);
    // Sortie non inversée
    TCCR0A |= (1 << COM0B1);
    OCR0B = 0;
    switch_off_rgb(DDD5);
    
    //GREEN PD6
    DDRD |= (1 << DDD6);
    // Mode Fast PWM
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    // Sortie non inversée
    TCCR0A |= (1 << COM0A1);
    // Prescaler = 64
    TCCR0B |= (1 << CS01) | (1 << CS00);
    OCR0A = 0;
    switch_off_rgb(DDD6);

    //BLUE PD3
    DDRD |= (1 << DDD3);
    // Mode Fast PWM
    TCCR2A |= (1 << WGM20) | (1 << WGM21);
    // Sortie non inversée
    TCCR2A |= (1 << COM2B1);
    // Prescaler = 64
    TCCR2B |= (1 << CS21) | (1 << CS20);
    OCR2B = 0;
    switch_off_rgb(DDD3);
}

void uart_init() {
    UBRR0H = 0;
    UBRR0L = 8; // 115200 baud rate with 16MHz clock, U2X0 enabled
    UCSR0B = (1 << TXEN0); // Enable transmitter
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits
    UCSR0C &= ~(1 << USBS0); // 1 stop bit


    UCSR0B |= (1 << RXEN0); // Enable receiver
    UCSR0B |= (1 << RXCIE0); // Enable receive complete interrupt
    SREG |= (1 << 7); // flag for Global Interrupt
}

void uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = c; // Put data into buffer, sends the data
}

char uart_rx() {
    while (!(UCSR0A & (1 << RXC0))); // Wait for data to be received
    return UDR0; // Get and return received data from buffer
}

void uart_tx_string(const char *str) {
    while (*str) {
        uart_tx(*str++);
    }
}

void transform(char *hex) { //transform hex string to int array #RRGGBB -> [#, R, R, G, G, B, B]
    for (int i = 1; i <= 6; i++) {
        if (hex[i] >= '0' && hex[i] <= '9')
            hex[i] -= '0';
        else if (hex[i] >= 'A' && hex[i] <= 'F')
            hex[i] = hex[i] - 'A' + 10;
    }
}

uint8_t ft_strlen(char *hex) {
    uint8_t i = 0;
    while (hex[i] != 0 && i < 128) {
        i++;
    }
    return i;
}

bool hex_format(char *hex) {
    if (hex[0] != '#')
        return 0;
    for (int i = 1; i <= 6; i++) {
        if (!((hex[i] >= '0' && hex[i] <= '9') || (hex[i] >= 'A' && hex[i] <= 'F'))) {
            return 0;
        }
    }
    return 1;
}

__attribute__((signal))
void USART_RX_vect() {
    char c = UDR0;
    static char hex[128] = {0};
    uart_tx(c); // Echo the received character back
    uint8_t i = 0;
    while (hex[i] != 0)
        i++;
    hex[i] = c;
    if (c == 13) { //end of line
        uart_tx_string("\r\n");
        if (ft_strlen(hex) == 8 && hex_format(hex)) { //if size == (# + 6 chars and \n) and format is correct // subject uses exemple zith majuscules only, so we will do the same
            transform(hex);
            
            //combine the two chars to get the int value of R, G and B // first char in the 4 left bits and second char in the 4 right bits
            set_rgb((hex[1] << 4) | hex[2], (hex[3] << 4) | hex[4], (hex[5] << 4) | hex[6]);
            for (int i = 0; i < 8; i++) //clear hex
                hex[i] = 0;
        }
        for (int i = 0; i < 128 && hex[i] != 0; i++) //clear hex (buffer size of 128 but stop when 0 so we don't make too much operations)
            hex[i] = 0;
    }
}

int main() {

    uart_init();
    init_rgb();

    while (1) {}
}
