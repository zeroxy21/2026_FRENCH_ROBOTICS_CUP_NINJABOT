/*
 * utils.h
 *
 *  Created on: Jan 17, 2026
 *      Author: agheles
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <AXL_A12.h>
#include <stdio.h>
#include "kst.h"
#include "stdint.h"
#include "stdbool.h"
#include "math.h"
#include "XL_430.h"
#include "stdlib.h"
#include "pump.h"
#include "Battery_manager.h"
#include "app_config.h"
#include "tof_manager.h"
#include"Neopixel.h"


#define CW_LEFT 0
#define CW_RIGHT 1
#define AX12_ERROR_VALUE 0xFFFF
#define WHEEL_RADIUS 7.075f
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define LEFT_UART huart1
#define RIGHT_UART huart1


extern uint16_t *integral_left;
extern uint16_t *integral_right;
extern uint16_t *prevErr_left;
extern uint16_t *prevErr_right;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

typedef struct State {int waiting ;int on ; int fail;} STATE;
void Robot_Move_Until_TOF_Distance(uint16_t target_distance_mm);
void HAND_STOP_DETECT();


#endif /* INC_UTILS_H_ */
