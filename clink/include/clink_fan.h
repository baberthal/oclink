//
//  clink_fan.h
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#ifndef clink_fan_h
#define clink_fan_h

#include "corsair_link.h"

typedef struct ocl_fan_info {
    char *name;
    int rpm;
    int mode;
} CorsairFanInfo;

typedef struct ocl_fan {
    CorsairFanInfo fan_info[32];
    int fan_count;
    OCL_Link *link;
} OCL_Fan;

struct ocl_fan_table {
    int one;
    int two;
    int three;
    int four;
    int five;
};

typedef struct ocl_fan_table ocl_fan_temp_table;
typedef struct ocl_fan_table ocl_fan_rpm_table;

OCL_Fan *ocl_fan_alloc(OCL_Link *link);

int ocl_fan_connected_fans(OCL_Fan *fan);
void ocl_fan_read_fans_info(OCL_Fan *fan, int idx, CorsairFanInfo *fan_info);
int ocl_fan_set_fans_info(OCL_Fan *fan, int idx, CorsairFanInfo fan_info);
void ocl_fan_print_info(CorsairFanInfo fan);

int ocl_fan_select_fan(OCL_Fan *fan, int idx);
int ocl_fan_current_fan(OCL_Fan *fan);
int ocl_fan_get_fan_count(OCL_Fan *fan);

int ocl_fan_get_fan_mode(OCL_Fan *fan);
int ocl_fan_set_fan_mode(OCL_Fan *fan, int mode);

int ocl_fan_get_fan_rpm(OCL_Fan *fan);
int ocl_fan_set_fan_rpm(OCL_Fan *fan, int rpm);

int ocl_fan_get_fan_pwm(OCL_Fan *fan);
int ocl_fan_set_fan_pwm(OCL_Fan *fan, int pwm);

int ocl_fan_get_current_rpm(OCL_Fan *fan);
int ocl_fan_get_max_rpm(OCL_Fan *fan);

int ocl_fan_get_fan_underspeed_threshold(OCL_Fan *fan);
int ocl_fan_set_fan_underspeed_threshold(OCL_Fan *fan, int thresh);

ocl_fan_rpm_table *ocl_fan_get_rpm_table(OCL_Fan *fan);
int ocl_fan_set_rpm_table(OCL_Fan *fan, ocl_fan_rpm_table table);

ocl_fan_temp_table *ocl_fan_get_temp_table(OCL_Fan *fan);
int ocl_fan_set_temp_table(OCL_Fan *fan, ocl_fan_temp_table table);

void ocl_fan_free(OCL_Fan *fan);

#endif /* clink_fan_h */
