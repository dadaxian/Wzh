#include "lcd.h"
#include "cfont.h"    
#include "preProcesser.h"
/*********************************************************************************
**********************MCU启明 STM32F407应用开发板(高配版)*************************
**********************************************************************************
* 文件名称: lcd.c                                                                *
* 文件简述：LCD显示驱动程序                                                      *
* 创建日期：2015.10.08                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：                                                                     *
**********************************************************************************
*********************************************************************************/

//初始化LCD的画笔颜色和背景色	   
u16 BRUSH_COLOR = BLACK;	//画笔颜色
u16 BACK_COLOR = WHITE;  //背景色 

//管理LCD驱动重要参数
u16  lcd_id;          //LCD ID
u8   dir_flag;        //横屏与竖屏控制：0，竖屏；1，横屏
u16  lcd_width;       //LCD的宽度
u16  lcd_height;      //LCD的高度
u16	 write_gramcmd;		//写gram指令
u16  setxcmd;		      //设置x坐标指令
u16  setycmd;		      //设置y坐标指令	

/****************************************************************************
* 名    称: void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value)
* 功    能：LCD写寄存器
* 入口参数：LCD_Reg: 寄存器地址
*           LCD_RegValue: 要写入的数据
* 返回参数：无
* 说    明：
****************************************************************************/
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value)
{
	LCD_CMD = LCD_Reg;		 //写入要写的寄存器序号	 
	LCD_DATA = LCD_Value;  //向寄存器写入的数据	    		 
}
/****************************************************************************
* 名    称: u16 LCD_ReadReg(u16 LCD_Reg)
* 功    能：LCD读寄存器
* 入口参数：LCD_Reg:寄存器地址
* 返回参数：读到该寄存器序号里的值
* 说    明：
****************************************************************************/
u16 LCD_ReadReg(u16 LCD_Reg)
{
	LCD_CMD = LCD_Reg;		//写入要读的寄存器序号
	delay_us(5);
	return LCD_DATA;		//返回读到的值
}
//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_CMD = write_gramcmd;
}
//lcd延时函数
void lcdm_delay(u8 i)
{
	while (i--);
}
/****************************************************************************
* 名    称: u16 LCD_BGRtoRGB(u16 bgr)
* 功    能：通过该函数将GBR转换RGB
* 入口参数：bgr:GBR格式的颜色值
* 返回参数：RGB格式的颜色值
* 说    明：从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式
****************************************************************************/
u16 LCD_BGRtoRGB(u16 bgr)
{
	u16  r, g, b, rgb;
	b = (bgr >> 0) & 0x1f;
	g = (bgr >> 5) & 0x3f;
	r = (bgr >> 11) & 0x1f;
	rgb = (b << 11) + (g << 5) + (r << 0);
	return(rgb);
}
/****************************************************************************
* 名    称: u16 LCD_GetPoint(u16 x,u16 y)
* 功    能：读取某点的颜色值
* 入口参数：x：x坐标
			y：y坐标
* 返回参数：此点的颜色
* 说    明：
****************************************************************************/
u16 LCD_GetPoint(u16 x, u16 y)
{
	vu16 r = 0, g = 0, b = 0;

	if (x >= lcd_width || y >= lcd_height)
		return 0;	 //超过了范围,直接返回		   
	LCD_SetCursor(x, y);
	if (lcd_id == 0x9341)
		LCD_CMD = 0X2E;   //9341 发送读GRAM指令
	else
		LCD_CMD = R34;    //其他IC发送读GRAM指令   		 				 
	if (LCD_DATA)
		r = 0;
	lcdm_delay(2);
	r = LCD_DATA;  		  						 //实际坐标颜色

	if (lcd_id == 0X9341)		//9341要分2次读出
	{
		lcdm_delay(2);
		b = LCD_DATA;
		g = r & 0XFF;		 //对于9341第一次读取的是RG的值,R在前,G在后,各占8位
		g <<= 8;
	}

	if (lcd_id == 0x9325)
		return r;
	else if (lcd_id == 0x9341)
		return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); //ILI9341需要公式转换一下
	else
		return LCD_BGRtoRGB(r);
}

//LCD开启显示
void LCD_DisplayOn(void)
{
	if (lcd_id == 0x9341)
		LCD_CMD = 0x29;
	else
		LCD_WriteReg(R7, 0x0173);  //开启显示
}

//LCD关闭显示
void LCD_DisplayOff(void)
{
	if (lcd_id == 0x9341)
		LCD_CMD = 0x28;
	else
		LCD_WriteReg(R7, 0x0);    //关闭显示 
}
/****************************************************************************
* 名    称: void LCD_SetCursor(u16 Xaddr, u16 Yaddr)
* 功    能：设置光标位置
* 入口参数：x：x坐标
			y：y坐标
* 返回参数：无
* 说    明：
****************************************************************************/
void LCD_SetCursor(u16 Xaddr, u16 Yaddr)
{
	if (lcd_id == 0X9341)
	{
		LCD_CMD = setxcmd;
		LCD_DATA = (Xaddr >> 8);
		LCD_DATA = (Xaddr & 0XFF);
		LCD_CMD = setycmd;
		LCD_DATA = (Yaddr >> 8);
		LCD_DATA = (Yaddr & 0XFF);
	}
	else
	{
		if (dir_flag == 1)Xaddr = lcd_width - 1 - Xaddr;//横屏就是调转x,y坐标
		LCD_WriteReg(setxcmd, Xaddr);
		LCD_WriteReg(setycmd, Yaddr);
	}
}
/****************************************************************************
* 名    称: void LCD_AUTOScan_Dir(u8 dir)
* 功    能：设置LCD的自动扫描方向
* 入口参数：dir：扫描方向
* 返回参数：无
* 说    明：
****************************************************************************/
void LCD_AUTOScan_Dir(u8 dir)
{
	u16 regval = 0;
	u16 dirreg = 0;
	u16 temp;

	if (dir_flag == 1)//横屏时
	{
		switch (dir)//方向转换
		{
		case 0:dir = 6; break;
		case 1:dir = 7; break;
		case 2:dir = 4; break;
		case 3:dir = 5; break;
		case 4:dir = 1; break;
		case 5:dir = 0; break;
		case 6:dir = 3; break;
		case 7:dir = 2; break;
		}
	}
	if (lcd_id == 0x9341) //9341很特殊
	{
		switch (dir)
		{
		case L2R_U2D://从左到右,从上到下
			regval |= (0 << 7) | (0 << 6) | (0 << 5);
			break;
		case L2R_D2U://从左到右,从下到上
			regval |= (1 << 7) | (0 << 6) | (0 << 5);
			break;
		case R2L_U2D://从右到左,从上到下
			regval |= (0 << 7) | (1 << 6) | (0 << 5);
			break;
		case R2L_D2U://从右到左,从下到上
			regval |= (1 << 7) | (1 << 6) | (0 << 5);
			break;
		case U2D_L2R://从上到下,从左到右
			regval |= (0 << 7) | (0 << 6) | (1 << 5);
			break;
		case U2D_R2L://从上到下,从右到左
			regval |= (0 << 7) | (1 << 6) | (1 << 5);
			break;
		case D2U_L2R://从下到上,从左到右
			regval |= (1 << 7) | (0 << 6) | (1 << 5);
			break;
		case D2U_R2L://从下到上,从右到左
			regval |= (1 << 7) | (1 << 6) | (1 << 5);
			break;
		}
		dirreg = 0X36;
		regval |= 0X08;
		LCD_WriteReg(dirreg, regval);

		if (regval & 0X20)
		{
			if (lcd_width < lcd_height)//交换X,Y
			{
				temp = lcd_width;
				lcd_width = lcd_height;
				lcd_height = temp;
			}
		}
		LCD_CMD = setxcmd;
		LCD_DATA = 0; LCD_DATA = 0;
		LCD_DATA = (lcd_width - 1) >> 8; LCD_DATA = (lcd_width - 1) & 0XFF;
		LCD_CMD = setycmd;
		LCD_DATA = 0; LCD_DATA = 0;
		LCD_DATA = (lcd_height - 1) >> 8; LCD_DATA = (lcd_height - 1) & 0XFF;
	}
	else
	{
		switch (dir)
		{
		case L2R_U2D://从左到右,从上到下
			regval |= (1 << 5) | (1 << 4) | (0 << 3);
			break;
		case L2R_D2U://从左到右,从下到上
			regval |= (0 << 5) | (1 << 4) | (0 << 3);
			break;
		case R2L_U2D://从右到左,从上到下
			regval |= (1 << 5) | (0 << 4) | (0 << 3);
			break;
		case R2L_D2U://从右到左,从下到上
			regval |= (0 << 5) | (0 << 4) | (0 << 3);
			break;
		case D2U_L2R://从下到上,从左到右
			regval |= (0 << 5) | (1 << 4) | (1 << 3);
			break;
		case D2U_R2L://从下到上,从右到左
			regval |= (0 << 5) | (0 << 4) | (1 << 3);
			break;
		case U2D_L2R://从上到下,从左到右
			regval |= (1 << 5) | (1 << 4) | (1 << 3);
			break;
		case U2D_R2L://从上到下,从右到左
			regval |= (1 << 5) | (0 << 4) | (1 << 3);
			break;
		}
		dirreg = 0X03;
		regval |= 1 << 12;
		LCD_WriteReg(dirreg, regval);
	}
}
/****************************************************************************
* 名    称: void LCD_Display_Dir(u8 dir)
* 功    能：设置LCD显示方向
* 入口参数：dir: 0,竖屏
				 1,横屏
* 返回参数：无
* 说    明：
****************************************************************************/
void LCD_Display_Dir(u8 dir)
{
	if (dir == 0)		  //竖屏
	{
		dir_flag = 0;
		lcd_width = 240;
		lcd_height = 320;
		if (lcd_id == 0X9341)
		{
			write_gramcmd = 0X2C;
			setxcmd = 0X2A;
			setycmd = 0X2B;
		}
		else
		{
			write_gramcmd = R34;
			setxcmd = R32;
			setycmd = R33;
		}
	}
	else 				  //横屏
	{
		dir_flag = 1;
		lcd_width = 320;
		lcd_height = 240;
		if (lcd_id == 0X9341)
		{
			write_gramcmd = 0X2C;
			setxcmd = 0X2A;
			setycmd = 0X2B;
		}
		else
		{
			write_gramcmd = R34;
			setxcmd = R33;
			setycmd = R32;
		}
	}
	LCD_AUTOScan_Dir(INIT_SCAN_DIR);	//设置扫描方向
}
/****************************************************************************
* 名    称: void LCD_DrawPoint(u16 x,u16 y)
* 功    能：画点（在该点写入画笔的颜色）
* 入口参数：x：x坐标
			y：y坐标
* 返回参数：无
* 说    明RUSH_COLOR:此点的颜色值
****************************************************************************/
void LCD_DrawPoint(u16 x, u16 y)
{
	LCD_SetCursor(x, y);		    //设置光标位置 
	LCD_WriteRAM_Prepare();	  //开始写入GRAM
	LCD_DATA = BRUSH_COLOR;
}
/****************************************************************************
* 名    称: void LCD_Color_DrawPoint(u16 x,u16 y,u16 color)
* 功    能：在设置的坐标处画相应颜色（在该点写入自定义颜色）
* 入口参数：x：x坐标
			y：y坐标
			color 此点的颜色值
* 返回参数：无
* 说    明：color:写入此点的颜色值   UCGUI调用该函数
****************************************************************************/
void LCD_Color_DrawPoint(u16 x, u16 y, u16 color)
{
	if (lcd_id == 0X9341)
	{
		LCD_CMD = setxcmd;
		LCD_DATA = (x >> 8);
		LCD_DATA = (x & 0XFF);
		LCD_CMD = setycmd;
		LCD_DATA = (y >> 8);
		LCD_DATA = (y & 0XFF);
	}
	else
	{
		if (dir_flag == 1)x = lcd_width - 1 - x;  //横屏就是调转x,y坐标
		LCD_WriteReg(setxcmd, x);
		LCD_WriteReg(setycmd, y);
	}
	LCD_CMD = write_gramcmd;
	LCD_DATA = color;
}
/****************************************************************************
* 名    称: void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
* 功    能：设置窗口,最后并设置画点坐标到窗口左上角(sx,sy)
* 入口参数：sx,sy:窗口起始坐标(左上角)
			width,height:窗口宽度和高度
* 返回参数：无
* 说    明：窗体大小:width*height.
****************************************************************************/
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height)
{
	u8  h1reg, h2reg, v1reg, v2reg;
	u16 h1val, h2val, v1val, v2val;
	width = sx + width - 1;
	height = sy + height - 1;

	if (lcd_id == 0X9341)
	{
		LCD_CMD = setxcmd;
		LCD_DATA = (sx >> 8);
		LCD_DATA = (sx & 0XFF);
		LCD_DATA = (width >> 8);
		LCD_DATA = (width & 0XFF);
		LCD_CMD = setycmd;
		LCD_DATA = (sy >> 8);
		LCD_DATA = (sy & 0XFF);
		LCD_DATA = (height >> 8);
		LCD_DATA = (height & 0XFF);
	}
	else
	{
		if (dir_flag == 1)  //横屏
		{
			h1val = sy;
			h2val = height;
			v1val = lcd_width - width - 1;
			v2val = lcd_width - sx - 1;
		}
		else           //竖屏
		{
			h1val = sx;
			h2val = width;
			v1val = sy;
			v2val = height;
		}
		h1reg = 0X50; h2reg = 0X51;     //水平方向窗口寄存器
		v1reg = 0X52; v2reg = 0X53;     //垂直方向窗口寄存器	  

		LCD_WriteReg(h1reg, h1val);
		LCD_WriteReg(h2reg, h2val);
		LCD_WriteReg(v1reg, v1val);
		LCD_WriteReg(v2reg, v2val);
		LCD_SetCursor(sx, sy);	      //设置光标位置
	}
}
//配置FSMC
void LCD_FSMC_Config()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);//使能PD,PE,PF,PG时钟  
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);//使能FSMC时钟  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        //PF10 推挽输出,控制背光
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);            //初始化 //PF10 推挽输出,控制背光

	GPIO_InitStructure.GPIO_Pin = (3 << 0) | (3 << 4) | (7 << 8) | (3 << 14);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);            //初始化  

	GPIO_InitStructure.GPIO_Pin = (0X1FF << 7);         //PE7~15,AF OUT
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);            //初始化  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         //PG2,FSMC_A12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);            //初始化  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;        //PG12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);            //初始化 

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);//PD0,AF12
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);//PD1,AF12
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);//PD15,AF12

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC); //PE7,AF12
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);//PE15,AF12

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2, GPIO_AF_FSMC);//PF12,AF12
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);

	readWriteTiming.FSMC_AddressSetupTime = 0XF;	 //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns	
	readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
	readWriteTiming.FSMC_DataSetupTime = 60;			 //数据保存时间为60个HCLK	=6*60=360ns
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x00;
	readWriteTiming.FSMC_DataLatency = 0x00;
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 

	writeTiming.FSMC_AddressSetupTime = 9;	     //地址建立时间（ADDSET）为9个HCLK =54ns 
	writeTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（A		
	writeTiming.FSMC_DataSetupTime = 8;		     //数据保存时间为6ns*9个HCLK=54ns
	writeTiming.FSMC_BusTurnAroundDuration = 0x00;
	writeTiming.FSMC_CLKDivision = 0x00;
	writeTiming.FSMC_DataLatency = 0x00;
	writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//   使用NE4 ，也就对应BTCR[6],[7]。
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit   
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // 使能BANK1 

	delay_ms(50); // delay 50 ms 
}
//初始化lcd
void LCD_Init(void)
{
	vu32 i = 0;
	LCD_FSMC_Config();
	LCD_WriteReg(0x0000, 0x0001);
	delay_ms(50);
	lcd_id = LCD_ReadReg(0x0000);    //读取LCD ID

	if (lcd_id < 0XFF || lcd_id == 0XFFFF || lcd_id == 0X9300) //读到ID不正确 
	{
		//尝试9341 ID的读取		
		LCD_CMD = 0XD3;
		lcd_id = LCD_DATA;	//dummy read 	
		lcd_id = LCD_DATA; 	//读到0X00
		lcd_id = LCD_DATA;   	//读取93								   
		lcd_id <<= 8;
		lcd_id |= LCD_DATA;  	//读取41 
	}
	if (lcd_id == 0X9341) //如果是这三个IC,则设置WR时序为最快
	{
		//重新配置写时序控制寄存器的时序   	 							    
		FSMC_Bank1E->BWTR[6] &= ~(0XF << 0); //地址建立时间（ADDSET）清零 	 
		FSMC_Bank1E->BWTR[6] &= ~(0XF << 8); //数据保存时间清零
		FSMC_Bank1E->BWTR[6] |= 3 << 0;		   //地址建立时间（ADDSET）为3个HCLK =18ns  	 
		FSMC_Bank1E->BWTR[6] |= 2 << 8;    	 //数据保存时间为6ns*3个HCLK=18ns
	}
	if (lcd_id == 0x9328) //ILI9328初始化  
	{
		LCD_WriteReg(0x00EC, 0x108F);
		LCD_WriteReg(0x00EF, 0x1234); // ADD        
		LCD_WriteReg(0x0001, 0x0100);
		LCD_WriteReg(0x0002, 0x0700); //电源开启                    
		LCD_WriteReg(0x0003, (1 << 12) | (3 << 4) | (0 << 3));
		LCD_WriteReg(0x0004, 0x0000);
		LCD_WriteReg(0x0008, 0x0202);
		LCD_WriteReg(0x0009, 0x0000);
		LCD_WriteReg(0x000a, 0x0000);//显示设置     
		LCD_WriteReg(0x000c, 0x0001);
		LCD_WriteReg(0x000d, 0x0000);
		LCD_WriteReg(0x000f, 0x0000);
		LCD_WriteReg(0x0010, 0x0000); //电源配置  
		LCD_WriteReg(0x0011, 0x0007);
		LCD_WriteReg(0x0012, 0x0000);
		LCD_WriteReg(0x0013, 0x0000);
		LCD_WriteReg(0x0007, 0x0001);
		delay_ms(50);
		LCD_WriteReg(0x0010, 0x1490);
		LCD_WriteReg(0x0011, 0x0227);
		delay_ms(50);
		LCD_WriteReg(0x0012, 0x008A);
		delay_ms(50);
		LCD_WriteReg(0x0013, 0x1a00);
		LCD_WriteReg(0x0029, 0x0006);
		LCD_WriteReg(0x002b, 0x000d);
		delay_ms(50);
		LCD_WriteReg(0x0020, 0x0000);
		LCD_WriteReg(0x0021, 0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0030, 0x0000); //伽马校正
		LCD_WriteReg(0x0031, 0x0604);
		LCD_WriteReg(0x0032, 0x0305);
		LCD_WriteReg(0x0035, 0x0000);
		LCD_WriteReg(0x0036, 0x0C09);
		LCD_WriteReg(0x0037, 0x0204);
		LCD_WriteReg(0x0038, 0x0301);
		LCD_WriteReg(0x0039, 0x0707);
		LCD_WriteReg(0x003c, 0x0000);
		LCD_WriteReg(0x003d, 0x0a0a);
		delay_ms(50);
		LCD_WriteReg(0x0050, 0x0000); //水平GRAM起始位置 
		LCD_WriteReg(0x0051, 0x00ef); //水平GRAM终止位置                    
		LCD_WriteReg(0x0052, 0x0000); //垂直GRAM起始位置                    
		LCD_WriteReg(0x0053, 0x013f); //垂直GRAM终止位置  
		LCD_WriteReg(0x0060, 0xa700);
		LCD_WriteReg(0x0061, 0x0001);
		LCD_WriteReg(0x006a, 0x0000);
		LCD_WriteReg(0x0080, 0x0000);
		LCD_WriteReg(0x0081, 0x0000);
		LCD_WriteReg(0x0082, 0x0000);
		LCD_WriteReg(0x0083, 0x0000);
		LCD_WriteReg(0x0084, 0x0000);
		LCD_WriteReg(0x0085, 0x0000);
		LCD_WriteReg(0x0090, 0x0010);
		LCD_WriteReg(0x0092, 0x0600);
		LCD_WriteReg(0x0007, 0x0133);//开启显示设置     
	}
	else if (lcd_id == 0x9325)//9325
	{
		LCD_WriteReg(0x00E5, 0x78F0);
		LCD_WriteReg(0x0001, 0x0100);
		LCD_WriteReg(0x0002, 0x0700);
		LCD_WriteReg(0x0003, 0x1030);
		LCD_WriteReg(0x0004, 0x0000);
		LCD_WriteReg(0x0008, 0x0202);
		LCD_WriteReg(0x0009, 0x0000);
		LCD_WriteReg(0x000A, 0x0000);
		LCD_WriteReg(0x000C, 0x0000);
		LCD_WriteReg(0x000D, 0x0000);
		LCD_WriteReg(0x000F, 0x0000);
		//power on sequence VGHVGL
		LCD_WriteReg(0x0010, 0x0000);
		LCD_WriteReg(0x0011, 0x0007);
		LCD_WriteReg(0x0012, 0x0000);
		LCD_WriteReg(0x0013, 0x0000);
		LCD_WriteReg(0x0007, 0x0000);
		//vgh 
		LCD_WriteReg(0x0010, 0x1690);
		LCD_WriteReg(0x0011, 0x0227);
		//delayms(100);
		//vregiout 
		LCD_WriteReg(0x0012, 0x009D); //0x001b
		//delayms(100); 
		//vom amplitude
		LCD_WriteReg(0x0013, 0x1900);
		//delayms(100); 
		//vom H
		LCD_WriteReg(0x0029, 0x0025);
		LCD_WriteReg(0x002B, 0x000D);
		//gamma
		LCD_WriteReg(0x0030, 0x0007);
		LCD_WriteReg(0x0031, 0x0303);
		LCD_WriteReg(0x0032, 0x0003);// 0006
		LCD_WriteReg(0x0035, 0x0206);
		LCD_WriteReg(0x0036, 0x0008);
		LCD_WriteReg(0x0037, 0x0406);
		LCD_WriteReg(0x0038, 0x0304);//0200
		LCD_WriteReg(0x0039, 0x0007);
		LCD_WriteReg(0x003C, 0x0602);// 0504
		LCD_WriteReg(0x003D, 0x0008);
		//ram
		LCD_WriteReg(0x0050, 0x0000);
		LCD_WriteReg(0x0051, 0x00EF);
		LCD_WriteReg(0x0052, 0x0000);
		LCD_WriteReg(0x0053, 0x013F);
		LCD_WriteReg(0x0060, 0xA700);
		LCD_WriteReg(0x0061, 0x0001);
		LCD_WriteReg(0x006A, 0x0000);
		//
		LCD_WriteReg(0x0080, 0x0000);
		LCD_WriteReg(0x0081, 0x0000);
		LCD_WriteReg(0x0082, 0x0000);
		LCD_WriteReg(0x0083, 0x0000);
		LCD_WriteReg(0x0084, 0x0000);
		LCD_WriteReg(0x0085, 0x0000);
		//
		LCD_WriteReg(0x0090, 0x0010);
		LCD_WriteReg(0x0092, 0x0600);

		LCD_WriteReg(0x0007, 0x0133);
		LCD_WriteReg(0x00, 0x0022);//
	}
	else if (lcd_id == 0X9341)	//9341初始化
	{
		LCD_CMD = 0xCF;
		LCD_DATA = 0x00;
		LCD_DATA = 0xC1;
		LCD_DATA = 0X30;
		LCD_CMD = 0xED;
		LCD_DATA = 0x64;
		LCD_DATA = 0x03;
		LCD_DATA = 0X12;
		LCD_DATA = 0X81;
		LCD_CMD = 0xE8;
		LCD_DATA = 0x85;
		LCD_DATA = 0x10;
		LCD_DATA = 0x7A;
		LCD_CMD = 0xCB;
		LCD_DATA = 0x39;
		LCD_DATA = 0x2C;
		LCD_DATA = 0x00;
		LCD_DATA = 0x34;
		LCD_DATA = 0x02;
		LCD_CMD = 0xF7;
		LCD_DATA = 0x20;
		LCD_CMD = 0xEA;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_CMD = 0xC0;    //Power control 
		LCD_DATA = 0x1B;   //VRH[5:0] 
		LCD_CMD = 0xC1;    //Power control 
		LCD_DATA = 0x01;   //SAP[2:0];BT[3:0] 
		LCD_CMD = 0xC5;    //VCM control 
		LCD_DATA = 0x30; 	 //3F
		LCD_DATA = 0x30; 	 //3C
		LCD_CMD = 0xC7;    //VCM control2 
		LCD_DATA = 0XB7;
		LCD_CMD = 0x36;    // Memory Access Control 
		LCD_DATA = 0x48;
		LCD_CMD = 0x3A;
		LCD_DATA = 0x55;
		LCD_CMD = 0xB1;
		LCD_DATA = 0x00;
		LCD_DATA = 0x1A;
		LCD_CMD = 0xB6;    // Display Function Control 
		LCD_DATA = 0x0A;
		LCD_DATA = 0xA2;
		LCD_CMD = 0xF2;    // 3Gamma Function Disable 
		LCD_DATA = 0x00;
		LCD_CMD = 0x26;    //Gamma curve selected 
		LCD_DATA = 0x01;
		LCD_CMD = 0xE0;    //Set Gamma 
		LCD_DATA = 0x0F;
		LCD_DATA = 0x2A;
		LCD_DATA = 0x28;
		LCD_DATA = 0x08;
		LCD_DATA = 0x0E;
		LCD_DATA = 0x08;
		LCD_DATA = 0x54;
		LCD_DATA = 0XA9;
		LCD_DATA = 0x43;
		LCD_DATA = 0x0A;
		LCD_DATA = 0x0F;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_CMD = 0XE1;    //Set Gamma 
		LCD_DATA = 0x00;
		LCD_DATA = 0x15;
		LCD_DATA = 0x17;
		LCD_DATA = 0x07;
		LCD_DATA = 0x11;
		LCD_DATA = 0x06;
		LCD_DATA = 0x2B;
		LCD_DATA = 0x56;
		LCD_DATA = 0x3C;
		LCD_DATA = 0x05;
		LCD_DATA = 0x10;
		LCD_DATA = 0x0F;
		LCD_DATA = 0x3F;
		LCD_DATA = 0x3F;
		LCD_DATA = 0x0F;
		LCD_CMD = 0x2B;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x01;
		LCD_DATA = 0x3f;
		LCD_CMD = 0x2A;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0xef;
		LCD_CMD = 0x11; //Exit Sleep
		delay_ms(120);
		LCD_CMD = 0x29; //display on	
	}
	LCD_Display_Dir(0);		 //初始化为竖屏
	LCD_BACK = 1;					   //点亮背光
	LCD_Clear(WHITE);
}
/****************************************************************************
* 名    称: void LCD_Clear(u16 color)
* 功    能：清屏函数
* 入口参数：color: 要清屏的填充色
* 返回参数：无
* 说    明：
****************************************************************************/
void LCD_Clear(u16 color)
{
	u32 i = 0;
	u32 pointnum = 0;

	pointnum = lcd_width*lcd_height; 	 //得到LCD总点数
	LCD_SetCursor(0x00, 0x00);	       //设置光标位置 
	LCD_WriteRAM_Prepare();     		 //开始写入GRAM	 	  
	for (i = 0; i < pointnum; i++)
	{
		LCD_DATA = color;
	}
}
/****************************************************************************
* 名    称: void LCD_Fill_onecolor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
* 功    能：在指定区域内填充单个颜色
* 入口参数：(sx,sy),(ex,ey):填充矩形对角坐标
			color:要填充的颜色
* 返回参数：无
* 说    明：区域大小为:(ex-sx+1)*(ey-sy+1)  
****************************************************************************/
void LCD_Fill_onecolor(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
	u16 i, j;
	u16 nlen = 0;

	nlen = ex - sx + 1;
	for (i = sy; i <= ey; i++)
	{
		LCD_SetCursor(sx, i);      				  //设置光标位置 
		LCD_WriteRAM_Prepare();     			  //开始写入GRAM	  
		for (j = 0; j < nlen; j++)LCD_DATA = color;	//设置光标位置 	    
	}
}
/****************************************************************************
* 名    称: void LCD_Draw_Picture(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
* 功    能：在指定区域内画入图片
* 入口参数：(sx,sy),(ex,ey):填充矩形对角坐标
			color:要填充的图片像素颜色数组
* 返回参数：无
* 说    明：区域大小为:(ex-sx+1)*(ey-sy+1)  
****************************************************************************/
void LCD_Draw_Picture(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
	u16 height, width;
	u16 i, j;
	width = ex - sx + 1; 			    //得到图片的宽度
	height = ey - sy + 1;			    //得到图片的高度
	for (i = 0; i < height; i++)
	{
		LCD_SetCursor(sx, sy + i);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for (j = 0; j < width; j++)LCD_DATA = color[i*height + j];//写入颜色值
	}
}
/****************************************************************************
* 名    称: void LCD_Draw_Line(u16 x1, u16 y1, u16 x2, u16 y2)
* 功    能：画线
* 入口参数：x1,y1:起点坐标
			x2,y2:终点坐标
* 返回参数：无
* 说    明：有三种情况的画线，水平线、垂直线与斜线(画线、画矩形与画圆参考网上代码)  
************************2****************************************************/
void LCD_Draw_Line(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 i;
	int xm1 = 0, ym2 = 0, model_x, model_y, model, mcx, mcy, mRow, mCol;

	model_x = x2 - x1;                  //计算直线的模 画线坐标增量 
	model_y = y2 - y1;
	mRow = x1;
	mCol = y1;
	if (model_x > 0)mcx = 1;
	else if (model_x == 0)mcx = 0;      //垂直线 
	else { mcx = -1; model_x = -model_x; }
	if (model_y > 0)mcy = 1;
	else if (model_y == 0)mcy = 0;      //水平线 
	else { mcy = -1; model_y = -model_y; }
	if (model_x > model_y)model = model_x;
	else model = model_y;
	for (i = 0; i <= model + 1; i++)       //画线输出 
	{
		LCD_DrawPoint(mRow, mCol);
		xm1 += model_x;
		ym2 += model_y;
		if (xm1 > model)
		{
			xm1 -= model;
			mRow += mcx;
		}
		if (ym2 > model)
		{
			ym2 -= model;
			mCol += mcy;
		}
	}
}
/****************************************************************************
* 名    称: void LCD_Draw_Rectangle(u16 x1, u16 y1, u16 x2, u16 y2)
* 功    能：画矩形
* 入口参数：(x1,y1),(x2,y2):矩形的对角坐标
* 返回参数：无
* 说    明：  
****************************************************************************/
void LCD_Draw_Rectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_Draw_Line(x1, y1, x2, y1);
	LCD_Draw_Line(x1, y1, x1, y2);
	LCD_Draw_Line(x1, y2, x2, y2);
	LCD_Draw_Line(x2, y1, x2, y2);
}
/****************************************************************************
* 名    称: void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
* 功    能：在指定位置画一个指定大小的圆
* 入口参数：(x,y):中心点
			r    :半径
* 返回参数：无
* 说    明：  
****************************************************************************/
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
	int a, b;
	int di;
	a = 0; b = r;
	di = 3 - (r << 1);             //判断下个点位置的标志
	while (a <= b)
	{
		LCD_DrawPoint(x0 + a, y0 - b);
		LCD_DrawPoint(x0 + b, y0 - a);
		LCD_DrawPoint(x0 + b, y0 + a);
		LCD_DrawPoint(x0 + a, y0 + b);
		LCD_DrawPoint(x0 - a, y0 + b);
		LCD_DrawPoint(x0 - b, y0 + a);
		LCD_DrawPoint(x0 - a, y0 - b);
		LCD_DrawPoint(x0 - b, y0 - a);
		a++;
		if (di < 0)di += 4 * a + 6;	  //使用Bresenham算法画圆     
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
	}
}
/****************************************************************************
* 名    称: void LCD_DisplayChar(u16 x,u16 y,u8 num,u8 size)
* 功    能：在指定位置显示一个字符
* 入口参数：x,y:起始坐标
			word:要显示的字符:abcdefg1234567890...
			size:字体大小 12/16/24
* 返回参数：无
* 说    明：取字模参考网上取字模方式，该字模取模方向为先从上到下，再从左到右  
****************************************************************************/
void LCD_DisplayChar(u16 x, u16 y, u8 word, u8 size)
{
	u8  bytenum, bytedata, a, b;
	u16 ymid = y;

	if (size == 12) bytenum = 12;        // 判断各个字体在字库数组中占的字节数
	else if (size == 16) bytenum = 16;
	else if (size == 24) bytenum = 36;
	else return;

	word = word - ' ';  //得到偏移后的值 因为空格之前的字符没在font.h中的数组里面

	for (b = 0; b < bytenum; b++)
	{
		if (size == 12)bytedata = char_1206[word][b]; 	 	  //调用1206字体
		else if (size == 16)bytedata = char_1608[word][b];	//调用1608字体
		else if (size == 24)bytedata = char_2412[word][b];	//调用2412字体
		else return;								                  //没有的字符数组，没字符字库
		for (a = 0; a < 8; a++)
		{
			if (bytedata & 0x80)LCD_Color_DrawPoint(x, y, BRUSH_COLOR);
			else LCD_Color_DrawPoint(x, y, BACK_COLOR);
			bytedata <<= 1;
			y++;
			if (y >= lcd_height)return;		//超区域，退出函数
			if ((y - ymid) == size)
			{
				y = ymid;
				x++;
				if (x >= lcd_width)return;	  //超区域，退出函数
				break;
			}
		}
	}
}
//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m, u8 n)
{
	u32 mid = 1;
	while (n--)mid *= m;
	return mid;
}
/****************************************************************************
* 名    称: void LCD_DisplayNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
* 功    能：在指定位置显示一串数字
* 入口参数：x,y:起点坐标
			num:数值(0~999999999);
			len:长度(即要显示的位数)
			size:字体大小
			mode: 0：高位为0不显示
				  1：高位为0显示0
* 返回参数：无
* 说    明：  
****************************************************************************/
void LCD_DisplayNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
	u8 t, numtemp;
	u8 end0 = 0;
	for (t = 0; t < len; t++)
	{
		numtemp = (num / LCD_Pow(10, len - t - 1)) % 10;
		if (end0 == 0 && t < (len - 1))
		{
			if (numtemp == 0)
			{
				if (mode)LCD_DisplayChar(x + (size / 2)*t, y, '0', size);
				else LCD_DisplayChar(x + (size / 2)*t, y, ' ', size);
				continue;
			}
			else end0 = 1;

		}
		LCD_DisplayChar(x + (size / 2)*t, y, numtemp + '0', size);
	}
}
/****************************************************************************
* 名    称: void LCD_DisplayNum_color(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
* 功    能：在指定位置显示一串自定义颜色的数字
* 入口参数：x,y:起点坐标
			num:数值(0~999999999);
			len:长度(即要显示的位数)
			size:字体大小
			mode: 0：高位为0不显示
				  1：高位为0显示0
			brushcolor：自定义画笔颜色
			backcolor： 自定义背景颜色
* 返回参数：无
* 说    明：  
****************************************************************************/
void LCD_DisplayNum_color(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode, u16 brushcolor, u16 backcolor)
{
	u16 bh_color, bk_color;

	bh_color = BRUSH_COLOR;  //暂存画笔颜色
	bk_color = BACK_COLOR;   //暂存背景颜色

	BRUSH_COLOR = brushcolor;
	BACK_COLOR = backcolor;

	LCD_DisplayNum(x, y, num, len, size, mode);

	BRUSH_COLOR = bh_color;   //不改变系统颜色
	BACK_COLOR = bk_color;
}
/****************************************************************************
* 名    称: void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p)
* 功    能：显示字符串
* 入口参数：x,y:起点坐标
*           size:字体大小
*           *p:字符串起始地址
* 返回参数：无
* 说    明：  
****************************************************************************/
void LCD_DisplayString(u16 x, u16 y, u8 size, u8 *p)
{
	while ((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
	{
		LCD_DisplayChar(x, y, *p, size);
		x += size / 2;
		if (x >= lcd_width) break;
		p++;
	}
}
/****************************************************************************
* 名    称: void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p)
* 功    能：显示自定义字符串
* 入口参数：x,y:起点坐标
*           width,height:区域大小
*           size:字体大小
*           *p:字符串起始地址
*           brushcolor：自定义画笔颜色
*           backcolor： 自定义背景颜色
* 返回参数：无
* 说    明：  
****************************************************************************/
void LCD_DisplayString_color(u16 x, u16 y, u8 size, u8 *p, u16 brushcolor, u16 backcolor)
{
	u16 bh_color, bk_color;

	bh_color = BRUSH_COLOR;  //暂存画笔颜色
	bk_color = BACK_COLOR;   //暂存背景颜色

	BRUSH_COLOR = brushcolor;
	BACK_COLOR = backcolor;

	LCD_DisplayString(x, y, size, p);

	BRUSH_COLOR = bh_color;   //不改变系统颜色
	BACK_COLOR = bk_color;
}




