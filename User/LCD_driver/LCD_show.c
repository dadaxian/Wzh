#include "LCD_show.h"
#include "LCD_print.h"
#include "stdlib.h"
#include "lcd.h"
#include "../preProcesser.h"
#include "preProcesser.h"
#include "main.h"
#include "stm32f4xx_tim.h"
#include "../ProcessFunctions.h"
#include "../devices/PhotoSwitch.h"

#include "../drivers/FlashOperation.h"
#include "../devices/CameraFlash.h"
#include "Magnetometer.h"

/**********settings***********/
#define LCD_DEBUG_SHOW_CHAR_SIZE 12

/**********settings***********/

LCD_Show_Structure_Typedef* LCD_Show_Structures[100];
LCD_Show_Structure_Typedef* LCD_Structure_Root;
LCD_Show_Structure_Typedef* LCD_Current_Root_Node;
LCD_Show_Structure_Typedef * LCD_Selected_Structure;
int LCD_DEBUG_Count = 0;
int LCD_Show_Structure_Count = 0;
int LCD_Selected_Index = 0;


unsigned short values[20];
extern int pido;

extern int PhotoSwitchData;
extern int ls;
extern int rs;
extern int currentDirection;
extern MapNode* nextNode;
extern u8 SEN2020Datas;
extern int lfEncoderCount;
extern int rfEncoderCount;
extern int lbEncoderCount;
extern int rbEncoderCount;
extern int lfCurrentEncoderCount;
extern int rfCurrentEncoderCount;
extern int lbCurrentEncoderCount;
extern int rbCurrentEncoderCount;
extern int motorNospeedCompensate;
extern u8 respidata;
extern int pre;
extern int pulse;
extern int turnByAngle_Speed;
extern  int Nodeid1;
extern  int Nodeid2;
extern int Test_Param_Turn_Angle;
extern int TurnByPulsePremeter;
extern int KnowTreasureFlag;
extern int flashData1;
extern int aimAngle;

void LCD_Show_User_Config()
{
	LCD_Show_Structure_Typedef* Map_Start = LCD_DEBUG_Permeter_Add(NULL, "Map_Start", 3);
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "beginTravel", 0, Map_Start)->LeftFunction = Travel_Begin;
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "beginTreasure", 0, Map_Start)->LeftFunction = Treasure_Main;
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "Treasure_One", 0, Map_Start)->LeftFunction = Treasure_OneTime;


	LCD_Show_Structure_Typedef* TreasureSettings = LCD_DEBUG_Permeter_Add(NULL, "TreasureSettings", 8); 
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "Reset_Value", 0, TreasureSettings)->LeftFunction = Treasure_Reset;
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "Read_Value", 0, TreasureSettings)->LeftFunction = Treasure_Read_Treasure_Index;
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "Write_test", 0, TreasureSettings)->LeftFunction = Treasure_Write;
	LCD_DEBUG_Permeter_Add_As_Child(&treasurePos, "treasurePos", 0, TreasureSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&treasureIndex, "treasureIndex", 0, TreasureSettings);
	LCD_DEBUG_Permeter_Add_As_Child(TreasureIndex + 0, "Treasure1", 0, TreasureSettings);
	LCD_DEBUG_Permeter_Add_As_Child(TreasureIndex + 1, "Treasure2", 0, TreasureSettings);
	LCD_DEBUG_Permeter_Add_As_Child(TreasureIndex + 2, "Treasure3", 0, TreasureSettings);

	
	LCD_Show_Structure_Typedef* TravelSettings = LCD_DEBUG_Permeter_Add(NULL, "TravelSettings", 10);
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "Reset_Value", 0, TravelSettings)->LeftFunction = Travel_Reset;
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "Read_Value", 0, TravelSettings)->LeftFunction = Travel_Read_Door_Status;
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "Write_test", 0, TravelSettings)->LeftFunction = Travel_Write_Door_Status;
	LCD_DEBUG_Permeter_Add_As_Child(&doorIndex, "doorIndex", 0, TravelSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&status, "status", 0, TravelSettings);
	LCD_DEBUG_Permeter_Add_As_Child(doorStatus + 0, "doorStatus0", 0, TravelSettings);
	LCD_DEBUG_Permeter_Add_As_Child(doorStatus + 1, "doorStatus1", 0, TravelSettings);
	LCD_DEBUG_Permeter_Add_As_Child(doorStatus + 2, "doorStatus2", 0, TravelSettings);
	LCD_DEBUG_Permeter_Add_As_Child(doorStatus + 3, "doorStatus3", 0, TravelSettings);



	LCD_Show_Structure_Typedef* RouteTests = LCD_DEBUG_Permeter_Add(NULL, "RouteTests", 10);
	LCD_DEBUG_Permeter_Add_As_Child(NULL, "MAP_goRoute_Test", NULL, RouteTests)->LeftFunction = MAP_goRoute_Test; 
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "MAP_goRoute_Test_900", NULL, RouteTests)->LeftFunction = MAP_goRoute_Test_900; 
	LCD_DEBUG_Permeter_Add_As_Child(&Nodeid1, "Nodeid1", 0,RouteTests);
	LCD_DEBUG_Permeter_Add_As_Child(&Nodeid2, "Nodeid2", 0, RouteTests);
	LCD_DEBUG_Permeter_Add_As_Child(&seesawAngle, "seesawAngle", 0, RouteTests);
	LCD_DEBUG_Permeter_Add_As_Child(&seesawGoOnPulse, "seesawGoOnPulse", 0, RouteTests);
	LCD_DEBUG_Permeter_Add_As_Child(&GoDownSpeed, "GoDownSpeed", 0, RouteTests);


	LCD_Show_Structure_Typedef* PidSettings = LCD_DEBUG_Permeter_Add(NULL, "PidSettings", 10);
	LCD_DEBUG_Permeter_Add_As_Child(&Track_Kp, "Kp", 0, PidSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&Track_Kd, "Kd", 0, PidSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&Track_kkp, "kkp", 0, PidSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&Track_kkd, "kkd", 0, PidSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&straightSpeed, "straightSpeed", 0, PidSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&Speed_High, "Speed_High", 0, PidSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&Speed_Mid, "Speed_Mid", 0, PidSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&Speed_Low, "Speed_Low", 0, PidSettings);
	LCD_DEBUG_Permeter_Add_As_Child(&Speed_VeryHigh, "Speed_Veryhigh", 0, PidSettings);

	LCD_Show_Structure_Typedef* SenserStatus = LCD_DEBUG_Permeter_Add(NULL, "SenserStatus", 8);
	LCD_DEBUG_Permeter_Add_As_Child(&IR_Front, "IR_Front", 0, SenserStatus)->refreshFunction = Test_getFrontIRData;
	LCD_DEBUG_Permeter_Add_As_Child(&IR_button_right, "IR_button_right", 0, SenserStatus)->refreshFunction = getButtonIRdata;
	LCD_DEBUG_Permeter_Add_As_Child(&IR_button_left, "IR_button_left", 0, SenserStatus);
	LCD_DEBUG_Permeter_Add_As_Child(&Click_Is_Down, "Click_Is_Down", 0, SenserStatus)->refreshFunction=Test_getClick_TriggeredData;
	LCD_DEBUG_Permeter_Add_As_Child(&LaserFrontIsOnline, "LaserFrontIsOnline", 0, SenserStatus)->refreshFunction = Laser_Check;
	LCD_DEBUG_Permeter_Add_As_Child(&LaserLeftIsOnline, "LaserLeftIsOnline", 0, SenserStatus);
	LCD_DEBUG_Permeter_Add_As_Child(&angle, "angle", 0, SenserStatus)->refreshFunction = getDir;
	
	LCD_Show_Structure_Typedef* CurrentStatus = LCD_DEBUG_Permeter_Add(NULL, "CurrentStatus", 5);
	LCD_DEBUG_Permeter_Add_As_Child(&currentDirection, "direction", 0, CurrentStatus);
	LCD_DEBUG_Permeter_Add_As_Child(&currentSpeed, "direction", 0, CurrentStatus);
	LCD_DEBUG_Permeter_Add_As_Child(&currentMotorRout, "currentMotorRout", 0, CurrentStatus);
	LCD_DEBUG_Permeter_Add_As_Child(&currentMotorLout, "currentMotorLout", 0, CurrentStatus);


	LCD_Show_Structure_Typedef* TestFunctions = LCD_DEBUG_Permeter_Add(NULL, "TestFunctions", 20);
	{
		LCD_Show_Structure_Typedef* IRControlTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "IRControl", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "IRControl", 0, IRControlTests)->LeftFunction = IRControl;


		LCD_Show_Structure_Typedef* TurnTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "Turn_Tests", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Test_turnToOnLine", 0, TurnTests)->LeftFunction = Test_turnToOnLine;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Test_turnByAngle", 0, TurnTests)->LeftFunction = Test_turnByAngle;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Test_turnAngleByPulse", 0, TurnTests)->LeftFunction = Test_turnAngleByPulse;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Test_turnAngleByPulseSpeed", 0, TurnTests)->LeftFunction = Test_turnAngleByPulseSpeed;
		LCD_DEBUG_Permeter_Add_As_Child(&Test_Param_Turn_Angle, "Angle", 0, TurnTests);
		LCD_DEBUG_Permeter_Add_As_Child(&Test_Param_Turn_Speed, "Test_Param_Turn_Speed", 0, TurnTests);

		LCD_Show_Structure_Typedef* SpeedTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "SpeedTests", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "SetSpeed", 0, SpeedTests)->LeftFunction = SetSpeed;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "hitTest", 0, SpeedTests)->LeftFunction = hitTest;
		LCD_DEBUG_Permeter_Add_As_Child(&ls, "ls", 0, SpeedTests);
		LCD_DEBUG_Permeter_Add_As_Child(&rs, "rs", 0, SpeedTests);

		LCD_Show_Structure_Typedef* seeSawTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "seeSawTests", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "seeSawTests", 0, seeSawTests)->LeftFunction = PF_seesawOnCircle;
		LCD_DEBUG_Permeter_Add_As_Child(&speedGive, "speedGive", 0, seeSawTests);
		LCD_DEBUG_Permeter_Add_As_Child(&speednomal, "speednomal", 0, seeSawTests);

		LCD_Show_Structure_Typedef* difSpeedTrackTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "difSpeedTrackTests", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "goWithPulseWithDifSpeedByLbCoderTest", 0, difSpeedTrackTests)->LeftFunction = goWithPulseWithDifSpeedByLbCoderTest;
		LCD_DEBUG_Permeter_Add_As_Child(&difLeftSpeed, "difLeftSpeed", 0, difSpeedTrackTests);
		LCD_DEBUG_Permeter_Add_As_Child(&difRightSpeed, "difRightSpeed", 0, difSpeedTrackTests);
		LCD_DEBUG_Permeter_Add_As_Child(&difSpeedLength, "difSpeedLength", 0, difSpeedTrackTests);

		LCD_Show_Structure_Typedef* TrackTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "Track_Tests", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Test_TrackNoExceptionForLength", 0, TrackTests)->LeftFunction = Test_TrackNoExceptionForLength;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Test_Track", 0, TrackTests)->LeftFunction = Test_Track;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "testGodown", 0, TrackTests)->LeftFunction = testGodown;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Test_goWithPulse", 0, TrackTests)->LeftFunction = Test_goWithPulse; 
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "PF_SingleSideBridge", 0, TrackTests)->LeftFunction = PF_SingleSideBridge;
		LCD_DEBUG_Permeter_Add_As_Child(&bridgeLeftSpeed, "bridgeLeftSpeed", 0, TrackTests);
		LCD_DEBUG_Permeter_Add_As_Child(&bridgeRightSpeed, "bridgeRightSpeed", 0, TrackTests);
		LCD_DEBUG_Permeter_Add_As_Child(&bridgeAngle, "bridgeAngle", 0, TrackTests);
		LCD_DEBUG_Permeter_Add_As_Child(&Test_Param_Go_length, "aimDis", 0, TrackTests);

		LCD_Show_Structure_Typedef* NolineTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "NolineTests", 10, TestFunctions); 
		LCD_DEBUG_Permeter_Add_As_Child(&NOLINE_count, "NOLINE_count", 0, NolineTests);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Test_Noline", 0, NolineTests)->LeftFunction = Test_Noline;

		LCD_Show_Structure_Typedef* VerticalTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "VerticalTests", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(&Vertical_count, "Vertical_count", 0, VerticalTests);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Test_Vertical", 0, VerticalTests)->LeftFunction = Test_Vertical; 

		LCD_Show_Structure_Typedef* BridgeTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "BridgeTests", 10, TestFunctions); 
		LCD_DEBUG_Permeter_Add_As_Child(&bridgeSpeed, "bridgeSpeed", 0, BridgeTests);
		LCD_DEBUG_Permeter_Add_As_Child(&Bridge_kp, "Bridge_kp", 0, BridgeTests);
		LCD_DEBUG_Permeter_Add_As_Child(&Bridge_kd, "Bridge_kd", 0, BridgeTests);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "PF_Bridge", 0, BridgeTests)->LeftFunction = PF_Bridge;

		LCD_Show_Structure_Typedef* ActionTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "Action_Tests", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Action_ShakeHead", 0, ActionTests)->LeftFunction = Action_ShakeHead;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Action_Handsup", 0, ActionTests)->LeftFunction = Action_Handsup;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Action_HandsupByOrder", 0, ActionTests)->LeftFunction = Action_HandsupByOrder;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Action_Handsdown", 0, ActionTests)->LeftFunction = Action_Handsdown;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Action_Ready", 0, ActionTests)->LeftFunction = Action_Ready;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Action_ArriveScene", 0, ActionTests)->LeftFunction = Action_ArriveScene;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "Action_FindTreasure", 0, ActionTests)->LeftFunction = Action_FindTreasure;
		LCD_DEBUG_Permeter_Add_As_Child(&currentSpeed, "currentSpeed", 0, TrackTests);

		LCD_Show_Structure_Typedef* QRCodeTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "QRCodeTests", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(&respidata, "respidata", 0, QRCodeTests);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "readQRcode1", 0, QRCodeTests)->LeftFunction = readQRcode1;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "FlashOn", 0, QRCodeTests)->LeftFunction = FlashOn;
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "FlashOff", 0, QRCodeTests)->LeftFunction = FlashOff;

		LCD_Show_Structure_Typedef* SpecialTrackTests = LCD_DEBUG_Permeter_Add_As_Child(NULL, "SpecialTrackTests", 10, TestFunctions);
		LCD_DEBUG_Permeter_Add_As_Child(&difSpeedLength, "testLength", 0, SpecialTrackTests);
		LCD_DEBUG_Permeter_Add_As_Child(&difLeftSpeed, "LeftSpeed", 0, SpecialTrackTests);
		LCD_DEBUG_Permeter_Add_As_Child(&difRightSpeed, "RightSpeed", 0, SpecialTrackTests);
		LCD_DEBUG_Permeter_Add_As_Child(NULL, "goWithPulseWithDifSpeedByLbCoderTest", 0, SpecialTrackTests)->LeftFunction = goWithPulseWithDifSpeedByLbCoderTest;



	}
	

}
void LCD_Show_Init()
{
	LCD_Structure_Root = (LCD_Show_Structure_Typedef*)malloc(sizeof(LCD_Show_Structure_Typedef));
	LCD_Structure_Root->maxChildCount = 20;
	LCD_Structure_Root->children = (LCD_Show_Structure_Typedef**)malloc(sizeof(LCD_Show_Structure_Typedef*) * 20);

	LCD_Current_Root_Node = LCD_Structure_Root;

	LCD_Show_User_Config();
	LCD_Structure_Calc_Postion();

}
void LCD_Show_progressBar(int maxIndex,int currentIndex)
{

}

/**
* @brief  API FUNCTION  add a Led_show_structure to lcds array
* @param  dataPtr:specifies  The pointer to the data to show
* @param  fontsize:specifies  font size of data to show
*          This parameter can be 12 16 24.
* @param  top: specifies the distance from the position to the top of the screen;
*          This parameter can be from 0 to the height of lcd Normally 320
* @param  left: specifies the distance from the position to the left of the screen;
*          This parameter can be from 0 to the width of lcd Normally 240
* @param  widthChar: specifies num of chararters to be show it decides the width of the block  will be cleaned afterwards
*          This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
* @param  name: specifies the name of the block it can be shown
*          This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
* @param  showBorder: specifies whether the borader border of the block is visable
*          This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
* @retval None
*/

//api function 
LCD_Show_Structure_Typedef* LCD_DEBUG_Permeter_Add(
	void * premeter,
	char * name,
	int childCount
)

{
	LCD_DEBUG_Permeter_Add_As_Child(premeter, name, childCount, LCD_Structure_Root);

}

LCD_Show_Structure_Typedef* LCD_DEBUG_Permeter_Add_As_Child(
	void * premeter,
	char * name,
	int childCount,
	LCD_Show_Structure_Typedef * parent

)
{
	char* charDisplay = (char *)malloc(sizeof(char)*strlen(name) + 5);

	*charDisplay = 0;

	if (childCount)
	{
		strcat(charDisplay, "> ");
		strcat(charDisplay, name);

	}
	else
	{
		strcat(charDisplay, name);

		if (premeter)
		{
			strcat(charDisplay, "=  ");
		}

	}


	LCD_Show_Structure_Typedef* stru = _LCD_Show_Structure_Add
	(
		premeter,
		LCD_DEBUG_SHOW_CHAR_SIZE,
		LCD_DEBUG_Count++*(LCD_DEBUG_SHOW_CHAR_SIZE + 3),
		5,
		10,
		charDisplay,
		0,
		_LCD_printDec
	);

	stru->children = NULL;
	if (childCount)
	{
		stru->children = (LCD_Show_Structure_Typedef**)malloc(sizeof(LCD_Show_Structure_Typedef*)*childCount);
	}
	stru->refreshFunction = NULL;
	stru->currentChildCount = 0;
	stru->maxChildCount = childCount;
	stru->LeftFunction = 0;
	stru->RightFunction = 0;

	LCD_Add_To_Node(parent, stru);


	return stru;
}

u8 LCD_Is_Main_Root()
{
	return LCD_Current_Root_Node == LCD_Structure_Root;

}

void LCD_Add_To_Node(LCD_Show_Structure_Typedef* parent, LCD_Show_Structure_Typedef* child)
{
	if (parent == NULL)
	{
		printf("parent is NULL");
		return;
	}
	if (parent->children == NULL || parent->maxChildCount == 0)
	{
		printf("no child for %s", parent->name);
		return;
	}

	else if (parent->currentChildCount >= parent->maxChildCount)
	{
		printf("to much child in %s", parent->name);
		return;

	}

	else
	{
		parent->children[parent->currentChildCount++] = child;
		child->parent = parent;

	}
}

void strcpy(char* dest, char * source)
{
	while (*dest++ = *source++);
}

void strcat(char* str1, char * str2)
{
	while (*(str1++));
	str1--;
	while (*(str1++) = *(str2++));
	str1 = 0;
}

LCD_Show_Structure_Typedef * _LCD_Show_Structure_Add(
	void  *dataPtr,
	u8 fontsize,
	u16 top,
	u16 left,
	u16 widthChar,
	char* name,
	u8 showBorder,
	void(*showFunction)(void *, LCD_Show_Structure_Typedef *)

)
{
	LCD_Show_Structure_Typedef* new_LCD_Structure = (LCD_Show_Structure_Typedef*)malloc(sizeof(LCD_Show_Structure_Typedef));
	if (!new_LCD_Structure)
	{
		LCD_printf("no memory for more lcds");
		return NULL;
	}
	new_LCD_Structure->name = name;
	if (name != NULL)
	{
		new_LCD_Structure->_namelength = strlen(new_LCD_Structure->name);
	}
	else
	{
		new_LCD_Structure->_namelength = 0;
	}
	new_LCD_Structure->widthChar = widthChar + new_LCD_Structure->_namelength;

	new_LCD_Structure->dataPtr = dataPtr;
	new_LCD_Structure->fontSize = fontsize;
	new_LCD_Structure->_fontWidth = fontsize / 2;
	new_LCD_Structure->top = top;

	new_LCD_Structure->showFunction = showFunction;
	new_LCD_Structure->left = left;
	new_LCD_Structure->showBorder = showBorder;
	new_LCD_Structure->_width = new_LCD_Structure->widthChar*new_LCD_Structure->_fontWidth;
	LCD_Show_Structures[LCD_Show_Structure_Count] = new_LCD_Structure;

	LCD_Show_Structure_Count++;
	return new_LCD_Structure;
}

int strlen(char * str)
{
	int i = 0;
	while (*(str++))
		i++;
	return i;
}

void LCD_Show()
{
	LCD_Show_Tree(LCD_Current_Root_Node);
}

void LCD_Show_Tree(LCD_Show_Structure_Typedef* lcd)
{
	int index = 0;
	LCD_Selected_Structure = LCD_Get_Current_Selected_Node();
	if (lcd != LCD_Structure_Root)
	{
		_LCD_Structure_Show(lcd);
	}
	for (int i = 0; i < lcd->currentChildCount; i++)
	{
		_LCD_Structure_Show(lcd->children[i]);
	}
}

void LCD_NextNode(LCD_Show_Structure_Typedef * currentNode)
{
	LCD_Selected_Index = 0;
	LCD_Current_Root_Node = currentNode;
	LCD_Show_Clear();
	LCD_Structure_Calc_Postion();
	LCD_Clear(0xffff);
}

void LCD_GoBack()
{
	if (LCD_Current_Root_Node->parent)
	{
		LCD_Selected_Index = 0;
		LCD_Current_Root_Node = LCD_Current_Root_Node->parent;
		LCD_Structure_Calc_Postion();
		LCD_Show_Clear();
		LCD_Clear(0xffff);
	}
}

void LCD_Show_Clear()
{
	LCD_Show_Structure_Typedef * lcd;

	for (int i = 0; i < LCD_Current_Root_Node->currentChildCount; i++)
	{
		lcd = LCD_Current_Root_Node->children[i];
		lcd->showBorder = false;

		_LCD_Show_Clear_Area(lcd);
		_LCD_Clear_Border(lcd);
	}

}

LCD_Show_Structure_Typedef* LCD_Get_Current_Selected_Node()
{
	if (LCD_Is_Main_Root())
	{
		return 	LCD_Current_Root_Node->children[LCD_Selected_Index];
	}
	else
	{
		if (LCD_Selected_Index == 0)
		{
			return LCD_Current_Root_Node;
		}
		else
		{
			return LCD_Current_Root_Node->children[LCD_Selected_Index - 1];
		}
	}
}

void _LCD_Structure_Show(LCD_Show_Structure_Typedef* lcd)
{
	lcd->_currentOffset = 0;
	_LCD_Show_Clear_Area(lcd);

	if (lcd == LCD_Selected_Structure)
	{
		_LCD_Show_Draw_Border(lcd);
	}
	else
	{
		_LCD_Clear_Border(lcd);
	}
	int charIndex = 0;
	if (lcd->refreshFunction)
	{
		lcd->refreshFunction();

	}
	if (lcd->name != NULL)
	{
		char * str = lcd->name;
		while (*str)
		{
			LCD_Show_PutCh(*str++, charIndex++, lcd);
		}
	}
	if (lcd->dataPtr != NULL)
	{
		int num = *((int *)(lcd->dataPtr));
		if (num < 0)
		{
			num = -num;
			LCD_Show_PutCh('-', charIndex++, lcd);
		}
		char str[32] = "";
		int len = 0;
		do	str[len++] = num % 10 + 48; while (num /= 10);
		while (len--)	LCD_Show_PutCh(str[len], charIndex++, lcd);
	}
}

void LCD_Show_PutCh(char ch, int charIndex, LCD_Show_Structure_Typedef* currentLcd)
{
	int x = currentLcd->left + charIndex*currentLcd->_fontWidth;
	int y = currentLcd->top;

	LCD_DisplayChar(x, y, ch, currentLcd->fontSize);
}

void LCD_Structure_Calc_Postion()
{
	int pos = 0;

	if (!LCD_Is_Main_Root())
	{
		LCD_Current_Root_Node->top = pos;
		pos++;
	}

	for (int i = 0; i < LCD_Current_Root_Node->currentChildCount; i++)
	{
		LCD_Show_Structure_Typedef* lcd = LCD_Current_Root_Node->children[i];
		lcd->top = pos*(LCD_DEBUG_SHOW_CHAR_SIZE + 3);
		pos++;
	}

}

void _LCD_Show_Clear_Area(LCD_Show_Structure_Typedef * lcd)
{
	LCD_Fill_onecolor(
		LIMIT(lcd->left, 0, WIDTH),
		LIMIT(lcd->top, 0, HEIGHT),
		LIMIT(lcd->left + lcd->_width, 0, WIDTH),
		LIMIT(lcd->top + lcd->fontSize, 0, HEIGHT),
		0xffff);
}

void _LCD_Show_Draw_Border(LCD_Show_Structure_Typedef * lcd)
{
	LCD_Draw_Rectangle(
		LIMIT(lcd->left - 1, 0, WIDTH),
		LIMIT(lcd->top - 1, 0, HEIGHT),
		LIMIT(lcd->left + lcd->_width + 1, 0, WIDTH),
		LIMIT(lcd->top + lcd->fontSize + 1, 0, HEIGHT)
	);
}

void _LCD_Clear_Border(LCD_Show_Structure_Typedef * lcd)
{
	u16 brushBefore = BRUSH_COLOR;
	BRUSH_COLOR = BACK_COLOR;

	LCD_Draw_Rectangle(
		LIMIT(lcd->left - 1, 0, WIDTH),
		LIMIT(lcd->top - 1, 0, HEIGHT),
		LIMIT(lcd->left + lcd->_width + 1, 0, WIDTH),
		LIMIT(lcd->top + lcd->fontSize + 1, 0, HEIGHT)
	);
	BRUSH_COLOR = brushBefore;
}

void _GET_LCD_VALUES(void)
{
	for (u8 i = 0; i < LCD_Show_Structure_Count; i++)
	{
		values[i] = *(int *)LCD_Show_Structures[i]->dataPtr;
	}
}

void _LCD_DEBUG_KEY_DOWN()
{
	
	int downMax = LCD_Current_Root_Node->currentChildCount;

	if (!LCD_Is_Main_Root())
	{
		downMax++;
	}
	if (LCD_Selected_Index + 1 < downMax)
	{
		LCD_Selected_Index++;
	}
}
void _LCD_DEBUG_KEY_UP()
{

	if (LCD_Selected_Index - 1 >= 0)
	{
		LCD_Selected_Index--;
	}

}
void _LCD_DEBUG_KEY_LEFT()
{

	LCD_Selected_Structure = LCD_Get_Current_Selected_Node();

	if (LCD_Selected_Structure->children)
	{
		LCD_NextNode(LCD_Selected_Structure);
	}
	else if (LCD_Selected_Structure->LeftFunction)
	{
		LCD_Selected_Structure->LeftFunction();
	}
	else
	{
		(*(int *)(LCD_Selected_Structure->dataPtr))--;
	}
}
void _LCD_DEBUG_KEY_RIGHT()
{

	LCD_Selected_Structure = LCD_Get_Current_Selected_Node();

	if (LCD_Selected_Structure == LCD_Current_Root_Node)
	{
		LCD_GoBack();
	}
	else if (LCD_Selected_Structure->RightFunction)
	{
		LCD_Selected_Structure->RightFunction();
	}
	else
	{
		(*(int *)LCD_Selected_Structure->dataPtr)++;
	}
}
