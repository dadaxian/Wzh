#ifndef __BSP_FLASH_H  
#define __BSP_FLASH_H  
#include "../../Driver/stm32f4xx.h"  
  
#define FIRST_ADDRESS 0x08010020
#define TREASURE_FIRST_ADDRESS 0x08011020
#define TRAVEL_FIRST_ADDRESS 0x08021020
//#define TRAVEL_FIRST_ADDRESS 0x08012020



/* Base address of the Flash sectors */
 #define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Sector 0, 16 Kbytes */
 #define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Sector 1, 16 Kbytes */
 #define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Sector 2, 16 Kbytes */
 #define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Sector 3, 16 Kbytes */
 #define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Sector 4, 64 Kbytes */
 #define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Sector 5, 128 Kbytes */
 #define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Sector 6, 128 Kbytes */
 #define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Sector 7, 128 Kbytes */
 #define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Sector 8, 128 Kbytes */
 #define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Sector 9, 128 Kbytes */
 #define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Sector 10, 128 Kbytes */
 #define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Sector 11, 128 Kbytes */

typedef enum {  
    FLASH_WRITE_OK = 1,  
    FLASH_WRITE_NO = 0,  
    FLASH_READ_OK = 1,  
    FLASH_READ_NO = 0  
}FLASH_FLAG ;  
extern  uint32_t GetSector(uint32_t Address);

unsigned char FLASH_Clear(u32 add);

extern int FlashDataCount;
extern unsigned char FLASH_WRITE( int * memory_data,int length);
unsigned char FLASH_WRITE_ONEDATA(int memory_data, int offset, u32 add);
extern unsigned char FLASH_READ(int * memory_data,unsigned short int n);

unsigned short FLASH_READ_ONEDATA(u8 offset, u32 address);
  
#endif /* __LED_H */ 
