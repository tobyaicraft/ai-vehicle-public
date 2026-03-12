#include "Drv_Dio.h"
#include "Port/Std/IfxPort.h"

void DrvDio_Init(void)
{
    /* LED1: P00.5 push-pull output */
    IfxPort_setPinModeOutput(&MODULE_P00, 5, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
}
