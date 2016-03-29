#include "hid_utils.h"

struct clx_device_list *clx_device_list_new(void)
{
    return calloc(1, sizeof(struct clx_device_list));
}

void clx_device_list_destroy(struct clx_device_list *list)
{
    CLX_LIST_FOREACH(list, first, next, cur)
    {
        if (cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);
}

void clx_device_list_clear(struct clx_device_list *list)
{
    CLX_LIST_FOREACH(list, first, next, cur)
    {
        free(cur->path);
        free(cur->serial_number);
        free(cur->manufacturer_string);
        free(cur->product_string);
        free(cur);
    }
}

void clx_device_list_clear_destroy(struct clx_device_list *list)
{
    clx_device_list_clear(list);
    clx_device_list_destroy(list);
}

void clx_device_list_push(struct clx_device_list *list,
                          struct clx_device_info *info)
{
    if (info == NULL || list == NULL) {
        return;
    }

    if (list->last == NULL) {
        list->first = info;
        list->last = info;
    }
    else {
        list->last->next = info;
        info->prev = list->last;
        list->last = info;
    }

    list->count++;
}

struct clx_device_info *clx_device_list_pop(struct clx_device_list *list)
{
    struct clx_device_info *node = list->last;
    return node != NULL ? clx_device_list_remove(list, node) : NULL;
}

void clx_device_list_unshift(struct clx_device_list *list,
                             struct clx_device_info *info)
{
    if (info == NULL || list == NULL) {
        return;
    }

    if (list->first == NULL) {
        list->first = info;
        list->last = info;
    }
    else {
        info->next = list->first;
        list->first->prev = info;
        list->first = info;
    }

    list->count++;
}

struct clx_device_info *clx_device_list_shift(struct clx_device_list *list)
{
    struct clx_device_info *node = list->first;
    return node != NULL ? clx_device_list_remove(list, node) : NULL;
}

struct clx_device_info *clx_device_list_remove(struct clx_device_list *list,
                                               struct clx_device_info *info)
{
    struct clx_device_info *result = NULL;
    if (!list->first && !list->last) {
        fprintf(stderr, "List is empty!\n");
        return NULL;
    }

    if (info == NULL) {
        fprintf(stderr, "Node can't be NULL\n");
        return NULL;
    }

    if (info == list->first && info == list->last) {
        list->first = NULL;
        list->last = NULL;
    }
    else if (info == list->first) {
        list->first = info->next;
        if (list->first == NULL) {
            fprintf(stderr, "Invalid List!\n");
            return NULL;
        }
        list->first->prev = NULL;
    }
    else if (info == list->last) {
        list->last = info->prev;
        if (list->first == NULL) {
            fprintf(stderr, "Invalid List!\n");
            return NULL;
        }
        list->last->next = NULL;
    }
    else {
        struct clx_device_info *after = info->next;
        struct clx_device_info *before = info->prev;
        after->prev = before;
        before->next = after;
    }

    list->count--;
    result = info;

    return result;
}

static int return_data(clx_hid_device *dev, uint8_t *data, size_t len);

static clx_hid_device *new_hid_device(void)
{
    clx_hid_device *dev = calloc(1, sizeof(clx_hid_device));
    dev->device_handle = NULL;
    dev->blocking = 1;
    dev->uses_numbered_reports = 0;
    dev->disconnected = 0;
    dev->run_loop_mode = NULL;
    dev->run_loop = NULL;
    dev->source = NULL;
    dev->input_report_buf = NULL;
    dev->input_reports = NULL;
    dev->shutdown_thread = 0;

    pthread_mutex_init(&dev->mutex, NULL);
    pthread_cond_init(&dev->condition, NULL);
    pthread_barrier_init(&dev->barrier, NULL, 2);
    pthread_barrier_init(&dev->shutdown_barrier, NULL, 2);

    return dev;
}

static void free_clx_hid_device(clx_hid_device *dev)
{
    if (!dev) return;

    struct input_report *rpt = dev->input_reports;
    while (rpt) {
        struct input_report *next = rpt->next;
        free(rpt->data);
        free(rpt);
        rpt = next;
    }

    if (dev->run_loop_mode) CFRelease(dev->run_loop_mode);
    if (dev->run_loop) CFRelease(dev->run_loop);
    free(dev->input_report_buf);

    pthread_barrier_destroy(&dev->shutdown_barrier);
    pthread_barrier_destroy(&dev->barrier);
    pthread_cond_destroy(&dev->condition);
    pthread_mutex_destroy(&dev->mutex);

    free(dev);
}

static IOHIDManagerRef hid_mgr = 0x0;

static int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key)
{
    CFTypeRef ref;
    int32_t value;

    ref = IOHIDDeviceGetProperty(device, key);
    if (ref) {
        if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
            CFNumberGetValue((CFNumberRef)ref, kCFNumberSInt32Type, &value);
            return value;
        }
    }

    return 0;
}

static unsigned short get_vendor_id(IOHIDDeviceRef device)
{
    return get_int_property(device, CFSTR(kIOHIDVendorIDKey));
}

static unsigned short get_product_id(IOHIDDeviceRef device)
{
    return get_int_property(device, CFSTR(kIOHIDProductIDKey));
}

static int32_t get_location_id(IOHIDDeviceRef device)
{
    return get_int_property(device, CFSTR(kIOHIDLocationIDKey));
}

static int32_t get_max_report_length(IOHIDDeviceRef device)
{
    return get_int_property(device, CFSTR(kIOHIDMaxInputReportSizeKey));
}

static int get_string_property(IOHIDDeviceRef device,
                               CFStringRef prop,
                               wchar_t *buf,
                               size_t len)
{
    CFStringRef str;

    if (!len) {
        return 0;
    }

    str = IOHIDDeviceGetProperty(device, prop);
    buf[0] = 0;

    if (str) {
        CFIndex str_len = CFStringGetLength(str);
        CFRange range;
        CFIndex used_buf_len;
        CFIndex chars_copied;

        len--;

        range.location = 0;
        range.length = ((size_t)str_len > len) ? len : (size_t)str_len;
        chars_copied = CFStringGetBytes(str,
                                        range,
                                        kCFStringEncodingUTF32LE,
                                        (char)'?',
                                        FALSE,
                                        (UInt8 *)buf,
                                        len * sizeof(wchar_t),
                                        &used_buf_len);
        if ((size_t)chars_copied == len) {
            buf[len] = 0;
        }
        else {
            buf[chars_copied] = 0;
        }

        return 0;
    }
    else {
        return -1;
    }
}

static int get_string_property_utf8(IOHIDDeviceRef device,
                                    CFStringRef prop,
                                    char *buf,
                                    size_t len)
{
    CFStringRef str;
    if (!len) {
        return 0;
    }

    str = IOHIDDeviceGetProperty(device, prop);
    buf[0] = 0;
    if (str) {
        len--;

        CFIndex str_len = CFStringGetLength(str);
        CFRange range;
        range.location = 0;
        range.length = str_len;
        CFIndex used_buf_len;
        CFIndex chars_copied;
        chars_copied = CFStringGetBytes(str,
                                        range,
                                        kCFStringEncodingUTF8,
                                        (char)'?',
                                        FALSE,
                                        (UInt8 *)buf,
                                        len,
                                        &used_buf_len);

        if ((size_t)used_buf_len == len) {
            buf[len] = 0;
        }
        else {
            buf[used_buf_len] = 0;
        }

        return used_buf_len;
    }
    else {
        return 0;
    }
}

static int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
    return get_string_property(device, CFSTR(kIOHIDSerialNumberKey), buf, len);
}

static int get_manufacturer_string(IOHIDDeviceRef device,
                                   wchar_t *buf,
                                   size_t len)
{
    return get_string_property(device, CFSTR(kIOHIDManufacturerKey), buf, len);
}

static int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
    return get_string_property(device, CFSTR(kIOHIDProductKey), buf, len);
}

/*  Implementation of wcsdup() for OSX */
static wchar_t *dup_wcs(const wchar_t *s)
{
    size_t len = wcslen(s);
    wchar_t *ret = malloc((len + 1) * sizeof(wchar_t));
    wcscpy(ret, s);

    return ret;
}

static int make_path(IOHIDDeviceRef device, char *buf, size_t len)
{
    int res;
    unsigned short vid, pid;
    char transport[32];
    int32_t location;

    buf[0] = '\0';

    res = get_string_property_utf8(
        device, CFSTR(kIOHIDTransportKey), transport, sizeof(transport));

    if (!res) {
        return -1;
    }

    location = get_location_id(device);
    vid = get_vendor_id(device);
    pid = get_product_id(device);

    res =
        snprintf(buf, len, "%s_%04hx_%04hx_%x", transport, vid, pid, location);

    buf[len - 1] = '\0';

    return res + 1;
}

static int init_hid_mgr(void)
{
    hid_mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (hid_mgr) {
        IOHIDManagerSetDeviceMatching(hid_mgr, NULL);
        IOHIDManagerScheduleWithRunLoop(
            hid_mgr, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        return 0;
    }

    return -1;
}

int clx_hid_init(void)
{
    if (!hid_mgr) {
        return init_hid_mgr();
    }

    return 0;  // Already initialized
}

int clx_hid_exit(void)
{
    if (hid_mgr) {
        IOHIDManagerClose(hid_mgr, kIOHIDOptionsTypeNone);
        CFRelease(hid_mgr);
        hid_mgr = NULL;
    }

    return 0;
}

static void process_pending_events(void)
{
    SInt32 res;
    do {
        res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.001, FALSE);
    } while (res != kCFRunLoopRunFinished && res != kCFRunLoopRunTimedOut);
}

#define BUF_LEN 256
struct clx_device_info *clx_hid_enumerate(unsigned short vid,
                                          unsigned short pid)
{
    struct clx_device_info *root = NULL;
    struct clx_device_info *cur_dev = NULL;
    CFIndex num_devices;
    int i;

    if (clx_hid_init() < 0) {
        return NULL;
    }

    process_pending_events();

    IOHIDManagerSetDeviceMatching(hid_mgr, NULL);
    CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);
    num_devices = CFSetGetCount(device_set);
    IOHIDDeviceRef *device_array = calloc(num_devices, sizeof(IOHIDDeviceRef));
    CFSetGetValues(device_set, (const void **)device_array);

    for (i = 0; i < num_devices; i++) {
        unsigned short dev_vid;
        unsigned short dev_pid;
        wchar_t buf[BUF_LEN];
        char cbuf[BUF_LEN];

        IOHIDDeviceRef dev = device_array[i];

        if (!dev) {
            continue;
        }

        dev_vid = get_vendor_id(dev);
        dev_pid = get_product_id(dev);

        if ((vid == 0x0 || vid == dev_vid) && (pid == 0x0 || pid == dev_pid)) {
            struct clx_device_info *tmp;
            size_t len;

            tmp = malloc(sizeof(struct clx_device_info));
            if (cur_dev) {
                cur_dev->next = tmp;
            }
            else {
                root = tmp;
            }
            cur_dev = tmp;

            cur_dev->usage_page =
                get_int_property(dev, CFSTR(kIOHIDPrimaryUsagePageKey));
            cur_dev->usage =
                get_int_property(dev, CFSTR(kIOHIDPrimaryUsageKey));

            cur_dev->next = NULL;
            len = make_path(dev, cbuf, sizeof(cbuf));
            cur_dev->path = strdup(cbuf);

            get_serial_number(dev, buf, BUF_LEN);
            cur_dev->serial_number = dup_wcs(buf);

            get_manufacturer_string(dev, buf, BUF_LEN);
            cur_dev->manufacturer_string = dup_wcs(buf);
            get_product_string(dev, buf, BUF_LEN);
            cur_dev->product_string = dup_wcs(buf);

            cur_dev->vendor_id = dev_vid;
            cur_dev->product_id = dev_pid;

            cur_dev->release_number =
                get_int_property(dev, CFSTR(kIOHIDVersionNumberKey));

            cur_dev->interface_number = -1;
        }
    }

    free(device_array);
    CFRelease(device_set);

    return root;
}

void clx_hid_free_enumeration(struct clx_device_info *devs)
{
    struct clx_device_info *d = devs;
    while (d) {
        struct clx_device_info *next = d->next;
        free(d->path);
        free(d->serial_number);
        free(d->manufacturer_string);
        free(d->product_string);
        free(d);
        d = next;
    }
}

clx_hid_device *clx_hid_open(unsigned short vendor_id,
                             unsigned short product_id,
                             const wchar_t *serial_number)
{
    struct clx_device_info *devs, *cur_dev;
    const char *path_to_open = NULL;
    clx_hid_device *handle = NULL;

    devs = clx_hid_enumerate(vendor_id, product_id);
    cur_dev = devs;
    while (cur_dev) {
        if (cur_dev->vendor_id == vendor_id &&
            cur_dev->product_id == product_id) {
            if (serial_number) {
                if (wcscmp(serial_number, cur_dev->serial_number) == 0) {
                    path_to_open = cur_dev->path;
                    break;
                }
            }
            else {
                path_to_open = cur_dev->path;
                break;
            }
        }
        cur_dev = cur_dev->next;
    }

    if (path_to_open) {
        handle = clx_hid_open_path(path_to_open);
    }

    clx_hid_free_enumeration(devs);

    return handle;
}

static void clx_hid_dev_removal_cb(void *ctx, IOReturn result, void *sender)
{
#pragma unused(result, sender)
    if (result != kIOReturnSuccess) {
        // TODO: Handle errors here
    }
    clx_hid_device *d = ctx;
    d->disconnected = 1;
    CFRunLoopStop(d->run_loop);
}

static void hid_report_cb(void *ctx,
                          IOReturn result,
                          void *sender,
                          IOHIDReportType report_type,
                          uint32_t report_id,
                          uint8_t *report,
                          CFIndex report_len)
{
#pragma unused(sender, report_type, report_id)
    if (result != kIOReturnSuccess) {
        // TODO: Handle errors here
    }

    struct input_report *rpt;
    clx_hid_device *dev = ctx;

    rpt = calloc(1, sizeof(struct input_report));
    rpt->data = calloc(1, report_len);
    memcpy(rpt->data, report, report_len);
    rpt->len = report_len;
    rpt->next = NULL;

    /* LOCK THIS SECTION */
    pthread_mutex_lock(&dev->mutex);

    if (dev->input_reports == NULL) {
        dev->input_reports = rpt;
    }
    else {
        struct input_report *cur = dev->input_reports;
        int num_queued = 0;
        while (cur->next != NULL) {
            cur = cur->next;
            num_queued++;
        }
        cur->next = rpt;

        if (num_queued > 30) {
            return_data(dev, NULL, 0);
        }
    }

    /* Signal a waiting thread that there is data */
    pthread_cond_signal(&dev->condition);

    /* UNLOCK */
    pthread_mutex_unlock(&dev->mutex);
}

/* This get called when the read_thread's run loop is signaled by
 * clx_hid_close(),
 * and serves to stop the read_thread's run loop
 */
static void perform_signal_cb(void *ctx)
{
    clx_hid_device *dev = ctx;
    CFRunLoopStop(dev->run_loop);
}

static void *read_thread(void *param)
{
    clx_hid_device *dev = param;
    SInt32 code;

    // Move the device's run loop to this thread
    IOHIDDeviceScheduleWithRunLoop(
        dev->device_handle, CFRunLoopGetCurrent(), dev->run_loop_mode);

    /* Create the RunLoopSource which is used to signal the event toop to stop
     * when clx_hid_close() is called. */
    CFRunLoopSourceContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.version = 0;
    ctx.info = dev;
    ctx.perform = &perform_signal_cb;
    dev->source = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &ctx);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), dev->source, dev->run_loop_mode);

    // Store the RunLoop so it can be stopped from clx_hid_close() and on device
    // disconnection
    dev->run_loop = CFRunLoopGetCurrent();

    pthread_barrier_wait(&dev->barrier);
    while (!dev->shutdown_thread && !dev->disconnected) {
        code = CFRunLoopRunInMode(dev->run_loop_mode, 1000, FALSE);
        if (code == kCFRunLoopRunFinished) {
            dev->disconnected = 1;
            break;
        }

        // Break if the run loop returns finished or stopped
        if (code != kCFRunLoopRunTimedOut &&
            code != kCFRunLoopRunHandledSource) {
            /* There was an error. Shutting down */
            dev->shutdown_thread = 1;
            break;
        }
    }

    /* Now that the read thread is stopping, Wake any threads which are
       waiting on data (in hid_read_timeout()). Do this under a mutex to
       make sure that a thread which is about to go to sleep waiting on
       the condition acutally will go to sleep before the condition is
       signaled. */
    pthread_mutex_lock(&dev->mutex);
    pthread_cond_broadcast(&dev->condition);
    pthread_mutex_unlock(&dev->mutex);

    pthread_barrier_wait(&dev->shutdown_barrier);

    return NULL;
}

clx_hid_device *clx_hid_open_path(const char *path)
{
    int i;
    clx_hid_device *dev = NULL;
    CFIndex num_devs;

    dev = new_hid_device();

    if (clx_hid_init() < 0) {
        return NULL;
    }

    process_pending_events();  // Give the HIDManager a chance to update

    CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);

    num_devs = CFSetGetCount(device_set);
    IOHIDDeviceRef *dev_array = calloc(num_devs, sizeof(IOHIDDeviceRef));
    CFSetGetValues(device_set, (const void **)dev_array);

    for (i = 0; i < num_devs; i++) {
        char cbuf[BUF_LEN];
        size_t len;
        IOHIDDeviceRef os_dev = dev_array[i];

        len = make_path(os_dev, cbuf, sizeof(cbuf));
        if (!strcmp(cbuf, path)) {
            IOReturn ret = IOHIDDeviceOpen(os_dev, kIOHIDOptionsTypeNone);
            if (ret == kIOReturnSuccess) {
                char str[32];

                free(dev_array);
                CFRetain(os_dev);
                CFRelease(device_set);
                dev->device_handle = os_dev;

                dev->max_input_report_len =
                    (CFIndex)get_max_report_length(os_dev);
                dev->input_report_buf =
                    calloc(dev->max_input_report_len, sizeof(uint8_t));

                sprintf(str, "CLX_HID_%p", (void *)os_dev);
                dev->run_loop_mode = CFStringCreateWithCString(
                    NULL, str, kCFStringEncodingASCII);
                IOHIDDeviceRegisterInputReportCallback(
                    os_dev,
                    dev->input_report_buf,
                    dev->max_input_report_len,
                    &hid_report_cb,
                    dev);
                IOHIDDeviceRegisterRemovalCallback(
                    dev->device_handle, clx_hid_dev_removal_cb, dev);

                pthread_create(&dev->thread, NULL, read_thread, dev);
                pthread_barrier_wait(&dev->barrier);

                return dev;
            }
            else {
                goto return_error;
            }
        }
    }

return_error:
    free(dev_array);
    CFRelease(device_set);
    free_clx_hid_device(dev);
    return NULL;
}

static int set_report(clx_hid_device *dev,
                      IOHIDReportType report_type,
                      const_clx_buf_t data,
                      size_t len)
{
    const_clx_buf_t data_to_send;
    size_t len_to_send;
    IOReturn res;

    if (dev->disconnected) {
        return -1;
    }

    if (data[0] == 0x0) {
        /* NOT using numbered reports. Don't send the report number */
        data_to_send = data + 1;
        len_to_send = len + 1;
    }
    else {
        data_to_send = data;
        len_to_send = len;
    }

    if (!dev->disconnected) {
        res = IOHIDDeviceSetReport(dev->device_handle,
                                   report_type,
                                   data[0],
                                   data_to_send,
                                   len_to_send);

        if (res == kIOReturnSuccess) {
            return len;
        }
        else {
            return -1;
        }
    }

    return -1;
}

int clx_hid_write(clx_hid_device *dev, const_clx_buf_t data, size_t len)
{
    return set_report(dev, kIOHIDReportTypeOutput, data, len);
}

/* Helper function, so it's not duplicated in clx_hid_read() */
static int return_data(clx_hid_device *dev, clx_buf_t data, size_t length)
{
    /* Copy the data out ot the linked list item (rpt) into the return buffer
     * (data), and delete the linked list item */
    struct input_report *rpt = dev->input_reports;
    size_t len = (length < rpt->len) ? length : rpt->len;
    memcpy(data, rpt->data, len);
    dev->input_reports = rpt->next;
    free(rpt->data);
    free(rpt);
    return len;
}

static int cond_wait(const clx_hid_device *dev,
                     pthread_cond_t *cond,
                     pthread_mutex_t *mutex)
{
    while (!dev->input_reports) {
        int res = pthread_cond_wait(cond, mutex);
        if (res != 0) {
            return res;
        }

        if (dev->shutdown_thread || dev->disconnected) {
            return -1;
        }
    }

    return 0;
}

static int cond_timedwait(const clx_hid_device *dev,
                          pthread_cond_t *cond,
                          pthread_mutex_t *mutex,
                          const struct timespec *abstime)
{
    while (!dev->input_reports) {
        int res = pthread_cond_timedwait(cond, mutex, abstime);
        if (res != 0) {
            return res;
        }

        if (dev->shutdown_thread || dev->disconnected) {
            return -1;
        }
    }

    return 0;
}

int clx_hid_read_timeout(clx_hid_device *dev,
                         clx_buf_t data,
                         size_t len,
                         int ms_wait)
{
    int bytes_read = -1;

    /* LOCK THIS SECTION */
    pthread_mutex_lock(&dev->mutex);

    if (dev->input_reports) {
        bytes_read = return_data(dev, data, len);
        goto ret;
    }

    if (dev->disconnected) {
        bytes_read = -1;
        goto ret;
    }

    if (dev->shutdown_thread) {
        bytes_read = -1;
        goto ret;
    }

    if (ms_wait == -1) {
        /* Blocking */
        int res;
        res = cond_wait(dev, &dev->condition, &dev->mutex);
        if (res == 0) {
            bytes_read = return_data(dev, data, len);
        }
        else {
            bytes_read = -1;
        }
    }
    else if (ms_wait > 0) {
        /* Nonblocking, but called with timeout */
        int res;
        struct timespec ts;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        TIMEVAL_TO_TIMESPEC(&tv, &ts);
        ts.tv_sec += ms_wait / 1000;
        ts.tv_nsec += (ms_wait % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000L) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000L;
        }

        res = cond_timedwait(dev, &dev->condition, &dev->mutex, &ts);
        if (res == 0) {
            bytes_read = return_data(dev, data, len);
        }
        else if (res == ETIMEDOUT) {
            bytes_read = 0;
        }
        else {
            bytes_read = -1;
        }
    }
    else {
        /* Purely non-blocking */
        bytes_read = 0;
    }

ret:
    /* UNLOCK */
    pthread_mutex_unlock(&dev->mutex);
    return bytes_read;
}

int clx_hid_read(clx_hid_device *dev, clx_buf_t data, size_t len)
{
    return clx_hid_read_timeout(dev, data, len, (dev->blocking) ? -1 : 0);
}

int clx_hid_set_nonblocking(clx_hid_device *dev, int nonblock)
{
    dev->blocking = !nonblock;
    return 0;
}

int clx_hid_send_feature_report(clx_hid_device *dev,
                                const_clx_buf_t data,
                                size_t len)
{
    return set_report(dev, kIOHIDReportTypeFeature, data, len);
}

int clx_hid_get_feature_report(clx_hid_device *dev, clx_buf_t data, size_t len)
{
    CFIndex length = len;
    IOReturn res;

    if (dev->disconnected) {
        return -1;
    }

    res = IOHIDDeviceGetReport(
        dev->device_handle, kIOHIDReportTypeFeature, data[0], data, &length);

    if (res == kIOReturnSuccess) {
        return len;
    }
    else {
        return -1;
    }
}

void clx_hid_close(clx_hid_device *dev)
{
    if (!dev) {
        return;
    }

    if (!dev->disconnected) {
        IOHIDDeviceRegisterInputReportCallback(dev->device_handle,
                                               dev->input_report_buf,
                                               dev->max_input_report_len,
                                               NULL,
                                               dev);

        IOHIDDeviceRegisterRemovalCallback(dev->device_handle, NULL, dev);

        IOHIDDeviceUnscheduleFromRunLoop(
            dev->device_handle, dev->run_loop, dev->run_loop_mode);

        IOHIDDeviceScheduleWithRunLoop(
            dev->device_handle, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
    }

    dev->shutdown_thread = 1; /* cause read_thread() to stop */

    CFRunLoopSourceSignal(dev->source);
    CFRunLoopWakeUp(dev->run_loop);

    pthread_barrier_wait(&dev->shutdown_barrier);
    pthread_join(dev->thread, NULL);

    if (!dev->disconnected) {
        IOHIDDeviceClose(dev->device_handle, kIOHIDOptionsTypeNone);
    }

    pthread_mutex_lock(&dev->mutex);
    while (dev->input_reports) {
        return_data(dev, NULL, 0);
    }
    pthread_mutex_unlock(&dev->mutex);
    CFRelease(dev->device_handle);

    free_clx_hid_device(dev);
}

int clx_hid_get_manufacturer_string(clx_hid_device *dev,
                                    wchar_t *str,
                                    size_t maxlen)
{
    return get_manufacturer_string(dev->device_handle, str, maxlen);
}

int clx_hid_get_product_string(clx_hid_device *dev, wchar_t *str, size_t maxlen)
{
    return get_product_string(dev->device_handle, str, maxlen);
}

int clx_hid_get_serial_number_string(clx_hid_device *dev,
                                     wchar_t *str,
                                     size_t maxlen)
{
    return get_serial_number(dev->device_handle, str, maxlen);
}

int clx_hid_get_indexed_string(clx_hid_device *dev, wchar_t *str, size_t maxlen)
{
#pragma unused(dev, str, maxlen)
    /* TODO: */
    return 0;
}

const wchar_t *clx_hid_error(clx_hid_device *dev)
{
#pragma unused(dev)
    /* TODO: */
    return NULL;
}
