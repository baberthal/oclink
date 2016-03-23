//
//  opt.c
//  OpenCorsairLink
//
//  Created by Morgan Lieberthal on 2/23/16.
//
//

#include "opt.h"
#include <assert.h>
#include <clink.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jml_debug.h"

#include "fan.h"
#include "led.h"
#include "link.h"
#include "proto.h"
#include "temp.h"

static int info_flag;
static int print_flag;

enum ocl_option_table {
    opt_halp = 1,
    opt_version = 2,
    opt_led_mode = 10,
    opt_led_rgb1 = 11,
    opt_led_rgb2 = 12,
    opt_led_rgb3 = 13,
    opt_led_rgb4 = 14,
    opt_temp_limit = 20,
    opt_fan_mode = 30,
    opt_fan_rpm = 31,
    opt_fan_pwm = 32,
    opt_fan_external = 33,
    opt_fan_warning = 34,
    opt_fan_table_rpm = 35,
    opt_fan_table_temp = 36,
};

struct OCL_Options *parse_args2(int argc, char *const *argv)
{
    struct OCL_Options *opts = malloc(sizeof(struct OCL_Options));
    check_mem(opts);
    int rc = parse_args(argc, argv, opts);
    jml_check(rc >= 0, "Error parsing arguments. See log for details.");
    return opts;
}

int parse_args(int argc, char *const *argv, struct OCL_Options *opts)
{
    assert(opts != NULL);
    int c, rc = 0;

    while (1) {
        static struct option long_options[] = {
            {"help", no_argument, 0, opt_halp},
            {"version", no_argument, 0, opt_version},

            {"info", no_argument, &info_flag, 1},
            {"print", no_argument, &print_flag, 1},

            {"led", required_argument, 0, 'l'},
            {"led-mode", required_argument, 0, opt_led_mode},
            {"rgb1", required_argument, 0, opt_led_rgb1},
            {"rgb2", required_argument, 0, opt_led_rgb2},
            {"rgb3", required_argument, 0, opt_led_rgb3},
            {"rgb4", required_argument, 0, opt_led_rgb4},

            {"temperature", required_argument, 0, 't'},
            {"temperature-limit", required_argument, 0, opt_temp_limit},
            {"temp-limit", required_argument, 0, opt_temp_limit},

            {"fan", required_argument, 0, 'f'},
            {"fan-mode", required_argument, 0, opt_fan_mode},
            {"fan-rpm", required_argument, 0, opt_fan_rpm},
            {"fan-pwm", required_argument, 0, opt_fan_pwm},
            {"fan-external", required_argument, 0, opt_fan_external},
            {"fan-warning", required_argument, 0, opt_fan_warning},
            {"fan-table-rpm", required_argument, 0, opt_fan_table_rpm},
            {"fan-table-temp", required_argument, 0, opt_fan_table_temp},

            {0, 0, 0, 0},
        };
        int opt_idx = 0;
        c = getopt_long(argc, argv, "hil:t:f:p", long_options, &opt_idx);

        if (c == -1) {
            break;
        }

        switch (c) {
        case 0:
            if (long_options[opt_idx].flag != 0) break;
            debug_log("Option %s", long_options[opt_idx].name);
            if (optarg) debug_log(" with arg %s", optarg);

        default:
            break;
        }
    }

    return rc;
}

static inline void print_help_footer(void)
{
    printf("Report bugs at: <https://github.com/baberthal/oclink/issues>\n");
    printf("oclink home page: <https://github.com/baberthal/oclink>\n");
    printf("General help using GNU software: <http://www.gnu.org/gethelp/>\n");
}

static void print_version(void)
{
    printf("oclink (OpenCorsairLink) %s\n", CLINK_VERSION_STRING);
    printf("Copyright (C) 2014 Sean Nelson <audiohacked@gmail.com>\n");
    printf("Copyright (C) 2016 J. Morgan Lieberthal <morgan@jmorgan.org>\n");
    printf(
        "License GPLv3+: GNU GPL version 3 or later "
        "<http://www.gnu.org/licenses/>\n");
    printf(
        "This is free software: you are free to change and redistribute "
        "it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n\n");
    print_help_footer();
}

static void print_help(void)
{
    printf("OVERVIEW: oclink OpenCorsairLink Interface for *nix systems.\n\n");

    printf("USAGE: oclink [options]\n\n");

    printf("SPECIAL OPTIONS:\n");
    printf("\t-h, --help :Prints this message and exits\n");
    printf("\t-v, --version :Prints the version message and exits\n");

    printf("OPTIONS:\n");
    printf("\t-i, --info :Print information about fans, pumps, and LEDs\n");
    printf(
        "\t-p, --print :Print ALL information about fans, pumps, and "
        "LEDs\n");

    printf(
        "\t-l, --led <led_number> :Select an LED to configure. Index begins "
        "at 1.\n");
    printf("\t\t--led-mode <LED_MODE> :Set an LED mode (in HH format).\n");
    printf("\t\t\tPOSSIBLE VALUES FOR <LED_MODE>:\n");
    printf("\t\t\t\t0x00 - Static Color\n");
    printf("\t\t\t\t0x40 - 2-Color Cycle (must specify --rgb1 and --rgb2)\n");
    printf("\t\t\t\t0x40 - 4-Color Cycle (must specify --rgb1 thru --rgb4)\n");
    printf("\t\t\t\t0xC0 - Temp Mode (must specify --rgb1 thru --rgb3\n");

    printf("\t\tRGB Settings:\n");
    printf(
        "\t\t Can be hex format (#FFFFFF) or a comma-separated list of "
        "values up to 255\n");
    printf("\t\t--rgb1 <RGB_Code> :Set color for led cycle 1\n");
    printf("\t\t--rgb2 <RGB_Code> :Set color for led cycle 2\n");
    printf("\t\t--rgb3 <RGB_Code> :Set color for led cycle 3\n");
    printf("\t\t--rgb4 <RGB_Code> :Set color for led cycle 4\n");

    printf(
        "\t-t, --temperature <sensor_number> :Select a Temperature "
        "Sensor\n");
    printf(
        "\t\t--temperature-limit <temp_limit> :Set Max Temperature (high "
        "temp warning)\n");

    printf(
        "\t-f, --fan <fan_number>{:<fan_number>} :Selects a fan to setup. "
        "Accepted values are 1 thru 4.\n");
    printf("\t\t--fan-mode <FAN_MODE> :Sets the mode for the selected fan\n");
    printf("\t\t\tPOSSIBLE VALUES FOR <FAN_MODE>:\n");
    printf("\t\t\t\t 2 - Fixed PWM (must specify --fan-pwm)\n");
    printf("\t\t\t\t 4 - Fixed RPM (must specify --fan-rpm)\n");
    printf("\t\t\t\t 6 - Default\n");
    printf("\t\t\t\t 8 - Quiet\n");
    printf("\t\t\t\t10 - Balanced\n");
    printf("\t\t\t\t12 - Performance\n");
    printf(
        "\t\t--fan-pwm <fan_PWM> :The desired PWM speed for the selected "
        "fan. NOTE: it only works when fan mode is set to Fixed PWM\n");
    printf(
        "\t\t--fan-rpm <fan_RPM> :The desired RPM for the selected fan. "
        "NOTE: it works only when fan mode is set to Fixed RPM\n");
    printf(
        "\t\t--fan-warning <fan_threshold> :Sets the Fan Warning Limit "
        "(Sets Underspeed Threshold).\n");

    print_help_footer();
}
