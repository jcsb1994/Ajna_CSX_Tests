/* ========================================
 *
 * Copyright CORO, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF CORO.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <stdbool.h>
#include "debounce.h"

//#include "28BYJ48.h"
#include "hall_effect.h"
#include "stepper.h"

#define IMO_FREQ (48000000)

#define ROW_TAXELS_NB (3)
#define COL_TAXELS_NB (5)
#define RX_NB (3)
#define TX_NB (5)
#define TAXELS_NB (ROW_TAXELS_NB * COL_TAXELS_NB)
#define BYTES_PER_TAXEL (2)
//#define TOTAL_PACKET_SIZE (uint8_t) (TAXELS_NB*BYTES_PER_TAXEL) 

#define MOTOR_UP (true)
#define MOTOR_DOWN (false)

#define TACT_BUTTON_DEBOUNCE_MAX (10)

#define PRINT_CSV_CONFIG 1
#define PRINT_COUNTER_CONFIG 0
#define STEPPER_CONFIG (0)



bool print_flag = false;
bool delay_to_next_print_flag = false;

uint8_t switch_1_iterator;
uint8_t switch_2_iterator;

CY_ISR(print_handler)
{
    delay_to_next_print_flag = false;
    printTimer_ClearInterrupt(printTimer_INTR_MASK_TC);
}

#if STEPPER_CONFIG
bool step_flag  = false;
uint8_t step_semaphore = 0;
bool motor_dir = MOTOR_DOWN;
    
CY_ISR(step_handler)
{
    step_flag = true;
    
    if (motor_dir == MOTOR_DOWN)
    {
        step_semaphore ++;
        if (step_semaphore > 7)
            step_semaphore  = 0;
    }
    else if (motor_dir == MOTOR_UP)
    {
        step_semaphore--;  
        if (step_semaphore > 7)
            step_semaphore  = 7;
    }
    
    stepTimer_ClearInterrupt(stepTimer_INTR_MASK_TC);
}
#endif // STEPPER_CONFIG

int main(void)
{
    CyGlobalIntEnable; 

    print_clk_Start();
    printTimer_Start();
    timerInt_StartEx(print_handler);
    UART_Start();

#if STEPPER_CONFIG
    step_clk_Start();
    stepTimer_Start();
    stepInt_StartEx(step_handler);
#endif // STEPPER_CONFIG

    EZI2C_Start();
    EZI2C_EzI2CSetBuffer1(
                          sizeof(CapSense_dsRam),
                          sizeof(CapSense_dsRam),
                          (uint8 *) &CapSense_dsRam 
                         );
    
    CapSense_Start();
    CapSense_ScanAllWidgets();
    
    cystatus status;
    //unsigned int taxel_raw_values[TAXELS_NB];
    //uint8_t capsense_raw_value_bytes[TOTAL_PACKET_SIZE];   
    
#if PRINT_CSV_CONFIG
 /*   char col_titles[30];
    sprintf(col_titles, "press_state; ");
    UART_UartPutString(col_titles);
    for(int i = 1; i <= TAXELS_NB; i++)
    {
        sprintf(col_titles, "element_%d; ",(i-1));
        UART_UartPutString(col_titles);
    }
    sprintf(col_titles, "sense_clock_val; modulator_clock_freq; ");
    UART_UartPutString(col_titles);
    sprintf(col_titles, "hw_clock_freq; ");
    UART_UartPutString(col_titles);
    sprintf(col_titles, "sub_conversions; ");
    UART_UartPutString(col_titles);  
    sprintf(col_titles, "idac_gain; ");
    UART_UartPutString(col_titles);  */
    
    UART_UartPutString("\r\n");           
#endif // PRINT_CSV_CONFIG    

    for(;;)
    {
        
#if STEPPER_CONFIG
    if(step_flag == true)
    {
        if((motor_dir == MOTOR_DOWN) && debounce_bottom_hall() == 0)
        {
            motor_dir = MOTOR_UP;
            CyDelay(10);
        }
        if((motor_dir == MOTOR_UP) && debounce_top_hall() == 0)
        {
            motor_dir = MOTOR_DOWN;        
            CyDelay(10);
        }
        sendStep(step_semaphore);
        step_flag = false;
    }
    
#endif // STEPPER_CONFIG
        
        
        if(CapSense_IsBusy() == CapSense_NOT_BUSY) // Initiate a new scan only if the CapSense block is idle 
        {
            CapSense_ProcessAllWidgets();
            CapSense_RunTuner();
            CapSense_ScanAllWidgets();
           
            if(delay_to_next_print_flag == false)
            {
                if(debounce(switchPin_1_Read, switch_1_iterator, TACT_BUTTON_DEBOUNCE_MAX) == 0) // switchPin_1_Read() == 0)
                {
                    char param_msg[10];

                    sprintf(param_msg, "Pressed; ");
                    UART_UartPutString(param_msg);
                    print_flag = true;
                    
                }
                
                if(debounce(switchPin_2_Read, switch_2_iterator, TACT_BUTTON_DEBOUNCE_MAX) == 0 && print_flag == false)
                {
                    char param_msg[10];

                    sprintf(param_msg, "Not Pressed; ");
                    UART_UartPutString(param_msg);
                    print_flag = true;
                    
                }
                    
                if(print_flag)
                {
                    char param_msg[10];

                    for(int i = 1; i <= TAXELS_NB; i++)
                    {
                        sprintf(param_msg, "%d; ", CapSense_dsRam.snsList.touchpad0[i-1].raw[0]);
                        UART_UartPutString(param_msg);
                    }
                    
                    int mod_clk_freq = IMO_FREQ / CapSense_MOD_CSX_CLK_VALUE;
                    //CapSense_SetParam(CapSense_MOD_CSX_CLK_PARAM_ID, 1);
                    sprintf(param_msg, "%d; ", mod_clk_freq);
                    UART_UartPutString(param_msg);
                    
                    sprintf(param_msg, "%d; ", (mod_clk_freq / CapSense_TOUCHPAD0_SNS_CLK_VALUE));       // Tx Clock (CSX equivalent of CSD Sense clock)
                    UART_UartPutString(param_msg);
                    
                    sprintf(param_msg, "%d; ", CapSense_HW_CLOCK_VALUE);
                    UART_UartPutString(param_msg);
                    
                    
                    sprintf(param_msg, "%d; ", CapSense_TOUCHPAD0_RESOLUTION_VALUE);    // sub-conversions
                    UART_UartPutString(param_msg);
                    
                    /*
                    sprintf(param_msg, "%d; ", CapSense_TOUCHPAD0_FINGER_TH_VALUE);
                    UART_UartPutString(param_msg);
                    
                    sprintf(param_msg, "%d; ", CapSense_TOUCHPAD0_NOISE_TH_VALUE);
                    UART_UartPutString(param_msg);
                    
                    sprintf(param_msg, "%d; ", CapSense_TOUCHPAD0_HYSTERESIS_VALUE);
                    UART_UartPutString(param_msg);
*/
                    
                    int idac_gain = 37;
                    
                    for (int i = 0; i <= CapSense_TOUCHPAD0_IDAC_GAIN_INDEX_VALUE; i++)
                    {
                        idac_gain *= 2;
                    }
                    
                    sprintf(param_msg, "%d; ", idac_gain);
                    UART_UartPutString(param_msg);
                    
                    UART_UartPutString("\r\n");
                    print_flag = false;
                    delay_to_next_print_flag = true;
                    printTimer_WriteCounter(500);
                }               
            }
        }
    }
}

/* [] END OF FILE */
