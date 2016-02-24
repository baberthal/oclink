#ifndef JML_COLOR_PRINTF_H
#define JML_COLOR_PRINTF_H 1

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

enum _JMLColors {
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    Reset,
};

typedef enum _JMLColors Color;

int color_printf(Color color, const char *__restrict fmt, ...);
int color_fprintf(FILE *__restrict file, Color color, const char *__restrict fmt, ...);

#define LogRed(...) color_printf(Red, __VA_ARGS__)

#endif /* JML_COLOR_PRINTF_H */