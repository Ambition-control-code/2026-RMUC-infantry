#include "main.h"
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);//CRC8校验
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);        //添加CRC8校验码
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);             //CRC16校验
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);                 //添加CRC16校验码

