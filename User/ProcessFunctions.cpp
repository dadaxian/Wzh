#include "ProcessFunctions.h"
#include "drivers\buttonIR.h"
#include "devices\sen2020.h"
#include "drivers\motor.h"
#include "mainmap.h"
#include "LobotServoController.h"
#include "track.h"
#include "maincpp.h"
#include "myMath.h"
#include "MAP\map.h"
#include "MAP\mapNode.h"
#include "drivers\RespCom.h"
#include "devices\CameraFlash.h"

extern int currentDirection;
extern int IR_Front;
extern MapNode* nextNode;
extern ExceptionFunction_t exceptionFunction;
extern TrackFucntion_t trackfunction;
extern MapNode* currentNode;
extern MapNode* startNode;
extern int RouteIndex;;
extern bool stopRouteFlag;
extern int nextTreasure;
extern int currentDirection; 
int bridgeLeftSpeed=50;
int bridgeRightSpeed=50;
int bridgeAngle=35;
int GoDownSpeed = 4;

int speednomal = 35;
int speedGive = 40;

int seesawAngle = -3;
int seesawGoOnPulse = 90;


int ls = 30;
int rs = 30;



extern Path* currentPath;

void hitTest() {
	hit(40, 155);
}

void PF_Bridge()
{
	LCD_printf("PF_Bridge");
 
	Action_Handsdown();

	FillFitter(1, 0);
	while (linecountAfterFitter() == 1)
	{
		getSEN2020DigitalData();
		getLines(SEN2020Array);
		getToLine_Mixed(0);
	}
	FillFitter(0, 0);
	int i = 0;
//	goWithPulse(10, FORWORD);
	while (i++ < 2000)
	{
		getButtonIRdata();
		track_on_bridge();
	}
	while (linecountAfterFitter() == 0)
	{
		getSEN2020DigitalData();
		getLines(SEN2020Array);
		getButtonIRdata();
		track_on_bridge();
	}
	TrackNoExceptionForLength(5, 20);
}
void PF_seesaw()
{
	
}
void PF_seesawOnCircle()
{
	setSpeed(0, 0);
	delay_ms(200);
	//goWithPulse(seesawGoOnPulse, FORWORD);
	//turnAngleByPulse(seesawAngle);
	goWithPulse(10, FORWORD);
	setSpeed(0, 0);
	delay_ms(200);
	turnToOnLine(20,0);//0 stand for leftswitch
	turnAngleByPulse(-3,15);
	
	int pul = seesawGoOnPulse * 1633 / 100;
	turnByAnglePulseReset();
	Encoder_pulseOnFlag = 1;

	while (Encoder_rfpulse<pul)
	{
		if (Laser_LeftIsOnLine())
			setSpeed(speednomal+ speedGive, speednomal);
		else if (Laser_FrontIsOnLine())
			setSpeed(speednomal, speednomal + speedGive);
		else
			setSpeed(speednomal, speednomal);
	}
	setSpeed(0, 0);

	Encoder_pulseOnFlag = 0;
	delay_ms(1000);
	
	//goWithPulse(10, FORWORD);
	//turnLeftToOnLine();
	////delay_ms(1000);
	for (int i = 0; i < 100; i++)
	{
		getSEN2020DigitalData();
		getLines(SEN2020Array);
	}
	
	if (linecountAfterFitter() != 0) {
		TrackNoExceptionForLength(5, 20);
		turnAngleByPulse(70);
	}
		
	//goWithPulse(seesawGoOnPulse, FORWORD);
	//delay_ms(1000);
	//turnRadiusTillLine(20, 50);
}
void PF_normal_scene()
{
	LCD_printf("PF_normal_scene");
	//hit
	hit(40, 200);
	//Action_ArriveScene();
	turnAngleByPulse(200);
	currentDirection += 180;
}
int currentTreasureIndex = 0;
void PF_Flat_scene()
{
	LCD_printf("PF_Flat_scene\n");
	
	LCD_printf("Arrive Scene\n");

	if (currentNode->id == 1)
	{
		goWithPulse(40, FORWORD);
		LCD_printf("Arrive Start Pos");
		turnAngleByPulse(180,40);
		currentDirection += 180;
		//setSpeed(0, 0);
		return;
	}

	hit(40, 155);
	
	LCD_printf("Arrive Scene\n");

	Action_ArriveScene();
	
#ifdef TREASURE

	//如果不是5号节点 举手表示找到宝藏
	if (currentNode->id != 5)
	{
		currentTreasureIndex++;
		LCD_printf("Find treasure! id=%d", currentNode->id);
	}

	//如果是最后两个节点 不扫描二维码
	if (currentNode->id == 41 || currentNode->id == 51)
	{
		goto goback;
	}
	// 如果已经知道了下一个宝藏的位置
	if (TreasureIndex[currentTreasureIndex] < 10)
	{
		LCD_printf(" Already Know next TreasureIndex=%d", TreasureIndex[currentTreasureIndex]);
		goto goback;
	}

	nextTreasure = AdjTillGetQrCode();
	
#endif // TREASURE


goback:
	//大概转180
	if (currentNode->id == 5)
		turnAngleByPulse(185);
	else if(currentNode->id == 15)
		turnAngleByPulse(180, 40);
	else if (currentNode->id == 41|| currentNode->id==51)
		turnAngleByPulse(175, 40);
	else
		turnAngleByPulse(175,40);

	//抬手表示找到宝藏
#ifdef TREASURE
	//如果不是5号节点 举手表示找到宝藏
	if (currentNode->id != 5)
	{
		Action_FindTreasure();
	}
	//if (currentNode->id != 5);
	//{
	//	Action_FindTreasure();
	//}
#endif

	//不同的平台节点向前走相应的距离
	//if (currentNode->id != 41 && currentNode->id != 51)
	//{
	//	goWithPulse(10, FORWORD);
	//	//摇摆直到转到有线
	//	int tempturnangle = 20;
	//	while (!turnToOnLine(tempturnangle, 1))
	//	{
	//		if (tempturnangle == 0)
	//			break;
	//		if (tempturnangle > 0)
	//		{
	//			tempturnangle = -tempturnangle - 20;
	//		}
	//		else
	//		{
	//			tempturnangle = -tempturnangle + 20;
	//		}
	//	}
	//}
		

	currentDirection -= 180;
	
	if (currentNode->id == 41)
	{
		TrackNoExceptionForLength(100, GoDownSpeed);
	}
	else if(currentNode->id == 51)
		TrackNoExceptionForLength(70, GoDownSpeed);
	else
	{
		TrackNoExceptionForLength(20, 20);
	}
}
int  AdjTillGetQrCode()
{
	int dif = 0;
	FlashOn();
	while (1)
	{
		readQRcode1();
		LCD_printf("Reading\n");
		while (respidata == 0)
		{
			;
		}
		LCD_printf("QRCCCODE=%d", respidata);
		if (respidata > 48 && respidata < 57)
		{
			nextTreasure = respidata - 48;
			break;
		}
		else
		{
			dif = -dif;
			if (dif > 0)
			{
				dif += 5;
			}
			else
			{
				dif -= 5;
			}
			turnByAngleByTime(dif);
		}
		respidata = 0;
	}
	FlashOff();
	return nextTreasure;
	}
void PF_testIR()
{
	LCD_printf("PF_testIR");

	getButtonIRdata();
}
void PF_SingleSideBridge()
{

	//LCD_printf("PF_SingleSideBridge\n");

	Path* nextPath = pathRoute[RouteIndex + 1];

	int nextPathDir = nextPath->getDirFromNode(nextNode);

	int intendTurnAngle = turnToDirection(currentDirection, nextPathDir);

	//从斜坡路旁边进入
	if (currentDirection == 0 || currentDirection == 180)
	{
		goWithPulse(25, FORWORD);
		//左转
		if (intendTurnAngle == 90)
		{
			goWithPulse(27, FORWORD);
			turnAngleByPulse(75);
			currentDirection+=90;
			turnRadiusTillLine(20, 25);
		
			TrackNoExceptionForLength(20, 0);
			return;
		}
		//右转
		else if (intendTurnAngle == -90)
		{
			goWithPulse(30, FORWORD);
			turnAngleByPulse(-80);
			currentDirection-=90;
			turnRadiusTillLine(25, 20);
		
			TrackNoExceptionForLength(20, 0);

			return;
		}
		//直行
		else if (intendTurnAngle == 0)
		{
			if (currentDirection == 0) 
			{
				bridgeAngle = 3;
				bridgeLeftSpeed = 60;
				bridgeRightSpeed = 0;
			}
			else if(currentDirection==180)
			{
				bridgeAngle = -3;
				bridgeLeftSpeed = 0;
				bridgeRightSpeed = 60;
			}
			turnAngleByPulse(bridgeAngle);
			goWithPulseWithDifSpeedByLbCoder(40, 100, 100);
			
			//turnRadiusTillLine(20, 20);

			
			//TrackNoExceptionForLength(20, 0);
			return;
			
		}
		else
		{
			LCD_printf("singleSideBridge Dir Error");
			while (1)
			{
				;
			}
		}

	}
	//从斜坡路中间进入
	else
	{
		if (intendTurnAngle == 90)
		{
			goWithPulse(20, FORWORD);
			turnAngleByPulse(80);
			currentDirection+=90;
			//turnByAngleByTime(90);
			//turnByAngle(90);
			turnRadiusTillLine(20, 40);
		}
		else if (intendTurnAngle == -90)
		{
			goWithPulse(20, FORWORD);
			turnAngleByPulse(-85);
			currentDirection-=90;
		//	turnByAngle(-90);
			//turnByAngleByTime(-90);
			turnRadiusTillLine(40, 20);
		}
		else
		{
			LCD_printf("singleSideBridge Dir Error");
			while (1)
			{
				;
			}
		}
	}

}
extern int PausedBydoorFlag;
void PF_Door()
{
	printf(" PF_Door");
	if (nextNode->paths[0]->weight == 0 || nextNode->paths[0]->weight == 0)
	
	{
		printf(" this door is already open");
		
		return;
	}		
	//hit
	hit(30, 500);
	delay_ms(500);
	//将这条路的权值设为无限大
	if (getFrontIRData() > 1000)
	{

		 
		//
		//write_OneData_To_Travel_Flash(100 - nextNode->id);

		stopRouteFlag = 1;
		PausedBydoorFlag = 1;
		for (int i = 0; i < nextNode->pathCount; i++)
		{
			if (nextNode->paths[i] != currentPath)
			{
				printf(" fuck this Door %d is closed", nextNode->id);
				nextNode->paths[i]->weight = 1000;
				break;
			}
		}
	}
	//将这条路权值设为0
	else
	{
		for (int pathCount_i = 0; pathCount_i < nextNode->pathCount; pathCount_i++)
			nextNode->paths[pathCount_i]->weight = 0;
		printf("yes this door %d is open", nextNode->id);

	}
	nextNode->processFunction = NULL;
}
void PF_CircleEnter()
{
	LCD_printf("PF_CircleEnter\n");
	TrackNoExceptionForLength(8, 0);
	delay_ms(50);
	if(startNode->id==24)
		turnAngleByPulse(5);
}
void PF_Flat_450_scene() {
	//goWithPulse(30, FORWORD);
	//TrackNoExceptionForLength(60,30);
	if(startNode->id==43)
		TrackNoExceptionForLength(100, 50);
	if (startNode->id == 41)
		TrackNoExceptionForLength(80, GoDownSpeed);
}

void hit(int speed, int time)
	{
		setSpeed(speed, speed);
		while (!Click_Triggered());
		//goWithPulse(25,FORWORD);
		setSpeed(-speed, -speed);
		LCD_printf("Hit\n");
		delay_ms(time);
		setSpeed(0, 0);
}
void  SetSpeed() {
	setSpeed(ls, rs);
}

void testGodown() 
{
	//大概转180
	turnAngleByPulse(200);

	//不同的平台节点向前走相应的距离
	if (currentNode->id != 41 && currentNode->id != 51)
		goWithPulse(15, FORWORD);
	else
		goWithPulse(10, FORWORD);

		//摇摆直到转到有线
		int tempTurnAngle = 5;
		while (!turnToOnLine(tempTurnAngle,1))
		{
			if (tempTurnAngle == 0)
				break;
			if (tempTurnAngle > 0) 
			{
				tempTurnAngle = -tempTurnAngle - 5;
			}
			else
			{
				tempTurnAngle = -tempTurnAngle + 5;
			}
			delay_ms(300);
		}
}

int irData[100];
int irDataCount = 0;
bool dataStartFlag = 0;
int ir_us = 0;
int ir_usStartFlag = 0;

u8 HIERIRdata[9] = {0xA5,//固定位
				0x81,//25度 打开
				0x20,//风向 时间
				0x00,//时间位
				0x00,//健康位
				0xE0,//低速
				0x20,//制冷
				0x00,//睡眠
				0x46};//校验和

void IRControl() 
{
	setPulse(0, 3000);
	setPulse(1, 3000);
	setPulse(0, 3000);
	setPulse(1, 3000);
	
	for (int i = 0; i < 9; i++)
	{
		sendByteData(HIERIRdata[i]);
	}

	IROff();
}

void sendByteData(u8 data)
{
	for (int i = 0; i < 8; i++)
	{
		sendBitData(data & 0x80);
		data &= 0x7f;
		data <<= 1;
	}
}

void sendBitData(u8 data)
{
	setPulse(0, 500);
	if(data==0)
		setPulse(data,600);
	else
	{
		setPulse(data, 1600);
	}
	//IROff();
}


/*设置持续一段时间的信号
 *
 *status 电平状态
 *
 *onTime持续时间（us）
 *
 **/
void setPulse(u8 status, int onTime)
{
	if (status == 0) 
	{
		IROn();
		
	}
	else
	{
		IROff();
	}
	ir_usStartFlag = 1;
	while (ir_us < onTime);
	ir_usStartFlag = 0;
	ir_us = 0;
}

void IROn()
{
	FlashOff();
}

void IROff()
{
	FlashOn();
}