/*
 * motor.c
 *
 *  Created on: Jan 9, 2026
 *      Author: agheles
 */


#include "utils.h"


uint16_t *integral_left;
uint16_t *integral_right;
uint16_t *prevErr_left;
uint16_t *prevErr_right;

volatile MOTOR motor_left={&huart1,1,0,CW_LEFT};

volatile MOTOR motor_right={&huart3,2,0,CW_RIGHT};

float Kp=10.0;
float Ki=0.0;
float Kd=0.0;


void AX12_Transmit_Safe(UART_HandleTypeDef* huart, uint8_t* pData, uint16_t Size) {

    HAL_HalfDuplex_EnableTransmitter(huart);


    HAL_UART_Transmit(huart, pData, Size, 10);


    while(__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);

    HAL_HalfDuplex_EnableReceiver(huart);
}
// -----------------------------------------------------------------------


/*
 * Définit la vitesse de rotation (Mode Wheel/Roue).
 */
void Set_Speed(volatile MOTOR* motor, uint16_t speed) {
	motor->speed=speed;
    uint8_t packet[9];
    uint16_t data_value = motor->speed;

    if (data_value > 1023) data_value = 1023;

    if (motor->cw) {
        data_value |= 1024;
    }

    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = motor->id;
    packet[3] = 0x05;
    packet[4] = 0x03;
    packet[5] = 0x20;

    packet[6] = (uint8_t)(data_value & 0xFF);
    packet[7] = (uint8_t)((data_value >> 8) & 0xFF);

    uint32_t sum = motor->id + 0x05 + 0x03 + 0x20 + packet[6] + packet[7];
    packet[8] = (uint8_t)(~sum);

    AX12_Transmit_Safe(motor->huart, packet, 9);
}


void Set_Wheel_Mode(UART_HandleTypeDef* huart, int id){
    uint8_t set_wheel_mode[] = {
        0xFF, 0xFF,
        (uint8_t)id,
        0x07,
        0x03,
        0x06,
        0x00, 0x00,
        0x00, 0x00,
        0x00 // Placeholder checksum
    };

    uint32_t sum = id + 0x07 + 0x03 + 0x06;
    set_wheel_mode[10] = (uint8_t)(~sum);

    AX12_Transmit_Safe(huart, set_wheel_mode, 11);
}


uint8_t calcul_checksum(uint8_t *paquet, int taille_totale) {
    int somme = 0;
    for (int i = 2; i < taille_totale - 1; i++) {
        somme = somme + paquet[i];
    }
    return (uint8_t)(~somme);
}

/*
 * Remet le moteur en mode Servo
 */
void Set_Servo_Mode(UART_HandleTypeDef* huart, uint8_t id) {
    uint8_t packet[11];

    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = 0x07;
    packet[4] = 0x03;
    packet[5] = 0x06;

    packet[6] = 0x00;
    packet[7] = 0x00;
    packet[8] = 0xFF;
    packet[9] = 0x03;

    uint32_t sum = id + 0x07 + 0x03 + 0x06 + 0x00 + 0x00 + 0xFF + 0x03;
    packet[10] = (uint8_t)(~sum);

    AX12_Transmit_Safe(huart, packet, 11);
}

// --- FONCTIONS DE LECTURE ---

uint16_t AX12_Read_2Bytes(UART_HandleTypeDef* huart, uint8_t id, uint8_t addr) {
    uint8_t tx_packet[8];
    uint8_t rx_buffer[8];

    tx_packet[0] = 0xFF;
    tx_packet[1] = 0xFF;
    tx_packet[2] = id;
    tx_packet[3] = 0x04;
    tx_packet[4] = 0x02;
    tx_packet[5] = addr;
    tx_packet[6] = 0x02;

    uint32_t sum = id + 0x04 + 0x02 + addr + 0x02;
    tx_packet[7] = (uint8_t)(~sum);


    HAL_HalfDuplex_EnableTransmitter(huart);
    HAL_UART_Transmit(huart, tx_packet, 8, 10);
    while(__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);
    HAL_HalfDuplex_EnableReceiver(huart);

    if (HAL_UART_Receive(huart, rx_buffer, 8, 20) == HAL_OK) {
        if (rx_buffer[0] == 0xFF && rx_buffer[1] == 0xFF && rx_buffer[2] == id) {
            if (rx_buffer[4] == 0) {
                uint8_t low = rx_buffer[5];
                uint8_t high = rx_buffer[6];
                return (high << 8) | low;
            }
        }
    }

    return AX12_ERROR_VALUE;
}

uint16_t Read_Angle(UART_HandleTypeDef* huart, uint8_t id) {
    return AX12_Read_2Bytes(huart, id, 0x24);
}

uint16_t Read_Speed(UART_HandleTypeDef* huart, uint8_t id) {
    return AX12_Read_2Bytes(huart, id, 0x26);
}

uint16_t Angle_From_Wheel(UART_HandleTypeDef* huart , uint8_t id){

	uint16_t test=0;



	return test;
}
/*
 * Positionne le moteur à un angle donné (Mode Joint/Servo).
 */
void Set_Angle(UART_HandleTypeDef* huart, uint8_t id, uint16_t goal_pos) {
    uint8_t packet[9];

    if (goal_pos > 1023) goal_pos = 1023;

    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = 0x05;
    packet[4] = 0x03;
    packet[5] = 0x1E;

    packet[6] = (uint8_t)(goal_pos & 0xFF);
    packet[7] = (uint8_t)((goal_pos >> 8) & 0xFF);

    uint32_t sum = id + 0x05 + 0x03 + 0x1E + packet[6] + packet[7];
    packet[8] = (uint8_t)(~sum);

    AX12_Transmit_Safe(huart, packet, 9);
}


//---PID-----

uint16_t PID_Controller(uint16_t desired, uint16_t real, uint16_t *integral, uint16_t *prevErr) {
    uint16_t error = desired - real;

    *integral += error;
    *integral = constrain(*integral, 0 , 1023.0/ Ki);

    float derivative = error - *prevErr;
    float output = Kp * error + Ki * (*integral) + Kd * derivative;

    *prevErr = error;

return constrain((uint16_t) output, 0, 1023);

}

//Command_vel

void Command_Vel(volatile MOTOR motor_left , volatile MOTOR motor_right , uint16_t speed_left, uint16_t speed_right){
/*
	uint16_t real_left = Read_Speed(motor_left.huart, motor_left.id);
	uint16_t real_right = Read_Speed(motor_right.huart,motor_right.id);
*/
	motor_right.speed= speed_right;
	motor_left.speed = speed_left;


}



void Go_Straight(volatile MOTOR* motor_left, volatile MOTOR* motor_right,uint16_t speed_right ,uint16_t speed_left){


	Set_Speed(motor_right,speed_right);

	Set_Speed(motor_left, speed_left);

}


//----Square test------

void Square_Test(UART_HandleTypeDef* huart_motor_left, UART_HandleTypeDef* huart_motor_right, uint8_t id_left, uint8_t id_right){



}


// --- SEQUENCE DE TEST ---
/*
void AX12_Test_Sequence(UART_HandleTypeDef* huart, uint8_t id){

    Set_Servo_Mode(huart, id);
    HAL_Delay(2000);

    Set_Angle(huart, id, 0);
    HAL_Delay(2000);

    Set_Angle(huart, id, 512);
    HAL_Delay(2000);

    Set_Angle(huart, id, 1023);
    HAL_Delay(2000);

    Set_Angle(huart, id, 512);
    HAL_Delay(2000);

    Set_Wheel_Mode(huart, id);
    HAL_Delay(2000);

    Set_Speed(huart, id, 500, 0);
    HAL_Delay(2000);

    Set_Speed(huart, id, 0, 0);
    HAL_Delay(2000);

    Set_Speed(huart, id, 1000, 1);
    HAL_Delay(2000);

    Set_Speed(huart, id, 0, 0);

    HAL_Delay(100);
    Set_Servo_Mode(huart, id);
}
*/


