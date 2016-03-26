//
//  clink_led.c
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#include "led.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jml_debug.h"
#include "proto.h"

OCL_Led *ocl_led_alloc(OCL_Link *link)
{
    OCL_Led *led = malloc(sizeof(OCL_Led));
    led->link = link;
    return led;
}

int ocl_led_select_led(OCL_Led *led, int idx)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x04;
    link->buf[1] = link->command_id++;
    link->buf[2] = WriteOneByte;
    link->buf[3] = LED_SelectCurrent;
    link->buf[4] = idx;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return link->buf[2];
}

int ocl_led_get_led_count(OCL_Led *led)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x03;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadOneByte;
    link->buf[3] = LED_Count;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return link->buf[2];
}

CorsairLink_LEDMode ocl_led_get_mode(OCL_Led *led)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x03;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadOneByte;
    link->buf[3] = LED_Mode;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return link->buf[2];
}

char *ocl_led_get_mode_string(CorsairLink_LEDMode mode)
{
    switch (mode) {
    case StaticColor:
        return "Static Color";

    case TwoColorCycle:
        return "Two Color Cycle";

    case FourColorCycle:
        return "Four Color Cycle";

    case TemperatureColor:
        return "Temperature Color";

    default:
        return "Unknown";
    }
}

int ocl_led_set_mode(OCL_Led *led, CorsairLink_LEDMode mode)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x03;
    link->buf[1] = link->command_id++;
    link->buf[2] = WriteOneByte;
    link->buf[3] = LED_Mode;
    link->buf[4] = mode;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return link->buf[2];
}

int ocl_led_get_color(OCL_Led *led, struct OCLLedColor *color)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x04;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadThreeBytes;
    link->buf[3] = LED_CurrentColor;
    link->buf[4] = 3;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    color->red = link->buf[3];
    color->green = link->buf[4];
    color->blue = link->buf[5];

    led->color_set_by.func = 1;

    return 0;
}

struct OCLLedColor *ocl_led_get_color2(OCL_Led *led)
{
    struct OCLLedColor *color = calloc(1, sizeof(struct OCLLedColor));
    int rc = ocl_led_get_color(led, color);
    if (rc != 0) {
        return NULL;
    }

    return color;
}

int ocl_led_get_temp_color(OCL_Led *led, struct OCLLedColor *color)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x04;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadThreeBytes;
    link->buf[3] = LED_TemperatureColor;
    link->buf[4] = 3;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    color->red = link->buf[3];
    debug_log("Red: %i", color->red);
    color->green = link->buf[4];
    debug_log("Green: %i", color->green);
    color->blue = link->buf[5];
    debug_log("Blue: %i", color->blue);

    led->color_set_by.func = 1;

    return 0;
}

int ocl_led_set_temp_color(OCL_Led *led, struct OCLLedColor *color)
{
    if (led->color_set_by.opts < 1) {
        log_error("You did not define the color.");
        return -1;
    }

    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x07;
    link->buf[1] = link->command_id++;
    link->buf[2] = WriteThreeBytes;
    link->buf[3] = LED_TemperatureMode;
    link->buf[4] = 3;

    link->buf[5] = color[0].red;
    link->buf[6] = color[0].green;
    link->buf[7] = color[0].blue;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return 0;
}

struct ocl_temp_mode *ocl_led_get_temp_mode_temp(OCL_Led *led)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x04;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadThreeBytes;
    link->buf[3] = LED_TemperatureMode;
    link->buf[4] = 6;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    struct ocl_temp_mode *mode = malloc(sizeof(struct ocl_temp_mode));
    mode->temp_1 = link->buf[5] << 8;
    mode->temp_1 += link->buf[4];

    mode->temp_2 = link->buf[7] << 8;
    mode->temp_2 += link->buf[6];

    mode->temp_3 = link->buf[9] << 8;
    mode->temp_3 += link->buf[8];

    return mode;
}

int ocl_led_set_temp_mode_temp(OCL_Led *led, struct ocl_temp_mode mode)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));

    link->buf[0] = 0x0D;

    link->buf[5] = link->command_id++;
    link->buf[6] = WriteThreeBytes;
    link->buf[7] = LED_TemperatureMode;
    link->buf[8] = 6;

    link->buf[9] = mode.temp_1 & 0x00ff;
    link->buf[10] = mode.temp_1 >> 8;

    link->buf[11] = mode.temp_2 & 0x00ff;
    link->buf[12] = mode.temp_2 >> 8;

    link->buf[13] = mode.temp_3 & 0x00ff;
    link->buf[14] = mode.temp_3 >> 8;

    int res = hid_write(link->handle, link->buf, 16);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return 0;
}

int ocl_led_get_temp_mode_color(OCL_Led *led, struct OCLLedColor *color)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x04;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadThreeBytes;
    link->buf[3] = LED_TemperatureModeColors;
    link->buf[4] = 9;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    color[0].red = link->buf[4];
    color[0].green = link->buf[5];
    color[0].blue = link->buf[6];

    color[1].red = link->buf[7];
    color[1].green = link->buf[8];
    color[1].blue = link->buf[9];

    color[2].red = link->buf[10];
    color[2].green = link->buf[11];
    color[2].blue = link->buf[12];

    led->color_set_by.func = 3;

    return 0;
}

int ocl_led_set_temp_mode_color(OCL_Led *led, struct OCLLedColor *color)
{
    if (led->color_set_by.opts < 3) {
        log_error("You did not define the colors, or the right number thereof");
        return -1;
    }

    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x11;

    link->buf[5] = link->command_id++;
    link->buf[6] = WriteThreeBytes;
    link->buf[7] = LED_TemperatureModeColors;
    link->buf[8] = 9;

    link->buf[9] = color[0].red;
    link->buf[10] = color[0].green;
    link->buf[11] = color[0].blue;

    link->buf[12] = color[1].red;
    link->buf[13] = color[1].green;
    link->buf[14] = color[1].blue;

    link->buf[15] = color[2].red;
    link->buf[16] = color[2].green;
    link->buf[17] = color[2].blue;

    int res = hid_write(link->handle, link->buf, 20);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return 0;
}

int ocl_led_get_cycle_colors(OCL_Led *led, struct OCLLedColor *color)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x04;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadThreeBytes;
    link->buf[3] = LED_CycleColors;
    link->buf[4] = 12;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    color[0].red = link->buf[4];
    color[0].green = link->buf[5];
    color[0].blue = link->buf[6];

    color[1].red = link->buf[7];
    color[1].green = link->buf[8];
    color[1].blue = link->buf[9];

    color[2].red = link->buf[10];
    color[2].green = link->buf[11];
    color[2].blue = link->buf[12];

    color[3].red = link->buf[13];
    color[3].green = link->buf[14];
    color[3].blue = link->buf[15];

    led->color_set_by.func = 4;

    return 0;
}
int ocl_led_set_cycle_colors(OCL_Led *led, struct OCLLedColor *color)
{
    OCL_Link *link = led->link;
    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x10;
    link->buf[1] = link->command_id++;
    link->buf[2] = WriteThreeBytes;
    link->buf[3] = LED_CycleColors;
    link->buf[4] = 0x0C;

    link->buf[5] = color[0].red;
    link->buf[6] = color[0].green;
    link->buf[7] = color[0].blue;

    link->buf[8] = color[1].red;
    link->buf[9] = color[1].green;
    link->buf[10] = color[1].blue;

    link->buf[11] = color[2].red;
    link->buf[12] = color[2].green;
    link->buf[13] = color[2].blue;

    link->buf[14] = color[3].red;
    link->buf[15] = color[3].green;
    link->buf[16] = color[3].blue;

    int res = hid_write(link->handle, link->buf, 18);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return 0;
}

void ocl_led_free(OCL_Led *led)
{
    if (!led) {
        return;
    }

    if (led->link) {
        led->link = NULL;
    }

    free(led);
}
