#include "main.h"
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);//CRC8У��
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);        //����CRC8У����
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);             //CRC16У��
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);                 //����CRC16У����
