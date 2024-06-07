/*
 * PollingRoutine.c
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template for projects.
 *
 */


#include "main.h"


extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;

extern TimerCallbackStruct timerCallback;

CAN_MsgStruct can1_msg =
{
	.hcan = &hcan1,
	.rxQueueSize = CAN_RX_QUEUE_SIZE,
	.txQueueSize = CAN_TX_QUEUE_SIZE,
	.canBusActive = false
};

UartBufferStruct uart2 =
{
	.huart = &huart2,
	.rx.queueSize = UART_RX_QUEUE_SIZE,
	.tx.queueSize = UART_TX_QUEUE_SIZE,
	.rx.uartType = UART_ASCII,
	.rx.irqByte = 0
};

uint8_t gearInfoData = {0};
uint8_t pwerModeData = {0};

void PollingInit(void)
{
	CAN_SetFilter(&hcan1);

	UART_EnableRxInterrupt(&uart2);
}

void PollingRoutine(void)
{
	CAN1_Parse(&can1_msg);
	CAN_SendMessage(&can1_msg);

	UART_Parse(&uart2);
	UART_CheckRxIntError(&uart2);

	PowerMode();
}

void CAN1_Parse(CAN_MsgStruct *msg)
{
	CanRxMsgTypeDef *rxPtr;
	char str[128] = {0};

	if(CAN_DataAvailable(msg))
	{
		rxPtr = msg->msgToParse;
		switch(rxPtr->header.IDE)
		{
			case CAN_ID_STD:
				if(rxPtr->header.StdId == BRAKE_GEAR_INFO_ID) // Gear information
				{
					GearInformationSerice(rxPtr->data);
					sprintf(str, "ID=0x%lX data[2]=0x%X", rxPtr->header.StdId, rxPtr->data[2]);
					NotifyUser(&uart2, str, strlen(str), true);
				}
				else if(rxPtr->header.StdId == POWER_MODE_ID) // Gear information
				{
					PowerModeService(rxPtr->data);
					sprintf(str, "ID=0x%lX data[0]=0x%X", rxPtr->header.StdId, rxPtr->data[0]);
					NotifyUser(&uart2, str, strlen(str), true);
				}
			break;
			case CAN_ID_EXT:
				// for vehicles with Extended ID
			break;
		}
	}
}

void UART_Parse(UartBufferStruct *msg)
{
	int status = NO_ERROR;
	char *ptr;
	char msg_copy[64] = {0};
	char retStr[64] = "ok";

	if(UART_RxMessagePending(msg))
	{
		ptr = (char*)msg->rx.msgToParse->data;
		memcpy(&msg_copy, ptr, strlen(ptr) - 2); // remove CR/LF
		RemoveSpaces(ptr);
		ToLower(ptr);

		if(strncmp(ptr, "powermode:", strlen("powermode:")) == 0)
		{
			status = PowerModeParse(ptr);
		}
		else if(strncmp(ptr, "gearinfo:", strlen("gearinfo:")) == 0)
		{
			status = GearInfoParse(ptr);
		}
		else
		{
			status = COMMAND_UNKNOWN;
		}

		if(status == NO_ACK)
		{
			return;
		}
		else if(status != NO_ERROR)
		{
			NotifyUser_Error(&uart2, msg_copy, status);
		}
		else
		{
			NotifyUser_PrintReply(&uart2, msg_copy, retStr);
		}
	}
}

void PowerMode(void)
{
	uint8_t pmStatus;

	pmStatus = PowerModeGetStatus();

	if(pmStatus == POWER_MODE_ACCESSORY || pmStatus == POWER_MODE_IGNITION)
	{
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	}
}

int PowerModeParse(char *msg)
{
	CanTxMsgTypeDef txMsg = {0};

	msg += strlen("powermode:");

	if(strncmp(msg, "off", strlen("off")) == 0)
	{
		pwerModeData = POWER_MODE_OFF;
	}
	else if(strncmp(msg, "acc", strlen("acc")) == 0)
	{
		pwerModeData = POWER_MODE_ACCESSORY;
	}
	else if(strncmp(msg, "run", strlen("run")) == 0)
	{
		pwerModeData = POWER_MODE_IGNITION;
	}
	else if(strncmp(msg, "crank", strlen("crank")) == 0)
	{
		pwerModeData = POWER_MODE_CRANK;
	}
	else
	{
		return VALUE_OUT_OF_RANGE;
	}

	txMsg.header.IDE = CAN_ID_STD;
	txMsg.header.StdId = POWER_MODE_ID;
	txMsg.header.DLC = 8;
	txMsg.header.RTR = CAN_RTR_DATA;
	txMsg.data[0] = pwerModeData;
	CAN_AddTxBuffer(&can1_msg, &txMsg);

	return NO_ERROR;
}

int GearInfoParse(char *msg)
{
	CanTxMsgTypeDef txMsg = {0};

	msg += strlen("gearinfo:");

	if(strncmp(msg, "park", strlen("park")) == 0)
	{
		gearInfoData = GEAR_PARK;
	}
	else if(strncmp(msg, "reverse", strlen("reverse")) == 0)
	{
		gearInfoData = GEAR_REVERSE;
	}
	else if(strncmp(msg, "neutral", strlen("neutral")) == 0)
	{
		gearInfoData = GEAR_NUETRAL;
	}
	else if(strncmp(msg, "drive", strlen("drive")) == 0)
	{
		gearInfoData = GEAR_DRIVE;
	}
	else
	{
		return VALUE_OUT_OF_RANGE;
	}

	// gear info
	txMsg.header.IDE = CAN_ID_STD;
	txMsg.header.StdId = BRAKE_GEAR_INFO_ID;
	txMsg.header.DLC = 8;
	txMsg.header.RTR = CAN_RTR_DATA;
	txMsg.data[2] = gearInfoData;
	CAN_AddTxBuffer(&can1_msg, &txMsg);

	return NO_ERROR;
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == uart2.huart)
	{
		UART_AddByteToBuffer(&uart2);
		UART_EnableRxInterrupt(&uart2);
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == uart2.huart)
	{
		UART_TxMessage_IT(&uart2); // transmit more if queue is not empty
	}
}

