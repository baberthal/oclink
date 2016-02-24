#include "color_printf.h"
#include <stdbool.h>
#include <string.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define XCODE_COLORS_ESCAPE "\033["

#define XCODE_COLORS_RESET_FG XCODE_COLORS_ESCAPE "fg;"
#define XCODE_COLORS_RESET_BG XCODE_COLORS_ESCAPE "bg;"
#define XCODE_COLORS_RESET XCODE_COLORS_ESCAPE ";"

static const char *color_code_for_ansi_color(Color color)
{
    switch (color) {
    case Red:
        return ANSI_COLOR_RED;

    case Green:
        return ANSI_COLOR_GREEN;

    case Yellow:
        return ANSI_COLOR_YELLOW;

    case Blue:
        return ANSI_COLOR_BLUE;

    case Magenta:
        return ANSI_COLOR_MAGENTA;

    case Cyan:
        return ANSI_COLOR_CYAN;

    default:
        return ANSI_COLOR_RESET;
    }
}

static const char *color_code_for_xcode_console_color(Color color)
{
    switch (color) {
    case Red:
        return XCODE_COLORS_ESCAPE "fg220,50,47;";

    case Green:
        return XCODE_COLORS_ESCAPE "fg113,153,0;";

    case Yellow:
        return XCODE_COLORS_ESCAPE "fg181,137,0;";

    case Blue:
        return XCODE_COLORS_ESCAPE "fg38,139,210;";

    case Magenta:
        return XCODE_COLORS_ESCAPE "fg211,54,130;";

    case Cyan:
        return XCODE_COLORS_ESCAPE "fg42,161,152;";

    default:
        return XCODE_COLORS_RESET;
    }
}

static bool have_xcode_colors(void)
{
    char *xcc_env = getenv("XcodeColors");
    if (xcc_env && (strcmp(xcc_env, "YES") == 0)) {
        return true;
    }

    return false;
}

static inline const char *color_code_for_color(Color color)
{
    return have_xcode_colors() ? color_code_for_xcode_console_color(color)
                               : color_code_for_ansi_color(color);
}

static const char *proper_escape_sequence(void)
{
    if (have_xcode_colors()) {
        return XCODE_COLORS_RESET;
    }

    return ANSI_COLOR_RESET;
}

int color_printf(Color color, const char *__restrict fmt, ...)
{
    int ret;
    va_list ap;
    const char *code = color_code_for_color(color);
    ret = fprintf(stdout, "%s", code);

    va_start(ap, fmt);
    ret = vfprintf(stdout, fmt, ap);
    va_end(ap);
    ret = fprintf(stdout, "%s", proper_escape_sequence());

    return (ret);
}

int color_fprintf(FILE *__restrict file, Color color, const char *fmt, ...)
{
    int ret;
    va_list ap;
    const char *code = color_code_for_color(color);
    ret = fprintf(file, "%s", code);

    va_start(ap, fmt);
    ret = vfprintf(file, fmt, ap);
    va_end(ap);
    ret = fprintf(file, "%s", proper_escape_sequence());

    return (ret);
}
