#include "App_Scheduler.h"
#include "Drv_Stm.h"
#include "Drv_Adc.h"
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

static void Task_10ms(void)
{
    adcValue_AN0 = DrvAdc_GetResult_AN0();
}

static void Task_100ms(void)
{
    static uint8 ledCounter = 0;

    /* ADC 12-bit (0~4095) -> LED toggle period: 1~20 (x100ms)
     * Pot left (0):    period = 1  -> fast blink (100ms)
     * Pot right (4095): period = 20 -> slow blink (2s)
     */
    uint8 period = (uint8)(adcValue_AN0 / 215) + 1;  /* 4095/19 ≈ 215 */
    if (period > 20)
    {
        period = 20;
    }

    ledCounter++;

    if (ledCounter >= period)
    {
        ledCounter = 0;
        IfxPort_togglePin(&MODULE_P00, 5);
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
