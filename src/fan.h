#ifndef clink_fan_h
#define clink_fan_h

#include "link.h"

/** \file clink_fan.h
 *  @defgroup Fan clink Fan
 *
 *  These structures and functions contain routines to get and modify
 *  information regarding fans connected to the CorsairLink system.
 * @{
 */

/**
 *  \brief Holds information relative to a specific fan in the system.
 */
typedef struct ocl_fan_info {
    char *name;  ///< The name of the fan
    int rpm;     ///< The fan's rpm
    int mode;    ///< The fan's mode
} CorsairFanInfo;

/**
 *  \brief The main fan structure.
 */
typedef struct ocl_fan {
    /// A list (or array, if you will) of `CorsairFanInfo` structures attached
    /// to the system
    CorsairFanInfo fan_info[32];
    int fan_count;   ///< The number of fans attached to the system
    OCL_Link *link;  ///< The associated link structure
} OCL_Fan;

/**
 *  \brief A table to hold various rpms and temperatures for the fans.
 */
struct ocl_fan_table {
    int one;    ///< Fan 1
    int two;    ///< Fan 2
    int three;  ///< Fan 3
    int four;   ///< Fan 4
    int five;   ///< Fan 5
};

/**
 *  \brief alias \ref ocl_fan_table as ocl_fan_temp_table
 */
typedef struct ocl_fan_table ocl_fan_temp_table;
/**
 *  \brief alias \ref ocl_fan_table as ocl_fan_rpm_table
 */
typedef struct ocl_fan_table ocl_fan_rpm_table;

/**
 *  Allocate a new fan structure. Must provide the link structure.
 *
 *  \param link The \ref OCL_Link to associate the fan with
 *
 *  \return The newly allocated fan structure
 */
CLINK_EXPORT OCL_Fan *ocl_fan_alloc(OCL_Link *link);

/**
 *  Get the number of connected fans
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return The number of connected fans
 */
CLINK_EXPORT int ocl_fan_connected_fans(OCL_Fan *fan);

/**
 *  Read the information of the fan at index idx into the pointer `fan_info`
 *
 *  \param fan      The associated \ref OCL_Fan structure
 *  \param idx      The index of the fan for which to get info
 *  \param fan_info A pointer to the fan info structure to put the values into
 */
CLINK_EXPORT void ocl_fan_read_fans_info(OCL_Fan *fan, int idx,
                                         CorsairFanInfo *fan_info);

/**
 *  Set the information for fan at index idx using the values in the structure
 * `fan_info`
 *
 *  \param fan      The associated \ref OCL_Fan structure
 *  \param idx      The index of the fan for which to set info
 *  \param fan_info The structure that holds the new values to set
 *
 *  \return 0 if successful, -1 otherwise
 */
CLINK_EXPORT int ocl_fan_set_fans_info(OCL_Fan *fan, int idx,
                                       CorsairFanInfo fan_info);

/**
 *  Dump the values of `fan` to stdout
 *
 *  \param fan The fan to dump values for
 */
CLINK_EXPORT void ocl_fan_print_info(CorsairFanInfo fan);

/**
 *  Select a connected fan to perform further operations on
 *
 *  \param fan The associated \ref OCL_Fan structure
 *  \param idx The index of the connected fan to select
 *
 *  \return The index of the newly selected fan
 */
CLINK_EXPORT int ocl_fan_select_fan(OCL_Fan *fan, int idx);

/**
 *  Get the currently selected fan
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return The index of the currently selected fan
 */
CLINK_EXPORT int ocl_fan_current_fan(OCL_Fan *fan);

/**
 *  Get the number of fans attached to the system
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return The number of connected fans
 */
CLINK_EXPORT int ocl_fan_get_fan_count(OCL_Fan *fan);

/**
 *  Get the current fan mode, expressed as an integer
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return The current mode
 */
CLINK_EXPORT int ocl_fan_get_fan_mode(OCL_Fan *fan);

/**
 *  Set the fan mode
 *
 *  \param fan  The associated \ref OCL_Fan structure
 *  \param mode The new mode to set
 *
 *  \return 0 if successful, -1 otherwise
 */
CLINK_EXPORT int ocl_fan_set_fan_mode(OCL_Fan *fan, int mode);

/**
 *  Get the rpm of the fan
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return the fan rpm
 */
CLINK_EXPORT int ocl_fan_get_fan_rpm(OCL_Fan *fan);

/**
 *  Set the rpm of the fan
 *
 *  \param fan The associated \ref OCL_Fan structure
 *  \param rpm the new rpm
 *
 *  \return 0 if successful, -1 otherwise
 */
CLINK_EXPORT int ocl_fan_set_fan_rpm(OCL_Fan *fan, int rpm);

/**
 *  Get the pwm of the fan
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return the fan pwm
 */
CLINK_EXPORT int ocl_fan_get_fan_pwm(OCL_Fan *fan);

/**
 *  Set the rpm of the fan
 *
 *  \param fan The associated \ref OCL_Fan structure
 *  \param pwm the new pwm
 *
 *  \return 0 if successful, -1 otherwise
 */
CLINK_EXPORT int ocl_fan_set_fan_pwm(OCL_Fan *fan, int pwm);

/**
 *  Get the current rpm of fan
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return the current rpm
 */
CLINK_EXPORT int ocl_fan_get_current_rpm(OCL_Fan *fan);

/**
 *  Get the max rpm of the fan
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return the max rpm
 */
CLINK_EXPORT int ocl_fan_get_max_rpm(OCL_Fan *fan);

/**
 *  Get the fan underspeed threshold
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return the fan underspeed threshold
 */
CLINK_EXPORT int ocl_fan_get_fan_underspeed_threshold(OCL_Fan *fan);

/**
 *  Set the fan underspeed threshold
 *
 *  \param fan    The associated \ref OCL_Fan structure
 *  \param thresh the new threshold
 *
 *  \return 0 if successful, -1 otherwise
 */
CLINK_EXPORT int ocl_fan_set_fan_underspeed_threshold(OCL_Fan *fan, int thresh);

/**
 *  Get the rpm table for fan
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return the rpm table
 */
CLINK_EXPORT ocl_fan_rpm_table *ocl_fan_get_rpm_table(OCL_Fan *fan);

/**
 *  Set the rpm table for fan
 *
 *  \param fan   The associated \ref OCL_Fan structure
 *  \param table The new rpm table
 *
 *  \return 0 if successful, -1 otherwise
 */
CLINK_EXPORT int ocl_fan_set_rpm_table(OCL_Fan *fan, ocl_fan_rpm_table table);

/**
 *  Get the temp table for fan
 *
 *  \param fan The associated \ref OCL_Fan structure
 *
 *  \return the temp table
 */
CLINK_EXPORT ocl_fan_temp_table *ocl_fan_get_temp_table(OCL_Fan *fan);

/**
 *  Set the temp table for fan
 *
 *  \param fan   The associated \ref OCL_Fan structure
 *  \param table The new temp table
 *
 *  \return 0 if successful, -1 otherwise
 */
CLINK_EXPORT int ocl_fan_set_temp_table(OCL_Fan *fan, ocl_fan_temp_table table);

/**
 *  Free a fan
 *
 *  \param fan the fan to free from memory
 */
CLINK_EXPORT void ocl_fan_free(OCL_Fan *fan);

/**@}*/

#endif /* clink_fan_h */
