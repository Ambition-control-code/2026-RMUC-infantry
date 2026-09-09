#include "new_rc.h"
#include "usart.h"
#include "struct_typedef.h"
#include "string.h"
uint8_t VT13_recieve_1[42],VT13_recieve_2[42];
void USART_RxDMA_MultiBufferStart(UART_HandleTypeDef *huart, uint32_t *SrcAddress, uint32_t *DstAddress, uint32_t *SecondMemAddress, uint32_t DataLength)
{	
  /* configuare the huart Reception Type TOIDLE */
   huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;

  /* configuare the huart Receicve Size */
   huart->RxXferSize = DataLength;
	
  /* Enable the DMA transfer for the receiver request */
  SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

  /* Enalbe IDLE interrupt */
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

  /* Disable DMA */
  do{
      __HAL_DMA_DISABLE(huart->hdmarx);
  }while(huart->hdmarx->Instance->CR & DMA_SxCR_EN);

  /* Configure the source memory Buffer address  */
  huart->hdmarx->Instance->PAR = (uint32_t)SrcAddress;

  /* Configure the destination memory Buffer address */
  huart->hdmarx->Instance->M0AR = (uint32_t)DstAddress;

  /* Configure DMA Stream destination address */
  huart->hdmarx->Instance->M1AR = (uint32_t)SecondMemAddress;

  /* Configure the length of data to be transferred from source to destination */
  huart->hdmarx->Instance->NDTR = DataLength;

  /* Enable double memory buffer */
  SET_BIT(huart->hdmarx->Instance->CR, DMA_SxCR_DBM);

  /* Enable DMA */
  __HAL_DMA_ENABLE(huart->hdmarx);
}




VT13_USE_t VT13_USE;
uint16_t new_live_3;
void USER_USART6_RxHandler(UART_HandleTypeDef *huart,uint16_t Size)
{
	new_live_3=500U;
  if(((((DMA_Stream_TypeDef  *)huart->hdmarx->Instance)->CR) & DMA_SxCR_CT ) == RESET)
  {
			__HAL_DMA_DISABLE(huart->hdmarx);

			huart->hdmarx->Instance->CR |= DMA_SxCR_CT;

		  if((VT13_recieve_1[0]==0xA9)&&(VT13_recieve_1[1]==0x53))
		  memcpy(VT13_USE.temp_data,VT13_recieve_1,21);
      __HAL_DMA_SET_COUNTER(huart->hdmarx,21*2);

  }
  else
  {
			__HAL_DMA_DISABLE(huart->hdmarx);

			huart->hdmarx->Instance->CR &= ~(DMA_SxCR_CT);
		
		  if((VT13_recieve_2[0]==0xA9)&&(VT13_recieve_2[1]==0x53))
		  memcpy(VT13_USE.temp_data,VT13_recieve_2,21);
      __HAL_DMA_SET_COUNTER(huart->hdmarx,21*2);

  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size)
{
if(huart->Instance == USART6)
	{		
		USER_USART6_RxHandler(huart,Size);	
	}

  /* reset the Reception Type */
	huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
	
  /* Enalbe IDLE interrupt */
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
	
  /* Enable the DMA transfer for the receiver request */
  SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
	
  /* Enable DMA */
  __HAL_DMA_ENABLE(huart->hdmarx);	
}

void tele_link_init(void)
{
	USART_RxDMA_MultiBufferStart(&huart6,(uint32_t *)&(huart6.Instance->DR),(uint32_t *)&VT13_recieve_1[0],(uint32_t *)&VT13_recieve_2[0],42U);
}

