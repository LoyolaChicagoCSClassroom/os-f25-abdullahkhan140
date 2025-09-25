#include "rprintf.h"

static func_ptr out_char;

// Print a string
static void outs(charptr s) {
    while (*s) {
        out_char(*s++);
    }
}

// Print a decimal integer
static void outnum(int num) {
    char buf[16];
    int i = 0;

    if (num == 0) {
        out_char('0');
        return;
    }
    if (num < 0) {
        out_char('-');
        num = -num;
    }
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i--) {
        out_char(buf[i]);
    }
}

void esp_vprintf(const func_ptr f_ptr, charptr ctrl, va_list argp) {
    out_char = f_ptr;
    for (; *ctrl; ctrl++) {
        if (*ctrl == '%') {
            ctrl++;
            if (*ctrl == 'd') {
                outnum(va_arg(argp, int));
            } else if (*ctrl == 's') {
                outs(va_arg(argp, charptr));
            } else {
                out_char('%');
                out_char(*ctrl);
            }
        } else {
            out_char(*ctrl);
        }
    }
}

void esp_printf(const func_ptr f_ptr, charptr ctrl, ...) {
    va_list argp;
    va_start(argp, ctrl);
    esp_vprintf(f_ptr, ctrl, argp);
    va_end(argp);
}
