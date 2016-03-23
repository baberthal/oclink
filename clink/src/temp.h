//
//  clink_temp.h
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#ifndef clink_temp_h
#define clink_temp_h

/** @file clink_temp.h
 *  @defgroup Temperature clink Temperature
 *  @{
 */

#include "link.h"

/**
 *  @brief A structure to hold references to the \ref OCL_Link and selected
 *   sensor
 */
typedef struct ocl_temp {
    OCL_Link *link;       ///< The associated \ref OCL_Link structure
    int selected_sensor;  ///< The currently selected sensor (cached)
} OCL_Temp;

/**
 *  @brief Allocate a new \ref OCL_Temp structure
 *
 *  @param link The \ref OCL_Link to associate the structure with
 *
 *  @return The newly allocated \ref OCL_Temp structure
 */
OCL_Temp *ocl_temp_alloc(OCL_Link *link);

/**
 *  @brief Select a temperature sensor
 *
 *  @param temp   The \ref OCL_Temp structure to select with
 *  @param sensor The index of the sensor to select
 *
 *  @return The index of the newly selected sensor
 */
int ocl_temp_select_sensor(OCL_Temp *temp, int sensor);

/**
 *  @brief Get the currently selected sensor
 *  @note This differs from \see OCL_Temp#selected_sensor in that it actually
 *   queries the \ref OCL_Link structure, rather than returning the last cached
 *   sensor index
 *
 *  @param temp The \ref OCL_Temp structure to query
 *
 *  @return The index of the selected sensor
 */
int ocl_temp_current_sensor(OCL_Temp *temp);

/**
 *  @brief Get the number of temperature sensors attached to the system
 *
 *  @param temp The \ref OCL_Temp structure to query
 *
 *  @return The number of sensors attached to the system
 */
int ocl_temp_get_temp_sensors(OCL_Temp *temp);

/**
 *  @brief Get the current temperature of the currently selected sensor
 *
 *  @param temp The \ref OCL_Temp structure to query
 *
 *  @return The temperature of the selected sensor (in celsius)
 */
int ocl_temp_get_temp(OCL_Temp *temp);

/**
 *  @brief Get the current temp limit of the selected sensor
 *
 *  @param temp The \ref OCL_Temp structure to query
 *
 *  @return The temp limit of the selected sensor
 */
int ocl_temp_get_temp_limit(OCL_Temp *temp);

/**
 *  @brief Set the temp limit of the selected sensor
 *
 *  @param temp  The \ref OCL_Temp structure to query
 *  @param limit The new temperature limit
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_temp_set_temp_limit(OCL_Temp *temp, int limit);

/**
 *  @brief Free an \ref OCL_Temp structure while preserving it's associated link
 *
 *  @param temp The \ref OCL_Temp structure to free
 */
void ocl_temp_free(OCL_Temp *temp);

/** @} */

#endif /* clink_temp_h */
