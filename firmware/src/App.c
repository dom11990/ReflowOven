/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

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

#include "app.h"
#include "Uart.h"
#include "Debug.h"
#include "stdio.h"
#include "driver/tmr/src/drv_tmr_local.h"
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

APP_DATA appData;

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

/* TODO:  Add any necessary local functions.
 */


void BSP_DelayUs(uint32_t microseconds) {
    uint32_t time;

    time = _CP0_GET_COUNT(); // Read Core Timer    
    time += (SYS_CLK_FREQ / 2 / 1000000) * microseconds; // calc the Stop Time    
    while ((int32_t) (time - _CP0_GET_COUNT()) > 0) {
    };
}
// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize(void) {
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    appData.led_on = 0;
    appData.profile.temperatures = pvPortMalloc(sizeof (int)*100);
    appData.profile.times = pvPortMalloc(sizeof (int)*100);
    appData.profile.entries = 0;
    appData.text = pvPortMalloc(64);
    appData.data = pvPortMalloc(64);

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks(void) {
    /* Check the application's current state. */
    switch (appData.state) {
            /* Application's initial state. */
        case APP_STATE_INIT:
        {
            Debug_Write("App initialized", LOG_LEVEL_INFO);
            appData.state = APP_STATE_AWAIT_COMMAND;


            //            while (1) {
            //BSP_DelayUs(200);

            //            }

            break;
        }

        case APP_STATE_AWAIT_COMMAND:
        {

            Debug_Write("Awaiting command...", LOG_LEVEL_INFO);
            char index = 0;
            memset(appData.text, 0, 64);
            memset(appData.data, 0, 64);
            while (index < 64) {
                char temp;
                if (!Uart_Read(&temp, 1, 1000)) {
                    appData.data[index] = temp;
                    if ('\n' == temp) {
                        //received the end of transmission byte
                        snprintf(appData.text, 64, "Command received: %s", appData.data);
                        Debug_Write(appData.text, LOG_LEVEL_INFO);
                        Parse_Command(appData.data);
                    }
                    index++;

                } else {
                    //had an error so we reset the receive sequence
                    return;
                }

            }
            //try to read one byte
            //            int result = Uart_Read(reader, 1, 100);
            //            if (!result) {
            //                //no errors let's print what we got
            //                int read_size = reader[0];
            //                result = Uart_Read(reader, read_size, 100);
            //                snprintf(data, 64, "%s", reader);
            //                Debug_Write(data, LOG_LEVEL_INFO);
            //                Parse_Command(reader);
            //            }
            //            vPortFree(reader);
            //            vPortFree(data);

            if (appData.led_on) {
                LEDOff();
                appData.led_on = 0;
            } else {
                //want LED off for now
                //LEDOn();
                appData.led_on = 1;
            }

            break;
        }

        default:
        {
            //should never be executed
            break;
        }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

int Parse_Command(char * command) {
    char* cmd;
    cmd = strtok(command, ",");
    //returns 0 if matched
    if (!strcmp("PROFILE", cmd)) {
        int time = 0;
        int temperature = 0;
        char * param1;
        char * param2;
        param1 = strtok(NULL, ",");
        param2 = strtok(NULL, ",");

        if (param1 == 0 || param2 == 0)
            goto Parse_Error;

        time = atoi(param1);
        temperature = atoi(param2);

        if (temperature == 0 || time == 0) {
            goto Parse_Error;
        } else {
            char * data = pvPortMalloc(64);
            snprintf(data, 64, "VALID COMMAND: %d %d", time, temperature);
            Debug_Write(data, LOG_LEVEL_INFO);
            vPortFree(data);
            appData.profile.times[appData.profile.entries] = time;
            appData.profile.temperatures[appData.profile.entries] = temperature;
            appData.profile.entries++;

            Sort_Profile_Entries(&appData.profile);
            return 0;
        }
    } else if (!strcmp("PROFILE?", cmd)) {
        char * temp = pvPortMalloc(sizeof (char)*32);
        int i;
        for (i = 0; i < appData.profile.entries; i++) {
            snprintf(temp, 32, "%d %d",
                    appData.profile.times[i],
                    appData.profile.temperatures[i]);
            Debug_Write(temp, LOG_LEVEL_INFO);
        }
        vPortFree(temp);
        Debug_Write("VALID COMMAND PROFILE?", LOG_LEVEL_INFO);
        return 0;
    } else if (!strcmp("CLEAR_PROFILE", cmd)) {
        //if there are no entires, the profile can't be executed
        appData.profile.entries = 0;
        Debug_Write("VALID COMMAND CLEAR PROFILE", LOG_LEVEL_INFO);
        return 0;
    } else if (!strcmp("START", cmd)) {
        //if there are no entires, the profile can't be executed

        Debug_Write("VALID COMMAND CLEAR PROFILE", LOG_LEVEL_INFO);
        return 0;
    } else if (!strcmp("ABORT", cmd)) {
        //if there are no entires, the profile can't be executed

        Debug_Write("VALID COMMAND CLEAR PROFILE", LOG_LEVEL_INFO);
        return 0;
    }

Parse_Error:
    Debug_Write("Invalid command", LOG_LEVEL_ERROR);
    Debug_Write(command, LOG_LEVEL_ERROR);
    return -1;

}

void Sort_Profile_Entries(Profile_t* profile) {

    Profile_t * temp_profile = pvPortMalloc(sizeof (Profile_t));
    temp_profile->entries = profile->entries;
    temp_profile->temperatures = pvPortMalloc(sizeof (int)*profile->entries);
    temp_profile->times = pvPortMalloc(sizeof (int)*profile->entries);
    int i;
    for (i = 0; i < profile->entries; i++) {
        int j;
        int next_time = 999;
        int index = 0;
        for (j = 0; j < profile->entries; j++) {
            //must be greater than 0, after we find an index we clear it
            //to avoid double counting
            if (profile->times[j] > 0) {
                if (profile->times[j] < next_time) {
                    index = j;
                    next_time = profile->times[j];
                }
            }
        }
        //we now have the index of the next smallest time; copy it over into 
        //the temp profile
        temp_profile->times[i] = next_time;
        temp_profile->temperatures[i] = profile->temperatures[index];
        //clear this so we dont double count, it will be restored at the end
        profile->times[index] = 0;
    }
    memcpy(profile->temperatures, temp_profile->temperatures, sizeof (int)*profile->entries);
    memcpy(profile->times, temp_profile->times, sizeof (int)*profile->entries);


    vPortFree(temp_profile->temperatures);
    vPortFree(temp_profile->times);
    vPortFree(temp_profile);
}

/*******************************************************************************
 End of File
 */
