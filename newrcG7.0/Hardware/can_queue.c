#include "can_queue.h"

#include <string.h>

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

#if (CAN_TX_QUEUE_SIZE == 0u)
#error "CAN_TX_QUEUE_SIZE must be greater than 0"
#endif

#if ((CAN_TX_QUEUE_SIZE & (CAN_TX_QUEUE_SIZE - 1u)) != 0u)
#error "CAN_TX_QUEUE_SIZE must be a power of two"
#endif

#define CAN_TX_QUEUE_MASK (CAN_TX_QUEUE_SIZE - 1u)

static CAN_Tx_Msg_t tx_buffer[CAN_TX_QUEUE_SIZE];
static volatile uint16_t tx_head;
static volatile uint16_t tx_tail;
static volatile uint8_t tx_busy;
static volatile uint8_t tx_need_kick;

static CAN_Tx_Msg_t tx_buffer_can2[CAN_TX_QUEUE_SIZE];
static volatile uint16_t tx2_head;
static volatile uint16_t tx2_tail;
static volatile uint8_t tx2_busy;
static volatile uint8_t tx2_need_kick;

typedef struct
{
	CAN_Tx_Msg_t *buf;
	volatile uint16_t *head;
	volatile uint16_t *tail;
	volatile uint8_t *busy;
	volatile uint8_t *need_kick;
} canq_ctx_t;

static inline uint32_t canq_enter_critical(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

static inline void canq_exit_critical(uint32_t primask)
{
	__set_PRIMASK(primask);
}

static inline canq_ctx_t canq_get_ctx(CAN_HandleTypeDef *hcan)
{
	if (hcan != NULL && hcan->Instance == CAN2)
	{
		canq_ctx_t ctx = {tx_buffer_can2, &tx2_head, &tx2_tail, &tx2_busy, &tx2_need_kick};
		return ctx;
	}
	else
	{
		canq_ctx_t ctx = {tx_buffer, &tx_head, &tx_tail, &tx_busy, &tx_need_kick};
		return ctx;
	}
}

uint8_t CAN_Queue_Push(uint32_t id, uint8_t *data)
{
	return CAN_Queue_PushEx(&hcan1, id, data, 8u);
}

uint8_t CAN_Queue_PushEx(CAN_HandleTypeDef *hcan, uint32_t id, uint8_t *data, uint8_t dlc)
{
	if (hcan == NULL)
	{
		return 0u;
	}

	if (data == NULL)
	{
		return 0u;
	}

	if (dlc > 8u)
	{
		dlc = 8u;
	}

	canq_ctx_t ctx = canq_get_ctx(hcan);
	uint32_t primask = canq_enter_critical();
	uint16_t head = *(ctx.head);
	uint16_t tail = *(ctx.tail);
	uint16_t next_head = (uint16_t)((head + 1u) & CAN_TX_QUEUE_MASK);
	if (next_head == tail)
	{
		canq_exit_critical(primask);
		return 0u;
	}

	ctx.buf[head].StdId = id;
	ctx.buf[head].DLC = dlc;
	memset(ctx.buf[head].Data, 0, sizeof(ctx.buf[head].Data));
	memcpy(ctx.buf[head].Data, data, dlc);
	*(ctx.head) = next_head;
	canq_exit_critical(primask);

	/* Always kick TX after enqueue so a previous send failure cannot stall
	   a non-empty queue forever waiting for a "was_empty" transition. */
	CAN_Process_Next_Send(hcan);

	return 1u;
}

void CAN_Process_Next_Send(CAN_HandleTypeDef *hcan)
{
	if (hcan == NULL)
	{
		return;
	}

	canq_ctx_t ctx = canq_get_ctx(hcan);
	uint32_t primask = canq_enter_critical();
	if (*(ctx.busy) != 0u)
	{
		*(ctx.need_kick) = 1u;
		canq_exit_critical(primask);
		return;
	}
	*(ctx.busy) = 1u;
	canq_exit_critical(primask);

	for (;;)
	{
		while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) > 0u)
		{
			CAN_Tx_Msg_t msg;
			uint16_t tail_prev;
			primask = canq_enter_critical();
			uint16_t head = *(ctx.head);
			uint16_t tail = *(ctx.tail);
			if (head == tail)
			{
				canq_exit_critical(primask);
				break;
			}

			tail_prev = tail;
			msg = ctx.buf[tail];
			*(ctx.tail) = (uint16_t)((tail + 1u) & CAN_TX_QUEUE_MASK);
			canq_exit_critical(primask);

			CAN_TxHeaderTypeDef tx_header;
			tx_header.StdId = msg.StdId;
			tx_header.ExtId = 0u;
			tx_header.IDE = CAN_ID_STD;
			tx_header.RTR = CAN_RTR_DATA;
			tx_header.DLC = msg.DLC;
			tx_header.TransmitGlobalTime = DISABLE;

			uint32_t mailbox;
			if (HAL_CAN_AddTxMessage(hcan, &tx_header, msg.Data, &mailbox) != HAL_OK)
			{
				primask = canq_enter_critical();
				*(ctx.tail) = tail_prev;
				canq_exit_critical(primask);
				break;
			}
		}

		primask = canq_enter_critical();
		if (*(ctx.need_kick) != 0u)
		{
			*(ctx.need_kick) = 0u;
			canq_exit_critical(primask);
			continue;
		}

		*(ctx.busy) = 0u;
		canq_exit_critical(primask);
		return;
	}
}
