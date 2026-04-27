#include "embedded.h"

// Convert decimal to BCD
uint8_t decimal_to_bcd(uint8_t decimal);
void write_rtc(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t days, uint8_t months, uint8_t years);
void read_rtc(uint16_t *time, uint16_t *date, uint16_t *year);
void parse_date(const char *str, uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *days, uint8_t *months, uint8_t *years);
int is_date(const char *str);
