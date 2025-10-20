/*---------------------------------------------------*/
/* Public Domain version of printf                   */
/* Rud Merriam, Compsult, Inc. Houston, Tx.         */
/* For Embedded Systems Programming, 1991           */
/*---------------------------------------------------*/

#include <stdarg.h>
#include "rprintf.h"

extern void putc(int c);   // Use your kernel_main.c function

typedef void (*func_ptr)(char);
typedef char* charptr;

static func_ptr out_char = putc;  // default output function
static int do_padding;
static int left_flag;
static int len;
static int num1;
static int num2;
static char pad_character;

/*---------------------------------------------------*/
/* Basic utility functions                           */
/*---------------------------------------------------*/
size_t strlen(const char *str) {
    size_t len = 0;
    while(str[len] != '\0') len++;
    return len;
}

int tolower(int c) {
    if(c >= 'A' && c <= 'Z') return c + ('a' - 'A');
    return c;
}

int isdig(int c) {
    return (c >= '0' && c <= '9') ? 1 : 0;
}

/*---------------------------------------------------*/
/* Padding helper                                    */
/*---------------------------------------------------*/
static void padding(const int l_flag) {
    if (do_padding && l_flag && (len < num1)) {
        for (int i = len; i < num1; i++)
            out_char(pad_character);
    }
}

/*---------------------------------------------------*/
/* Output string with padding                        */
/*---------------------------------------------------*/
static void outs(charptr lp) {
    if(lp == NULL) lp = "(null)";

    len = strlen(lp);
    padding(!left_flag);

    while (*lp && num2--) out_char(*lp++);
    len = strlen(lp);
    padding(left_flag);
}

/*---------------------------------------------------*/
/* Output number                                     */
/*---------------------------------------------------*/
static void outnum(unsigned int num, const int base) {
    charptr cp;
    int negative;
    char outbuf[32];
    const char digits[] = "0123456789ABCDEF";

    if ((int)num < 0 && base != 16) {
        negative = 1;
        num = -num;
    } else {
        negative = 0;
    }

    cp = outbuf;
    do { *cp++ = digits[num % base]; } while ((num /= base) > 0);
    if (negative) *cp++ = '-';
    *cp-- = 0;

    len = strlen(outbuf);
    padding(!left_flag);
    while(cp >= outbuf) out_char(*cp--);
    padding(left_flag);
}

/*---------------------------------------------------*/
/* Get number from format string                     */
/*---------------------------------------------------*/
static int getnum(charptr* linep) {
    int n = 0;
    charptr cp = *linep;
    while (isdig((int)*cp)) n = n * 10 + ((*cp++) - '0');
    *linep = cp;
    return n;
}

/*---------------------------------------------------*/
/* Main printf functions                              */
/*---------------------------------------------------*/
void esp_printf(const func_ptr f_ptr, charptr ctrl, ...) {
    va_list args;
    va_start(args, ctrl);
    esp_vprintf(f_ptr, ctrl, args);
    va_end(args);
}

void esp_vprintf(const func_ptr f_ptr, charptr ctrl, va_list argp) {
    out_char = f_ptr;

    int long_flag, dot_flag;
    char ch;

    for (; *ctrl; ctrl++) {
        if (*ctrl != '%') { out_char(*ctrl); continue; }

        dot_flag = long_flag = left_flag = do_padding = 0;
        pad_character = ' ';
        num2 = 32767;

    try_next:
        ch = *(++ctrl);

        if (isdig((int)ch)) {
            if (dot_flag) num2 = getnum(&ctrl);
            else {
                if (ch == '0') pad_character = '0';
                num1 = getnum(&ctrl);
                do_padding = 1;
            }
            ctrl--;
            goto try_next;
        }

        switch (tolower((int)ch)) {
            case '%': out_char('%'); continue;
            case '-': left_flag = 1; break;
            case '.': dot_flag = 1; break;
            case 'l': long_flag = 1; break;

            case 'i':
            case 'd':
                if (long_flag || ch == 'D') outnum(va_arg(argp, long), 10);
                else outnum(va_arg(argp, int), 10);
                continue;

            case 'x': outnum((long)va_arg(argp, int), 16); continue;
            case 's': outs(va_arg(argp, charptr)); continue;
            case 'c': out_char(va_arg(argp, int)); continue;

            case '\\':
                switch (*ctrl) {
                    case 'a': out_char(0x07); break;
                    case 'h': out_char(0x08); break;
                    case 'r': out_char(0x0D); break;
                    case 'n': out_char(0x0D); out_char(0x0A); break;
                    default: out_char(*ctrl); break;
                }
                ctrl++;
                break;

            default: continue;
        }
        goto try_next;
    }
}
