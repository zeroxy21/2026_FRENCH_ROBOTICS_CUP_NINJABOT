/*
 * XL_430.c
 *
 *  Created on: Jan 28, 2026
 *      Author: agheles
 */


#include "utils.h"
#include "main.h"
#include "XL_430.h"
extern int team_is_yellow;

// Variable locale pour stocker le pointeur vers l'UART
static UART_HandleTypeDef *xl430_huart;
static int motor_1_sens= -1;
volatile int32_t left_pos = 0;
volatile int32_t right_pos = 0;
volatile uint8_t xl430_rx_flag = 0;// Drapeau de fin de réception
uint8_t xl430_rx_buffer[20];    // Buffer de réception
volatile PID_Config robot_pid = {
    .Kp = 0.5f,
    .Ki = 0.01f,
    .Kd = 0.1f,
    .last_error_1 = 0,
    .last_error_2 = 0,
    .integral_1 = 0,
    .integral_2 = 0
};
// --- TABLE CRC 16 (Identique au XL320 - Protocol 2.0) ---
static const uint16_t crc_table[256] = {
    0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
    0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
    0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
    0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
    0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
    0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
    0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
    0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
    0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
    0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
    0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
    0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
    0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
    0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
    0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
    0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
    0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
    0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
    0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
    0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
    0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
    0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
    0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
    0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
    0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
    0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
    0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};

// --- FONCTIONS INTERNES ---

static uint16_t update_crc(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size) {
    uint16_t i, j;
    for(j = 0; j < data_blk_size; j++) {
        i = ((uint16_t)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }
    return crc_accum;
}

static void XL430_Transmit(uint8_t *packet, uint16_t len) {
    if(xl430_huart == NULL) return;

    // Nettoyage buffer avant envoi
    __HAL_UART_FLUSH_DRREGISTER(xl430_huart);

    HAL_UART_Transmit(xl430_huart, packet, len, 100);
}

// --- FONCTIONS UTILISATEUR ---

void XL430_Init(UART_HandleTypeDef *huart) {
    xl430_huart = huart;
}

// Fonction 1 octet (Byte) - OK pour Torque Enable, LED
void XL430_WriteByte(uint8_t id, uint16_t address, uint8_t value) {
    uint8_t pkt[16];

    // Header
    pkt[0] = 0xFF; pkt[1] = 0xFF; pkt[2] = 0xFD; pkt[3] = 0x00;
    pkt[4] = id;
    pkt[5] = 0x06; pkt[6] = 0x00; // Length = 6
    pkt[7] = 0x03; // INST_WRITE

    // Params
    pkt[8] = (uint8_t)(address & 0xFF);
    pkt[9] = (uint8_t)((address >> 8) & 0xFF);
    pkt[10] = value;

    // CRC
    uint16_t crc = update_crc(0, pkt, 11);
    pkt[11] = crc & 0xFF;
    pkt[12] = crc >> 8;

    XL430_Transmit(pkt, 13);
}

// Fonction 2 octets (Word) - OK pour PWM, etc.
void XL430_WriteWord(uint8_t id, uint16_t address, uint16_t value) {
    uint8_t pkt[16];

    // Header
    pkt[0] = 0xFF; pkt[1] = 0xFF; pkt[2] = 0xFD; pkt[3] = 0x00;
    pkt[4] = id;
    pkt[5] = 0x07; pkt[6] = 0x00; // Length = 7
    pkt[7] = 0x03; // INST_WRITE

    // Params
    pkt[8] = (uint8_t)(address & 0xFF);
    pkt[9] = (uint8_t)((address >> 8) & 0xFF);
    pkt[10] = (uint8_t)(value & 0xFF);
    pkt[11] = (uint8_t)((value >> 8) & 0xFF);

    // CRC
    uint16_t crc = update_crc(0, pkt, 12);
    pkt[12] = crc & 0xFF;
    pkt[13] = crc >> 8;

    XL430_Transmit(pkt, 14);
}

// [NOUVEAU] Fonction 4 octets (DWord) - NECESSAIRE pour Position XL430
void XL430_WriteDWord(uint8_t id, uint16_t address, uint32_t value) {
    uint8_t pkt[18];

    // Header
    pkt[0] = 0xFF; pkt[1] = 0xFF; pkt[2] = 0xFD; pkt[3] = 0x00;
    pkt[4] = id;
    pkt[5] = 0x09; pkt[6] = 0x00; // Length = 9 (Inst(1)+Addr(2)+Data(4)+CRC(2))
    pkt[7] = 0x03; // INST_WRITE

    // Params
    pkt[8] = (uint8_t)(address & 0xFF);
    pkt[9] = (uint8_t)((address >> 8) & 0xFF);

    // Data (Little Endian)
    pkt[10] = (uint8_t)(value & 0xFF);
    pkt[11] = (uint8_t)((value >> 8) & 0xFF);
    pkt[12] = (uint8_t)((value >> 16) & 0xFF);
    pkt[13] = (uint8_t)((value >> 24) & 0xFF);

    // CRC
    uint16_t crc = update_crc(0, pkt, 14);
    pkt[14] = crc & 0xFF;
    pkt[15] = crc >> 8;

    XL430_Transmit(pkt, 16);
}

// --- Wrappers mis à jour pour XL430 ---

void XL430_SetTorque(uint8_t id, uint8_t enable) {
    XL430_WriteByte(id, XL430_ADDR_TORQUE_ENABLE, enable);
}

void XL430_SetLED(uint8_t id, uint8_t enable) {
    // Note: XL430 n'a que LED ON/OFF (1/0), pas de couleurs
    XL430_WriteByte(id, XL430_ADDR_LED, enable);
}

void XL430_SetGoalPosition(uint8_t id, int32_t position) {

    if (id == 1) {

        int32_t position_physique = position * motor_1_sens;

        left_pos = position;

        XL430_WriteDWord(id, XL430_ADDR_GOAL_POSITION, (uint32_t)position_physique);
    }
    else if(id == 2){
        right_pos = position;
        XL430_WriteDWord(id, XL430_ADDR_GOAL_POSITION, (uint32_t)position);
    }
    else {

            XL430_WriteDWord(id, XL430_ADDR_GOAL_POSITION, (uint32_t)position);
        }
}
void XL430_SetGoalVelocity(uint8_t id, uint32_t velocity) {
    // Utile pour XL430
    XL430_WriteDWord(id, XL430_ADDR_GOAL_VELOCITY, velocity);
}

void XL430_SetStatusReturnLevel(uint8_t id, uint8_t level) {
    XL430_WriteByte(id, XL430_ADDR_STATUS_RETURN, level);
}

static uint32_t XL430_DegToTicks(float deg)
{
    return (uint32_t)((deg * 4096.0f) / 360.0f);
}

void XL430_SetPositionMode(uint8_t id)
{
    XL430_SetTorque(id, 0);
    HAL_Delay(10);

    XL430_WriteByte(id, XL430_ADDR_OPERATING_MODE, 3); // Position Control
    HAL_Delay(10);

    XL430_SetTorque(id, 1);
    HAL_Delay(10);
}

void XL430_GoToAngle(uint8_t id, float angle_deg)
{
    uint32_t ticks = XL430_DegToTicks(angle_deg);
    XL430_SetGoalPosition(id, ticks);
}


void XL430_SetExtendedPositionMode(uint8_t id)
{
    /* 1. Disable torque (obligatoire pour EEPROM) */
    XL430_SetTorque(id, 0);
    HAL_Delay(10);

    /* 2. Set Operating Mode = 4 (Extended Position) */
    XL430_WriteByte(id, XL430_ADDR_OPERATING_MODE, 4);
    HAL_Delay(50);

    /* 3. Enable torque */
    XL430_SetTorque(id, 1);
    HAL_Delay(10);
}

int32_t XL430_GetPresentPosition(uint8_t id) {

    // 1. Préparation du paquet de requête (14 octets)
    uint8_t tx[14] = {
        0xFF, 0xFF, 0xFD, 0x00, id, 0x07, 0x00, 0x02,
        XL430_ADDR_PRESENT_POSITION & 0xFF, XL430_ADDR_PRESENT_POSITION >> 8,
        0x04, 0x00, 0x00, 0x00
    };

    // Calcul CRC
    uint16_t crc = update_crc(0, tx, 12);
    tx[12] = crc & 0xFF;
    tx[13] = crc >> 8;

    // 2. Nettoyage avant transaction
    // On vide les flags d'erreur pour éviter que l'IT ne se bloque immédiatement
    __HAL_UART_CLEAR_FLAG(xl430_huart, UART_CLEAR_OREF | UART_CLEAR_FEF | UART_CLEAR_PEF);

    // On réinitialise le drapeau de réception
    xl430_rx_flag = 0;

    // 3. Transmission (En mode bloquant pour garantir le timing du switch TX/RX)
    // Le Half-Duplex est sensible : on veut être sûr d'avoir fini d'envoyer avant d'écouter.
    HAL_HalfDuplex_EnableTransmitter(xl430_huart);
    if(HAL_UART_Transmit(xl430_huart, tx, 14, 10) != HAL_OK) {
        return 0;
    }

    // IMPORTANT : Attendre que le dernier bit soit physiquement sorti
    while(__HAL_UART_GET_FLAG(xl430_huart, UART_FLAG_TC) == RESET);

    HAL_HalfDuplex_EnableReceiver(xl430_huart);

    if(HAL_UART_Receive_DMA(xl430_huart, xl430_rx_buffer, 15) != HAL_OK) {
        return 0;
    }


    uint32_t start_tick = HAL_GetTick();
    while (xl430_rx_flag == 0) {

        if ((HAL_GetTick() - start_tick) > 10) {
            // Si timeout, on annule la réception par interruption pour ne pas corrompre la suite
            HAL_UART_AbortReceive_IT(xl430_huart);
            return 0; // Erreur Timeout
        }
    }

    // 6. Traitement des données (Si on arrive ici, l'interruption a mis xl430_rx_flag à 1)

    // Vérification CRC
    uint16_t rx_crc = (uint16_t)(xl430_rx_buffer[13] | (xl430_rx_buffer[14] << 8));
    if (update_crc(0, xl430_rx_buffer, 13) == rx_crc && xl430_rx_buffer[8] == 0) {

        int32_t pos = (int32_t)(xl430_rx_buffer[9] | (xl430_rx_buffer[10] << 8) |
                                (xl430_rx_buffer[11] << 16) | (xl430_rx_buffer[12] << 24));

        return (id == 1) ? pos * motor_1_sens : pos;
    }

    return 0; // Erreur CRC ou Erreur Servo
}

int Revs_to_number(int rev){ //rev doit rester entre -256 et 256

	return rev*4096;
}

void XL430_SetVelocityMode(uint8_t id)
{
    // 1. Désactiver le couple (Obligatoire pour écrire dans l'EEPROM)
    XL430_SetTorque(id, 0);
    osDelay(10);

    // 2. Écrire 1 (Velocity Control Mode) à l'adresse Operating Mode (11)
    XL430_WriteByte(id, XL430_ADDR_OPERATING_MODE, 1);
    osDelay(10); // Laisse le temps au servo de sauvegarder

    // 3. Réactiver le couple
    XL430_SetTorque(id, 1);
    osDelay(10);
}

void Motor_Init(){
    XL430_Init(&huart3);
    XL430_SetVelocityMode(1);
    osDelay(20);
    XL430_SetVelocityMode(2);
    osDelay(20);
    XL430_SetPositionMode(3);
    osDelay(20);
    ARM_DOWN_2();
    osDelay(20);
    HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin,team_is_yellow);

}

void XL430_SetVelocity_Sync(uint8_t id1, int32_t val1, uint8_t id2, int32_t val2)
{
    uint8_t tx_buffer[30]; // Taille suffisante pour 2 moteurs
    uint16_t idx = 0;

    // Calcul de la longueur du paquet (Protocol 2.0)
    // Inst(1) + Addr(2) + LenData(2) + 2*(ID(1)+Data(4)) + CRC(2) = 17 octets (Length Field)
    // Packet Length Field = 17

    // 1. Header
    tx_buffer[idx++] = 0xFF;
    tx_buffer[idx++] = 0xFF;
    tx_buffer[idx++] = 0xFD;
    tx_buffer[idx++] = 0x00;
    tx_buffer[idx++] = 0xFE; // Broadcast ID
    tx_buffer[idx++] = 17;   // Longueur Low (17)
    tx_buffer[idx++] = 0;    // Longueur High
    tx_buffer[idx++] = 0x83; // SYNC WRITE

    // 2. Paramètres globaux (Adresse Vitesse = 104, Longueur = 4)
    tx_buffer[idx++] = 104; // 0x68
    tx_buffer[idx++] = 0x00;
    tx_buffer[idx++] = 0x04; // 4 octets de données
    tx_buffer[idx++] = 0x00;

    // --- MOTEUR 1 ---
    tx_buffer[idx++] = id1;
    if (id1 == 1) val1 = -val1; // Inversion signe si ID 1

    tx_buffer[idx++] = val1 & 0xFF;
    tx_buffer[idx++] = (val1 >> 8) & 0xFF;
    tx_buffer[idx++] = (val1 >> 16) & 0xFF;
    tx_buffer[idx++] = (val1 >> 24) & 0xFF;

    // --- MOTEUR 2 ---
    tx_buffer[idx++] = id2;
    if (id2 == 1) val2 = -val2; // Inversion signe si ID 1 (sécurité)

    tx_buffer[idx++] = val2 & 0xFF;
    tx_buffer[idx++] = (val2 >> 8) & 0xFF;
    tx_buffer[idx++] = (val2 >> 16) & 0xFF;
    tx_buffer[idx++] = (val2 >> 24) & 0xFF;

    // 3. CRC
    uint16_t crc = update_crc(0, tx_buffer, idx);
    tx_buffer[idx++] = crc & 0xFF;
    tx_buffer[idx++] = crc >> 8;

    // 4. Envoi
    HAL_HalfDuplex_EnableTransmitter(xl430_huart);
    HAL_UART_Transmit(xl430_huart, tx_buffer, idx, 10);
    while(__HAL_UART_GET_FLAG(xl430_huart, UART_FLAG_TC) == RESET);
}

void Robot_Wait_Until_Target(int32_t target1, int32_t target2, volatile PID_Config *pid) {
    int32_t err1 = 999;

    // On boucle tant que l'erreur est supérieure à 10 ticks (~0.2 degré)
    while (abs(err1) > 10) {
        // On utilise ta fonction PID que nous avons créée
        XL430_Update_PID(1, 2, target1, target2, pid);

        // On recalcule l'erreur pour la condition de sortie
        err1 = target1 - XL430_GetPresentPosition(1);

        HAL_Delay(10); // Fréquence d'échantillonnage 100Hz
    }

    // Stop complet à la fin
    XL430_SetVelocity_Sync(1, 0, 2, 0);
}

void XL430_Update_PID(uint8_t id1, uint8_t id2, int32_t target1, int32_t target2, volatile PID_Config *pid) {

    // 1. Lecture des positions actuelles
    int32_t pos1 = XL430_GetPresentPosition(id1);
    int32_t pos2 = XL430_GetPresentPosition(id2);

    // 2. Calcul des erreurs
    int32_t err1 = target1 - pos1;
    int32_t err2 = target2 - pos2;

    // 3. Mise à jour des intégrales séparément
    pid->integral_1 += err1;
    pid->integral_2 += err2;

    // 4. Calcul des dérivées séparément
    int32_t deriv1 = err1 - pid->last_error_1;
    int32_t deriv2 = err2 - pid->last_error_2;

    // 5. Calcul des vitesses avec les 3 termes (P, I, D) pour les DEUX moteurs
    int32_t vel1 = (int32_t)(pid->Kp * err1 + pid->Ki * pid->integral_1 + pid->Kd * deriv1);
    int32_t vel2 = (int32_t)(pid->Kp * err2 + pid->Ki * pid->integral_2 + pid->Kd * deriv2);

    // 6. Envoi synchronisé
    XL430_SetVelocity_Sync(id1, vel1, id2, vel2);

    // 7. Sauvegarde des erreurs pour le prochain cycle
    pid->last_error_1 = err1;
    pid->last_error_2 = err2;
}

void Robot_Move_Forward(float distance_mm){


    int32_t ticks_cible = MM_TO_TICKS(distance_mm);

    int32_t start1 = XL430_GetPresentPosition(1);
    int32_t start2 = XL430_GetPresentPosition(2);

    XL430_SetVelocity_Sync(1, 180, 2, 180);


    while (1) {

        int32_t current1 = XL430_GetPresentPosition(1);
        int32_t current2 = XL430_GetPresentPosition(2);


        int32_t parcouru1 = abs(current1 - start1);
        int32_t parcouru2 = abs(current2 - start2);


        if (parcouru1 >= ticks_cible || parcouru2 >= ticks_cible) {
            break;
        }

        osDelay(20);
    }

    XL430_SetVelocity_Sync(1, 0, 2, 0);
}

void Robot_Rotate(float angle_deg) {

    // 1. On rend l'angle positif pour le calcul absolu de la cible
    float angle_positif = angle_deg;
    if (angle_positif < 0) {
        angle_positif = -angle_positif;
    }

    // 2. Utilisation de votre macro pour calculer directement les ticks cible
    int32_t ticks_cible = DEG_TO_TICKS(angle_positif);

    // 3. Récupération de la position de départ
    int32_t start1 = XL430_GetPresentPosition(1);
    int32_t start2 = XL430_GetPresentPosition(2);

    // 4. Lancement des moteurs
    if (angle_deg > 0) {
        // Rotation Gauche (Anti-horaire)
        XL430_SetVelocity_Sync(1, -50, 2, 50);
    } else {
        // Rotation Droite (Horaire)
        XL430_SetVelocity_Sync(1, 50, 2, -50);
    }

    // 5. Boucle de surveillance
    while (1) {
        int32_t current1 = XL430_GetPresentPosition(1);
        int32_t current2 = XL430_GetPresentPosition(2);

        int32_t parcouru1 = abs(current1 - start1);
        int32_t parcouru2 = abs(current2 - start2);

        // Dès qu'une roue a atteint le nombre de ticks requis
        if (parcouru1 >= ticks_cible || parcouru2 >= ticks_cible) {
            break;
        }

        osDelay(10);
    }

    // 6. Arrêt des moteurs
    XL430_SetVelocity_Sync(1, 0, 2, 0);
}


void XL430_Square_Test(){

	for (int i = 0; i < 4; i++) {
	        Robot_Move_Forward(100.0);
	        Robot_Rotate(90.0);
	    }

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {


    if (huart->Instance == xl430_huart->Instance) {
        xl430_rx_flag = 1; // On signale que le buffer est plein
    }
}



void Robot_Move_Revolutions_Extended(float revolutions) {
    // 1. Calculate the number of ticks for the requested revolutions
    // 1 revolution = 4096 ticks
    int32_t delta_ticks = (int32_t)(revolutions * 4096.0f);

    // 2. Read the current "logical" position of both motors
    // Note: Your GetPresentPosition already accounts for motor_1_sens,
    // so we deal purely with logical forward/backward values here.
    int32_t start1 = XL430_GetPresentPosition(1);
    int32_t start2 = XL430_GetPresentPosition(2);

    // 3. Calculate the absolute target positions
    int32_t target1 = start1 + delta_ticks;
    int32_t target2 = start2 + delta_ticks;

    // 4. Send the new target positions
    // Your SetGoalPosition already handles the physical inversion for Motor 1
    XL430_SetGoalPosition(1, target1);
    XL430_SetGoalPosition(2, target2);

    // 5. Wait until both motors reach their targets
    while (1) {
        int32_t current1 = XL430_GetPresentPosition(1);
        int32_t current2 = XL430_GetPresentPosition(2);

        // Calculate the absolute error (distance remaining)
        int32_t error1 = abs(target1 - current1);
        int32_t error2 = abs(target2 - current2);

        // If both motors are within a 15-tick tolerance (~1.3 degrees), we consider the move complete
        if (error1 <= 15 && error2 <= 15) {
            break;
        }

        // Yield to the RTOS / prevent CPU hogging while waiting
        osDelay(10);
    }
}

void Robot_Move_Backwards(float distance_mm)
{
    // abs() pour garantir que ticks_cible est toujours positif
    int32_t ticks_cible = MM_TO_TICKS(distance_mm < 0 ? -distance_mm : distance_mm);

    int32_t start1 = XL430_GetPresentPosition(1);
    int32_t start2 = XL430_GetPresentPosition(2);

    // Vitesse négative = recul
    XL430_SetVelocity_Sync(1, -180, 2, -180);

    while (1)
    {
        int32_t current1 = XL430_GetPresentPosition(1);
        int32_t current2 = XL430_GetPresentPosition(2);

        int32_t parcouru1 = abs(current1 - start1);
        int32_t parcouru2 = abs(current2 - start2);

        if (parcouru1 >= ticks_cible || parcouru2 >= ticks_cible) {
            break;
        }

        osDelay(20);
    }

    XL430_SetVelocity_Sync(1, 0, 2, 0);
}

void ARM_UP(){
	XL430_GoToAngle(3, 240.0);
}
void ARM_DOWN_1(){
	XL430_GoToAngle(3, 180.0);
}
void ARM_DOWN_2(){
	XL430_GoToAngle(3, 150.0);
}
