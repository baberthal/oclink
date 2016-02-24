//
//  clink_led.h
//  clink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

/** @file clink_led.h
 *  @defgroup Led clink Led
 *  @{
 */

#ifndef clink_led_h
#define clink_led_h

#include "corsair_link.h"

/**
 *  @brief Represent's an RGB Color
 *  @struct OCLLedColor
 */
struct OCLLedColor {
    int red;   ///< The Color's Red Component
    int green; ///< The Color's Green Component
    int blue;  ///< The Color's Blue Component
};

/**
 *  @brief The Main LED Structure
 */
typedef struct ocl_led {
    /**
     *  @brief A list of colors associated with the \ref OCL_Link
     */
    struct OCLLedColor color[4];
    /**
     *  @brief Describes whether the link's colors were set by options or a
     * function
     */
    union {
        int opts;
        int func;
    } color_set_by;
    OCL_Link *link; ///< The associated \ref OCL_Link structure
} OCL_Led;

/**
 *  @brief Holds temperature settings for temperature modes
 */
struct ocl_temp_mode {
    int temp_1; ///< The First Temperature
    int temp_2; ///< The Second Temperature
    int temp_3; ///< The Third Temperature
};

/**
 *  @brief Allocates a new LED Structure
 *
 *  @param link The associated \ref OCL_Link Structure
 *
 *  @return The newly allocated \ref OCL_Led Structure
 */
OCL_Led *ocl_led_alloc(OCL_Link *link);

/**
 *  @brief Select an led on the system
 *
 *  @param led The associated \ref OCL_Link Structure
 *  @param idx The index of the led to select
 *
 *  @return The index of the newly selected led
 */
int ocl_led_select_led(OCL_Led *led, int idx);

/**
 *  @brief Get the number of LEDs attached to the system
 *
 *  @param led The \ref OCL_Led structure to query
 *
 *  @return The number of LEDs attached to the system
 */
int ocl_led_get_led_count(OCL_Led *led);

/**
 *  @brief Get the current mode of the LEDs
 *
 *  @param led The \ref OCL_Led structure to query
 *
 *  @return The current LED mode, expressed as an integer
 */
int ocl_led_get_mode(OCL_Led *led);

/**
 *  @brief Set the LED mode for the system
 *
 *  @param led  The \ref OCL_Led structure to set
 *  @param mode The new mode
 *
 *  @return The new mode, expressed as an integer
 */
int ocl_led_set_mode(OCL_Led *led, int mode);

/**
 *  @brief Copy the current LED Color into `color`
 *
 *  @param[in]  led   The \ref OCL_Led structure to query
 *  @param[out] color The destination to copy the colors
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_led_get_color(OCL_Led *led, struct OCLLedColor *color);

/**
 *  @brief Same as \ref ocl_led_get_color, but returns the color instead of
 * success / failure
 *
 *  @param led The \ref OCL_Led structure to query
 *
 *  @return The \ref OCLLedColor structure
 */
struct OCLLedColor *ocl_led_get_color2(OCL_Led *led);

/**
 *  @brief Get the led colors associated with the current temperature register
 *
 *  @param[in] led   The \ref OCL_Led structure to query
 *  @param[out] color The color structure to copy value into
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_led_get_temp_color(OCL_Led *led, struct OCLLedColor *color);

/**
 *  @brief Set the led colors associated with the current temperature register
 *
 *  @param led   The \ref OCL_Led structure to set
 *  @param color The new color, enclosed in an \ref OCLLedColor structure
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_led_set_temp_color(OCL_Led *led, struct OCLLedColor *color);

/**
 *  @brief Get the temperature table for the current temp mode
 *
 *  @param led The \ref OCL_Led structure to query
 *
 *  @return The table of temperatures associated with the current temp mode
 */
struct ocl_temp_mode *ocl_led_get_temp_mode_temp(OCL_Led *led);

/**
 *  @brief Set the temperature table for the current mode
 *
 *  @param led  the \ref OCL_Led structure to set
 *  @param mode The new temperature mode structure
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_led_set_temp_mode_temp(OCL_Led *led, struct ocl_temp_mode mode);

/**
 *  @brief Copy the color associated with the current temp mode into `color`
 *
 *  @param[in] led   The \ref OCL_Led structure to query
 *  @param[out] color The color structure to copy values into
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_led_get_temp_mode_color(OCL_Led *led, struct OCLLedColor *color);

/**
 *  @brief Set the color associated with the current temp mode using the values
 * in `color`
 *
 *  @param led   The \ref OCL_Led structure to query
 *  @param color The color structure holding the new values for the color
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_led_set_temp_mode_color(OCL_Led *led, struct OCLLedColor *color);

/**
 *  @brief Get the LED cycle colors of the system
 *
 *  @param[in] led   The \ref OCL_Led structure to query
 *  @param[out] color The color structure to copy values into
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_led_get_cycle_colors(OCL_Led *led, struct OCLLedColor *color);

/**
 *  @brief Set the LED cycle colors using the values in `color`
 *
 *  @param led   The \ref OCL_Led structure to set
 *  @param color The structure containing the new color values
 *
 *  @return 0 if successful, -1 otherwise
 */
int ocl_led_set_cycle_colors(OCL_Led *led, struct OCLLedColor *color);

/**
 *  @brief Free an \ref OCL_Led structure while preserving the associated \ref
 * OCL_Link
 *
 *  @param led The \ref OCL_Led structure to free
 */
void ocl_led_free(OCL_Led *led);

/**@}*/

#endif /* clink_led_h */
