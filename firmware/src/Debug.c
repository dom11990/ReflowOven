/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    debug.c

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

#include "debug.h"
#include "string.h"
#include "stdio.h"
#include "portmacro.h"
#include "Debug_Local.h"


#include "peripheral/peripheral.h"
//#include "peripheral/usart/plib_usart.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define TX_FIFO_DEPTH 8
#define RX_BUFFER_SIZE 32
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

DEBUG_DATA debugData;

void DEBUG_Initialize(void) {
    debugData.send_count = 0;
    debugData.uart_instance = 1;
    /* Place the App state machine in its initial state. */
    debugData.state = DEBUG_STATE_READ;
    debugData.overflow_count = 0;
    debugData.log_level = LOG_LEVEL_TRACE;
    memset(&debugData.request, 0, sizeof (DEBUG_REQUEST));

    debugData.q_request = xQueueCreate(10, sizeof (DEBUG_REQUEST));

    debugData.tx_space_free = xQueueCreate(TX_FIFO_DEPTH,sizeof(char));
    char i = TX_FIFO_DEPTH;
    //load the transmit queue with the TX_FIFO_DEPTH
    for(i;i;i--){
        xQueueSend(debugData.tx_space_free,&i,portMAX_DELAY);
    }
    
    debugData.rx_bytes = xQueueCreate(RX_BUFFER_SIZE, sizeof (char));

    debugData.lock = xSemaphoreCreateMutex();
    xSemaphoreGive(debugData.lock);

    //create and configure uart
    debugData.uart = DRV_USART_Open(debugData.uart_instance,
            DRV_IO_INTENT_READWRITE);
    DRV_USART_ByteReceiveCallbackSet(debugData.uart_instance,
            Debug_Byte_Rx_Callback);
    DRV_USART_ByteTransmitCallbackSet(debugData.uart_instance,
            Debug_Byte_Tx_Callback);
    //    DRV_USART_BufferEventHandlerSet(debugData.uart, Uart_Debug_Event_Handler,
    //            (uintptr_t) & debugData);


    //create and configure timer
    debugData.system_timer = DRV_TMR_Open(0, DRV_IO_INTENT_EXCLUSIVE);
    //    DRV_TMR_CounterValue32BitSet(appData.system_timer,0);

    DRV_TMR_CounterClear(debugData.system_timer);
    debugData.freq = DRV_TMR_CounterFrequencyGet(debugData.system_timer);
    DRV_TMR_DIVIDER_RANGE range;
    DRV_TMR_DividerRangeGet(debugData.system_timer, &range);
    DRV_TMR_AlarmRegister(debugData.system_timer,
            range.dividerMax, true, (uintptr_t) & debugData, Timer_Callback);
    DRV_TMR_AlarmEnable(debugData.system_timer, true);
    DRV_TMR_Start(debugData.system_timer);
}

/******************************************************************************
  Function:
    void DEBUG_Tasks ( void )

  Remarks:
    See prototype in debug.h.
 */

void DEBUG_Tasks(void) {

    /* Check the application's current state. */
    //    char * buffer_to_free;
    DEBUG_REQUEST req;
    bool good = xQueueReceive(debugData.q_request,
            &req, portMAX_DELAY);
    if (!good) {
        return;
    }
    if (req.is_transmit) {
        //call transmit

        while (req.size > req.index) {
//            if (DRV_USART_TransmitBufferIsFull(debugData.uart)) {
//                BSP_DelayUs(100);
//                //CHECKING WHAT IT WOULD BE LIKE IF BLOCKING
//                //xSemaphoreTake(debugData.tx_space_free, portMAX_DELAY);
//            }
            char q_temp;
            xQueueReceive(debugData.tx_space_free,&q_temp,portMAX_DELAY);
            DRV_USART_WriteByte(debugData.uart, req.buffer[req.index]);
            req.index++;
            if (req.index == req.size) {
                //we're done!!
                //wait for the final semaphore to come out
                vPortFree(req.buffer);
                  //CHECKING WHAT IT WOULD BE LIKE IF BLOCKING  
                //          xSemaphoreTake(debugData.tx_space_free, portMAX_DELAY);
            }

        }


    } else {
        //call receive
        while (req.size > req.index) {
            xQueueReceive(debugData.rx_bytes, &req.buffer[req.index],
                    portMAX_DELAY);
            req.index++;
            if (req.index == req.size) {
                //we're done!!
                //TODO: notify the original caller that the receive is complete
                //TODO: timeout?
                vPortFree(req.buffer);
            }

        }

    }
}

//______________________________________________________________________________

uint64_t Get_Ticks_Full() {
    uint64_t ticks = DRV_TMR_CounterValue32BitGet(debugData.system_timer);
    uint64_t overflow = debugData.overflow_count;
    overflow <<= 32;
    ticks += overflow;
    return ticks;
}
//______________________________________________________________________________

uint32_t Get_Ticks() {
    return DRV_TMR_CounterValue32BitGet(debugData.system_timer);
}
//______________________________________________________________________________

float Get_System_Time() {
    float systemtime = DRV_TMR_CounterValue32BitGet(debugData.system_timer);
    //max timer tick count divided by the frequency times the number of resets
    //2^32 since 32bit timer
    uint32_t overflow_seconds = 0xFFFFFFFF / ((debugData.freq));
    float overflow_time = debugData.overflow_count * overflow_seconds;

    return systemtime / debugData.freq + overflow_time;
}


//______________________________________________________________________________

bool Debug_Write(const char* text, LOG_LEVEL level) {

    if (level < debugData.log_level) {
        //        xSemaphoreGive(debugData.lock);
        return false;
    }

    //lock it for thread safety
    xSemaphoreTake(debugData.lock, portMAX_DELAY);
    
    float sys_time = Get_System_Time();

    //fill the request struct
    DEBUG_REQUEST request;
    volatile int stringlenght = strlen(text);
    request.size = 32 + strlen(text);
    request.buffer = pvPortMalloc(request.size);
    //snprintf may return more than the buffer size, in that case
    //we have to limit ourselves to the buffer size
    uint32_t write_count = snprintf(request.buffer, request.size,
            "[%f]:  %s\r\n", sys_time, text);
    request.size = min(request.size, write_count);
    request.index = 0;
    request.is_transmit = true;
    //send it over to the task for processing
    bool success = xQueueSend(debugData.q_request, &request, portMAX_DELAY);
    if (!success) {
        //unable to enqueue, this message will not be sent and freed by the 
        //dequeue process. We need to free the buffer to avoid a leak.
        vPortFree(request.buffer);
    }
    xSemaphoreGive(debugData.lock);
    return success;
}

//______________________________________________________________________________

void Debug_Task_Stacks() {
    uint32_t count = uxTaskGetNumberOfTasks();
    TaskStatus_t * status = pvPortMalloc(sizeof (TaskStatus_t) * count);
    TaskStatus_t * index = status;
    uint32_t runtime;
    uxTaskGetSystemState(status, count, &runtime);
    char * taskinfo = pvPortMalloc(32);
    while (count) {
        snprintf(taskinfo, 32, "%s stack left: %d", index->pcTaskName,
                index->usStackHighWaterMark);
        Debug_Write(taskinfo, LOG_LEVEL_TRACE);
        index++;
        count--;
    }
    vPortFree(taskinfo);
    vPortFree(status);
}


/*******************************************************************************
 End of File
 */
