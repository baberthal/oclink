#ifndef CLINK_DARWIN_H
#define CLINK_DARWIN_H 1

#include "clink_darwin_cfg.h"
#include "clink_darwin_export.h"
#include "util.h"

typedef struct _CLD_Device {
    OCL_List *elements;
    IOHIDCallback removal_callback;
} CLD_Device;

typedef struct _CLD_Connection {
    IOHIDManagerRef manager;
    CFDictionaryRef matching_dict;

    IOHIDDeviceRef *devices;
    OCL_List *device_enumerator;

    CFRunLoopRef run_loop;

    IOHIDDeviceCallback device_matching_callback;
    IOHIDDeviceCallback device_removal_callback;
    IOHIDReportCallback device_report_callback;
    IOHIDValueCallback device_value_callback;
} CLD_Connection;

CLINK_DARWIN_EXPORT CLD_Connection *cld_connection_new(
    CFDictionaryRef matching_dict, IOHIDDeviceCallback matching_callback,
    IOHIDDeviceCallback removal_callback, IOHIDReportCallback report_callback,
    IOHIDValueCallback value_callback);

#define cld_connection_new2() cld_connection_new(NULL, NULL, NULL, NULL, NULL)

CLINK_DARWIN_EXPORT int cld_connection_open(CLD_Connection *conn);

CLINK_DARWIN_EXPORT int cld_connection_close(CLD_Connection *connection);

CLINK_DARWIN_EXPORT void cld_connection_destroy(CLD_Connection *connection);

#endif /* ifndef CLINK_DARWIN_H */

/* vim: set ts=8 sw=4 tw=0 ft=cpp noet :*/
