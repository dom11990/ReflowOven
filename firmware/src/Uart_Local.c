
#include "Uart_Local.h"

#include "Debug.h"

extern UART_DATA uartData;

//______________________________________________________________________________
void Uart_Byte_Tx_Callback(const SYS_MODULE_INDEX index)
{
    char temp;
    uartData.send_count++;
    UBaseType_t woken = pdFALSE;
    xQueueReceiveFromISR(uartData.tx_space_free,&temp, &woken);
    portEND_SWITCHING_ISR(woken);
}
void Uart_Byte_Rx_Callback(const SYS_MODULE_INDEX index)
{
    UBaseType_t woken = pdFALSE;
    char data;
    data = U1RXREG;
    xQueueSendFromISR(uartData.rx_bytes, &data, &woken);
    portEND_SWITCHING_ISR(woken);
}
void Uart_Timeout(TimerHandle_t xTimer)
{
    UART_DATA * data = (UART_DATA*) pvTimerGetTimerID(xTimer);
    xSemaphoreGive(data->timeout_occurred);
}

int Uart_Read_Low_Level(char * buffer, uint32_t size, uint32_t timeout_ms){
    int error = 0;
    xQueueReset(uartData.q_rx_reply);
    UART_REQUEST req;
    req.buffer = buffer;
    req.index = 0;
    req.is_transmit = false;
    req.size = size;
    req.timeout_ms = timeout_ms;

    //send the read request to the task
    xQueueSend(uartData.q_request, &req, portMAX_DELAY);
    //wait for the reply
    xQueueReceive(uartData.q_rx_reply, &error, portMAX_DELAY);
    return error;
}
