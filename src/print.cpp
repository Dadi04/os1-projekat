#include "../h/print.hpp"
#include "../lib/hw.h"

void kputc(char c) {
    while (!(*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT)) {}
    *((char*)CONSOLE_TX_DATA) = c;
}

void kprintString(const char *str) {
    if (!str) return;
    while (*str) {
        kputc(*str);
        str++;
    }
}

void kprintInt(uint64 num) {
    if (num == 0) {
        kputc('0');
        return;
    }

    char buf[64];
    int i = 0;

    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (--i >= 0) {
        kputc(buf[i]);
    }
}