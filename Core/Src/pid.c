/*
 * move.c
 *
 *  Created on: Nov 25, 2023
 *      Author: 蒟蒻是这样的
 */
#include "pid.h"

PidStr FLPid, FRPid, RLPid, RRPid;
PidStr xPid, yPid;
PidStr anglePid;

void PID_Init(PidStr* a, float kp, float ki, float kd, float max)
{
	a->err = 0;
	a->iErr = 0;
	a->lErr = 0;
	a->kp = kp;
	a->ki = ki;
	a->kd = kd;
}

float PID_Cal(PidStr* a, float cur, float goal)
{
	a->err = goal - cur;
	a->iErr += a->err;

	if (a->iErr > a->max)
		a->iErr = a->max;
	if (a->iErr < -a->max)
		a->iErr = -a->max;

	float pwm = a->kp * a->err + a->ki * a->iErr + a->kd * (a->err - a->lErr);
	a->lErr = a->err;
	if (pwm >= MAX_PWM)
		pwm = MAX_PWM;
	if (pwm <= MIN_PWM)
		pwm = MIN_PWM;
	return pwm;
}
