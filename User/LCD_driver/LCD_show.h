#ifndef __LCD_SHOW_H
#define __LCD_SHOW_H

#include "stm32f4xx.h"
#include "../bsp_flash.h"
typedef struct _LCD_Show_Structure LCD_Show_Structure_Typedef;

typedef struct  _LCD_Show_Structure
{
	u16 top;
	u16 left;
	u16 widthChar;
	u16 _width;
	u8 fontSize;
	u8 _fontWidth;
	
	char * name;
	int _namelength;
	void* dataPtr;
	void *  voiddataptr;
	u8 showBorder;
	void (*showFunction)(void *, LCD_Show_Structure_Typedef * );
	void(*LeftFunction)(void);
	void(*RightFunction)(void);
	void(*refreshFunction)(void);

	int _currentOffset;
	

	LCD_Show_Structure_Typedef ** children;
	LCD_Show_Structure_Typedef	* parent;
	int maxChildCount;
	int currentChildCount;
	


} LCD_Show_Structure_Typedef;

extern LCD_Show_Structure_Typedef * LCD_Show_Structures[];
extern int LCD_Show_Structure_Count;
extern int LCD_Selected_Index;
extern unsigned short values[20];

void LCD_Show_User_Config();

void LCD_Show_Init();

//void READ_FLASH_VALUE(void);

LCD_Show_Structure_Typedef * LCD_DEBUG_Permeter_Add_As_Child(void * premeter, char * name, int childCount, LCD_Show_Structure_Typedef * parent);

LCD_Show_Structure_Typedef * LCD_DEBUG_Permeter_Add(void * premeter, char * name, int childCount);

LCD_Show_Structure_Typedef * LCD_DEBUG_Permeter_Add_As_Child(void * premeter, char * name, int childCount, LCD_Show_Structure_Typedef * parent);

void LCD_Add_To_Node(LCD_Show_Structure_Typedef * parent, LCD_Show_Structure_Typedef * child);

void strcpy(char * dest, char * source);

void strcat(char * str1, char * str2);


LCD_Show_Structure_Typedef * _LCD_Show_Structure_Add(void * dataPtr, u8 fontsize, u16 top, u16 left, u16 widthChar, char * name, u8 showBorder, void(*showFunction)(void *, LCD_Show_Structure_Typedef *));

int strlen(char * str);

void LCD_Show();

void LCD_Show_Tree(LCD_Show_Structure_Typedef * lcd);

void LCD_NextNode(LCD_Show_Structure_Typedef * currentNode);

void LCD_GoBack();

void LCD_Show_Clear();

LCD_Show_Structure_Typedef * LCD_Get_Current_Selected_Node();

void _LCD_Structure_Show(LCD_Show_Structure_Typedef * lcd);

void LCD_Show_PutCh(char ch, int charIndex, LCD_Show_Structure_Typedef * currentLcd);



void LCD_Structure_Calc_Postion();


void _LCD_Show_Clear_Area(LCD_Show_Structure_Typedef * lcd);

void _LCD_Show_Draw_Border(LCD_Show_Structure_Typedef * lcd);

void _LCD_Clear_Border(LCD_Show_Structure_Typedef * lcd);

void _GET_LCD_VALUES(void);

void _LCD_DEBUG_KEY_DOWN();

void _LCD_DEBUG_KEY_UP();

void _LCD_DEBUG_KEY_LEFT();

void _LCD_DEBUG_KEY_RIGHT();

#endif // !__LCD_SHOW_H
