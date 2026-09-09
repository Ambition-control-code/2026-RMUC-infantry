
#ifndef CAN_QUEUE_H
#define CAN_QUEUE_H

#include "can.h"

#define CAN_TX_QUEUE_SIZE 128u

typedef struct
{
	uint32_t StdId;
	uint8_t  DLC;
	uint8_t  Data[8];
} CAN_Tx_Msg_t;

uint8_t CAN_Queue_Push(uint32_t id, uint8_t *data);
uint8_t CAN_Queue_PushEx(CAN_HandleTypeDef *hcan, uint32_t id, uint8_t *data, uint8_t dlc);
void CAN_Process_Next_Send(CAN_HandleTypeDef *hcan);

#endif // CAN_QUEUE_H
