#include "7seg.h"

void display_one_seg(uint8_t num, uint8_t value) {
    // uint8_t seg_value;

    uint8_t current_value = expander_read_PORT(0);

    //set to one the four left bits
    current_value |= 0xF0;
    //print current_value for debug
    
    // for (int j = 7; j >= 0; j--) {
    //     uart_tx((current_value & (1 << j)) ? '1' : '0');
    // }
    // uart_tx_string("\r\n");
    
    
    i2c_start(); // Switch off every segs
    i2c_write(EXPANDER_ADR << 1); // Write to expander
    i2c_write(2); // Set register pointer to output port for the display
    i2c_write(current_value);
    i2c_stop();
    
    i2c_start(); // Select segments
    i2c_write(EXPANDER_ADR << 1); // Write to expander
    i2c_write(3); // Set register pointer to output port for the display
    i2c_write(value); // Write segment value to display the number
    i2c_stop();
    
    current_value &= ~(1 << (num + 4)); // Write segment value to display the number
    i2c_start(); // Select number
    i2c_write(EXPANDER_ADR << 1); // Write to expander
    i2c_write(2); // Set register pointer to output port for the display
    i2c_write(current_value);
    i2c_stop();
}

void int_to_7seg(int16_t value) {
    // Define segment values for digits 0-9 (active low)
    uint8_t segment_values[10] = {
        0b00111111, // 0
        0b00000110, // 1
        0b01011011, // 2
        0b01001111, // 3
        0b01100110, // 4
        0b01101101, // 5
        0b01111101, // 6
        0b00000111, // 7
        0b01111111, // 8
        0b01101111  // 9
    };
    
    val_7seg[0] = segment_values[value / 1000]; // Display the least significant digit on the 7-segment display
    val_7seg[1] = segment_values[value / 100 % 10]; // Display the second digit on the 7-segment display
    val_7seg[2] = segment_values[value / 10 % 10]; // Display the third digit on the 7-segment display
    val_7seg[3] = segment_values[value % 10]; // Display the fourth digit on the 7-segment display
}

void switch_off_7seg() {
    i2c_start();
    i2c_write(EXPANDER_ADR << 1); // Write to expander
    i2c_write(3); // Set register pointer to output port for the display
    i2c_write(0x00); // Write segment value to turn off the display
    // i2c_stop();
}
