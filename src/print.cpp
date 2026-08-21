#include "../h/print.hpp"

void printString(const char *str) {
    if (!str) return;
    while (*str) {
        putc(*str);
        str++;
    }
}

void printInt(long long num, int base) {
    if (base < 2 || base > 16) return;

    if (num == 0) {
        putc('0');
        return;
    }

    char buf[64];
    int i = 0;
    bool isNegative = false;

    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    unsigned long long n = (unsigned long long)num;
    static const char digits[] = "0123456789ABCDEF";

    while (n > 0) {
        buf[i++] = digits[n % base];
        n /= base;
    }

    if (isNegative) {
        buf[i++] = '-';
    }

    while (--i >= 0) {
        putc(buf[i]);
    }
}

void printHex(unsigned long long num) {
    printString("0x");
    printInt((long long)num, 16);
}