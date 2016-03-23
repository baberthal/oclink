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

#define jml_check(assertion, message, ...)     \
    do {                                       \
        if (!(assertion)) {                    \
            log_error(message, ##__VA_ARGS__); \
        }                                      \
    } while (0)

#ifndef check
#define check(assertion, message, ...) jml_check(assertion, message, ...)
#endif /* check */

#define sentinel(jmp, MESSAGE, ...)        \
    {                                      \
        log_error(MESSAGE, ##__VA_ARGS__); \
        errno = 0;                         \
        goto jmp;                          \
    }

#define check_mem(A) jml_check((A), "No memory.")

#define check_debug(A, M, ...)      \
    if (!(A)) {                     \
        debug_log(M, ##__VA_ARGS__) \
    }

#endif /* jml_debug_h */
