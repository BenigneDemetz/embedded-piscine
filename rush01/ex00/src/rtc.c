#include "embedded.h"

// Convert decimal to BCD
uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

void write_rtc(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t days, uint8_t months, uint8_t years) {
    uint8_t address = 0b10100010;
    
    // Convert decimal to BCD
    seconds = decimal_to_bcd(seconds);
    minutes = decimal_to_bcd(minutes);
    hours = decimal_to_bcd(hours);
    days = decimal_to_bcd(days);
    months = decimal_to_bcd(months);
    years = decimal_to_bcd(years);
    
    i2c_start();
    i2c_write(address); // Write to RTC
    i2c_write(0x02); // Set register pointer to seconds register


    i2c_write(seconds);
    i2c_write(minutes);
    i2c_write(hours);
    i2c_write(days);
    i2c_write(1); // Weekday (1 = Monday)
    i2c_write(months);
    i2c_write(years);
    
    i2c_stop();
    
    uart_tx_string("RTC updated\r\n");
}

void read_rtc(uint16_t *time, uint16_t *date, uint16_t *year) { // rtc PCF8563
    uint8_t address = 0b10100010;
    i2c_start();
    i2c_write(address);
    i2c_write(0x02);
    i2c_start();
    i2c_write(address + 1);
    uint8_t seconds = i2c_read(1);
    uint8_t minutes = i2c_read(1);
    uint8_t hours = i2c_read(1);
    uint8_t days = i2c_read(1);
    uint8_t weekdays = i2c_read(1);
    uint8_t months = i2c_read(1);
    uint8_t years = i2c_read(0);
    i2c_stop();

    seconds &= (0x7F);
    minutes &= (0x7F);
    hours &= (0x3F);
    days &= (0x3F);
    months &= (0x1F);
    years &= (0xFF);
    
    seconds = (seconds & 0x0F) + ((seconds >> 4) * 10);
    minutes = (minutes & 0x0F) + ((minutes >> 4) * 10);
    hours = (hours & 0x0F) + ((hours >> 4) * 10);
    days = (days & 0x0F) + ((days >> 4) * 10);
    months = (months & 0x0F) + ((months >> 4) * 10);
    years = (years & 0x0F) + ((years >> 4) * 10);

    *time = hours * 100 + minutes;
    *date = days * 100 + months;
    *year = years + 2000;
}

void parse_date(const char *str, uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *days, uint8_t *months, uint8_t *years) {
    *hours = (str[0] - '0') * 10 + (str[1] - '0');
    *minutes = (str[3] - '0') * 10 + (str[4] - '0');
    *seconds = (str[6] - '0') * 10 + (str[7] - '0');
    *days = (str[9] - '0') * 10 + (str[10] - '0');
    *months = (str[12] - '0') * 10 + (str[13] - '0');
    *years = (str[15] - '0') * 10 + (str[16] - '0');
}

int is_date(const char *str) {
    uint8_t hours, minutes, seconds, days, months, years;

    hours = (str[0] - '0') * 10 + (str[1] - '0');
    minutes = (str[3] - '0') * 10 + (str[4] - '0');
    seconds = (str[6] - '0') * 10 + (str[7] - '0');
    days = (str[9] - '0') * 10 + (str[10] - '0');
    months = (str[12] - '0') * 10 + (str[13] - '0');
    years = (str[15] - '0') * 10 + (str[16] - '0');
    return (
        hours < 24 && minutes < 60 && seconds < 60 &&
        days < 32 && months < 13 && years < 100
    );
}
