/*
 * motor.h
 *
 *  Created on: Jan 9, 2026
 *      Author: agheles
 */

#ifndef INC_AXL_A12_H_
#define INC_AXL_A12_H_

#include "main.h"

typedef struct Motor {UART_HandleTypeDef *huart ;
	uint8_t id ;
	uint16_t speed ;
	int cw ;}	MOTOR;


extern volatile MOTOR motor_left;
extern volatile MOTOR motor_right;


void AX12_Transmit_Safe(UART_HandleTypeDef* huart, uint8_t* pData, uint16_t Size);


uint8_t calcul_checksum(uint8_t *paquet, int taille_totale);


void Set_Servo_Mode(UART_HandleTypeDef* huart, uint8_t id);
void Set_Wheel_Mode(UART_HandleTypeDef* huart, int id);


void Set_Angle(UART_HandleTypeDef* huart, uint8_t id, uint16_t goal_pos);
void Set_Speed(volatile MOTOR* motor, uint16_t speed);

void Go_Straight(volatile MOTOR* motor_left, volatile MOTOR* motor_right,uint16_t speed_right ,uint16_t speed_left);

uint16_t Read_Angle(UART_HandleTypeDef* huart, uint8_t id);
uint16_t Read_Speed(UART_HandleTypeDef* huart, uint8_t id);


void AX12_Test_Sequence(UART_HandleTypeDef* huart, uint8_t id);

#endif /* INC_AXL_A12_H_ */
