/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    debug.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef _DEBUG_H
#define _DEBUG_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "system_config.h"
#include "system_definitions.h"

#include "queue.h"
#include "semphr.h"


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
    // DOM-IGNORE-END 

    // *****************************************************************************
    // *****************************************************************************
    // Section: Type Definitions
    // *****************************************************************************
    // *****************************************************************************

    // *****************************************************************************

    /* Application states

      Summary:
        Application states enumeration

      Description:
        This enumeration defines the valid application states.  These states
        determine the behavior of the application at various times.
     */

    typedef enum {
        /* Application's state machine's initial state. */
        DEBUG_STATE_READ = 0,
        DEBUG_STATE_WAIT_FOR_TX_FINISH = 1,

        /* TODO: Define states used by the application state machine. */

    } DEBUG_STATES;


    // *****************************************************************************

    /* Application Data

      Summary:
        Holds application data

      Description:
        This structure holds the application's data.

      Remarks:
        Application strings and buffers are be defined outside this structure.
     */
    typedef enum {
        LOG_LEVEL_TRACE = 0,
        LOG_LEVEL_DEBUG = 1,
        LOG_LEVEL_INFO = 2,
        LOG_LEVEL_WARNING = 3,
        LOG_LEVEL_ERROR = 4,
    } LOG_LEVEL;

    typedef struct {
        int size;
        int index;
        char * buffer;
        bool is_transmit;
    } DEBUG_REQUEST;

    typedef struct {
        /* The application's current state */
        DEBUG_STATES state;
        LOG_LEVEL log_level;

        //---uart items
        DRV_HANDLE uart;
        int uart_instance;
        //backlog of requests
        QueueHandle_t q_request;
        QueueHandle_t rx_bytes;
        //currently active request
        DEBUG_REQUEST request;

        SemaphoreHandle_t lock;

        QueueHandle_t tx_space_free;

        //---timer items
        DRV_HANDLE system_timer;
        //system time tick frequency
        size_t freq;
        //how often the system timer has overrun
        int overflow_count;
        //any messages with a lower log level will not be written out
        int send_count;

    } DEBUG_DATA;

    // *****************************************************************************
    // *****************************************************************************
    // Section: Application Callback Routines
    // *****************************************************************************
    // *****************************************************************************
    /* These routines are called by drivers when certain events occur.
     */

    // *****************************************************************************
    // *****************************************************************************
    // Section: Application Initialization and State Machine Functions
    // *****************************************************************************
    // *****************************************************************************

    /*******************************************************************************
      Function:
        void DEBUG_Initialize ( void )

      Summary:
         MPLAB Harmony application initialization routine.

      Description:
        This function initializes the Harmony application.  It places the 
        application in its initial state and prepares it to run so that its 
        APP_Tasks function can be called.

      Precondition:
        All other system initialization routines should be called before calling
        this routine (in "SYS_Initialize").

      Parameters:
        None.

      Returns:
        None.

      Example:
        <code>
        DEBUG_Initialize();
        </code>

      Remarks:
        This routine must be called from the SYS_Initialize function.
     */

    void DEBUG_Initialize(void);


    /*******************************************************************************
      Function:
        void DEBUG_Tasks ( void )

      Summary:
        MPLAB Harmony Demo application tasks function

      Description:
        This routine is the Harmony Demo application's tasks function.  It
        defines the application's state machine and core logic.

      Precondition:
        The system and application initialization ("SYS_Initialize") should be
        called before calling this.

      Parameters:
        None.

      Returns:
        None.

      Example:
        <code>
        DEBUG_Tasks();
        </code>

      Remarks:
        This routine must be called from SYS_Tasks() routine.
     */

    void DEBUG_Tasks(void);

    /**
     * Writes the input string out to the console.
     * @param buffer null terminated string to write
     * @return true on success. 
     */
    bool Debug_Write(const char * buffer, LOG_LEVEL level);

    /**
     * Writes the remaining stack for each task out the debug console.
     */
    void Debug_Task_Stacks();

    /**
     * Returns the number of kernel ticks since start. Ignores overruns.
     * @return 
     */
    uint32_t Get_Ticks();
    /**
     * Returns full count of all kernel ticks since start. Includes overruns.
     * @return 
     */
    uint64_t Get_Ticks_Full();

    //returns the total time up in seconds
    float Get_System_Time();

#endif /* _DEBUG_H */

    //DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

