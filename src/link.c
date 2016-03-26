//
//  corsair_link.c
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#include "link.h"
#include <stdarg.h>
#include <string.h>
#include "config.h"
#include "jml_debug.h"
#include "proto.h"

static const unsigned short CorsairVendorID = 0x1b1c;
static const unsigned short CorsairProductID = 0x0c04;
static const int max_read_wait = 5000;

static int hid_read_wrapper(hid_device *handle, cl_buf_t *buf)
{
    int res = 0, sleep_total = 0;
    while (res == 0 && sleep_total < max_read_wait) {
        res = hid_read(handle, buf, sizeof(buf));
        if (res < 0) fprintf(stderr, "Unable to read from device.\n");

        SLEEP(100);
    }

    if (sleep_total == max_read_wait) {
        res = 0;
    }

    DEBUG_HID_READ(buf);

    return 1;
}

static int hid_wrapper(hid_device *handle, cl_buf_t *buf, size_t buf_size)
{
    DEBUG_HID_WRITE(buf);
    int id = buf[1];

    int res = hid_write(handle, buf, buf_size);
    jml_check(res >= 0, "Unable to write. -- %ls", hid_error(handle));
    if (res < 0) {
        return -1;
    }

    res = hid_read_wrapper(handle, buf);
    jml_check(res >= 0, "Unable to read. -- %ls", hid_error(handle));
    if (res < 0) {
        return -1;
    }

    if (buf[0] != id) {
        fprintf(stderr, "Got wrong reply: %d (expected %d)\n", buf[0], id);
    }

    return res;
}

OCL_Link *ocl_link_alloc(void)
{
    OCL_Link *link = malloc(sizeof(OCL_Link));
    link->handle = NULL;
    link->command_id = 0x81;
    link->hid_wrapper = hid_wrapper;
    link->hid_read_wrapper = hid_read_wrapper;

    return link;
}

static inline bool is_supported_device(int device_id)
{
    return (device_id == CL_H80i) || (device_id == CL_H100i) ||
           (device_id != CL_H110i);
}

int ocl_link_init(OCL_Link *link)
{
    if (link->handle != NULL) {
        fprintf(stderr, "Link already initialized.");
        return -1;
    }

    if (hid_init() == -1) return -1;

    link->handle = hid_open(CorsairVendorID, CorsairProductID, NULL);
    if (!link->handle) {
        fprintf(stderr,
                "[ERROR] - Unable to open Corsair H80i, "
                "H100i or H110i "
                "CPU Cooler.\n");
        return -1;
    }
    hid_set_nonblocking(link->handle, 1);

    link->device_id = ocl_link_get_device_id(link);

    if (!is_supported_device(link->device_id)) {
        fprintf(stderr, "Device ID: %2x mismatch.\n", link->device_id);
        ocl_link_close(link);
        ocl_link_free(link);
        return -1;
    }

    return 0;
}

void ocl_link_close(OCL_Link *link)
{
    if (link->handle == NULL) {
        return;
    }

    hid_close(link->handle);
    hid_exit();
    link->handle = NULL;
}

void ocl_link_free(OCL_Link *link)
{
    if (link->handle) {
        ocl_link_close(link);
    }

    if (link->device_info) {
        hid_free_enumeration(link->device_info);
    }

    free(link);
}

static inline void ocl_reset_buf(OCL_Link *link)
{
    memset(link->buf, 0x00, sizeof(link->buf));
}

int ocl_link_build_command(OCL_Link *link, int command_len,
                           CorsairLink_OpCode op_code,
                           CorsairLink_Command command, ...)
{
    // TODO: Implement
    int i = 0;
    int rc = 0;

    ocl_reset_buf(link);
    link->buf[0] = command_len;
    link->buf[1] = link->command_id++;
    link->buf[2] = op_code;
    link->buf[3] = command;

    va_list argp;
    va_start(argp, command);
    for (i = 4; i < command_len; i++) {
        link->buf[i] = va_arg(argp, int);
    }

    return rc;
}

int ocl_link_get_device_id(OCL_Link *link)
{
    memset(link->buf, 0, sizeof(link->buf));

    link->buf[0] = 0x03;  // Length
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadOneByte;
    link->buf[3] = DeviceID;
    link->buf[4] = 0x00;

    int res = hid_write(link->handle, link->buf, 17);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));
    hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return link->buf[2];
}

int ocl_link_get_fw_version(OCL_Link *link)
{
    memset(link->buf, 0, sizeof(link->buf));

    link->buf[0] = 0x03;  // Length
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadTwoBytes;
    link->buf[3] = FirmwareID;
    link->buf[4] = 0x00;

    int res = hid_write(link->handle, link->buf, 17);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));

    hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    int firmware = link->buf[3] << 8;
    firmware += link->buf[2];

    return firmware;
}

int ocl_link_get_product_name(OCL_Link *link, char *out_name)
{
    memset(link->buf, 0, sizeof(link->buf));

    link->buf[0] = 0x03;  // Length
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadThreeBytes;
    link->buf[3] = ProductName;
    link->buf[4] = 0x08;

    int res = hid_write(link->handle, link->buf, 8);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));

    hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    memcpy(out_name, link->buf + 3, 8);
    return 0;
}

int ocl_link_get_device_status(OCL_Link *link)
{
    memset(link->buf, 0, sizeof(link->buf));

    link->buf[0] = 0x03;  // Length
    link->buf[1] = link->command_id++;
    link->buf[2] = ReadOneByte;
    link->buf[3] = Status;
    link->buf[4] = 0x00;

    int res = hid_write(link->handle, link->buf, 17);
    jml_check(res >= 0, "Unable to write -- %ls", hid_error(link->handle));

    hid_read_wrapper(link->handle, link->buf);
    jml_check(res >= 0, "Unable to read -- %ls", hid_error(link->handle));

    return link->buf[2];
}
