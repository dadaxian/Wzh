#include "Magnetometer.h"
#include "I2C.h"
#include "math.h"

u8 buff[8];
int x = 0;
int y = 0;
int z = 0;
int angle = 0;

//void Single_Write_HMC5883(u8 REG_Address, u8 REG_data)
//{
//	//HMC5883_Start();                  //起始信号
//	//HMC5883_SendByte(SlaveAddress);   //发送设备地址+写信号
//	//HMC5883_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf 
//	//HMC5883_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf
//	//HMC5883_Stop();                   //发送停止信号
//
//}
//
//u8 Single_Read_HMC5883(u8 REG_Address)
//{
//	Start();
//}
//
//void Multiple_Read_HMC5883()
//{
//
//}

void getDir()
{
	I2C_ReadSomeDataFromSlave(SlaveAddress, REG_Address, 6, buff);

	x = buff[0] << 8 | buff[1]; //Combine MSB and LSB of X Data output register
	z = buff[2] << 8 | buff[3]; //Combine MSB and LSB of Z Data output register
	y = buff[4] << 8 | buff[5]; //Combine MSB and LSB of Y Data output register
	angle = atan2((double)y, (double)x) * (180 / 3.14159265) + 180; // angle in degrees

	delay_ms(1);
}
