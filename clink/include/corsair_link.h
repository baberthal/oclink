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

/**
 *  @file corsair_link.h
 *  @brief The main hid functions to talk to the CorsairLink System
 *  @defgroup Link clink Link
 *  @{
 */

/**
 *  @brief Define a type for the \ref OCL_Link buffer. For convenience
 */
typedef unsigned char cl_buf_t;

/**
 *  @brief The main Link structure to handle HID reading and writing
 */
typedef struct OCL_Link {
    hid_device *handle;  ///< The HID Device Handle for the CorsairLink system
    cl_buf_t buf[256];   ///< The buffer to read and write HID Commands
    uint32_t command_id; ///< The command id of the link
    int device_id; ///< The device ID, as read by the \ref OCL_Link structure
    struct hid_device_info *device_info; ///< HID Device Info for the system
                                         /**
                                          *  @brief A pointer to a function to wrap HID calls for associated
                                          *   structures to use
                                          */
    int (*hid_wrapper)(hid_device *handle, cl_buf_t *buf, size_t buf_size);
    /**
     *  @brief A pointer to a function to wrap HID read calls
     */
    int (*hid_read_wrapper)(hid_device *handle, cl_buf_t *buf);
} OCL_Link;

/**
 * @brief An enum to hold the product IDs for our supported products
 */
typedef enum {
    CL_H80i  = 0x3b,
    CL_H100i = 0x3c,
    CL_H110i = 0x41,
} OCL_CorsairModels;

/**
 *  @brief Allocate and initialize the shared link structure
 *
 *  @return The shared link structure. Should always return the same structure
 */
OCL_Link *const ocl_link_shared_link(void);

/**
 *  @brief Close the link
 *
 *  @param link The link to close
 */
void ocl_link_close(OCL_Link *link);

/**
 *  @brief Free the link
 *
 *  @param link The link to free
 */
void ocl_link_free(OCL_Link *link);

/**
 *  @brief Get the device ID of the attached CorsairLink system
 *
 *  @param link The link to query
 *
 *  @return The device id of the attached CorsairLinkSystem
 */
int ocl_link_get_device_id(OCL_Link *link);

/**
 *  @brief Get the firmware version of the attached CorsairLink system
 *
 *  @param link The link to query
 *
 *  @return The firmware version
 */
int ocl_link_get_fw_version(OCL_Link *link);

/**
 *  @brief Copy the product name string into `out_name`
 *
 *  @param link     The link to query
 *  @param[out] out_name A pointer to copy the string into
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_link_get_product_name(OCL_Link *link, char *out_name);

/**
 *  @brief Get the device status of the attached CorsairLink system
 *
 *  @param link The link to query
 *
 *  @return The device status, expressed as an integer
 */
int ocl_link_get_device_status(OCL_Link *link);

/** @} */

#endif /* corsair_link_h */
