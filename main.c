/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the RutDevKit-OBDII_Application
*              Application for ModusToolbox.
*
* Related Document: See README.md
*
*
*  Created on: 2021-09-01
*  Company: Rutronik Elektronische Bauelemente GmbH
*  Address: Jonavos g. 30, Kaunas 44262, Lithuania
*  Author: GDR
*
*******************************************************************************
* (c) 2019-2021, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*
* Rutronik Elektronische Bauelemente GmbH Disclaimer: The evaluation board
* including the software is for testing purposes only and,
* because it has limited functions and limited resilience, is not suitable
* for permanent use under real conditions. If the evaluation board is
* nevertheless used under real conditions, this is done at one’s responsibility;
* any liability of Rutronik is insofar excluded
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "obd.h"

void handle_error(void);

int main(void)
{
    cy_rslt_t result;
    uint32_t spd = 0;
    uint32_t rev = 0;
    uint32_t vlt = 0;
    int32_t int_air = 0;
    uint32_t pres = 0;
    uint32_t flow = 0;
    _Bool status = false;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    /*Initialize LEDs*/
    result = cyhal_gpio_init( LED1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}
    result = cyhal_gpio_init( LED2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}

    /*Initialize CANFD Driver Stand-By pin */
    result = cyhal_gpio_init( CANFD_STB, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}

    /*Enable debug output via KitProg UART*/
    result = cy_retarget_io_init( KITPROG_TX, KITPROG_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }

    /*Initialize CAN FD and OBD*/
    if(CY_CANFD_SUCCESS != obd_can_init())
    {handle_error();}

    for (;;)
    {
        printf("\x1b[2J\x1b[;H");
        printf("\r\n                 OBD-II DEMO                       \r\n");
        printf("___________________________________________________\r\n\r\n");

    	status = obd_vehicle_speed(&spd);
    	if(status)
    	{
    		printf("   SPEED: %d km/h   ", (unsigned int)spd);
    	}
    	else
    	{
    		printf("   SPEED: N.A. km/h  ");
    	}

    	status = obd_engine_speed(&rev);
    	if(status)
    	{
    		printf("   REVOLUTIONS: %d rpm\r\n", (unsigned int)rev);
    	}
    	else
    	{
    		printf("   REVOLUTIONS: N.A. rpm\r\n");
    	}

    	status = obd_ctrl_mod_voltage(&vlt);
    	if(status)
    	{
    		printf("   VOLTAGE: %d V   ", (unsigned int)vlt);
    	}
    	else
    	{
    		printf("   VOLTAGE: N.A. V  ");
    	}

    	status = obd_in_air_temp(&int_air);
    	if(status)
    	{
    		printf("  AIR TEMPERATURE: %d °C\r\n", (signed int)int_air);
    	}
    	else
    	{
    		printf("  AIR TEMPERATURE: N.A. °C\r\n");
    	}

    	status = obd_in_air_press(&pres);
    	if(status)
    	{
    		printf("   PRESSURE: %d kPa ", (unsigned int)pres);
    	}
    	else
    	{
    		printf("   PRESSURE: N.A. kPa ");
    	}

    	status = obd_air_mass(&flow);
    	if(status)
    	{
    		printf(" MASS AIR FLOW: %d g/sec\r\n", (unsigned int)flow);
    	}
    	else
    	{
    		printf(" MASS AIR FLOW: N.A. g/sec\r\n");
    	}

        printf("___________________________________________________\r\n");

        CyDelay(1000);
        if(obd_is_online())
        {
        	cyhal_gpio_write(LED2, CYBSP_LED_STATE_OFF);
        	cyhal_gpio_toggle(LED1);
        }
        else
        {
        	cyhal_gpio_write(LED1, CYBSP_LED_STATE_OFF);
        	cyhal_gpio_toggle(LED2);
        }
    }
}

void handle_error(void)
{
    /* Disable all interrupts. */
   __disable_irq();
   cyhal_gpio_write(LED1, CYBSP_LED_STATE_OFF);
   cyhal_gpio_write(LED2, CYBSP_LED_STATE_ON);
   CY_ASSERT(0);
}

/* [] END OF FILE */
