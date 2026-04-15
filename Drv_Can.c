#include "Drv_Can.h"
#include "Multican/Can/IfxMultican_Can.h"
#include "Port/Std/IfxPort.h"

#define CAN_MSG_ID_TX_ADC       0x200
#define CAN_MSG_ID_RX_LED       0x100

static IfxMultican_Can        canModule;
static IfxMultican_Can_Node   canNode0;
static IfxMultican_Can_MsgObj canTxMsgObj;
static IfxMultican_Can_MsgObj canRxMsgObj;

void DrvCan_Init(void)
{
    /* Step 1: Enable CAN transceiver (TLE9251VSJ) - P20.6 LOW to exit standby */
    IfxPort_setPinModeOutput(&MODULE_P20, 6, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(&MODULE_P20, 6);

    /* Step 2: CAN module initialization */
    IfxMultican_Can_Config canConfig;
    IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);
    IfxMultican_Can_initModule(&canModule, &canConfig);

    /* Step 3: CAN Node 0 initialization - 500kbps, P20.7(RX), P20.8(TX) */
    IfxMultican_Can_NodeConfig nodeConfig;
    IfxMultican_Can_Node_initConfig(&nodeConfig, &canModule);

    nodeConfig.nodeId    = IfxMultican_NodeId_0;
    nodeConfig.baudrate  = 500000;
    nodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;
    nodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
    nodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;
    nodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

    IfxMultican_Can_Node_init(&canNode0, &nodeConfig);

    /* Step 4a: TX Message Object (MsgObj 0) - ID 0x200 */
    {
        IfxMultican_Can_MsgObjConfig msgObjConfig;
        IfxMultican_Can_MsgObj_initConfig(&msgObjConfig, &canNode0);

        msgObjConfig.msgObjId              = 0;
        msgObjConfig.messageId             = CAN_MSG_ID_TX_ADC;
        msgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;
        msgObjConfig.frame                 = IfxMultican_Frame_transmit;
        msgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;
        msgObjConfig.control.extendedFrame = FALSE;
        msgObjConfig.control.matchingId    = TRUE;

        IfxMultican_Can_MsgObj_init(&canTxMsgObj, &msgObjConfig);
    }

    /* Step 4b: RX Message Object (MsgObj 1) - ID 0x100 */
    {
        IfxMultican_Can_MsgObjConfig msgObjConfig;
        IfxMultican_Can_MsgObj_initConfig(&msgObjConfig, &canNode0);

        msgObjConfig.msgObjId              = 1;
        msgObjConfig.messageId             = CAN_MSG_ID_RX_LED;
        msgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;
        msgObjConfig.frame                 = IfxMultican_Frame_receive;
        msgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;
        msgObjConfig.control.extendedFrame = FALSE;
        msgObjConfig.control.matchingId    = TRUE;

        IfxMultican_Can_MsgObj_init(&canRxMsgObj, &msgObjConfig);
    }
}

void DrvCan_SendAdcValue(uint16 adcValue)
{
    IfxMultican_Message txMsg;
    IfxMultican_Message_init(&txMsg, CAN_MSG_ID_TX_ADC,
                             (uint32)adcValue, 0x00000000,
                             IfxMultican_DataLengthCode_8);

    /* Non-blocking: skip if bus is busy */
    IfxMultican_Can_MsgObj_sendMessage(&canTxMsgObj, &txMsg);
}

boolean DrvCan_ReceiveLedStatus(uint8 *ledStatus)
{
    if (IfxMultican_Can_MsgObj_isRxPending(&canRxMsgObj))
    {
        IfxMultican_Message rxMsg;
        IfxMultican_Message_init(&rxMsg, 0, 0, 0, IfxMultican_DataLengthCode_8);

        IfxMultican_Status readStatus = IfxMultican_Can_MsgObj_readMessage(&canRxMsgObj, &rxMsg);

        if (readStatus & IfxMultican_Status_newData)
        {
            *ledStatus = (uint8)(rxMsg.data[0] & 0xFFu);
            return TRUE;
        }
    }

    return FALSE;
}
