#ifndef __MAGNETOMETER_H
#define __MAGNETOMETER_H
#include "stm32f4xx.h"
#include "common.h"

#define	SlaveAddress   0x3C	  //����������IIC�����еĴӵ�ַ
#define REG_Address    0x03   //�ڲ��Ĵ�����ַ����ο�����pdf 
							//��������HMC5883�ڲ��Ƕ����ݣ���ַ��Χ0x3~0x5

extern u8 buff[8];
extern int x;
extern int y;
extern int z;
extern int angle;

//void  Single_Write_HMC5883(u8 REG_Address, u8 REG_data);   //����д������
//u8 Single_Read_HMC5883(u8 REG_Address);                   //������ȡ�ڲ��Ĵ�������
//void  Multiple_Read_HMC5883();                                  //�����Ķ�ȡ�ڲ��Ĵ�������

void getDir();

#endif