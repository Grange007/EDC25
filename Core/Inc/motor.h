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

//#define LX 1.73f
//#define LY 1.81f

#define MAX_SPEED 2.5
#define MIN_SPEED 1.0
#define UNKNOWN 10.8

extern float goal_speed[4];
extern float FL, FR, RL, RR;
extern float yaw;

void Move(uint8_t id, float pwm);
void Update_Dual_Pwm();
void Update_Single_Pwm();
void Mecanum_Speed(float vx, float vy, float w);
void Mecanum_Pos(Position_edc25 now, Position_edc25 goal);
