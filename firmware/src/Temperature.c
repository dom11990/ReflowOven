/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    temperature.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "Temperature.h"
#include "Debug.h"
#include "Temperature_Local.h"

#include "system_config/default/system_config.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
 */
#define AVERAGES 10
TEMPERATURE_DATA temperatureData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
 */

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************




// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void TEMPERATURE_Initialize ( void )

  Remarks:
    See prototype in temperature.h.
 */

void TEMPERATURE_Initialize(void) {
    /* Place the App state machine in its initial state. */
    temperatureData.state = TEMPERATURE_STATE_INIT;
    temperatureData.q_done = xQueueCreate(1, sizeof (bool));
    temperatureData.spi = DRV_SPI_Open(0, DRV_IO_INTENT_READWRITE);
    temperatureData.status.thermocouple = 0;
    temperatureData.status.internal = 0;
    temperatureData.status.fault = false;
    temperatureData.status.fault_short_to_gnd = false;
    temperatureData.status.fault_short_to_vcc = false;
    temperatureData.status.fault_open = false;
    temperatureData.latest_read = 0;
    temperatureData.count = 0;
    temperatureData.measured = pvPortMalloc(sizeof (float)*AVERAGES);

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void TEMPERATURE_Tasks ( void )

  Remarks:
    See prototype in temperature.h.
 */

void TEMPERATURE_Tasks(void) {
    /* Check the application's current state. */
    switch (temperatureData.state) {
            /* Application's initial state. */
        case TEMPERATURE_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized) {

                temperatureData.state = TEMPERATURE_READ;
            }
            break;
        }
        case TEMPERATURE_START:
        {
            temperatureData.count = 0;
            temperatureData.state = TEMPERATURE_READ;
            break;
        }
        case TEMPERATURE_READ:
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            MAX_nCSStateSet(0);
            DRV_SPI_BUFFER_HANDLE handle;
            DRV_SPI_BUFFER_HANDLE result;
            result = DRV_SPI_BufferAddRead2(temperatureData.spi,
                    &temperatureData.latest_read, 4,
                    SPI_Callback, (void *) &temperatureData, &handle);
            
            temperatureData.state = TEMPERATURE_PROCESS;
            break;
        }
        case TEMPERATURE_PROCESS:
        {
            bool done;
            xQueueReceive(temperatureData.q_done, &done, portMAX_DELAY);
            MAX_nCSStateSet(1);
            temperatureData.status = Parse_Reply(temperatureData.latest_read);

            temperatureData.measured[temperatureData.count] =
                    temperatureData.status.thermocouple;
            
            temperatureData.count++;
            //if we need more samples, continue looping. otherwise go to done 
            if (temperatureData.count < AVERAGES)
                temperatureData.state = TEMPERATURE_READ;
            else
                temperatureData.state = TEMPERATURE_DONE;
            
            break;
        }
        case TEMPERATURE_DONE:
        {
            int i = 0;
            temperatureData.average = 0;
            for (i = 0; i < AVERAGES; i++)
                temperatureData.average += temperatureData.measured[i];
            temperatureData.average /= AVERAGES;
            char * buffer = pvPortMalloc(32);
            snprintf(buffer, 64, "Current Temp: %f Fault: %d",
                    temperatureData.average,temperatureData.status.fault);
            Debug_Write(buffer, LOG_LEVEL_DEBUG);
            vPortFree(buffer);
            temperatureData.state = TEMPERATURE_START;
        }
        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
