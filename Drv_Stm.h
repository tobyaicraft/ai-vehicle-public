#ifndef DRV_STM_H
#define DRV_STM_H

#include "Ifx_Types.h"
#include "Stm/Std/IfxStm.h"

#define ISR_PRIORITY_STM0   10

extern IfxStm_CompareConfig stmConfig;

void DrvSTM_Init(void);

#endif /* DRV_STM_H */
