/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef HALL_EFFECT_H
#define HALL_EFFECT_H

#include "project.h"

#define MAXIMUM (10)
extern int top_integrator;
extern int bottom_integrator;

uint8_t debounce_top_hall();

uint8_t debounce_bottom_hall();

/* [] END OF FILE */
#endif  // HALL_EFFECT_H