#include "bsp_flash.h"
#include "main.h"
#include "string.h"

#define OFFSET_LENGTH sizeof(OFFSET_Buffer)
#define SIZE (OFFSET_LENGTH / 4 + ((OFFSET_LENGTH%4)?1:0))
#define FLASH_SAVE_ADDR  0x08060000

/**
  * @brief          get the sector number of flash
  * @param[in]      address: flash address
  * @retval         sector number
  */
/**
  * @brief          »ñÈ¡flashµÄsectorºÅ
  * @param[in]      address: flash µØÖ·
  * @retval         sectorºÅ
  */
uint32_t ger_sector(uint32_t address);

/**
  * @brief          erase flash
  * @param[in]      address: flash address
  * @param[in]      len: page num
  * @retval         none
  */
/**
  * @brief          ²Á³ýflash
  * @param[in]      address: flash µØÖ·
  * @param[in]      len: Ò³ÊýÁ¿
  * @retval         none
  */
void flash_erase_address(uint32_t address, uint16_t len)
{
    FLASH_EraseInitTypeDef flash_erase;
    uint32_t error;

    flash_erase.Sector = ger_sector(address);
    flash_erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    flash_erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    flash_erase.NbSectors = len;

    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&flash_erase, &error);
    HAL_FLASH_Lock();
}

/**
  * @brief          write data to one page of flash
  * @param[in]      start_address: flash address
  * @param[in]      buf: data point
  * @param[in]      len: data num
  * @retval         success 0, fail -1
  */
/**
  * @brief          ÍùÒ»Ò³flashÐ´Êý¾Ý
  * @param[in]      start_address: flash µØÖ·
  * @param[in]      buf: Êý¾ÝÖ¸Õë
  * @param[in]      len: Êý¾Ý³¤¶È
  * @retval         success 0, fail -1
  */
int8_t flash_write_single_address(uint32_t start_address, uint32_t *buf, uint32_t len)
{
    static uint32_t uw_address;
    static uint32_t end_address;
    static uint32_t *data_buf;
    static uint32_t data_len;

    HAL_FLASH_Unlock();

    uw_address = start_address;
    end_address = get_next_flash_address(start_address);
    data_buf = buf;
    data_len = 0;

    while (uw_address <= end_address)
    {

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,uw_address, *data_buf) == HAL_OK)
        {
            uw_address += 4;
            data_buf++;
            data_len++;
            if (data_len == len)
            {
                break;
            }
        }
        else
        {
            HAL_FLASH_Lock();
            return -1;
        }
    }

    HAL_FLASH_Lock();
    return 0;
}


/**
  * @brief          write data to some pages of flash
  * @param[in]      start_address: flash start address
  * @param[in]      end_address: flash end address
  * @param[in]      buf: data point
  * @param[in]      len: data num
  * @retval         success 0, fail -1
  */
/**
  * @brief          Íù¼¸Ò³flashÐ´Êý¾Ý
  * @param[in]      start_address: flash ¿ªÊ¼µØÖ·
  * @param[in]      end_address: flash ½áÊøµØÖ·
  * @param[in]      buf: Êý¾ÝÖ¸Õë
  * @param[in]      len: Êý¾Ý³¤¶È
  * @retval         success 0, fail -1
  */
int8_t flash_write_muli_address(uint32_t start_address, uint32_t end_address, uint32_t *buf, uint32_t len)
{
    uint32_t uw_address = 0;
    uint32_t *data_buf;
    uint32_t data_len;

    HAL_FLASH_Unlock();

    uw_address = start_address;
    data_buf = buf;
    data_len = 0;
    while (uw_address <= end_address)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,uw_address, *data_buf) == HAL_OK)
        {
            uw_address += 4;
            data_buf++;
            data_len++;
            if (data_len == len)
            {
                break;
            }
        }
        else
        {
            HAL_FLASH_Lock();
            return -1;
        }
    }

    HAL_FLASH_Lock(); 
    return 0;
}

/**
  * @brief          read data for flash
  * @param[in]      address: flash address
  * @param[out]     buf: data point
  * @param[in]      len: data num
  * @retval         none
  */
/**
  * @brief          ´Óflash¶ÁÊý¾Ý
  * @param[in]      start_address: flash µØÖ·
  * @param[out]     buf: Êý¾ÝÖ¸Õë
  * @param[in]      len: Êý¾Ý³¤¶È
  * @retval         none
  */

/**
  * @brief          get the sector number of flash
  * @param[in]      address: flash address
  * @retval         sector number
  */
/**
  * @brief          »ñÈ¡flashµÄsectorºÅ
  * @param[in]      address: flash µØÖ·
  * @retval         sectorºÅ
  */
uint32_t ger_sector(uint32_t address)
{
    uint32_t sector = 0;
    if ((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_SECTOR_0;
    }
    else if ((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_SECTOR_1;
    }
    else if ((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_SECTOR_2;
    }
    else if ((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_SECTOR_3;
    }
    else if ((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_SECTOR_4;
    }
    else if ((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_SECTOR_5;
    }
    else if ((address < ADDR_FLASH_SECTOR_7) && (address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_SECTOR_6;
    }
    else if ((address < ADDR_FLASH_SECTOR_8) && (address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_SECTOR_7;
    }
    else if ((address < ADDR_FLASH_SECTOR_9) && (address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_SECTOR_8;
    }
    else if ((address < ADDR_FLASH_SECTOR_10) && (address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_SECTOR_9;
    }
    else if ((address < ADDR_FLASH_SECTOR_11) && (address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_SECTOR_10;
    }
    else if ((address < ADDR_FLASH_SECTOR_12) && (address >= ADDR_FLASH_SECTOR_11))
    {
        sector = FLASH_SECTOR_11;
    }
    else
    {
        sector = FLASH_SECTOR_11;
    }

    return sector;
}

/**
  * @brief          get the next page flash address
  * @param[in]      address: flash address
  * @retval         next page flash address
  */
/**
  * @brief          »ñÈ¡ÏÂÒ»Ò³flashµØÖ·
  * @param[in]      address: flash µØÖ·
  * @retval         ÏÂÒ»Ò³flashµØÖ·
  */
uint32_t get_next_flash_address(uint32_t address)
{
    uint32_t sector = 0;

    if ((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
    {
        sector = ADDR_FLASH_SECTOR_1;
    }
    else if ((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1))
    {
        sector = ADDR_FLASH_SECTOR_2;
    }
    else if ((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2))
    {
        sector = ADDR_FLASH_SECTOR_3;
    }
    else if ((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3))
    {
        sector = ADDR_FLASH_SECTOR_4;
    }
    else if ((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4))
    {
        sector = ADDR_FLASH_SECTOR_5;
    }
    else if ((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5))
    {
        sector = ADDR_FLASH_SECTOR_6;
    }
    else if ((address < ADDR_FLASH_SECTOR_7) && (address >= ADDR_FLASH_SECTOR_6))
    {
        sector = ADDR_FLASH_SECTOR_7;
    }
    else if ((address < ADDR_FLASH_SECTOR_8) && (address >= ADDR_FLASH_SECTOR_7))
    {
        sector = ADDR_FLASH_SECTOR_8;
    }
    else if ((address < ADDR_FLASH_SECTOR_9) && (address >= ADDR_FLASH_SECTOR_8))
    {
        sector = ADDR_FLASH_SECTOR_9;
    }
    else if ((address < ADDR_FLASH_SECTOR_10) && (address >= ADDR_FLASH_SECTOR_9))
    {
        sector = ADDR_FLASH_SECTOR_10;
    }
    else if ((address < ADDR_FLASH_SECTOR_11) && (address >= ADDR_FLASH_SECTOR_10))
    {
        sector = ADDR_FLASH_SECTOR_11;
    }
    else /*(address < FLASH_END_ADDR) && (address >= ADDR_FLASH_SECTOR_23))*/
    {
        sector = FLASH_END_ADDR;
    }
    return sector;
}


///////Ò»ÄîÍ¨Ìì  ÉñÄ§ÎÞ¾å\\\\\\///////Ò»ÄîÍ¨Ìì  ÉñÄ§ÎÞ¾å\\\\\\///////Ò»ÄîÍ¨Ìì  ÉñÄ§ÎÞ¾å\\\\\\//////Ò»ÄîÍ¨Ìì  ÉñÄ§ÎÞ¾å\\\\\\

//¶ÁÈ¡Ö¸¶¨µØÖ·µÄ×Ö(32Î»Êý¾Ý) 
//faddr:¶ÁµØÖ· 
//·µ»ØÖµ:¶ÔÓ¦Êý¾Ý.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}

//»ñÈ¡Ä³¸öµØÖ·ËùÔÚµÄflashÉÈÇø
//addr:flashµØÖ·
//·µ»ØÖµ:0~11,¼´addrËùÔÚµÄÉÈÇø
u8 STMFLASH_GetFlashSector(u32 addr)
{
    if (addr < ADDR_FLASH_SECTOR_1)
        return FLASH_SECTOR_0;
    else if (addr < ADDR_FLASH_SECTOR_2)
        return FLASH_SECTOR_1;
    else if (addr < ADDR_FLASH_SECTOR_3)
        return FLASH_SECTOR_2;
    else if (addr < ADDR_FLASH_SECTOR_4)
        return FLASH_SECTOR_3;
    else if (addr < ADDR_FLASH_SECTOR_5)
        return FLASH_SECTOR_4;
    else if (addr < ADDR_FLASH_SECTOR_6)
        return FLASH_SECTOR_5;
    else if (addr < ADDR_FLASH_SECTOR_7)
        return FLASH_SECTOR_6;
    return FLASH_SECTOR_7;

}
///////Ò»ÄîÍ¨Ìì  ÉñÄ§ÎÞ¾å\\\\\\///////Ò»ÄîÍ¨Ìì  ÉñÄ§ÎÞ¾å\\\\\\///////Ò»ÄîÍ¨Ìì  ÉñÄ§ÎÞ¾å\\\\\\//////Ò»ÄîÍ¨Ìì  ÉñÄ§ÎÞ¾å\\\\\\

//´ÓÖ¸¶¨µØÖ·¿ªÊ¼Ð´ÈëÖ¸¶¨³¤¶ÈµÄÊý¾Ý
//ÌØ±ð×¢Òâ:ÒòÎªSTM32F4µÄÉÈÇøÊµÔÚÌ«´ó,Ã»°ì·¨±¾µØ±£´æÉÈÇøÊý¾Ý,ËùÒÔ±¾º¯Êý
//         Ð´µØÖ·Èç¹û·Ç0XFF,ÄÇÃ´»áÏÈ²Á³ýÕû¸öÉÈÇøÇÒ²»±£´æÉÈÇøÊý¾Ý.ËùÒÔ
//         Ð´·Ç0XFFµÄµØÖ·,½«µ¼ÖÂÕû¸öÉÈÇøÊý¾Ý¶ªÊ§.½¨ÒéÐ´Ö®Ç°È·±£ÉÈÇøÀï
//         Ã»ÓÐÖØÒªÊý¾Ý,×îºÃÊÇÕû¸öÉÈÇøÏÈ²Á³ýÁË,È»ºóÂýÂýÍùºóÐ´. 
//¸Ãº¯Êý¶ÔOTPÇøÓòÒ²ÓÐÐ§!¿ÉÒÔÓÃÀ´Ð´OTPÇø!
//OTPÇøÓòµØÖ··¶Î§:0X1FFF7800~0X1FFF7A0F(×¢Òâ£º×îºó16×Ö½Ú£¬ÓÃÓÚOTPÊý¾Ý¿éËø¶¨£¬±ðÂÒÐ´£¡£¡)
//WriteAddr:ÆðÊ¼µØÖ·(´ËµØÖ·±ØÐëÎª4µÄ±¶Êý!!)
//pBuffer:Êý¾ÝÖ¸Õë
//NumToWrite:×Ö(32Î»)Êý(¾ÍÊÇÒªÐ´ÈëµÄ32Î»Êý¾ÝµÄ¸öÊý.) 
uint16_t Man,WCIS;
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	u32 SectorError=0;
	u32 addrx=0;
	u32 endaddr=0;	
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//·Ç·¨µØÖ·
    
	HAL_FLASH_Unlock();             //½âËø	
	addrx=WriteAddr;				//Ð´ÈëµÄÆðÊ¼µØÖ·
	endaddr=WriteAddr+NumToWrite*4;	//Ð´ÈëµÄ½áÊøµØÖ·
    
	if(addrx<0X1FFF0000)
	{
		while(addrx<endaddr)		//É¨ÇåÒ»ÇÐÕÏ°­.(¶Ô·ÇFFFFFFFFµÄµØ·½,ÏÈ²Á³ý)
		{
			 if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//ÓÐ·Ç0XFFFFFFFFµÄµØ·½,Òª²Á³ýÕâ¸öÉÈÇø
			{   
				FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //²Á³ýÀàÐÍ£¬ÉÈÇø²Á³ý 
				FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //Òª²Á³ýµÄÉÈÇø
				FlashEraseInit.NbSectors=1;                             //Ò»´ÎÖ»²Á³ýÒ»¸öÉÈÇø
				FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //µçÑ¹·¶Î§£¬  VCC=2.7~3.6VÖ®¼ä!!
				flash_erase_address(addrx,SIZE);
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
				{
					break;//·¢Éú´íÎóÁË	
				}
				}else addrx+=4;
				FLASH_WaitForLastOperation(FLASH_WAITETIME);                //µÈ´ýÉÏ´Î²Ù×÷Íê³É
		}
	}
	FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //µÈ´ýÉÏ´Î²Ù×÷Íê³É
	if(FlashStatus==HAL_OK)
	{
		 while(WriteAddr<endaddr)//Ð´Êý¾Ý
		 {
			 Man++;
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//Ð´ÈëÊý¾Ý
			{ 
				WCIS++;
				break;	//Ð´ÈëÒì³£
			}
			WriteAddr+=4;
			pBuffer++;
		}  
	}
	HAL_FLASH_Lock();           //ÉÏËø
} 

//´ÓÖ¸¶¨µØÖ·¿ªÊ¼¶Á³öÖ¸¶¨³¤¶ÈµÄÊý¾Ý
//ReadAddr:ÆðÊ¼µØÖ·
//pBuffer:Êý¾ÝÖ¸Õë
//NumToRead:×Ö(32Î»)Êý
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//¶ÁÈ¡4¸ö×Ö½Ú.
		ReadAddr+=4;//Æ«ÒÆ4¸ö×Ö½Ú.	
	}
}




//////////////////////////////////////////²âÊÔÓÃ///////////////////////////////////////////
//WriteAddr:ÆðÊ¼µØÖ·
//WriteData:ÒªÐ´ÈëµÄÊý¾Ý
void Test_Write(u32 WriteAddr,u32 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//Ð´ÈëÒ»¸ö×Ö 
}
