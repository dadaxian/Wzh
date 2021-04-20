#ifndef __MAGNETOMETER_H
#define __MAGNETOMETER_H
#include "stm32f4xx.h"
#include "common.h"

#define	SlaveAddress   0x3C	  //定义器件在IIC总线中的从地址
#define REG_Address    0x03   //内部寄存器地址，请参考中文pdf 
							//连续读出HMC5883内部角度数据，地址范围0x3~0x5

extern u8 buff[8];
extern int x;
extern int y;
extern int z;
extern int angle;

//void  Single_Write_HMC5883(u8 REG_Address, u8 REG_data);   //单个写入数据
//u8 Single_Read_HMC5883(u8 REG_Address);                   //单个读取内部寄存器数据
//void  Multiple_Read_HMC5883();                                  //连续的读取内部寄存器数据

void getDir();

#endif