
#ifndef __PROCESSFUNCTION_H
#define __PROCESSFUNCTION_H

#include "stm32f4xx.h"

#include "common.h"

extern int bridgeLeftSpeed;
extern int bridgeRightSpeed;
extern int bridgeAngle;

extern int seesawAngle;
extern int seesawGoOnPulse;
extern int GoDownSpeed;

extern int speednomal;
extern int speedGive;

extern int ls ;
extern int rs;
void PF_Bridge();

void PF_seesaw();

void PF_seesawOnCircle();

void PF_normal_scene();

void PF_Flat_scene();

int AdjTillGetQrCode();

void PF_testIR();

void PF_SingleSideBridge();

void SetSpeed();

void testGodown();

void SetSpeed(int ls, int rs);

void TreasureCar();

void hitTest();

void PF_Door();

void PF_CircleEnter();

void PF_Flat_450_scene();

void hit(int speed, int time);


extern int irData[100];
extern int irDataCount;
extern bool dataStartFlag;
extern int ir_us;
extern int ir_usStartFlag;


void sendByteData(u8 data);
void IRControl();

void sendBitData(u8 data);
void setPulse(u8 status, int onTime);
void IROn();
void IROff();
#endif // !__PROCESSFUNCTION_H
