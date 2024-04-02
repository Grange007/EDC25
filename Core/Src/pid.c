/*
 * move.c
 *
 *  Created on: Nov 25, 2023
 *      Author: 蒟蒻是这样的
 */
#include "pid.h"

PidStr FLP_Pid, FRP_Pid, RLP_Pid, RRP_Pid;
PidStr FLN_Pid, FRN_Pid, RLN_Pid, RRN_Pid;
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
	a->max = max;
}

float PID_Cal(PidStr* a, float cur, float goal)
{
	a->err = goal - cur;
	a->iErr += a->err;

//	if (a->iErr > a->max)
//		a->iErr = a->max;
//	if (a->iErr < -a->max)
//		a->iErr = -a->max;

	float pwm = a->kp * a->err + a->ki * a->iErr + a->kd * (a->err - a->lErr);
	a->lErr = a->err;

	if (pwm >= MAX_PWM)
		pwm = MAX_PWM;
	if (pwm <= MIN_PWM)
		pwm = MIN_PWM;
	return pwm;
}

float Pos_Cal(PidStr* a, float cur, float goal)
{
	a->err = goal - cur;
	float v = a->kp * a->err;
	if (v >= MAX_VELOCITY)
		v = MAX_VELOCITY;
	if (v <= MIN_VELOCITY)
		v = MIN_VELOCITY;
	return v;
}

float Angle_Cal(PidStr* a, float cur, float goal)
{
	a->err = goal - cur;
	a->iErr += a->err;

	if (a->iErr > a->max)
		a->iErr = a->max;
	if (a->iErr < -a->max)
		a->iErr = -a->max;

	float o = a->kp * a->err + a->ki * a->iErr + a->kd * (a->err - a->lErr);
	a->lErr = a->err;
	
	if (o >= MAX_OMEGA)
		o = MAX_OMEGA;
	if (o <= MIN_OMEGA)
		o = MIN_OMEGA;
	return o;
}
