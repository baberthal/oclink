#ifndef OCL_HID_UTILS
#define OCL_HID_UTILS 1

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <locale.h>
#include <sys/time.h>
#include <unistd.h>
#include <wchar.h>

#include "clink_darwin_cfg.h"
#include "clink_darwin_export.h"
#include "pthread_barrier.h"

struct clx_device_info {
    char *path;                     ///< Platform-specific device path
    unsigned short vendor_id;       ///< Device Vendor ID
    unsigned short product_id;      ///< Device Product ID
    wchar_t *serial_number;         ///< Device Serial Number
    unsigned short release_number;  ///< Release Number (aka Version Number)
    wchar_t *manufacturer_string;   ///< Manufacturer String
    wchar_t *product_string;        ///< Product String
    unsigned short usage_page;      ///< Device Usage Page (OSX Only)
    unsigned short usage;           ///< Device Usage (OSX Only)
    int interface_number;           ///< The USB Interface for this device
    struct clx_device_info *next;   ///< Pointer to the next one in list
    struct clx_device_info *prev;   ///< Pointer to the previous one in list
};

struct clx_device_list {
    struct clx_device_info *first;
    struct clx_device_info *last;
    int count;
};

struct clx_device_list *clx_device_list_new(void);
void clx_device_list_destroy(struct clx_device_list *list);
void clx_device_list_clear(struct clx_device_list *list);
void clx_device_list_clear_destroy(struct clx_device_list *list);

#define clx_device_list_count(L) ((L)->count)
#define clx_device_list_first(L) ((L)->first)
#define clx_device_list_last(L) ((L)->last)

void clx_device_list_push(struct clx_device_list *list,
                          struct clx_device_info *info);
struct clx_device_info *clx_device_list_pop(struct clx_device_list *list);

void clx_device_list_unshift(struct clx_device_list *list,
                             struct clx_device_info *info);
struct clx_device_info *clx_device_list_shift(struct clx_device_list *list);

struct clx_device_info *clx_device_list_remove(struct clx_device_list *list,
                                               struct clx_device_info *info);

#define CLX_LIST_FOREACH(LIST, START, M, V) \
    struct clx_device_info *_info = NULL;   \
    struct clx_device_info *V = NULL;       \
    for (V = _info = LIST->START; _info != NULL; V = _info = _info->M)

/**
 * @brief Linked list to hold input reports received from the device
 */
struct input_report {
    uint8_t *data;              ///< The data received
    size_t len;                 ///< The length of the data, in bytes
    struct input_report *next;  ///< A pointer to the next input report
};

struct clx_hid_device_ {
    IOHIDDeviceRef device_handle;
    int blocking;
    int uses_numbered_reports;
    int disconnected;

    CFStringRef run_loop_mode;
    CFRunLoopRef run_loop;
    CFRunLoopSourceRef source;

    uint8_t *input_report_buf;
    CFIndex max_input_report_len;
    struct input_report *input_reports;

    pthread_t thread;
    pthread_mutex_t mutex; /* Protects input reports */
    pthread_cond_t condition;
    pthread_barrier_t barrier;
    pthread_barrier_t shutdown_barrier;
    int shutdown_thread;
};

typedef struct clx_hid_device_ clx_hid_device;

typedef unsigned char *clx_buf_t;
typedef const unsigned char *const_clx_buf_t;

int clx_hid_init(void);

int clx_hid_exit(void);

struct clx_device_info *clx_hid_enumerate(unsigned short vid,
                                          unsigned short pid);

void clx_hid_free_enumeration(struct clx_device_info *devs);

clx_hid_device *clx_hid_open(unsigned short vendor_id,
                             unsigned short product_id,
                             const wchar_t *serial_number);

clx_hid_device *clx_hid_open_path(const char *path);

int clx_hid_write(clx_hid_device *dev, const_clx_buf_t data, size_t len);

int clx_hid_read_timeout(clx_hid_device *dev,
                         clx_buf_t data,
                         size_t len,
                         int ms_wait);

int clx_hid_read(clx_hid_device *dev, clx_buf_t data, size_t len);

int clx_hid_set_nonblocking(clx_hid_device *dev, int nonblock);

int clx_hid_send_feature_report(clx_hid_device *dev,
                                const_clx_buf_t data,
                                size_t len);

int clx_hid_get_feature_report(clx_hid_device *dev, clx_buf_t data, size_t len);

void clx_hid_close(clx_hid_device *dev);

int clx_hid_get_manufacturer_string(clx_hid_device *dev,
                                    wchar_t *str,
                                    size_t maxlen);

int clx_hid_get_product_string(clx_hid_device *dev,
                               wchar_t *str,
                               size_t maxlen);

int clx_hid_get_serial_number_string(clx_hid_device *dev,
                                     wchar_t *str,
                                     size_t maxlen);

int clx_hid_get_indexed_string(clx_hid_device *dev,
                               wchar_t *str,
                               size_t maxlen);

const wchar_t *clx_hid_error(clx_hid_device *dev);

#endif /* ifndef OCL_HID_UTILS */
