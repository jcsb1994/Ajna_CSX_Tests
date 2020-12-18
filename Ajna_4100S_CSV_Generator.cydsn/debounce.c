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

#include "debounce.h"

uint8_t debounce(uint8_t(*func_to_debounce)(), int integrator, uint8_t threshold)
{
    uint8_t input = func_to_debounce(); 

    if (input == 0)
    {
        if (integrator > 0)
            integrator--;
    }
    else if (integrator < threshold)
        integrator++;

    if (integrator == 0)
        return 0;
    else if (integrator >= threshold)
        integrator = threshold; // defensive code if integrator becomes corrupted

return 1;
}

/* [] END OF FILE */
