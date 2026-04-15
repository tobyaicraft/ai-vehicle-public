#include "App_Scheduler.h"
#include "Drv_Stm.h"
#include "Drv_Adc.h"
#include "Drv_Can.h"
#include "Drv_Pwm.h"
#include "Drv_Dio.h"
#include "Port/Std/IfxPort.h"
#include "IfxCpu_Irq.h"

static volatile uint32 counter_1ms;
static volatile uint8 flag_1ms;
static volatile uint8 flag_10ms;
static volatile uint8 flag_100ms;

IFX_INTERRUPT(ISR_Stm0_Compare0, 0, ISR_PRIORITY_STM0)
{
    IfxStm_clearCompareFlag(&MODULE_STM0, stmConfig.comparator);
    IfxStm_increaseCompare(&MODULE_STM0, stmConfig.comparator, stmConfig.ticks);

    flag_1ms = 1;
    counter_1ms++;

    if (counter_1ms % 10 == 0)
    {
        flag_10ms = 1;
    }

    if (counter_1ms % 100 == 0)
    {
        flag_100ms = 1;
        counter_1ms = 0;
    }
}

void Scheduler_init(void)
{
}

static void Task_1ms(void)
{
    /* Reserved for future use */
}

static volatile uint16 adcValue_AN0;
static volatile uint8  canRxLedStatus = 0;
volatile float32 g_pwmDutySet = 50.0f;
volatile uint8   g_motorDir   = 0;      /* 0=Stop, 1=Forward, 2=Reverse */

static void Task_10ms(void)
{
    DrvPwm_UpdateCapture();

    adcValue_AN0 = DrvAdc_GetResult_AN0();

    DrvPwm_SetDuty(g_pwmDutySet);
    DrvDio_SetMotorFL((MotorDirection)g_motorDir);

    /* Poll CAN RX for LED status from TC237 */
    uint8 rxLed;
    if (DrvCan_ReceiveLedStatus(&rxLed))
    {
        canRxLedStatus = rxLed;
    }
}

static void Task_100ms(void)
{
    /* Send ADC potentiometer value over CAN (ID 0x200) */
    DrvCan_SendAdcValue(adcValue_AN0);

    /* Control LED1 based on received LED status from TC237 */
    /* LED1 (P00.5) is active-low: LOW = ON, HIGH = OFF */
    if (canRxLedStatus != 0)
    {
        IfxPort_setPinLow(&MODULE_P00, 5);
    }
    else
    {
        IfxPort_setPinHigh(&MODULE_P00, 5);
    }
}

void Scheduler_run(void)
{
    if (flag_1ms)
    {
        flag_1ms = 0;
        Task_1ms();
    }

    if (flag_10ms)
    {
        flag_10ms = 0;
        Task_10ms();
    }

    if (flag_100ms)
    {
        flag_100ms = 0;
        Task_100ms();
    }
}
