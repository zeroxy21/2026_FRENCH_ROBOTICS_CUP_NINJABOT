/*
 * pump.c
 *
 *  Created on: Mar 16, 2026
 *      Author: agheles
 */

#include "main.h"
#include "kst.h"
extern TIM_HandleTypeDef htim3;
#define PUMP_MAX_SPEED 100


void Pump_Start(){

     KST_Servo_SetAngle(&htim3, TIM_CHANNEL_3, 90.0);
}


void Pump_Stop() {

	KST_Servo_SetAngle(&htim3, TIM_CHANNEL_3, 0.0);

}
