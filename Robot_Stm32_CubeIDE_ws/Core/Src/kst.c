/*
 * kst.c
 *
 *  Created on: Jan 12, 2026
 *      Author: agheles
 */


#include "main.h"
extern TIM_HandleTypeDef htim3;

#define KST_PULSE_MIN    500
#define KST_PULSE_NEUTER 1500
#define KST_PULSE_MAX    2500
#define KST_DEGREE_LIMIT 90

void KST_Init(){

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

}


void KST_Servo_SetAngle(TIM_HandleTypeDef *htim, uint32_t Channel, float angle) {

    if (angle < -KST_DEGREE_LIMIT) angle = -KST_DEGREE_LIMIT;
    if (angle > KST_DEGREE_LIMIT)  angle = KST_DEGREE_LIMIT;


    uint16_t pulse_us = (uint16_t)(KST_PULSE_NEUTER + (angle * 11.11f));


    __HAL_TIM_SET_COMPARE(htim, Channel, pulse_us);
}
