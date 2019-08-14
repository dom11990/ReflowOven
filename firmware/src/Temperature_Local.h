/* 
 * File:   Temperature_Local.h
 * Author: dom
 *
 * Created on April 8, 2018, 10:01 AM
 */

#ifndef TEMPERATURE_LOCAL_H
#define	TEMPERATURE_LOCAL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "Temperature.h"

void SPI_Callback(DRV_SPI_BUFFER_EVENT event,
        DRV_SPI_BUFFER_HANDLE bufferHandle, void * context);

MAX31855_REPLY Parse_Reply(uint32_t reply);



#ifdef	__cplusplus
}
#endif

#endif	/* TEMPERATURE_LOCAL_H */

