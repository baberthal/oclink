//
//  clink_fan.c
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//
#include "fan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jml_debug.h"
#include "proto.h"

OCL_Fan *ocl_fan_alloc(OCL_Link *link)
{
    OCL_Fan *fan = malloc(sizeof(OCL_Fan));
    fan->link = link;
    fan->fan_count = ocl_fan_get_fan_count(fan);
    fan->fan_info = ocl_list_create();

    return fan;
}

int ocl_fan_select_fan(OCL_Fan *fan, int idx)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x04;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = WriteOneByte;
    cl->buf[3] = FAN_Select;
    cl->buf[4] = idx;

    cl->hid_wrapper(cl->handle, cl->buf, 8);
    return cl->buf[2];
}

int ocl_fan_current_fan(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x03;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadOneByte;
    cl->buf[3] = FAN_Select;

    cl->hid_wrapper(cl->handle, cl->buf, 8);
    return cl->buf[2];
}

int ocl_fan_get_fan_count(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    cl->buf[0] = 0x03;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = WriteOneByte;
    cl->buf[3] = FAN_Count;
    cl->buf[4] = 0x00;

    cl->hid_wrapper(cl->handle, cl->buf, 8);
    return cl->buf[2];
}

int ocl_fan_get_fan_mode(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x03;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadOneByte;
    cl->buf[3] = FAN_Mode;
    cl->buf[4] = 0x00;

    cl->hid_wrapper(cl->handle, cl->buf, 8);
    return cl->buf[2];
}

int ocl_fan_set_fan_mode(OCL_Fan *fan, int mode)
{
    OCL_Link *cl = fan->link;
    cl->buf[0] = 0x03;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = WriteOneByte;
    cl->buf[3] = FAN_Mode;
    cl->buf[4] = mode;
    cl->hid_wrapper(cl->handle, cl->buf, 8);
    return 0;
}

int ocl_fan_get_fan_rpm(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x04;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadTwoBytes;
    cl->buf[3] = FAN_FixedRPM;
    cl->buf[4] = 0x00;
    cl->hid_wrapper(cl->handle, cl->buf, 8);
    int RPM = cl->buf[3] << 8;
    RPM += cl->buf[2];
    return RPM;
}

int ocl_fan_set_fan_rpm(OCL_Fan *fan, int rpm)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x05;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = WriteTwoBytes;
    cl->buf[3] = FAN_FixedRPM;
    cl->buf[4] = rpm & 0x00ff;
    cl->buf[5] = rpm >> 8;
    cl->hid_wrapper(cl->handle, cl->buf, 8);
    return 0;
}

int ocl_fan_get_fan_pwm(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x03;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadOneByte;
    cl->buf[3] = FAN_FixedPWM;
    cl->hid_wrapper(cl->handle, cl->buf, 8);
    return cl->buf[2];
}

int ocl_fan_set_fan_pwm(OCL_Fan *fan, int pwm)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x04;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = WriteOneByte;
    cl->buf[3] = FAN_FixedPWM;
    cl->buf[4] = pwm;
    cl->hid_wrapper(cl->handle, cl->buf, 8);
    return 0;
}

int ocl_fan_get_current_rpm(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x03;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadTwoBytes;
    cl->buf[3] = FAN_ReadRPM;
    cl->hid_wrapper(cl->handle, cl->buf, 8);
    int RPM = cl->buf[3] << 8;
    RPM += cl->buf[2];
    return RPM;
}

int ocl_fan_get_max_rpm(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x03;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadTwoBytes;
    cl->buf[3] = FAN_MaxRecordedRPM;
    cl->hid_wrapper(cl->handle, cl->buf, 8);
    int MaxRPM = cl->buf[3] << 8;
    MaxRPM += cl->buf[2];
    return MaxRPM;
}

int ocl_fan_get_fan_underspeed_threshold(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x03;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadTwoBytes;
    cl->buf[3] = FAN_UnderSpeedThreshold;
    cl->hid_wrapper(cl->handle, cl->buf, 8);
    int threshold = cl->buf[3] << 8;
    threshold += cl->buf[2];
    return 0;
}

int ocl_fan_set_fan_underspeed_threshold(OCL_Fan *fan, int thresh)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x05;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = WriteTwoBytes;
    cl->buf[3] = FAN_UnderSpeedThreshold;
    cl->buf[4] = thresh & 0x00FF;
    cl->buf[5] = thresh >> 8;
    return 0;
}

ocl_fan_rpm_table *ocl_fan_get_rpm_table(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;

    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x0F;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadThreeBytes;
    cl->buf[3] = FAN_RPMTable;
    cl->buf[4] = 0x0A;
    cl->hid_wrapper(cl->handle, cl->buf, 24);

    ocl_fan_rpm_table *table = malloc(sizeof(ocl_fan_rpm_table));

    table->one = cl->buf[4] << 8;
    table->one += cl->buf[3];

    table->two = cl->buf[6] << 8;
    table->two += cl->buf[5];

    table->three = cl->buf[8] << 8;
    table->three += cl->buf[7];

    table->four = cl->buf[10] << 8;
    table->four += cl->buf[9];

    table->five = cl->buf[12] << 8;
    table->five += cl->buf[11];

    return table;
}

int ocl_fan_set_rpm_table(OCL_Fan *fan, ocl_fan_rpm_table table)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x0F;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = WriteThreeBytes;
    cl->buf[3] = FAN_RPMTable;
    cl->buf[4] = 0x0A;

    cl->buf[5] = table.one & 0x00FF;
    cl->buf[6] = table.one >> 8;

    cl->buf[7] = table.two & 0x00FF;
    cl->buf[8] = table.two >> 8;

    cl->buf[9] = table.three & 0x00FF;
    cl->buf[10] = table.three >> 8;

    cl->buf[11] = table.four & 0x00FF;
    cl->buf[12] = table.four >> 8;

    cl->buf[13] = table.five & 0x00FF;
    cl->buf[14] = table.five >> 8;

    cl->hid_wrapper(cl->handle, cl->buf, 24);

    return 0;
}

ocl_fan_temp_table *ocl_fan_get_temp_table(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x0F;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadThreeBytes;
    cl->buf[3] = FAN_TempTable;
    cl->buf[4] = 0x0A;
    cl->hid_wrapper(cl->handle, cl->buf, 24);

    ocl_fan_temp_table *table = malloc(sizeof(ocl_fan_temp_table));

    table->one = cl->buf[4] << 8;
    table->one += cl->buf[3];

    table->two = cl->buf[6] << 8;
    table->two += cl->buf[5];

    table->three = cl->buf[8] << 8;
    table->three += cl->buf[7];

    table->four = cl->buf[10] << 8;
    table->four += cl->buf[9];

    table->five = cl->buf[12] << 8;
    table->five += cl->buf[11];

    return table;
}

int ocl_fan_set_temp_table(OCL_Fan *fan, ocl_fan_temp_table table)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));
    cl->buf[0] = 0x0F;
    cl->buf[1] = cl->command_id++;
    cl->buf[2] = ReadThreeBytes;
    cl->buf[3] = FAN_TempTable;
    cl->buf[4] = 0x0A;
    cl->hid_wrapper(cl->handle, cl->buf, 24);

    cl->buf[5] = table.one & 0x00FF;
    cl->buf[6] = table.one >> 8;

    cl->buf[7] = table.two & 0x00FF;
    cl->buf[8] = table.two >> 8;

    cl->buf[9] = table.three & 0x00FF;
    cl->buf[10] = table.three >> 8;

    cl->buf[11] = table.four & 0x00FF;
    cl->buf[12] = table.four >> 8;

    cl->buf[13] = table.five & 0x00FF;
    cl->buf[14] = table.five >> 8;

    cl->hid_wrapper(cl->handle, cl->buf, 24);

    return 0;
}

static char *ocl_fan_get_fan_mode_string(int mode)
{
    char *mode_string = NULL;
    switch (mode) {
    case FixedPWM:
        asprintf(&mode_string, "Fixex PWM");
        break;
    case FixedRPM:
        asprintf(&mode_string, "Fixed RPM");
        break;
    case Default:
        asprintf(&mode_string, "Default");
        break;
    case Quiet:
        asprintf(&mode_string, "Quiet");
        break;
    case Balanced:
        asprintf(&mode_string, "Balanced");
        break;
    case Performance:
        asprintf(&mode_string, "Performance");
        break;
    case Custom:
        asprintf(&mode_string, "Custom");
        break;

    default:
        asprintf(&mode_string, "N/A (%02X)", mode);
        break;
    }

    return mode_string;
}

void ocl_fan_print_info(CorsairFanInfo fan)
{
    fprintf(stdout, "%s:\n", fan.name);
    fprintf(stdout, "\tMode: %s\n", ocl_fan_get_fan_mode_string(fan.mode));
    fprintf(stdout, "\tRPM: %i\n", fan.rpm);
}

int ocl_fan_connected_fans(OCL_Fan *fan)
{
    OCL_Link *cl = fan->link;
    int fans = 0, i = 0, fanMode = 0;

    for (i = 0; i < 5; i++) {
        memset(cl->buf, 0x00, sizeof(cl->buf));
        // Read fan Mode
        cl->buf[0] = 0x07;              // Length
        cl->buf[1] = cl->command_id++;  // Command ID
        cl->buf[2] = WriteOneByte;      // Command Opcode
        cl->buf[3] = FAN_Select;        // Command data...
        cl->buf[4] = i;                 // select fan
        cl->buf[5] = cl->command_id++;  // Command ID
        cl->buf[6] = ReadOneByte;       // Command Opcode
        cl->buf[7] = FAN_Mode;          // Command data...

        int res = hid_write(cl->handle, cl->buf, 11);
        jml_check(res >= 0, "Unable to write -- %ls", hid_error(cl->handle));
        res = cl->hid_read_wrapper(cl->handle, cl->buf);
        jml_check(res >= 0, "Unable to read -- %ls", hid_error(cl->handle));
        fanMode = cl->buf[4];

        if (fanMode != 0x03) {
            fans++;
        }
    }

    return fans;
}

void ocl_fan_read_fans_info(OCL_Fan *fan, int idx, CorsairFanInfo *fan_info)
{
    OCL_Link *cl = fan->link;
    int res = 0;

    if (idx < 4) {
        asprintf(&fan_info->name, "Fan %i", idx + 1);
    }
    else {
        asprintf(&fan_info->name, "Pump");
    }

    memset(cl->buf, 0x00, sizeof(cl->buf));
    // Read fan Mode
    cl->buf[0] = 0x07;              // Length
    cl->buf[1] = cl->command_id++;  // Command ID
    cl->buf[2] = WriteOneByte;      // Command Opcode
    cl->buf[3] = FAN_Select;        // Command data...
    cl->buf[4] = idx;               // select fan
    cl->buf[5] = cl->command_id++;  // Command ID
    cl->buf[6] = ReadOneByte;       // Command Opcode
    cl->buf[7] = FAN_Mode;          // Command data...

    res = hid_write(cl->handle, cl->buf, 11);
    if (res < 0) {
        fprintf(stderr, "Error: Unable to write() %s\n",
                (char *)hid_error(cl->handle));
    }

    res = cl->hid_read_wrapper(cl->handle, cl->buf);
    if (res < 0) {
        fprintf(stderr, "Error: Unable to read() %s\n",
                (char *)hid_error(cl->handle));
    }
    fan_info->mode = cl->buf[4] & 0x0E;

    memset(cl->buf, 0x00, sizeof(cl->buf));
    // Read fan RPM
    cl->buf[0] = 0x07;              // Length
    cl->buf[1] = cl->command_id++;  // Command ID
    cl->buf[2] = WriteOneByte;      // Command Opcode
    cl->buf[3] = FAN_Select;        // Command data...
    cl->buf[4] = idx;               // select fan
    cl->buf[5] = cl->command_id++;  // Command ID
    cl->buf[6] = ReadTwoBytes;      // Command Opcode
    cl->buf[7] = FAN_ReadRPM;       // Command data...

    res = hid_write(cl->handle, cl->buf, 11);
    if (res < 0) {
        fprintf(stderr, "Error: Unable to write() %s\n",
                (char *)hid_error(cl->handle));
    }

    res = cl->hid_read_wrapper(cl->handle, cl->buf);
    // All data is little-endian.
    int rpm = cl->buf[5] << 8;
    rpm += cl->buf[4];

    fan_info->rpm = rpm;
}

int ocl_fan_set_fans_info(OCL_Fan *fan, int idx, CorsairFanInfo fan_info)
{
    OCL_Link *cl = fan->link;
    memset(cl->buf, 0x00, sizeof(cl->buf));

    if (fan_info.mode == FixedPWM || fan_info.mode == FixedRPM ||
        fan_info.mode == Default || fan_info.mode == Quiet ||
        fan_info.mode == Balanced || fan_info.mode == Performance ||
        fan_info.mode == Custom) {
        cl->buf[0] = 0x0b;              // Length
        cl->buf[1] = cl->command_id++;  // Command ID
        cl->buf[2] = WriteOneByte;      // Command Opcode
        cl->buf[3] = FAN_Select;        // Command data...
        cl->buf[4] = idx;               // select fan
        cl->buf[5] = cl->command_id++;  // Command ID
        cl->buf[6] = WriteOneByte;      // Command Opcode
        cl->buf[7] = FAN_Mode;          // Command data...
        cl->buf[8] = fan_info.mode;
        cl->buf[9] = cl->command_id++;  // Command ID
        cl->buf[10] = ReadOneByte;      // Command Opcode
        cl->buf[11] = FAN_Mode;         // Command data...

        int res = hid_write(cl->handle, cl->buf, 17);
        if (res < 0) {
            fprintf(stderr, "%s", (char *)hid_error(cl->handle));
            return 1;
        }

        res = cl->hid_read_wrapper(cl->handle, cl->buf);
        if (res < 0) {
            fprintf(stderr, "%s", (char *)hid_error(cl->handle));
            return 1;
        }
        if (fan_info.mode != cl->buf[6]) {
            fprintf(stderr, "Cannot set fan mode.");
            return 1;
        }
    }
    else {
        fprintf(stderr, "Invalid fan mode.");
        return 1;
    }
    if (fan_info.rpm != 0) {
        memset(cl->buf, 0x00, sizeof(cl->buf));

        cl->buf[0] = 0x0b;              // Length
        cl->buf[1] = cl->command_id++;  // Command ID
        cl->buf[2] = WriteOneByte;      // Command Opcode
        cl->buf[3] = FAN_Select;        // Command data...
        cl->buf[4] = idx;               // select fan
        cl->buf[5] = cl->command_id++;  // Command ID
        cl->buf[6] = WriteTwoBytes;     // Command Opcode
        cl->buf[7] = FAN_FixedRPM;      // Command data...
        cl->buf[8] = fan_info.rpm & 0x00FF;
        cl->buf[9] = fan_info.rpm >> 8;
        cl->buf[10] = cl->command_id++;  // Command ID
        cl->buf[11] = ReadTwoBytes;      // Command Opcode
        cl->buf[12] = FAN_ReadRPM;       // Command data...

        int res = hid_write(cl->handle, cl->buf, 18);
        if (res < 0) {
            fprintf(stderr, "%s", (char *)hid_error(cl->handle));
            return 1;
        }

        res = cl->hid_read_wrapper(cl->handle, cl->buf);
        if (res < 0) {
            fprintf(stderr, "%s", (char *)hid_error(cl->handle));
            return 1;
        }
        // All data is little-endian.
        int rpm = cl->buf[5] << 8;
        rpm += cl->buf[4];
        if (fan_info.rpm != rpm) {
            fprintf(stderr, "Cannot set fan RPM.");
            return 1;
        }
    }

    return 0;
}

void ocl_fan_free(OCL_Fan *fan)
{
    if (!fan) {
        return;
    }

    free(fan);
}
