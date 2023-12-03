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

#define LX 10
#define LY 10
#define R 10

typedef struct
{
	float x, y;
}PosStr;

extern float goal_speed[4];
void Move(uint8_t id, float pwm);
void Update_Pwm();
void Mecanum_Speed(float vx, float vy, float w);
void Mecanum_Pos(PosStr now, PosStr goal);
