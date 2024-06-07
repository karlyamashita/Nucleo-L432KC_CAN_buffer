/*
 * CAN_Handler.c
 *
 *  Created on: Jan 7, 2023
 *      Author: codek
 */


#include "main.h"
#include "CAN_Handler.h"


extern CAN_HandleTypeDef hcan1;
extern CAN_MsgStruct can1_msg;


/*
Description: Callback when message is pending. Get message by passing buffer address. Then increment buffer pointer.
input: CAN instance
output: none
*/

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CanRxMsgTypeDef *ptr;

	if(hcan == &hcan1)
	{
		ptr = &can1_msg.rxQueue[can1_msg.rxPtr.index_IN];
		can1_msg.canBusActive = 1; // indicate msg received. You can use a timer to put device to sleep if this flag is not updated
		HAL_CAN_GetRxMessage(hcan, (uint32_t) CAN_RX_FIFO0, &ptr->header, ptr->data); // Get CAN message and save to queue.
		RingBuff_Ptr_Input(&can1_msg.rxPtr, can1_msg.rxQueueSize); // increment queue pointer
	}
}

// The 3 Tx mailboxes callbacks.
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan == &hcan1)
	{
		CAN_SendMessage(&can1_msg); // Call to send more Tx messages in queue
	}
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan == &hcan1)
	{
		CAN_SendMessage(&can1_msg);
	}
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan == &hcan1)
	{
		CAN_SendMessage(&can1_msg);
	}
}
