#include "clink_darwin.h"

static void default_matching_cb(void *ctx,
                                IOReturn result,
                                void *sender,
                                IOHIDDeviceRef device)
{
}

static void default_removal_cb(void *ctx,
                               IOReturn result,
                               void *sender,
                               IOHIDDeviceRef device)
{
}

static void default_value_cb(void *ctx,
                             IOReturn result,
                             void *sender,
                             IOHIDValueRef value)
{
}

static void default_report_cb(void *ctx,
                              IOReturn result,
                              void *sender,
                              IOHIDReportType type,
                              uint32_t report_id,
                              uint8_t *report,
                              CFIndex report_len)
{
}

static CFDictionaryRef default_matching_dict(void)
{
    CFMutableDictionaryRef result;
    result = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                       0,
                                       &kCFTypeDictionaryKeyCallBacks,
                                       &kCFTypeDictionaryValueCallBacks);

    return result;
}

/* cld_connection_alloc {{{ */
static inline CLD_Connection *cld_connection_alloc(
    CFDictionaryRef matching_dict,
    IOHIDDeviceCallback matching_callback,
    IOHIDDeviceCallback removal_callback,
    IOHIDReportCallback report_callback,
    IOHIDValueCallback value_callback)
{
    CLD_Connection *conn = calloc(1, sizeof(CLD_Connection));

    if (matching_dict == NULL) {
        conn->matching_dict = default_matching_dict();
    }
    else {
        conn->matching_dict = matching_dict;
    }

    if (matching_callback == NULL) {
        conn->device_matching_callback = default_matching_cb;
    }
    else {
        conn->device_matching_callback = matching_callback;
    }

    if (removal_callback == NULL) {
        conn->device_removal_callback = default_removal_cb;
    }
    else {
        conn->device_removal_callback = removal_callback;
    }

    if (report_callback == NULL) {
        conn->device_report_callback = default_report_cb;
    }
    else {
        conn->device_report_callback = report_callback;
    }

    if (value_callback == NULL) {
        conn->device_value_callback = default_value_cb;
    }
    else {
        conn->device_value_callback = value_callback;
    }

    return conn;
}

/* }}} cld_connection_alloc */

CLD_Connection *cld_connection_new(CFDictionaryRef matching_dict,
                                   IOHIDDeviceCallback matching_callback,
                                   IOHIDDeviceCallback removal_callback,
                                   IOHIDReportCallback report_callback,
                                   IOHIDValueCallback value_callback)
{
    CLD_Connection *conn = cld_connection_alloc(matching_dict,
                                                matching_callback,
                                                removal_callback,
                                                report_callback,
                                                value_callback);

    conn->manager =
        IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    IOHIDManagerSetDeviceMatching(conn->manager, conn->matching_dict);

    return conn;
}

static inline void cld_handle_ioreturn_err(void *ctx, IOReturn ret)
{
}

int cld_connection_open(CLD_Connection *conn)
{
    if (conn == NULL) {
        fprintf(stderr, "Passed a null connection\n");
        return -1;
    }

    if (conn->manager == NULL) {
        fprintf(stderr, "Connection <%p> has a null manager\n", (void *)conn);
        return -1;
    }

    IOReturn rc = IOHIDManagerOpen(conn->manager, kIOHIDOptionsTypeNone);
    if (rc != kIOReturnSuccess) {
        cld_handle_ioreturn_err(NULL, rc);
        return -1;
    }
}

int cld_connection_close(CLD_Connection *connection);

void cld_connection_destroy(CLD_Connection *connection);
