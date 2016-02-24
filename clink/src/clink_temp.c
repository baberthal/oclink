//
//  clink_temp.c
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#include "clink_temp.h"
#include "clink_proto.h"
#include "jml_debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OCL_Temp *ocl_temp_alloc(OCL_Link *link)
{
    OCL_Temp *temp = malloc(sizeof(OCL_Temp));
    temp->link = link;
    temp->selected_sensor = 0;

    return temp;
}

int ocl_temp_select_sensor(OCL_Temp *temp, int sensor)
{
    temp->selected_sensor = sensor;

    OCL_Link *link = temp->link;

    memset(link->buf, 0x00, sizeof(link->buf));

    link->buf[0] = 0x04;
    link->buf[1] = link->command_id++;
    link->buf[2] = WriteOneByte;
    link->buf[3] = TEMP_SelectActiveSensor;
    link->buf[4] = sensor;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    jml_check(link->buf[2] == sensor, "Something strange happened.");
    return link->buf[2];
}

int ocl_temp_current_sensor(OCL_Temp *temp)
{
    OCL_Link *link = temp->link;

    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x03;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadOneByte;
    link->buf[3] = TEMP_SelectActiveSensor;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    jml_check(link->buf[2] == temp->selected_sensor,
              "Something strange happened.");
    return link->buf[2];
}

int ocl_temp_get_temp_sensors(OCL_Temp *temp)
{
    OCL_Link *link = temp->link;

    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x03;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadOneByte;
    link->buf[3] = TEMP_CountSensors;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return link->buf[2];
}

int ocl_temp_get_temp(OCL_Temp *temp)
{
    OCL_Link *link = temp->link;

    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x03;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadTwoBytes;
    link->buf[3] = TEMP_Read;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    int temperature = link->buf[3] << 8;
    temperature += link->buf[2];

    return temperature;
}

int ocl_temp_get_temp_limit(OCL_Temp *temp)
{
    OCL_Link *link = temp->link;

    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x03;
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadTwoBytes;
    link->buf[3] = TEMP_Limit;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    int limit = link->buf[5] << 8;
    limit += link->buf[4];

    return limit;
}

int ocl_temp_set_temp_limit(OCL_Temp *temp, int limit)
{
    OCL_Link *link = temp->link;

    memset(link->buf, 0x00, sizeof(link->buf));
    link->buf[0] = 0x05;
    link->buf[1] = link->command_id++;
    link->buf[2] = WriteTwoBytes;
    link->buf[3] = TEMP_Limit;
    link->buf[4] = limit & 0x00FF;
    link->buf[5] = limit >> 8;

    int res = hid_write(link->handle, link->buf, 11);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    if (res < 0)
        return -1;

    res = link->hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));
    if (res < 0)
        return -1;

    return 0;
}

void ocl_temp_free(OCL_Temp *temp)
{
    if (!temp) {
        return;
    }

    if (temp->link) {
        temp->link = NULL;
    }

    free(temp);
}
