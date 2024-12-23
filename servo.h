#ifndef _SERVO_H
#define _SERVO_H

#include <math.h>
#include <Arduino.h>
#include <SCServo.h>  


extern SCSCL sc;  


void angle_set(int ID, float angle ,int speed);  
float mapTo(float val, float I_Min, float I_Max, float O_Min, float O_Max);
int ts(int angle);
void servo_init();








#endif