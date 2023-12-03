/*
 * motor.h
 *
 *  Created on: Dec 2, 2023
 *      Author: 小土豆
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_
#endif /* INC_MOTOR_H_ */

#include "main.h"

#define MAX_VELOCITY 60
#define MIN_VELOCITY -60

typedef struct
{
	float x, y;
}PosStr;

void Move(uint8_t id, float pwm);
void Update_Pwm(PosStr now, PosStr goal);
