#include "Debug_Local.h"
#include "Debug.h"


extern DEBUG_DATA debugData;

//______________________________________________________________________________
void Debug_Byte_Tx_Callback(const SYS_MODULE_INDEX index)
{
    debugData.send_count++;
    UBaseType_t woken = pdFALSE;
    //the value is irrelevant, only that something is pushed into the queue
    char temp = 1;
    xQueueSendFromISR(debugData.tx_space_free,&temp, &woken);
    portEND_SWITCHING_ISR(woken);
    
}
void Debug_Byte_Rx_Callback(const SYS_MODULE_INDEX index)
{
    UBaseType_t woken = pdFALSE;
    char data;
    data = U2RXREG;
    xQueueSendFromISR(debugData.rx_bytes, &data, &woken);
    portEND_SWITCHING_ISR(woken);
}

//______________________________________________________________________________
void Timer_Callback(uintptr_t context, uint32_t alarmCount)
{
    DEBUG_DATA * data = (DEBUG_DATA*) context;
    data->overflow_count++;
    return;
}
