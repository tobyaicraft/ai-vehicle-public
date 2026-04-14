#include "Drv_Pwm.h"
#include "Gtm/Atom/Pwm/IfxGtm_Atom_Pwm.h"
#include "Gtm/Std/IfxGtm_Tim.h"
#include "Gtm/Std/IfxGtm_Atom.h"
#include "_PinMap/IfxGtm_PinMap.h"

/* PWM output driver (ATOM0_4 -> P02.4) */
static IfxGtm_Atom_Pwm_Driver atomPwmDriver;
static uint32 pwmPeriodTicks;

/* TIM channel pointer for polling */
static Ifx_GTM_TIM_CH *timChannel;
static float32 captureClockFreq;

/* Debug variables - visible in debugger */
volatile float32 g_periodMs    = 0.0f;
volatile float32 g_dutyPercent = 0.0f;

void DrvPwm_Init(void)
{
    Ifx_GTM *gtm = &MODULE_GTM;

    /* Enable GTM module */
    IfxGtm_enable(gtm);

    /* Configure GTM clocks */
    float32 moduleFreq = IfxGtm_Cmu_getModuleFrequency(gtm);
    IfxGtm_Cmu_setGclkFrequency(gtm, moduleFreq);
    IfxGtm_Cmu_setClkFrequency(gtm, IfxGtm_Cmu_Clk_0, moduleFreq);
    IfxGtm_Cmu_enableClocks(gtm, IFXGTM_CMU_CLKEN_FXCLK | IFXGTM_CMU_CLKEN_CLK0);

    /* ========== PWM Output: ATOM0_4 -> TOUT4 -> P02.4 ========== */
    {
        IfxGtm_Atom_Pwm_Config pwmConfig;
        IfxGtm_Atom_Pwm_initConfig(&pwmConfig, gtm);

        pwmConfig.atom        = IfxGtm_Atom_0;
        pwmConfig.atomChannel = IfxGtm_Atom_Ch_4;

        /* Period = 10ms (100Hz)
         * period ticks = moduleFreq * 0.01s */
        pwmPeriodTicks = (uint32)(moduleFreq * 0.01f);
        pwmConfig.period   = pwmPeriodTicks;
        pwmConfig.dutyCycle = pwmPeriodTicks / 2;   /* 초기값 50% */

        pwmConfig.pin.outputPin  = &IfxGtm_ATOM0_4_TOUT4_P02_4_OUT;
        pwmConfig.pin.outputMode = IfxPort_OutputMode_pushPull;
        pwmConfig.pin.padDriver  = IfxPort_PadDriver_cmosAutomotiveSpeed1;

        pwmConfig.synchronousUpdateEnabled = FALSE;
        pwmConfig.immediateStartEnabled    = TRUE;

        IfxGtm_Atom_Pwm_init(&atomPwmDriver, &pwmConfig);
    }

    /* ========== TIM Input Capture: TIM0_0 -> TIN0 -> P02.0 (polling) ========== */
    {
        timChannel = IfxGtm_Tim_getChannel(&gtm->TIM[0], IfxGtm_Tim_Ch_0);

        /* Reset channel */
        IfxGtm_Tim_Ch_resetChannel(&gtm->TIM[0], IfxGtm_Tim_Ch_0);

        /* Configure TIM channel */
        timChannel->CTRL.U = 0;
        timChannel->CTRL.B.TIM_MODE = IfxGtm_Tim_Mode_pwmMeasurement;
        timChannel->CTRL.B.DSL      = 1;                       /* Active on falling edge (leftAligned PWM) */
        timChannel->CTRL.B.CNTS_SEL = IfxGtm_Tim_CntsSel_cntReg;
        timChannel->CTRL.B.GPR0_SEL = IfxGtm_Tim_GprSel_cnts;  /* GPR0 = duty (pulse length) */
        timChannel->CTRL.B.GPR1_SEL = IfxGtm_Tim_GprSel_cnts;  /* GPR1 = period */

        IfxGtm_Tim_Ch_setClockSource(timChannel, IfxGtm_Cmu_Clk_0);

        /* Configure input source: current channel, TIN0 */
        gtm->TIM[0].IN_SRC.B.VAL_0 = 1;
        gtm->TIM[0].IN_SRC.B.MODE_0 = 1;
        timChannel->CTRL.B.CICTRL = IfxGtm_Tim_Input_currentChannel;

        /* Configure input pin: P02.0 */
        IfxGtm_PinMap_setTimTin(&IfxGtm_TIM0_0_TIN0_P02_0_IN, IfxPort_InputMode_noPullDevice);

        captureClockFreq = IfxGtm_Tim_Ch_getCaptureClockFrequency(gtm, timChannel);

        /* Enable TIM channel last */
        timChannel->CTRL.B.TIM_EN = 1;
    }
}

void DrvPwm_SetDuty(float32 dutyPercent)
{
    if (dutyPercent < 0.0f) dutyPercent = 0.0f;
    if (dutyPercent > 100.0f) dutyPercent = 100.0f;

    uint32 dutyTicks = (uint32)(pwmPeriodTicks * dutyPercent / 100.0f);

    IfxGtm_Atom_Ch_setCompareOne(atomPwmDriver.atom, atomPwmDriver.atomChannel, dutyTicks);
}

void DrvPwm_UpdateCapture(void)
{
    if (IfxGtm_Tim_Ch_isNewValueEvent(timChannel))
    {
        uint32 dutyTicks   = timChannel->GPR0.B.GPR0;   /* GPR0 = pulse length (duty) */
        uint32 periodTicks = timChannel->GPR1.B.GPR1;  /* GPR1 = period */

        if (periodTicks > 0)
        {
            g_periodMs    = (float32)periodTicks / captureClockFreq * 1000.0f;
            g_dutyPercent = (float32)dutyTicks / (float32)periodTicks * 100.0f;
        }

        IfxGtm_Tim_Ch_clearNewValueEvent(timChannel);
    }
}
