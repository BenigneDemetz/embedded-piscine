#ifndef UART_H
#define UART_H

#include <avr/io.h>

void init_uart();
void uart_tx(char c);
char uart_rx();
void uart_tx_string(const char *str);
void uart_received();

#endif // UART_H
