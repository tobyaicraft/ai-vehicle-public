#ifndef DRV_DIO_H
#define DRV_DIO_H

#include "Ifx_Types.h"

typedef enum
{
    MOTOR_STOP = 0,
    MOTOR_FORWARD,
    MOTOR_REVERSE
} MotorDirection;

void DrvDio_Init(void);
void DrvDio_SetMotorFL(MotorDirection dir);

#endif /* DRV_DIO_H */
