//
//  clink_led.h
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#ifndef clink_led_h
#define clink_led_h

#include "corsair_link.h"

struct OCLLedColor {
    int red;
    int green;
    int blue;
};

typedef struct ocl_led {
    struct OCLLedColor color[4];
    union {
        int opts;
        int func;
    } color_set_by;
    OCL_Link *link;
} OCL_Led;

struct ocl_temp_mode {
    int temp_1;
    int temp_2;
    int temp_3;
};

OCL_Led *ocl_led_alloc(OCL_Link *link);

int ocl_led_select_led(OCL_Led *led, int idx);
int ocl_led_get_led_count(OCL_Led *led);

int ocl_led_get_mode(OCL_Led *led);
int ocl_led_set_mode(OCL_Led *led, int mode);

int ocl_led_get_color(OCL_Led *led, struct OCLLedColor *color);

int ocl_led_get_temp_color(OCL_Led *led, struct OCLLedColor *color);
int ocl_led_set_temp_color(OCL_Led *led, struct OCLLedColor *color);

struct ocl_temp_mode *ocl_led_get_temp_mode_temp(OCL_Led *led);
int ocl_led_set_temp_mode_temp(OCL_Led *led, struct ocl_temp_mode mode);

int ocl_led_get_temp_mode_color(OCL_Led *led, struct OCLLedColor *color);
int ocl_led_set_temp_mode_color(OCL_Led *led, struct OCLLedColor *color);

int ocl_led_get_cycle_colors(OCL_Led *led, struct OCLLedColor *color);
int ocl_led_set_cycle_colors(OCL_Led *led, struct OCLLedColor *color);

void ocl_led_free(OCL_Led *led);

#endif /* clink_led_h */
