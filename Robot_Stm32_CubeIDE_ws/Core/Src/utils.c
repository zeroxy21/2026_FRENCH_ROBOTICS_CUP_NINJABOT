/*
 * utils.c
 *
 *  Created on: Jan 17, 2026
 *      Author: agheles
 */
#include"utils.h"

void Robot_Move_Until_TOF_Distance(uint16_t target_distance_mm)
{
    XL430_SetVelocity_Sync(1, 70, 2, 70);

    while (1)
    {
        // On lit directement ce que la tâche System a mis à jour
        // status != 255 signifie que la dernière lecture I2C a réussi
        if (tof_sensors[0].status != 255 && tof_sensors[0].distance_mm <= target_distance_mm)
        {
            break;
        }

        osDelay(50); // Inutile d'aller plus vite que la tâche System (~300ms)
    }

    XL430_SetVelocity_Sync(1, 0, 2, 0);
}

void HAND_STOP_DETECT(){

	if(tof_sensors[1].status != 255 && tof_sensors[1].distance_mm <= 100){
		XL430_SetVelocity_Sync(1, 0, 2, 0);
	}


}







