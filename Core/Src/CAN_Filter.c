
#include "main.h"
#include "CAN_Filter.h"


extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;


/*
function: Set CAN filter to pass all ID's
input: hcan instance
output: none
*/
void CAN_SetFilter(CAN_HandleTypeDef *hcan)
{
	static CAN_FilterTypeDef sFilterConfig;

	if(hcan == &hcan1)
	{
		sFilterConfig.FilterBank = 0;
	}
#ifdef HCAN2
	else if(hcan == &hcan2)
	{
		sFilterConfig.FilterBank = 14;
	}
#endif

	sFilterConfig.FilterMode             = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale            = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh           = 0x0000;
	sFilterConfig.FilterIdLow            = 0x0000;
	sFilterConfig.FilterMaskIdHigh       = 0x0000;
	sFilterConfig.FilterMaskIdLow        = 0x0000;
	sFilterConfig.FilterFIFOAssignment   = CAN_FILTER_FIFO0;
	sFilterConfig.FilterActivation       = ENABLE;

	if(HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_CAN_Start(hcan) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY ) != HAL_OK) // enables CAN notification.
	{
		Error_Handler();
	}
}





