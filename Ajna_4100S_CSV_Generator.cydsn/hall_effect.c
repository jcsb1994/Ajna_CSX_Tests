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


#include "hall_effect.h"


#define MAXIMUM (10)
int top_integrator = MAXIMUM;
int bottom_integrator = MAXIMUM;

uint8_t debounce_top_hall()
{
  uint8_t input = topHallPin_Read(); 

  if (input == 0)
  {
    if (top_integrator > 0)
      top_integrator--;
  }
  else if (top_integrator < MAXIMUM)
    top_integrator++;

  if (top_integrator == 0)
    return 0;
  else if (top_integrator >= MAXIMUM)
    top_integrator = MAXIMUM; // defensive code if integrator becomes corrupted

  return 1;
}

uint8_t debounce_bottom_hall()
{
  uint8_t input = bottomHallPin_Read(); 

  if (input == 0)
  {
    if (bottom_integrator > 0)
      bottom_integrator--;
  }
  else if (bottom_integrator < MAXIMUM)
    bottom_integrator++;

  if (bottom_integrator == 0)
    return 0;
  else if (bottom_integrator >= MAXIMUM)
    bottom_integrator = MAXIMUM; // defensive code if integrator becomes corrupted

  return 1;
}



/* [] END OF FILE */
