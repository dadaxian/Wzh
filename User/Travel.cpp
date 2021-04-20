#include "Travel.h"	
#include "main.h"
#include "mainmap.h"

int PausedBydoorFlag = 0;
u8 beginTravelFlag = 0;
int doorStatus[4];
int doorIndex;
int status;

void Travel_Begin()
{
	beginTravelFlag = 1;
}
void Travel_Init()
{
	Travel_Read_Door_Status();

	for (int i = 0; i < 4; i++)
	{
		int doorPos = getDoorPos(i);

		if (doorStatus[i] == 1)
		{
			allNode[doorPos]->paths[0]->weight = 0;
			allNode[doorPos]->paths[1]->weight = 0;
		}
		else if (doorStatus[i] == 2)
		{
			allNode[doorPos]->paths[0]->weight = 100;
			allNode[doorPos]->paths[1]->weight = 100;
		}
		else
		{
			allNode[doorPos]->paths[0]->weight = 2;
			allNode[doorPos]->paths[1]->weight = 2;
		}
	}
}

void Travel_Main()
{
	Travel_Init();

	waitingForBegin();

	int targetArray[] = { 5,7,13,11,12,34,51,41,32,1 };
	int targetArrayLen = 10;
	int currentTarget = 0;
	int currentNodeId = 1;
	currentNode = SearchForNodeByID(currentNodeId);

	for (int i = 0; i < targetArrayLen; i++)
	{
		PausedBydoorFlag = 1;
		while (PausedBydoorFlag == true)
		{
			PausedBydoorFlag = false;
			MAP_goRoute(currentNode->id, targetArray[i]);
		}
	}
	setSpeed(0, 0);

}
void Travel_Reset()
{
	FLASH_Clear(TRAVEL_FIRST_ADDRESS);
}
void Travel_Write_Door_Status()
{
	Travel_Write_Door_Status(doorIndex, status);
}
void Travel_Write_Door_Status(int doorIndex, int status)
{
	Flash_Write_One_Data(status, TRAVEL_FIRST_ADDRESS, doorIndex);
}
void Travel_Read_Door_Status()
{
	int value;
	for (int i = 0; i < 4; i++)
	{
		value = FLASH_READ_ONEDATA(i, TRAVEL_FIRST_ADDRESS);
		if (value > 2)
		{
			value = 0;
		}
		doorStatus[i] = value;
	}
}
int getDoorPos(int doorIndex)
{
	if (doorIndex > 3)
	{
		printf("getDoorArrayPos error");
		while (1)
		{
			;
		}
	}
	return doorArrayPos[doorIndex];

}