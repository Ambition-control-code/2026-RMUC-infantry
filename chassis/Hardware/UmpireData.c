#include "main.h"
#include "string.h"

#define Dma_BufferSize 136
#define FrameSize 136
#define FrameHeader_Length 5U
#define CMDID_Length 2U
#define CRC16_Length 2U

typedef struct
{
    u8 HC;
    u8 dizhi : 7;
    u8 jindu;
    u16 huan;
} shujiu;

static void Umpire_ResetFrameState(void);
static void Umpire_ParseFrame(void);
static void Umpire_RxData(uint8_t *pData, uint16_t len);

shujiu JISHU;
Robot_Para robot;
volatile uint16_t g_umpire_watchdog_cnt = 0;
u8 DMA_Buffer[128];
u8 Frame[FrameSize];
u8 DMA_TX_Buffer[Dma_BufferSize];
u8 DMA_RX_Buffer[Dma_BufferSize];
static u8 Umpire_Rx_Buf[2][Dma_BufferSize];

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;

u16 heceng16(u8 a, u8 b)
{
    u16 c = 0;
    c = ((c + a) << 8) + b;
    return c;
}

u32 heceng32(u8 a, u8 b, u8 c, u8 d)
{
    u32 chu = 0;
    u16 zhong1 = 0;
    u16 zhong2 = 0;

    zhong1 = heceng16(a, b);
    zhong2 = heceng16(c, d);
    chu = ((0x00000000 + zhong1) << 16) + zhong2;
    return chu;
}

u8 tiqv(u8 shuru, u8 wei, u8 shu)
{
    u8 shuchu;
    u8 weiyi;

    weiyi = 8 - wei + 1 - shu;
    shuchu = shuru << weiyi;
    shuchu = shuchu >> (8 - shu);
    return shuchu;
}

u16 cmd_id;
u16 data_length;
u8 a_ = 1;

volatile union zhuanhua
{
    float powerr;
    unsigned char g[4];
} ppp;

int16_t power_limit_hero;

static void Umpire_ResetFrameState(void)
{
    JISHU.HC = 0;
    JISHU.dizhi = 0;
    JISHU.jindu = 0;
    JISHU.huan = 0;
    a_ = 1;
    cmd_id = 0;
    data_length = 0;
    Frame[0] = 0;
}

static void Umpire_ParseFrame(void)
{
    cmd_id = heceng16(Frame[6], Frame[5]);
    if (!Verify_CRC16_Check_Sum(Frame, 5 + data_length + 4))
    {
        Umpire_ResetFrameState();
        return;
    }

    g_umpire_watchdog_cnt = 0;

    switch (cmd_id)
    {
    case 0x0001:
        break;

    case 0x0002:
        robot.winner = Frame[7];
        break;

    case 0x0003:
        robot.red_3_HP = heceng16(Frame[12], Frame[11]);
        robot.blue_3_HP = heceng16(Frame[28], Frame[27]);
        break;

    case 0x0004:
        break;

    case 0x0101:
        break;

    case 0x0102:
        break;

    case 0x0103:
        break;

    case 0x0104:
        break;

    case 0x0105:
        break;

    case 0x0201:
        robot.robot_id = Frame[7];
        robot.robot_level = Frame[8];
        robot.remain_HP = heceng16(Frame[10], Frame[9]);
        robot.max_HP = heceng16(Frame[12], Frame[11]);
        robot.shooter_id2_17mm_cooling_rate = heceng16(Frame[14], Frame[13]);
        robot.shooter_id1_17mm_cooling_limit = heceng16(Frame[16], Frame[15]);
        robot.chassis_power_limit = heceng16(Frame[18], Frame[17]);
        robot.mains_power_shooter_output = (Frame[19] & 0x04) >> 2;
        robot.mains_power_gimbal_output = Frame[19] & 0x01;
        robot.mains_power_chassis_output = (Frame[19] & 0x02) >> 1;
        break;

    case 0x0202:
        robot.classis_voit = heceng16(Frame[8], Frame[7]);
        robot.classis_current = heceng16(Frame[10], Frame[9]);
        ppp.g[0] = Frame[11];
        ppp.g[1] = Frame[12];
        ppp.g[2] = Frame[13];
        ppp.g[3] = Frame[14];
        robot.classis_power = ppp.powerr;
        robot.classis_power_buffer = heceng16(Frame[16], Frame[15]);
        robot.shooter_heat0 = heceng16(Frame[18], Frame[17]);
        robot.shooter_heat1 = heceng16(Frame[19], Frame[20]);
        robot.mobile_shooter_heat2 = heceng16(Frame[21], Frame[22]);
        break;

    case 0x0203:
        break;

    case 0x0204:
        break;

    case 0x0205:
        break;

    case 0x0206:
        break;

    case 0x0207:
        robot.bullet_type = Frame[7];
        robot.bullet_freq = Frame[9];
        ppp.g[0] = Frame[10];
        ppp.g[1] = Frame[11];
        ppp.g[2] = Frame[12];
        ppp.g[3] = Frame[13];
        robot.bullet_speed = ppp.powerr;
        break;

    case 0x0208:
        break;

    case 0x0209:
        robot.rfid_status = Frame[7];
        robot.fortress = (robot.rfid_status >> 24) & 0x01;
        break;

    case 0x020A:
        break;

    case 0x0301:
        break;

    default:
        break;
    }

    Umpire_ResetFrameState();
}

static void Umpire_RxData(uint8_t *pData, uint16_t len)
{
    uint16_t rx_index = 0;
    uint16_t frame_total_length = 0;

    if ((pData == NULL) || (len < FrameHeader_Length))
    {
        return;
    }

    if (len > Dma_BufferSize)
    {
        len = Dma_BufferSize;
    }

    JISHU.HC = (u8)len;
    JISHU.dizhi = 0;

    while ((rx_index + FrameHeader_Length) <= len)
    {
        JISHU.dizhi = rx_index;

        if (pData[rx_index] != 0xA5)
        {
            rx_index++;
            continue;
        }

        if (!Verify_CRC8_Check_Sum(&pData[rx_index], FrameHeader_Length))
        {
            rx_index++;
            continue;
        }

        data_length = heceng16(pData[rx_index + 2], pData[rx_index + 1]);
        frame_total_length = data_length + FrameHeader_Length + CMDID_Length + CRC16_Length;

        if ((frame_total_length > FrameSize) || ((rx_index + frame_total_length) > len))
        {
            break;
        }

        if (!Verify_CRC16_Check_Sum(&pData[rx_index], frame_total_length))
        {
            rx_index++;
            continue;
        }

        memcpy(Frame, &pData[rx_index], frame_total_length);
        JISHU.huan = frame_total_length;
        Umpire_ParseFrame();
        rx_index += frame_total_length;
    }
}

void Umpire_UsartInit(void)
{
    memset(Umpire_Rx_Buf, 0, sizeof(Umpire_Rx_Buf));
    memset(DMA_RX_Buffer, 0, sizeof(DMA_RX_Buffer));
    memset(Frame, 0, sizeof(Frame));
    Umpire_ResetFrameState();

    CLEAR_BIT(huart6.Instance->CR1, USART_CR1_IDLEIE);
    CLEAR_BIT(huart6.Instance->CR3, USART_CR3_DMAR);

    __HAL_DMA_DISABLE(&hdma_usart6_rx);
    while ((hdma_usart6_rx.Instance->CR & DMA_SxCR_EN) != RESET)
    {
    }

    DMA2->LIFCR = DMA_LIFCR_CFEIF1 | DMA_LIFCR_CDMEIF1 |
                  DMA_LIFCR_CTEIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTCIF1;

    hdma_usart6_rx.Instance->CR &=
        ~(DMA_SxCR_CT | DMA_SxCR_DBM | DMA_SxCR_HTIE | DMA_SxCR_TCIE |
          DMA_SxCR_TEIE | DMA_SxCR_DMEIE);
    hdma_usart6_rx.Instance->PAR = (uint32_t)&huart6.Instance->DR;
    hdma_usart6_rx.Instance->M0AR = (uint32_t)Umpire_Rx_Buf[0];
    hdma_usart6_rx.Instance->M1AR = (uint32_t)Umpire_Rx_Buf[1];
    hdma_usart6_rx.Instance->NDTR = Dma_BufferSize;
    hdma_usart6_rx.Instance->CR |= DMA_SxCR_DBM;

    __HAL_UART_CLEAR_PEFLAG(&huart6);
    SET_BIT(huart6.Instance->CR3, USART_CR3_DMAR);
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);

    HAL_NVIC_SetPriority(USART6_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);

    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

    __HAL_DMA_ENABLE(&hdma_usart6_rx);
}

void USART6_IRQHandler(void)
{
    uint16_t this_time_rx_len;
    uint8_t *rx_buf;

    if ((huart6.Instance->SR & UART_FLAG_IDLE) == RESET)
    {
        return;
    }

    __HAL_UART_CLEAR_PEFLAG(&huart6);

    __HAL_DMA_DISABLE(&hdma_usart6_rx);
    while ((hdma_usart6_rx.Instance->CR & DMA_SxCR_EN) != RESET)
    {
    }

    this_time_rx_len = Dma_BufferSize - hdma_usart6_rx.Instance->NDTR;

    if ((hdma_usart6_rx.Instance->CR & DMA_SxCR_CT) == RESET)
    {
        rx_buf = Umpire_Rx_Buf[0];
        hdma_usart6_rx.Instance->CR |= DMA_SxCR_CT;
    }
    else
    {
        rx_buf = Umpire_Rx_Buf[1];
        hdma_usart6_rx.Instance->CR &= ~DMA_SxCR_CT;
    }

    hdma_usart6_rx.Instance->NDTR = Dma_BufferSize;
    DMA2->LIFCR = DMA_LIFCR_CFEIF1 | DMA_LIFCR_CDMEIF1 |
                  DMA_LIFCR_CTEIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTCIF1;
    __HAL_DMA_ENABLE(&hdma_usart6_rx);

    if ((this_time_rx_len > 0) && (this_time_rx_len <= Dma_BufferSize))
    {
        memcpy(DMA_RX_Buffer, rx_buf, this_time_rx_len);
        Umpire_RxData(DMA_RX_Buffer, this_time_rx_len);
    }
}

void DMA_USART1_Send(uint8_t *data, uint16_t size)
{
    if ((data == NULL) || (size == 0))
    {
        return;
    }

    if (size > Dma_BufferSize)
    {
        size = Dma_BufferSize;
    }

    memcpy(DMA_TX_Buffer, data, size);
    HAL_UART_Transmit_IT(&huart1, DMA_TX_Buffer, size);
}
