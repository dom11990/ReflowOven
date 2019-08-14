#include "Temperature_Local.h"


//______________________________________________________________________________
void SPI_Callback(DRV_SPI_BUFFER_EVENT event,
        DRV_SPI_BUFFER_HANDLE bufferHandle, void * context)
{
    TEMPERATURE_DATA * data = (TEMPERATURE_DATA*) context;
    bool done = true;
    UBaseType_t woken = false;
    //notify the task that the acquisition is complete
    xQueueSendFromISR(data->q_done, &done, &woken);
    portEND_SWITCHING_ISR(woken);
}

//______________________________________________________________________________
MAX31855_REPLY Parse_Reply(uint32_t reply)
{
    MAX31855_REPLY status;
    int value = (reply >> 18) & 0x1FFF;
    //see if we are negative
    if (reply & (1 << 31))
    {
        //if we are  negative, we need to make the number positive
        //need the maske because we only have 13 bits
        value = (~value + 1) & 0x1FFF;
        status.thermocouple = -1 * value;
    } else
    {
      status.thermocouple = value;
    }
    status.thermocouple /= 4.0;


    value = (reply >> 4) & 0x000007FF;
    //see if we are negative
    if (reply & (1 << 15))
    {
        //if we are  negative, we need to make the number positive
        //need the maske because we only have 11 bits
        value = ((~value) + 1) & 0x000007FF;
        status.internal = -1 * value;
    } else
    {
        status.internal = value;
    }
    status.internal /= 16.0;

    status.fault = reply & (1 << 16);
    status.fault_short_to_vcc = reply & (1 << 2);
    status.fault_short_to_gnd = reply & (1 << 1);
    status.fault_open = reply & (1);
    return status;
}
