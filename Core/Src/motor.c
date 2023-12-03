/*
 * motor.c
 *
 *  Created on: Dec 2, 2023
 *      Author: 小土豆
 */
#include "tim.h"
#include "usart.h"

#include "jy62.h"
#include "motor.h"
#include "pid.h"

#include <math.h>

int goal_speed[4];

void Move(uint8_t id, float pwm)
{
	if (id == 1)
	{
		if (pwm > 0)
		{// FL +
			HAL_GPIO_WritePin(F_in3_GPIO_Port, F_in3_Pin, 0);
			HAL_GPIO_WritePin(F_in4_GPIO_Port, F_in4_Pin, 1);
		}
		else
		{// FL -
			HAL_GPIO_WritePin(F_in3_GPIO_Port, F_in3_Pin, 1);
			HAL_GPIO_WritePin(F_in4_GPIO_Port, F_in4_Pin, 0);
		}
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, fabs(pwm));
	}
	else if (id == 2)
	{
		if (pwm > 0)
		{// FR +
			HAL_GPIO_WritePin(F_in1_GPIO_Port, F_in1_Pin, 0);
			HAL_GPIO_WritePin(F_in2_GPIO_Port, F_in2_Pin, 1);
		}
		else
		{// FR -
			HAL_GPIO_WritePin(F_in1_GPIO_Port, F_in1_Pin, 1);
			HAL_GPIO_WritePin(F_in2_GPIO_Port, F_in2_Pin, 0);
		}
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, fabs(pwm));
	}
	else if (id == 3)
	{
		if (pwm > 0)
		{// RL +
			HAL_GPIO_WritePin(R_in3_GPIO_Port, R_in3_Pin, 0);
			HAL_GPIO_WritePin(R_in4_GPIO_Port, R_in4_Pin, 1);
		}
		else
		{// RL -
			HAL_GPIO_WritePin(R_in3_GPIO_Port, R_in3_Pin, 1);
			HAL_GPIO_WritePin(R_in4_GPIO_Port, R_in4_Pin, 0);
		}
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, fabs(pwm));
	}
	else if (id == 4)
	{
		if (pwm > 0)
		{// RR +
			HAL_GPIO_WritePin(R_in1_GPIO_Port, R_in1_Pin, 0);
			HAL_GPIO_WritePin(R_in2_GPIO_Port, R_in2_Pin, 1);
		}
		else
		{// RR -
			HAL_GPIO_WritePin(R_in1_GPIO_Port, R_in1_Pin, 1);
			HAL_GPIO_WritePin(R_in2_GPIO_Port, R_in2_Pin, 0);
		}
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, fabs(pwm));
	}
}

void Update_Pwm(PosStr now, PosStr goal)
{

	float xVel = PID_Cal(&xPid, now.x, goal.x);
	float yVel = PID_Cal(&yPid, now.y, goal.y);
//	if (xVel > MAX_VELOCITY)
//		xVel = MAX_VELOCITY;
//	if (xVel < MIN_VELOCITY)
//		xVel = MIN_VELOCITY;
//	if (yVel > MAX_VELOCITY)
//		yVel = MAX_VELOCITY;
//	if (yVel < MIN_VELOCITY)
//		yVel = MIN_VELOCITY;

//	u1_printf("xVel=%f, yVel=%f\n", xVel, yVel)


	int FLCnt = __HAL_TIM_GET_COUNTER(&htim2);
	__HAL_TIM_SetCounter(&htim2, 0);
	if (FLCnt > 32767)
		FLCnt = 65535 - FLCnt;
	else
		FLCnt = 0 - FLCnt;
	float FLNow = 1.0 * FLCnt / 10.8;
	float FLPwm = PID_Cal(&FLPid, FLNow, xVel - yVel);
	Move(1, FLPwm);

	int FRCnt = __HAL_TIM_GET_COUNTER(&htim3);
	__HAL_TIM_SetCounter(&htim3, 0);
	if (FRCnt > 32767)
		FRCnt = FRCnt - 65535;
	else
		FRCnt = FRCnt - 0;
	float FRNow = 1.0 * FRCnt / 10.8;
	float FRPwm = PID_Cal(&FRPid, FRNow, xVel + yVel);
	Move(2, FRPwm);

	int RLCnt = __HAL_TIM_GET_COUNTER(&htim4);
	__HAL_TIM_SetCounter(&htim4, 0);
	if (RLCnt > 32767)
		RLCnt = 65535 - RLCnt;
	else
		RLCnt = 0 - RLCnt;
	float RLNow = 1.0 * RLCnt / 10.8;
	float RLPwm = PID_Cal(&RLPid, RLNow, xVel + yVel);
	Move(3, RLPwm);

	int RRCnt = __HAL_TIM_GET_COUNTER(&htim5);
	__HAL_TIM_SetCounter(&htim5, 0);
	if (RRCnt > 32767)
		RRCnt = RRCnt - 65535;
	else
		RRCnt = RRCnt - 0;
	float RRNow = 1.0 * RRCnt / 10.8;
	float RRPwm = PID_Cal(&RRPid, RRNow, xVel - yVel);
	Move(4, RRPwm);

//	u1_printf("FLNow=%f FLPwm=%f ", FLNow, FLPwm);
//	u1_printf("FRNow=%f FRPwm=%f\n", FRNow, FRPwm);
//	u1_printf("RLNow=%f RLPwm=%f ", RLNow, RLPwm);
//	u1_printf("RRNow=%f RRPwm=%f\n", RRNow, RRPwm);
}

void Mecanum(float vx, float vy, float w)
{
    float FL, FR, RL, RR;
    FL = (vx - vy - (LX + LY) * w) / R;
    FL = (vx + vy + (LX + LY) * w) / R;
    FL = (vx + vy - (LX + LY) * w) / R;
    FL = (vx - vy + (LX + LY) * w) / R;

    // 限制最大速度
    float max = fabs(FL);
    if (fabs(FR) > max)
        max = fabs(FR);
    if (fabs(RL) > max)
        max = fabs(RL);
    if (fabs(RR) > max)
        max = fabs(RR);
    if (max > MAX_VELOCITY)
    {
        FL = FL / max * MAX_VELOCITY;
        FR = FR / max * MAX_VELOCITY;
        RL = RL / max * MAX_VELOCITY;
        RR = RR / max * MAX_VELOCITY;
    }

    // 限制最小速度
    if (FL < MIN_VELOCITY)
        FL = MIN_VELOCITY;
    if (FR < MIN_VELOCITY)
        FR = MIN_VELOCITY;
    if (RL < MIN_VELOCITY)
        RL = MIN_VELOCITY;
    if (RR < MIN_VELOCITY)
        RR = MIN_VELOCITY;

    // 更新电机速度
    goal_speed[0] = FL;
    goal_speed[1] = FR;
    goal_speed[2] = RL;
    goal_speed[3] = RR;
}
