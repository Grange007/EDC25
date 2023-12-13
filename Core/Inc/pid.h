/*
 * move.h
 *
 *  Created on: Nov 25, 2023
 *      Author: 蒟蒻是这样的
 */

#ifndef INC_MOVE_H_
#define INC_MOVE_H_
#endif /* INC_MOVE_H_ */

#define MAX_PWM 800
#define MIN_PWM -800

typedef struct
{
	float kp, ki, kd;
	float err, iErr, lErr;
	float max;
}PidStr;

extern PidStr FLPid, FRPid, RLPid, RRPid;
extern PidStr xPid, yPid;
extern PidStr anglePid;

void PID_Init(PidStr* a, float kp, float ki, float kd, float max);
float PID_Cal(PidStr* a, float cur, float goal);


