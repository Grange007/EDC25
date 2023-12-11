/*
说明：
1.把.h/.c文件放到Inc和Src里面，然后在Makefile里面添加.c文件的路径
2.开启对应串口DMA中断以及串口全局中断，在中断回调函数里面添加以下代码：

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
 {
   
    if(huart == (jy62选择的串口))
    {
      jy62MessageRecord();
    }
}

3.在main.c中User Code 2 处使能中断

void jy62_Init(UART_HandleTypeDef* huart); 

4.之后可在主函数中调用下面的接口函数来获得相应值

5.注释未对齐确实不是编写代码的同学没有注意对齐，而是从vscode粘过来之后已经对齐的注释出现错位了。
反之，在wolai代码块中对齐的注释粘贴回vscode中也会出现一样的情况，所以这里也没有重新对齐注释，
向大家道歉，没有办法满足代码美观性的需求
*/

#ifndef JY62_H
#define JY62_H
#include "stm32f1xx_hal.h"
#define JY62_MESSAGE_LENGTH 200 //开大数组放置读入数据时，一个数据包横跨两次接受数据的情况
#define g 9.8                   //定义重力加速度值

//加速度
struct Acce
{
    float accelerate_x;
    float accelerate_y;
    float accelerate_z;
};
//角速度
struct Velo
{
    float velocity_x;
    float velocity_y;
    float velocity_z;
};

struct Angl 
{
    float roll;
    float pitch;
    float yaw;
};


struct Temp
{
    float temperature;
};

extern UART_HandleTypeDef* jy62_huart;

/**************接口*************************/
void jy62_Init(UART_HandleTypeDef* huart);          //初始化
void jy62MessageRecord(void);            //实时记录信息，在每次接收完成后更新数据，重新开启中断

void SetBaud(int Baud);                             //设置波特率，可选择115200或9600
void SetHorizontal(void);                           //设置为水平放置模式
void SetVertical(void);                             //设置为垂直放置模式
void InitAngle(void);                               //初始化z轴角度为0
void Calibrate(void);                               //校准加速度计
void SleepOrAwake(void);                            //设置休眠/唤醒

float GetVeloX(void);                               //获得角速度
float GetVeloY(void);
float GetVeloZ(void);

float GetAccX(void);                                //获得加速度
float GetAccY(void);
float GetAccZ(void);

float GetRoll(void);                                //绕x轴旋转角度（横滚角）
float GetPitch(void);                               //绕y轴旋转角度（俯仰角）
float GetYaw(void);                                 //绕z轴旋转角度（偏航角）
float GetTemperature(void);

void Decode();

#endif
