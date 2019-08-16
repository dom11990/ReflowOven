/*******************************************************************************
 MPLAB Harmony Application Source File
  
 Company:
   Microchip Technology Inc.
  
 File Name:
   uart.c

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
#include "string.h"
#include "stdio.h"
#include "Uart_Local.h"
#include "Uart.h"
#include "timers.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


#define TX_FIFO_DEPTH 4
#define RX_BUFFER_DEPTH 256
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

UART_DATA uartData;
void UART_Initialize(void)
{
    uartData.send_count = 0;
    uartData.uart_instance = 0;
    /* Place the App state machine in its initial state. */
    memset(&uartData.request, 0, sizeof (UART_REQUEST));

    uartData.q_request = xQueueCreate(10, sizeof (UART_REQUEST));
    uartData.q_rx_reply = xQueueCreate(2, sizeof (int));

    //use the queue size to limit enqueues to the uart buffer
    //when sending, wait to enqueue, then call the driver write byte
    //in the callback of the uart, dequeue
    uartData.tx_space_free = xQueueCreate(TX_FIFO_DEPTH,sizeof(char));

    
    uartData.rx_bytes = xQueueCreate(RX_BUFFER_DEPTH, sizeof (char));

    uartData.lock = xSemaphoreCreateMutex();
    xSemaphoreGive(uartData.lock);

    //create the timout functionalities via a semaphore and a timer
    uartData.timeout_occurred = xSemaphoreCreateBinary();
    uartData.tmr_timeout = xTimerCreate("Uart_Timeout", 100, false,
            &uartData, Uart_Timeout);

    //create and configure uart
    uartData.uart = DRV_USART_Open(uartData.uart_instance,
            DRV_IO_INTENT_READWRITE);
    DRV_USART_ByteReceiveCallbackSet(uartData.uart_instance,
            Uart_Byte_Rx_Callback);
    DRV_USART_ByteTransmitCallbackSet(uartData.uart_instance,
            Uart_Byte_Tx_Callback);
}
/******************************************************************************
  Function:
    void UART_Tasks ( void )

  Remarks:
    See prototype in uart.h.
 */

void UART_Tasks(void)
{

    /* Check the application's current state. */
    //    char * buffer_to_free;
    UART_REQUEST req;
    bool good = xQueueReceive(uartData.q_request,
            &req, portMAX_DELAY);
    if (!good)
    {
        return;
    }
    if (req.is_transmit)
    {
        //call transmit

        while (req.size > req.index)
        {   
            char temp = 0;
            //blocks here until a spot in the queue opens up
            xQueueSend(uartData.tx_space_free,&temp,portMAX_DELAY);
            DRV_USART_WriteByte(uartData.uart, req.buffer[req.index]);
            req.index++;
        }
        //done with the transmit, free the buffer
        vPortFree(req.buffer);


    } else
    {
        //call receive

        //this call should go through immediately
        //just making sure the semaphore is not available, the timer
        //will give in its callback if it expires
        //TODO: the timer seems redundant
        xSemaphoreTake(uartData.timeout_occurred, 0);
        //set the timer up with our timeout
        xTimerChangePeriod(uartData.tmr_timeout,
                req.timeout_ms / portTICK_PERIOD_MS, portMAX_DELAY);
        //start the timer
        xTimerReset(uartData.tmr_timeout, portMAX_DELAY);
        //set this to 1 so that the while loop runs
        //it will get set to a proper value in the loop
        float end_time = Get_System_Time() + req.timeout_ms / 1000.0;
        uint32_t timeout_ticks = req.timeout_ms / portTICK_PERIOD_MS;
        while (req.size > req.index && timeout_ticks > 0)
        {
            float current_time = Get_System_Time();
            if (current_time > end_time)
            {
                timeout_ticks = 0;
            } else
            {
                timeout_ticks = 1000 * (end_time - current_time) / portTICK_PERIOD_MS;
            }
            //            timeout = xTimerGetExpiryTime(uartData.tmr_timeout);
            //            timeout = xTimerIsTimerActive(uartData.tmr_timeout) ? timeout : 0;
            if (xQueueReceive(uartData.rx_bytes,
                    &req.buffer[req.index], timeout_ticks))
            {
                req.index++;
            }

        }
        //if we had an error, set the error code to -2
        xTimerStop(uartData.tmr_timeout, portMAX_DELAY);
        int error = 0;
        if (!timeout_ticks)
        {
            error = -2; //timeout error
        }
        xQueueSend(uartData.q_rx_reply, &error, portMAX_DELAY);

    }
}

//______________________________________________________________________________
bool Uart_Write(const char * buffer, uint32_t size)
{
    //lock it for thread safety
    xSemaphoreTake(uartData.lock, portMAX_DELAY);

    //fill the request struct
    UART_REQUEST request;
    request.size = size;
    request.buffer = pvPortMalloc(request.size);
    //copy the data over
    memcpy(request.buffer, buffer, size);
    request.index = 0;
    request.is_transmit = true;
    //send it over to the task for processing
    bool success = xQueueSend(uartData.q_request, &request, portMAX_DELAY);

    if (!success)
    {
        vPortFree(request.buffer);
    }
    xSemaphoreGive(uartData.lock);
    return success;
}

//______________________________________________________________________________
int Uart_Read(char * buffer, uint32_t size, uint32_t timeout_ms)
{
    if (!xSemaphoreTake(uartData.lock, portMAX_DELAY))
    {
        return -1;
    }
    int error = Uart_Read_Low_Level(buffer, size, timeout_ms);
    //release lock on the action
    xSemaphoreGive(uartData.lock);
    return error;
}


//______________________________________________________________________________
bool Uart_Start_Read_Complex()
{
    xSemaphoreTake(uartData.lock, portMAX_DELAY);
}

//______________________________________________________________________________
int Uart_Read_Complex(char * buffer, uint32_t size, uint32_t timeout_ms)
{
    int error = Uart_Read_Low_Level(buffer, size, timeout_ms);
    return error;
}

//______________________________________________________________________________
void Uart_End_Read_Complex()
{
    xSemaphoreTake(uartData.lock, portMAX_DELAY);
}