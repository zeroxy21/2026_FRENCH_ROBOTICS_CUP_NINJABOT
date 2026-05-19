/*
 * XL_430.h
 *
 *  Created on: Jan 28, 2026
 *      Author: agheles
 */

#ifndef INC_XL_430_H_
#define INC_XL_430_H_

#include <stdint.h>

// --- ZONE EEPROM (Modifiables seulement quand Torque Enable = 0) ---
#define XL430_ADDR_OPERATING_MODE     11   // 1 byte | 1: Vel, 3: Pos, 4: Extended Pos, 16: PWM
#define XL430_ADDR_HOMING_OFFSET      20   // 4 bytes
#define XL430_ADDR_MOVING_THRESHOLD   24   // 4 bytes
#define XL430_ADDR_TEMPERATURE_LIMIT  31   // 1 byte
#define XL430_ADDR_MAX_VOLTAGE_LIMIT  32   // 2 bytes
#define XL430_ADDR_MIN_VOLTAGE_LIMIT  34   // 2 bytes
#define XL430_ADDR_PWM_LIMIT          36   // 2 bytes
#define XL430_ADDR_VELOCITY_LIMIT     44   // 4 bytes
#define XL430_ADDR_MAX_POS_LIMIT      48   // 4 bytes
#define XL430_ADDR_MIN_POS_LIMIT      52   // 4 bytes

// --- ZONE RAM (Modifiables en temps réel) ---
#define XL430_ADDR_TORQUE_ENABLE      64   // 1 byte
#define XL430_ADDR_LED                65   // 1 byte
#define XL430_ADDR_STATUS_RETURN      68   // 1 byte

// --- GAINS PID (Très important pour la précision) ---
#define XL430_ADDR_VELOCITY_I_GAIN    76   // 2 bytes
#define XL430_ADDR_VELOCITY_P_GAIN    78   // 2 bytes
#define XL430_ADDR_POSITION_D_GAIN    80   // 2 bytes
#define XL430_ADDR_POSITION_I_GAIN    82   // 2 bytes
#define XL430_ADDR_POSITION_P_GAIN    84   // 2 bytes
#define XL430_ADDR_FEEDFORWARD_2ND    88   // 2 bytes
#define XL430_ADDR_FEEDFORWARD_1ST    90   // 2 bytes

// --- CIBLES (Targets) ---
#define XL430_ADDR_GOAL_PWM           100  // 2 bytes
#define XL430_ADDR_GOAL_VELOCITY      104  // 4 bytes
#define XL430_ADDR_PROFILE_ACCEL      108  // 4 bytes
#define XL430_ADDR_PROFILE_VELOCITY   112  // 4 bytes
#define XL430_ADDR_GOAL_POSITION      116  // 4 bytes

// --- ÉTAT (Read Only) ---
#define XL430_ADDR_MOVING             122  // 1 byte
#define XL430_ADDR_PRESENT_PWM        124  // 2 bytes
#define XL430_ADDR_PRESENT_VELOCITY   128  // 4 bytes
#define XL430_ADDR_PRESENT_POSITION   132  // 4 bytes
#define XL430_ADDR_PRESENT_VOLTAGE    144  // 2 bytes
#define XL430_ADDR_PRESENT_TEMP       146  // 1 byte
//----- Dimensions des Roues en mm

#define WHEEL_DIAMETER 47.6//mm
#define DISTANCE_BEETWEEN_WHEELS 162.93f //mm
#define TICKS_PER_MM 27.58f
#define TICKS_PER_REV     4096.0f
#define TRACK_WIDTH       174.82f  // Entraxe entre les deux roues en mm
#define MM_TO_TICKS(mm)   (int32_t)((mm * TICKS_PER_REV) / (3.14159f * WHEEL_DIAMETER))
#define DEG_TO_TICKS(deg) (int32_t)(((deg * 3.14159f *(0.92)*TRACK_WIDTH) / 360.0f) * (TICKS_PER_REV / (3.14159f *WHEEL_DIAMETER)))
//(0.971) coeff correcteur sur 10 tours
typedef struct {
    float Kp, Ki, Kd;

    // Mémoires séparées pour la roue gauche (1) et droite (2)
    int32_t last_error_1;
    int32_t last_error_2;
    float integral_1;
    float integral_2;
} PID_Config;

// Prototype
void XL430_Update_PID(uint8_t id1, uint8_t id2, int32_t target1, int32_t target2, volatile PID_Config *pid);

extern volatile int32_t left_pos;
extern  volatile int32_t right_pos;
extern volatile PID_Config robot_pid ;

// --- PROTOTYPES ---
void XL430_SetVelocity_Sync(uint8_t id1, int32_t val1, uint8_t id2, int32_t val2);
void XL430_Init(UART_HandleTypeDef *huart);
void Motor_Init();
void XL430_SetVelocityMode(uint8_t id);

void XL430_WriteByte(uint8_t id, uint16_t address, uint8_t value);
void XL430_WriteWord(uint8_t id, uint16_t address, uint16_t value);
void XL430_WriteDWord(uint8_t id, uint16_t address, uint32_t value);

void XL430_SetTorque(uint8_t id, uint8_t enable);
void XL430_SetLED(uint8_t id, uint8_t enable);
void XL430_SetGoalPosition(uint8_t id, int32_t position);
void XL430_SetGoalVelocity(uint8_t id, uint32_t velocity);
void XL430_SetStatusReturnLevel(uint8_t id, uint8_t level);
void XL430_GoToAngle(uint8_t id, float angle_deg);
void XL430_SetPositionMode(uint8_t id);
static uint32_t XL430_DegToTicks(float deg);
void XL430_SetExtendedPositionMode(uint8_t id);
int32_t XL430_GetPresentPosition(uint8_t id);
void XL430_Spin(uint8_t id_1,int32_t nb_spins);
int Revs_to_number(int rev);
//void XL430_Move_Distance(uint8_t id, float distance_mm);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Robot_Wait_Until_Target(int32_t target1, int32_t target2, volatile PID_Config *pid);
void Robot_Move_Forward(float distance_mm);
void Robot_Rotate(float angle_deg) ;
void XL430_Square_Test();
void Robot_Move_Revolutions_Extended(float revolutions);
void Robot_Move_Backwards(float distance_mm);
void ARM_DOWN_2();
void ARM_DOWN_1();
void ARM_UP();
#endif /* INC_XL_430_H_ */
