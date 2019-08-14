/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    uart.h

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

#ifndef _UART_H
#define _UART_H

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
#include "timers.h"

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


    // *****************************************************************************

    /* Application Data

      Summary:
        Holds application data

      Description:
        This structure holds the application's data.

      Remarks:
        Application strings and buffers are be defined outside this structure.
     */


    typedef struct {
        int size;
        int index;
        char * buffer;
        bool is_transmit;
        uint32_t timeout_ms;
    } UART_REQUEST;

    typedef struct {
        //---uart items
        DRV_HANDLE uart;
        int uart_instance;
        //backlog of requests
        QueueHandle_t q_request;
        //is filled by the isr with incoming data
        QueueHandle_t rx_bytes;
        //task enqueues here when a receive completes
        QueueHandle_t q_rx_reply;
        //currently active request
        UART_REQUEST request;
        TimerHandle_t tmr_timeout;
        SemaphoreHandle_t timeout_occurred;
        SemaphoreHandle_t lock;
        //is given by the isr whenever a byte is free in the transmit buffer
        QueueHandle_t tx_space_free;
        int send_count;
    } UART_DATA;

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
        void UART_Initialize ( void )

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
        UART_Initialize();
        </code>

      Remarks:
        This routine must be called from the SYS_Initialize function.
     */

    void UART_Initialize(void);


    /*******************************************************************************
      Function:
        void UART_Tasks ( void )

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
        UART_Tasks();
        </code>

      Remarks:
        This routine must be called from SYS_Tasks() routine.
     */

    void UART_Tasks(void);

    bool Uart_Write(const char* buffer, uint32_t size);

    /** 
     * Used to initiate a complex read such as performing multiple reads
     * back to back while guaranteeing no other tasks can jump in.
     * @return true on success
     */
    bool Uart_Start_Read_Complex();

    /**
     * Performs a read into the provided buffer. If a timeout occurs, the function
     * will return non-zero. The user must call Uart_Start_Read_Complex with
     * true return value before calling this method.
     * @param buffer
     * @param size
     * @param timeout_ms
     * @return 0 on success, otherwise, an error code
     */
    int Uart_Read_Complex(char * buffer, uint32_t size, uint32_t timeout_ms);

    /**
     * Used to terminate a complex read process. Call this function after all
     * complex reads have completed to release the lock on the uart. Only
     * call this method if a successful Uart_Start_Read_Complex was called
     */
    void Uart_End_Read_Complex();



#endif /* _UART_H */

    //DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

