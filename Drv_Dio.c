#include "Drv_Dio.h"
#include "Port/Std/IfxPort.h"

void DrvDio_Init(void)
{
    /* LED1: P00.5 push-pull output */
    IfxPort_setPinModeOutput(&MODULE_P00, 5, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    /* FL motor direction: IN1=P00.0, IN2=P00.1 */
    IfxPort_setPinModeOutput(&MODULE_P00, 0, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&MODULE_P00, 1, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(&MODULE_P00, 0);
    IfxPort_setPinLow(&MODULE_P00, 1);
}

void DrvDio_SetMotorFL(MotorDirection dir)
{
    switch (dir)
    {
    case MOTOR_FORWARD:
        IfxPort_setPinHigh(&MODULE_P00, 0);  /* IN1 = High */
        IfxPort_setPinLow(&MODULE_P00, 1);   /* IN2 = Low  */
        break;
    case MOTOR_REVERSE:
        IfxPort_setPinLow(&MODULE_P00, 0);   /* IN1 = Low  */
        IfxPort_setPinHigh(&MODULE_P00, 1);  /* IN2 = High */
        break;
    case MOTOR_STOP:
    default:
        IfxPort_setPinLow(&MODULE_P00, 0);   /* IN1 = Low  */
        IfxPort_setPinLow(&MODULE_P00, 1);   /* IN2 = Low  */
        break;
    }
}
