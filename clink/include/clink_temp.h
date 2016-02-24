//
//  clink_temp.h
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#ifndef clink_temp_h
#define clink_temp_h

#include "corsair_link.h"

typedef struct ocl_temp {
    OCL_Link *link;
    int selected_sensor;
} OCL_Temp;

OCL_Temp *ocl_temp_alloc(OCL_Link *link);

int ocl_temp_select_sensor(OCL_Temp *temp, int sensor);
int ocl_temp_current_sensor(OCL_Temp *temp);

int ocl_temp_get_temp_sensors(OCL_Temp *temp);
int ocl_temp_get_temp(OCL_Temp *temp);

int ocl_temp_get_temp_limit(OCL_Temp *temp);
int ocl_temp_set_temp_limit(OCL_Temp *temp, int limit);

void ocl_temp_free(OCL_Temp *temp);

#endif /* clink_temp_h */
