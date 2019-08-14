/* 
 * File:   Debug_Local.h
 * Author: dom
 *
 * Created on April 8, 2018, 10:09 AM
 */

#ifndef DEBUG_LOCAL_H
#define	DEBUG_LOCAL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "driver/usart/drv_usart.h"
#include "driver/tmr/drv_tmr.h"

    /**
     * Called when the last byte is transmitted
     * @param index
     */
    void Debug_Byte_Tx_Callback(const SYS_MODULE_INDEX index);
    /**
     * Called when data is available. Enqueues the byte to the rx_bytes queue
     * @param index
     */
    void Debug_Byte_Rx_Callback(const SYS_MODULE_INDEX index);


    /**
     * Callback that is called to get the total runtime
     * @param context
     * @param alarmCount
     */
    void Timer_Callback(uintptr_t context, uint32_t alarmCount);


#ifdef	__cplusplus
}
#endif

#endif	/* DEBUG_LOCAL_H */

