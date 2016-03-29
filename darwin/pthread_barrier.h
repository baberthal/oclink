#ifndef PTHREAD_BARRIER_H
#define PTHREAD_BARRIER_H 1

#include <errno.h>
#include <pthread.h>
#include "clink_darwin_export.h"

typedef int pthread_barrierattr_t;
typedef struct pthread_barrier {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int trip_count;
} pthread_barrier_t;

CLINK_DARWIN_EXPORT int pthread_barrier_init(pthread_barrier_t *barrier,
                                             const pthread_barrierattr_t *attr,
                                             unsigned int count);

CLINK_DARWIN_EXPORT int pthread_barrier_destroy(pthread_barrier_t *barrier);

CLINK_DARWIN_EXPORT int pthread_barrier_wait(pthread_barrier_t *barrier);

#endif /* ifndef PTHREAD_BARRIER_H */

/* vim: set ts=8 sw=4 tw=0 ft=c et :*/
