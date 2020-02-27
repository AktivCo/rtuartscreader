#include "ConsoleColor.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void log_log(const char* color, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);

    char* colored_fmt = (char*)calloc(strlen(fmt) + strlen(color) + strlen(COLOR_NORMAL) + 1, sizeof(char));

    strcpy(colored_fmt, color);
    strcat(colored_fmt, fmt);
    strcat(colored_fmt, COLOR_NORMAL);

    vprintf(colored_fmt, args);

    free(colored_fmt);

    va_end(args);
}

void log_xxd(const char* color, const char* msg, const unsigned char* buffer, int len) {
    const char hex_pattern[] = "%02X ";
    char hex_byte[sizeof(hex_pattern)];

    char* colored_buf = (char*)calloc(strlen(color) + len * 3 + strlen(msg) + strlen(COLOR_NORMAL) + 1, sizeof(char));

    strcpy(colored_buf, color);
    strcat(colored_buf, msg);

    for (int i = 0; i < len; ++i) {
        (void)snprintf(hex_byte, sizeof(hex_pattern), hex_pattern, buffer[i]);
        strcat(colored_buf, hex_byte);
    }

    strcat(colored_buf, COLOR_NORMAL);

    printf("%s\r\n", colored_buf);

    free(colored_buf);
}
