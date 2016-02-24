//
//  opt.c
//  OpenCorsairLink
//
//  Created by Morgan Lieberthal on 2/23/16.
//
//

#include "opt.h"
#include "jml_debug.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clink_fan.h>
#include <clink_led.h>
#include <clink_proto.h>
#include <clink_temp.h>
#include <corsair_link.h>

static int info_flag;
static int help_flag;
static int verbose_flag;
static int print_flag;
static int version_flag;

// clang-format off
static struct option long_options[] = {
    { "help",              no_argument, &help_flag,    1 },
    { "info",              no_argument, &info_flag,    1 },
    { "verbose",           no_argument, &verbose_flag, 1 },
    { "print",             no_argument, &print_flag,   1 },
    { "version",           no_argument, &version_flag, 1 },

    { "led",               required_argument, 0,    'l' },
    { "led-mode",          required_argument, NULL, 10  },
    { "rgb1",              required_argument, NULL, 11  },
    { "rgb2",              required_argument, NULL, 12  },
    { "rgb3",              required_argument, NULL, 13  },
    { "rgb4",              required_argument, NULL, 14  },

    { "temperature",       required_argument, 0,    't' },
    { "temperature-limit", required_argument, NULL, 20  },
    { "temp-limit",        required_argument, NULL, 20  },

    { "fan",               required_argument, 0,    'f' },
    { "fan-mode",          required_argument, NULL, 30  },
    { "fan-rpm",           required_argument, NULL, 31  },
    { "fan-pwm",           required_argument, NULL, 32  },
    { "fan-external",      required_argument, NULL, 33  },
    { "fan-warning",       required_argument, NULL, 34  },
    { "fan-table-rpm",     required_argument, NULL, 35  },
    { "fan-table-temp",    required_argument, NULL, 36  },

    {0, 0, 0, 0},
};
// clang-format on

struct OCL_Options *parse_args2(int argc, const char **argv)
{
    struct OCL_Options *opts = malloc(sizeof(struct OCL_Options));
    check_mem(opts);
    int rc = parse_args(argc, argv, opts);
    jml_check(rc >= 0, "Error parsing arguments. See log for details.");
    return opts;
}

int parse_args(int argc, const char **argv, struct OCL_Options *opts)
{
    int rc = 0;
    return rc;
}

static void print_version(void)
{
    fprintf(stdout, "oclink (OpenCorsairLink) 0.2\n");
    fprintf(stdout, "Copyright (C) 2014 Sean Nelson <audiohacked@gmail.com>\n");
    fprintf(stdout, "Copyright (C) 2016 J. Morgan Lieberthal "
                    "<morgan@jmorgan.org>\n");
    fprintf(stdout, "License GPLv2+: GNU GPL version 2 or later "
                    "<http://www.gnu.org/licenses/>\n");
    fprintf(stdout, "This is free software: you are free to change and "
                    "redistribute it.\n");
    fprintf(stdout, "There is NO WARRANTY, to the extent permitted by law.\n");
}
