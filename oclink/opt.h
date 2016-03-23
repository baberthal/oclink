//
//  opt.h
//  OpenCorsairLink
//
//  Created by Morgan Lieberthal on 2/23/16.
//
//

#ifndef opt_h
#define opt_h

#include <led.h>
#include <stdio.h>

/**
 *  @brief A struct to hold an option
 */
typedef struct OCLOption {
    const char *name;  ///< The name of the option
} OCLOption;

struct OCL_Options {
    int info_flag;
    int help_flag;
    int verbose_flag;
    int print_flag;
    int version_flag;
    int fan_num;
    int fan_high_num;
    int fan_mode;
    int fan_pwm;
    int fan_rpm;
    int fan_threshold;
    int led_number;
    int led_mode;
    struct OCLLedColor *leds;
    int temp_number;
};

int parse_args(int argc, char *const *argv, struct OCL_Options *opts);
struct OCL_Options *parse_args2(int argc, char *const *argv);

#endif /* opt_h */
