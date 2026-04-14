#ifndef DRV_PWM_H
#define DRV_PWM_H

#include "Ifx_Types.h"

void DrvPwm_Init(void);
void DrvPwm_SetDuty(float32 dutyPercent);
void DrvPwm_UpdateCapture(void);

/* Debug variables - read via debugger */
extern volatile float32 g_periodMs;
extern volatile float32 g_dutyPercent;

#endif /* DRV_PWM_H */
