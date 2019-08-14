/* 
 * File:   Uart_Local.h
 * Author: dom
 *
 * Created on April 9, 2018, 8:32 PM
 */

#ifndef UART_LOCAL_H
#define	UART_LOCAL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "Uart.h"

    /**
     * Called when the last byte is transmitted
     * @param index
     */
    void Uart_Byte_Tx_Callback(const SYS_MODULE_INDEX index);
    /**
     * Called when data is available. Enqueues the byte to the rx_bytes queue
     * @param index
     */
    void Uart_Byte_Rx_Callback(const SYS_MODULE_INDEX index);

    /**
     * Called when a timeout on a receive occurs
     * @param xTimer
     */
    void Uart_Timeout(TimerHandle_t xTimer);
    
    int Uart_Read_Low_Level(char * buffer, uint32_t size, uint32_t timeout_ms);
    
#ifdef	__cplusplus
}
#endif

#endif	/* UART_LOCAL_H */

