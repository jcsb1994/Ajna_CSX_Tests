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

#include "28BYJ48.h"


void sendStep(uint8_t semaphore)
{
    switch(semaphore)
    {
         case 0:
                stepPin_1_Write(1);
                stepPin_2_Write(0);
                stepPin_3_Write(0);
                stepPin_4_Write(0);
                break;
            case 1:
                stepPin_1_Write(1);
                stepPin_2_Write(1);
                stepPin_3_Write(0);
                stepPin_4_Write(0);
                break;
            case 2:
                stepPin_1_Write(0);
                stepPin_2_Write(1);
                stepPin_3_Write(0);
                stepPin_4_Write(0);
                break;
            case 3:
                stepPin_1_Write(0);
                stepPin_2_Write(1);
                stepPin_3_Write(1);
                stepPin_4_Write(0);
                break;
            case 4:
                stepPin_1_Write(0);
                stepPin_2_Write(0);
                stepPin_3_Write(1);
                stepPin_4_Write(0);
                break;
            case 5:
                stepPin_1_Write(0);
                stepPin_2_Write(0);
                stepPin_3_Write(1);
                stepPin_4_Write(1);
                break;
            case 6:
                stepPin_1_Write(0);
                stepPin_2_Write(0);
                stepPin_3_Write(0);
                stepPin_4_Write(1);
                break;
            case 7:
                stepPin_1_Write(1);
                stepPin_2_Write(0);
                stepPin_3_Write(0);
                stepPin_4_Write(1);
                break;
            default:
                stepPin_1_Write(0);
                stepPin_2_Write(0);
                stepPin_3_Write(0);
                stepPin_4_Write(0);
                break;
    }
    
}

/* [] END OF FILE */
