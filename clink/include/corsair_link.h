//
//  corsair_link.h
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#ifndef corsair_link_h
#define corsair_link_h

#include <hidapi/hidapi.h>
#include <stdlib.h>

typedef unsigned char cl_buf_t;

typedef struct OCL_Link {
    hid_device *handle;
    cl_buf_t buf[256];
    uint32_t command_id;
    int device_id;
    struct hid_device_info *device_info;
    int (*hid_wrapper)(hid_device *handle, cl_buf_t *buf, size_t buf_size);
    int (*hid_read_wrapper)(hid_device *handle, cl_buf_t *buf);
} OCL_Link;

typedef enum {
    CL_H80i = 0x3b,
    CL_H100i = 0x3c,
    CL_H110i = 0x41,
} OCL_CorsairModels;

OCL_Link *const ocl_link_shared_link(void);
void ocl_link_close(OCL_Link *link);
void ocl_link_free(OCL_Link *link);
int ocl_link_get_device_id(OCL_Link *link);
int ocl_link_get_fw_version(OCL_Link *link);
int ocl_link_get_product_name(OCL_Link *link, char *out_name);
int ocl_link_get_device_status(OCL_Link *link);

#endif /* corsair_link_h */
