#include "vofa.h"
#include "usart.h"

typedef union {
    float vf;
    uint8_t vu8[4];
} typedefvofa;

// 包含 4 个数据通道和 1 个帧尾通道
static typedefvofa sendarray[5];

void vofa_transmit(float v1, float v2, float v3, float v4)
{
    HAL_StatusTypeDef status;

    if (huart1.gState != HAL_UART_STATE_READY) {
        return;
    }

    // 配置 JustFloat 帧尾: 0x00 0x00 0x80 0x7F
    sendarray[4].vu8[0] = 0x00;
    sendarray[4].vu8[1] = 0x00;
    sendarray[4].vu8[2] = 0x80;
    sendarray[4].vu8[3] = 0x7f;

    // 填充数据
    sendarray[0].vf = v1;
    sendarray[1].vf = v2;
    sendarray[2].vf = v3;
    sendarray[3].vf = v4;

    status = HAL_UART_Transmit_DMA(&huart1, (uint8_t *)sendarray, sizeof(sendarray));
    if (status != HAL_OK) {
        return;
    }
}
