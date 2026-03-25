#include "Drv_Adc.h"
#include "Vadc/Adc/IfxVadc_Adc.h"

static IfxVadc_Adc vadc;
static IfxVadc_Adc_Group adcGroup0;
static IfxVadc_Adc_Channel adcChannel_AN0;

void DrvAdc_Init(void)
{
    /* Module configuration */
    IfxVadc_Adc_Config adcConfig;
    IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);
    IfxVadc_Adc_initModule(&vadc, &adcConfig);

    /* Group 0 configuration - Background Scan mode */
    IfxVadc_Adc_GroupConfig groupConfig;
    IfxVadc_Adc_initGroupConfig(&groupConfig, &vadc);

    groupConfig.groupId = IfxVadc_GroupId_0;
    groupConfig.master  = IfxVadc_GroupId_0;

    groupConfig.arbiter.requestSlotBackgroundScanEnabled = TRUE;
    groupConfig.backgroundScanRequest.autoBackgroundScanEnabled = TRUE;
    groupConfig.backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;

    IfxVadc_Adc_initGroup(&adcGroup0, &groupConfig);

    /* Channel 0 (AN0) - potentiometer on TC275 Lite Kit */
    IfxVadc_Adc_ChannelConfig chConfig;
    IfxVadc_Adc_initChannelConfig(&chConfig, &adcGroup0);

    chConfig.channelId       = IfxVadc_ChannelId_0;
    chConfig.resultRegister  = IfxVadc_ChannelResult_0;
    chConfig.backgroundChannel = TRUE;

    IfxVadc_Adc_initChannel(&adcChannel_AN0, &chConfig);

    /* Add AN0 to background scan */
    unsigned channels = (1 << IfxVadc_ChannelId_0);
    IfxVadc_Adc_setBackgroundScan(&vadc, &adcGroup0, channels, channels);

    /* Start continuous background scan */
    IfxVadc_Adc_startBackgroundScan(&vadc);
}

uint16 DrvAdc_GetResult_AN0(void)
{
    Ifx_VADC_RES result;
    result = IfxVadc_Adc_getResult(&adcChannel_AN0);

    if (result.B.VF)
    {
        return (uint16)result.B.RESULT;
    }

    return 0;
}
