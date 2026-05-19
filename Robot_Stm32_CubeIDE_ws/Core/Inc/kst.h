/*
 * kst.h
 *
 *  Created on: Jan 12, 2026
 *      Author: agheles
 */

#ifndef INC_KST_H_
#define INC_KST_H_
void KST_Init();
void KST_Servo_SetAngle(TIM_HandleTypeDef *htim, uint32_t Channel, float angle);

#endif /* INC_KST_H_ */
