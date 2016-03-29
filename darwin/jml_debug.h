//
//  jml_debug.h
//  rb_clang
//
//  Created by Morgan Lieberthal on 2/14/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#ifndef jml_debug_h
#define jml_debug_h

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef __FILENAME__
#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif /* __FILENAME__ */

#define MESSAGE_FORMAT "(%s:%d:%s)"
#define ERROR_MESSAGE_FORMAT "(%s:%d:%s) (errno: %s)"

#if DEBUG

#define debug_log(M, ...)                                             \
    fprintf(stderr, "[DEBUG] - " MESSAGE_FORMAT M "\n", __FILENAME__, \
            __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)

#else

#define debug_log(M, ...)

#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_error(M, ...)                                                   \
    fprintf(stderr, "[ERROR] - " ERROR_MESSAGE_FORMAT M "\n", __FILENAME__, \
            __LINE__, __PRETTY_FUNCTION__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...)                                                   \
    fprintf(stderr, "[WARN] - " ERROR_MESSAGE_FORMAT M "\n", __FILENAME__, \
            __LINE__, __PRETTY_FUNCTION__, clean_errno(), ##__VA_ARGS)

#define log_info(M, ...)                                                       \
    fprintf(stderr, "[INFO] - " MESSAGE_FORMAT M "\n", __FILENAME__, __LINE__, \
            __PRETTY_FUNCTION__, ##__VA_ARGS__)

#define cl_check(assertion, message, ...)      \
    do {                                       \
        if (!(assertion)) {                    \
            log_error(message, ##__VA_ARGS__); \
        }                                      \
    } while (0)

#define jml_check cl_check

#define cl_check_jmp(assertion, message, ...)  \
    do {                                       \
        if (!(assertion)) {                    \
            log_error(message, ##__VA_ARGS__); \
            errno = 0;                         \
            goto error;                        \
        }                                      \
    } while (0)

#define sentinel(jmp, MESSAGE, ...)        \
    {                                      \
        log_error(MESSAGE, ##__VA_ARGS__); \
        errno = 0;                         \
        goto jmp;                          \
    }

#define check_mem(A) cl_check((A), "No memory.")

#define check_mem_jmp(A) cl_check_jmp((A), "No memory.")

#define check_debug(A, M, ...)      \
    if (!(A)) {                     \
        debug_log(M, ##__VA_ARGS__) \
    }

#define check_debug_jmp(A, M, ...)   \
    if (!(A)) {                      \
        debug_log(M, ##__VA_ARGS__); \
        errno = 0;                   \
        goto error;                  \
    }

#if DEBUG_HID
#define DEBUG_HID_READ(buf)                                                 \
    do {                                                                    \
        fprintf(stdout, "[DEBUG READ] - %s [%s:%d]\n", __PRETTY_FUNCTION__, \
                __FILENAME__, __LINE__);                                    \
        for (size_t _z = 0; _z < sizeof((buf)); _z++) {                     \
            if ((_z + 1) % 16 != 0) {                                       \
                fprintf(stdout, "%02X ", (buf)[_z]);                        \
            }                                                               \
            else {                                                          \
                fprintf(stdout, "%02X\n", (buf)[_z]);                       \
            }                                                               \
        }                                                                   \
        fprintf(stdout, "\n");                                              \
    } while (0)

#define DEBUG_HID_WRITE(buf)                                                 \
    do {                                                                     \
        fprintf(stdout, "[DEBUG WRITE] - %s [%s:%d]\n", __PRETTY_FUNCTION__, \
                __FILENAME__, __LINE__);                                     \
        for (size_t _z = 0; _z < sizeof((buf)); _z++) {                      \
            if ((_z + 1) % 16 != 0) {                                        \
                fprintf(stdout, "%02X ", (buf)[_z]);                         \
            }                                                                \
            else {                                                           \
                fprintf(stdout, "%02X\n", (buf)[_z]);                        \
            }                                                                \
        }                                                                    \
        fprintf(stdout, "\n");                                               \
    } while (0)
#else
#define DEBUG_HID_READ(buf)
#define DEBUG_HID_WRITE(buf)
#endif

#if DEBUG

#define DEBUG_LINK_STRUCT(link)                                             \
    do {                                                                    \
        fprintf(stderr,                                                     \
                "Link <%p>:\n  Handle: <%p>\n  Command ID: %d (%02X)\n  "   \
                "Device Id: %02x\n  Buffer:\n ",                            \
                (void *)(link), (void *)(link)->handle, (link)->command_id, \
                (link)->command_id, (link)->device_id);                     \
        for (size_t __i = 0; __i < sizeof((link)->buf); __i++) {            \
            if (((__i + 1) % 16) == 0) {                                    \
                fprintf(stderr, " %02X\n ", (link)->buf[__i]);              \
            }                                                               \
            else {                                                          \
                fprintf(stderr, " %02X", (link)->buf[__i]);                 \
            }                                                               \
        }                                                                   \
        fprintf(stderr, "\n\n");                                            \
    } while (0)

#else
#define DEBUG_LINK_STRUCT(link)
#endif

#endif /* jml_debug_h */
