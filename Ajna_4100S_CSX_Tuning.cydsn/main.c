/*****************************************************************************
* File Name: main.c
*
* Version: 1.00
*
* Description: This example demonstrates how to tune a mutual capacitance (CSX)
*              based button using CapSense Tuner GUI.
*
* Related Document: CE228931_PSoC4_CapSense_CSX_Tuning.pdf
*
* Hardware Dependency: This example supports the following kits:
*                      CY8CKIT-149 PSoC® 4100S Plus Prototyping Kit,  
*                      CY8CKIT-041-41XX PSoC® 4100S CapSense Pioneer Kit, 
*                      CY8CKIT-041-40XX PSoC® 4 S-Series Pioneer Kit,  
*                      CY8CKIT-145-40XX PSoC® 4000S CapSense Prototyping Kit.
******************************************************************************
* Copyright (2020), Cypress Semiconductor Corporation.
******************************************************************************
* This software, including source code, documentation and related materials
* ("Software") is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and 
* foreign), United States copyright laws and international treaty provisions. 
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the 
* Cypress source code and derivative works for the sole purpose of creating 
* custom software in support of licensee product, such licensee product to be
* used only in conjunction with Cypress's integrated circuit as specified in the
* applicable agreement. Any reproduction, modification, translation, compilation,
* or representation of this Software except as specified above is prohibited 
* without the express written permission of Cypress.
* 
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes to the Software without notice. 
* Cypress does not assume any liability arising out of the application or use
* of Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use as critical components in any products 
* where a malfunction or failure may reasonably be expected to result in 
* significant injury or death ("ACTIVE Risk Product"). By including Cypress's 
* product in a ACTIVE Risk Product, the manufacturer of such system or application
* assumes all risk of such use and in doing so indemnifies Cypress against all
* liability. Use of this Software may be limited by and subject to the applicable
* Cypress software license agreement.
*****************************************************************************/

/*******************************************************************************
*   Included Headers
*******************************************************************************/

#include "project.h"
#include <stdio.h>
#include <stdbool.h>

#define ROW_TAXELS_NB (3)
#define COL_TAXELS_NB (5)
#define RX_NB (3)
#define TX_NB (5)
#define TAXELS_NB (ROW_TAXELS_NB * COL_TAXELS_NB)
#define BYTES_PER_TAXEL (2)
#define TOTAL_PACKET_SIZE (uint8_t) (TAXELS_NB*BYTES_PER_TAXEL) 

#define BIST_CP_TEST_CONFIG 0
#define ALL_BIST_CONFIG 0
#define READ_AND_PRINT_TAXELS_CONFIG 0

bool print_counter_flag = false;

CY_ISR(print_handler)
{
    print_counter_flag = true;
    printTimer_ClearInterrupt(printTimer_INTR_MASK_TC);
}

int main(void)
{
    
    CyGlobalIntEnable; //En global ints

    Clock_1_Start();
    printTimer_Start();
    timerInt_StartEx(print_handler);
    
    EZI2C_Start();
    UART_Start();
    
    /* Set the CapSense data structure as the I2C buffer to be exposed to the 
     * master on primary slave address interface. Any I2C host tools such as 
     * the Tuner or the Bridge Control Panel can read this buffer but you can  
     * connect only one tool at a time.
     */
    EZI2C_EzI2CSetBuffer1(
                          sizeof(CapSense_dsRam),
                          sizeof(CapSense_dsRam),
                          (uint8 *) &CapSense_dsRam 
                         );
    
    CapSense_Start();
    CapSense_ScanAllWidgets();
    
    cystatus status;
    unsigned int taxel_raw_values[TAXELS_NB];
    uint8_t capsense_raw_value_bytes[TOTAL_PACKET_SIZE];
   
    for(;;)
    {
        if(CapSense_IsBusy() == CapSense_NOT_BUSY) // Initiate a new scan only if the CapSense block is idle 
        {
            // Performs full data processing of all enabled widgets
            CapSense_ProcessAllWidgets();
            CapSense_RunTuner();
            CapSense_ScanAllWidgets();

            if(print_counter_flag)
            {
                char msg[30];
                
#if BIST_CP_TEST_CONFIG            
                CapSense_TST_MEASUREMENT_STATUS_ENUM state; //can be accessed with another API fct
                                
                sprintf(msg, "BIST Pin Cp (fF)\n\n\r  Rx Pins\r\n\n");
                UART_UartPutString(msg);
                for (int i =0; i < RX_NB; i ++)
                {
                        char msg[30];
                        sprintf(msg, "RX%d: %lu", i, CapSense_GetSensorCapacitance(CapSense_TOUCHPAD0_WDGT_ID, i, &state));
                        UART_UartPutString(msg);
                        sprintf(msg, "      status: %lu\r\n", status);
                        UART_UartPutString(msg);
                }
                
                sprintf(msg, "  Tx Pins\r\n\n");
                UART_UartPutString(msg);
                for (int i =0; i < TX_NB; i ++)
                {
                        char msg[30];
                        sprintf(msg, "TX%d: %lu", i, CapSense_GetSensorCapacitance(CapSense_TOUCHPAD0_WDGT_ID, i, &state));
                        UART_UartPutString(msg);
                        sprintf(msg, "      status: %lu\r\n", status);
                        UART_UartPutString(msg);
                }
                //CapSense_MeasureAllSensorCapacitance();          
#endif  //BIST_CP_TEST_CONFIG  

#if ALL_BIST_CONFIG

                while (CapSense_IsBusy() != CapSense_NOT_BUSY);

                uint32 testsResults = CapSense_RunSelfTest(CapSense_TST_RUN_SELF_TEST_MASK);
                if (testsResults & CapSense_TST_GLOBAL_CRC)
                {
                    sprintf(msg, "GLOBAL CRC FAILED\r\n");
                    UART_UartPutString(msg);
                }
                if (testsResults & CapSense_TST_WDGT_CRC)
                {
                    sprintf(msg, "WIDGET CRC FAILED\r\n");
                    UART_UartPutString(msg);
                }
                if (testsResults & CapSense_TST_BSLN_DUPLICATION)
                {
                    sprintf(msg, "BASELINE CONSISTENSY FAILED\r\n");
                    UART_UartPutString(msg);
                }
                if (testsResults & CapSense_TST_SNS_SHORT)
                {
                    sprintf(msg, "SHORT DETECTED\r\n");
                    UART_UartPutString(msg);
                }
                if (testsResults & CapSense_TST_SNS_CAP)
                {
                    sprintf(msg, "SENSOR CAP FAILED\r\n");
                    UART_UartPutString(msg);
                }
                if (testsResults & CapSense_TST_VDDA)
                {
                    sprintf(msg, "VDDA VOLTAGE FAILED\r\n");
                    UART_UartPutString(msg);
                }
                if (testsResults == 0)
                {
                    sprintf(msg, "BIST SUCCEEDED!\r\n");
                    UART_UartPutString(msg);
                }
#endif // ALL_BIST_CONFIG

#if READ_AND_PRINT_TAXELS_CONFIG
                for(int i = 1; i <= TAXELS_NB; i++)
                {
                    char msg[10];
                    sprintf(msg, "%d    ", CapSense_dsRam.snsList.touchpad0[i-1].raw[0]);
                    UART_UartPutString(msg);
                    if (!(i % COL_TAXELS_NB))
                    {
                        UART_UartPutString("\r\n");   
                    }
                    //taxel_raw_values[i] = CapSense_dsRam.snsList.touchpad0[i].raw[0];
                    //u16tobytes(&capsense_raw_value_bytes[i*2], taxel_raw_values[i]);
                }
#endif // READ_AND_PRINT_TAXELS_CONFIG

                UART_UartPutString("\r\n\n");
                print_counter_flag = false;
            }
        } 
    }
}

/* [] END OF FILE */
