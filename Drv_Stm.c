#include "Drv_Stm.h"

IfxStm_CompareConfig stmConfig;

void DrvSTM_Init(void)
{
    IfxStm_initCompareConfig(&stmConfig);

    stmConfig.ticks           = (uint32)IfxStm_getTicksFromMilliseconds(&MODULE_STM0, 1);
    stmConfig.triggerPriority = ISR_PRIORITY_STM0;
    stmConfig.typeOfService   = IfxSrc_Tos_cpu0;

    IfxStm_initCompare(&MODULE_STM0, &stmConfig);

    /* Stop STM when debugger halts (breakpoint) */
    IfxStm_enableOcdsSuspend(&MODULE_STM0);
}
