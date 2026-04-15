#ifndef DRV_CAN_H
#define DRV_CAN_H

#include "Ifx_Types.h"

void DrvCan_Init(void);
void DrvCan_SendAdcValue(uint16 adcValue);
boolean DrvCan_ReceiveLedStatus(uint8 *ledStatus);

#endif /* DRV_CAN_H */
