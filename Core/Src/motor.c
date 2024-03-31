/*
 * motor.c
 *
 *  Created on: Dec 2, 2023
 *      Author: 蒟蒻是这样的
 */


#include "motor.h"
#include "jy62.h"
#include "pid.h"
#include "tim.h"
#include "usart.h"

#include <math.h>

float goal_speed[4];

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

void Update_Pwm()
{
	int FLCnt = __HAL_TIM_GET_COUNTER(&htim2);
	__HAL_TIM_SetCounter(&htim2, 0);
	if (FLCnt > 32767)
		FLCnt = 65535 - FLCnt;
	else
		FLCnt = 0 - FLCnt;
	float FLNow = 1.0 * FLCnt / UNKNOWN;
	float FLPwm;
	if (goal_speed[0] >= 0)
		FLPwm = PID_Cal(&FLP_Pid, FLNow, goal_speed[0]);
	else
		FLPwm = PID_Cal(&FLN_Pid, FLNow, goal_speed[0]);
	Move(1, FLPwm);

	int FRCnt = __HAL_TIM_GET_COUNTER(&htim3);
	__HAL_TIM_SetCounter(&htim3, 0);
	if (FRCnt > 32767)
		FRCnt = FRCnt - 65535;
	else
		FRCnt = FRCnt - 0;
	float FRNow = 1.0 * FRCnt / UNKNOWN;
	float FRPwm;
	if (goal_speed[1] >= 0)
		FRPwm = PID_Cal(&FRP_Pid, FRNow, goal_speed[1]);
	else
		FRPwm = PID_Cal(&FRN_Pid, FRNow, goal_speed[1]);
	Move(2, FRPwm);

	int RLCnt = __HAL_TIM_GET_COUNTER(&htim4);
	__HAL_TIM_SetCounter(&htim4, 0);
	if (RLCnt > 32767)
		RLCnt = 65535 - RLCnt;
	else
		RLCnt = 0 - RLCnt;
	float RLNow = 1.0 * RLCnt / UNKNOWN;
	float RLPwm;
	if (goal_speed[2] >= 0)
		RLPwm = PID_Cal(&RLP_Pid, RLNow, goal_speed[2]);
	else
		RLPwm = PID_Cal(&RLN_Pid, RLNow, goal_speed[2]);
	Move(3, RLPwm);

	int RRCnt = __HAL_TIM_GET_COUNTER(&htim5);
	__HAL_TIM_SetCounter(&htim5, 0);
	if (RRCnt > 32767)
		RRCnt = RRCnt - 65535;
	else
		RRCnt = RRCnt - 0;
	float RRNow = 1.0 * RRCnt / UNKNOWN;
	float RRPwm;
	if (goal_speed[3] >= 0)
		RRPwm = PID_Cal(&RRP_Pid, RRNow, goal_speed[3]);
	else
		RRPwm = PID_Cal(&RRN_Pid, RRNow, goal_speed[3]);
	Move(4, RRPwm);

//	u1_printf("FLNow=%f FLPwm=%f ", FLNow, FLPwm);
//	u1_printf("FRNow=%f FRPwm=%f\n", FRNow, FRPwm);
//	u1_printf("RLNow=%f RLPwm=%f ", RLNow, RLPwm);
//	u1_printf("RRNow=%f RRPwm=%f\n", RRNow, RRPwm);
}

void Mecanum_Speed(float vx, float vy, float w)
{
    float FL, FR, RL, RR;
    FL = (vx + vy + (LX + LY) * w) / R;
    FR = (vx - vy - (LX + LY) * w) / R;
    RL = (vx - vy + (LX + LY) * w) / R;
    RR = (vx + vy - (LX + LY) * w) / R;

//    u1_printf("vx:%f vy:%f w:%f\n", vx, vy, w);
//    u1_printf("FL:%f FR:%f RL:%f RR:%f\n", FL, FR, RL, RR);

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

    // 更新电机速度
    goal_speed[0] = FL;
    goal_speed[1] = FR;
    goal_speed[2] = RL;
    goal_speed[3] = RR;
}

void Mecanum_Pos(Position_edc25 now, Position_edc25 goal)
{
	float yaw = GetYaw();
	if (yaw > 180)
		yaw = yaw - 360;
	else
		yaw = yaw - 0;
	// u1_printf("%f\n", yaw);

	float vx = PID_Cal(&xPid, now.posx, goal.posx);
	float vy = PID_Cal(&yPid, now.posy, goal.posy);
	float w = PID_Cal(&anglePid, yaw, 0);
	Mecanum_Speed(vx, vy, w);
//	Mecanum_Speed(50.0f, 0.0f, w);
}
