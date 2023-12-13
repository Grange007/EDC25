/*
 * motor.h
 *
 *  Created on: Dec 2, 2023
 *      Author: 蒟蒻是这样的
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_
#endif /* INC_MOTOR_H_ */

#include "zigbee_edc25.h"

#define UNKNOWN 10.8

#define MAX_VELOCITY 60

#define LX 1.73
#define LY 1.81
#define R 0.3

extern float goal_speed[4];

void Move(uint8_t id, float pwm);
void Update_Pwm();
void Mecanum_Speed(float vx, float vy, float w);
void Mecanum_Pos(Position_edc25 now, Position_edc25 goal);
