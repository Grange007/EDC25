
/*注：注释未对齐确实不是编写代码的同学没有注意对齐，而是从vscode粘贴过来之后已经对齐的注释出现错位了
 *反之，在wolai代码块中对齐的注释粘贴回vscode中也会出现一样的情况，所以这里也没有重新对齐注释，
 *向大家道歉，没有办法满足阅读代码美观性的需求
 */
#include "jy62.h"

volatile uint8_t jy62Receive[JY62_MESSAGE_LENGTH];  //实时记录收到的信息
volatile uint8_t jy62Message[JY62_MESSAGE_LENGTH];  //确认无误后用于解码的信息
uint8_t initAngle[3] = {0xFF, 0xAA, 0x52};
uint8_t calibrateAcce[3] = {0xFF, 0xAA, 0x67};
uint8_t setBaud115200[3] = {0xFF, 0xAA, 0x63};
uint8_t setBaud9600[3] = {0xFF, 0xAA, 0x64};
uint8_t setHorizontal[3] = {0xFF, 0xAA, 0x65};
uint8_t setVertical[3] = {0xFF, 0xAA, 0x66};
uint8_t sleepAndAwake[3] = {0xFF, 0xAA, 0x60};
UART_HandleTypeDef* jy62_huart;

struct Acce Accelerate;                //储存加速度
struct Velo Velocity;                //储存角速度
struct Angl Angle;                  //储存角度值
struct Temp Temperature;                //储存温度值
/***********************接口****************************/

void jy62_Init(UART_HandleTypeDef *huart)
{
  jy62_huart = huart;
  HAL_UART_Receive_DMA(jy62_huart, jy62Receive, JY62_MESSAGE_LENGTH);
}

//void jy62MessageRecord(void)
//{
//  int idx = 0;                              //接受数据位指针
//  int i = 0;
//  while(idx < JY62_MESSAGE_LENGTH){                    //在判断JY62_MESSAGE_LENGTH(200)的长度内进行查找
//    while(jy62Receive[idx] != 0x55) idx++;                //接收数据为指针找到包头0x55
//    uint8_t sum = 0;                          //计算校验位
//    for(i = 0; i < 10; i++) sum += jy62Receive[idx + i];        //判断校验位正确与否
//    if(sum == jy62Receive[idx + 10]){                  //校验位正确，认为收到正确包
//      for(i = 0; i < 11; i++) jy62Message[i] = jy62Receive[idx + i];  //读入jy62Message中
//      Decode();                           //解析
//      idx += 11;                            //继续下一个包读入
//    }else{
//      idx++;                              //校验位错误，数据指针向后挪动，继续找到合理的数据包
//    }
//  }
//  HAL_UART_Receive_DMA(jy62_huart, jy62Receive, JY62_MESSAGE_LENGTH);    //吃完一次jy62Receive数组，继续吃下一个
//}

void jy62MessageRecord(void)
{
  static uint8_t i = 0, sum = 0;
  for (int j = 0; j < JY62_MESSAGE_LENGTH; j++){
    if (i == 0 && jy62Receive[j] != 0x55) continue;
    if (i == 10){
        if (sum == jy62Receive[j]){
            jy62Message[i++] = jy62Receive[j];
            Decode();
        }
        i = 0;
        sum = 0;
        continue;
    }
    sum += jy62Receive[j];
    jy62Message[i++] = jy62Receive[j];
  }
  HAL_UART_Receive_DMA(jy62_huart, jy62Receive, JY62_MESSAGE_LENGTH);
}

void SetBaud(int Baud)
{
  if(Baud == 115200)
  {
    HAL_UART_Transmit(jy62_huart,setBaud115200, 3, HAL_MAX_DELAY);
  }
  else if(Baud == 9600)
  {
    HAL_UART_Transmit(jy62_huart, setBaud9600, 3, HAL_MAX_DELAY);
  }
}

void SetHorizontal()
{
  HAL_UART_Transmit(jy62_huart, setHorizontal, 3, HAL_MAX_DELAY);
}

void SetVertical()
{
  HAL_UART_Transmit(jy62_huart, setVertical, 3, HAL_MAX_DELAY);
}

void InitAngle()
{
  HAL_UART_Transmit(jy62_huart, initAngle, 3, HAL_MAX_DELAY);
}

void Calibrate()
{
  HAL_UART_Transmit(jy62_huart, calibrateAcce, 3, HAL_MAX_DELAY);
}

void SleepOrAwake()
{
  HAL_UART_Transmit(jy62_huart, sleepAndAwake, 3, HAL_MAX_DELAY);
}


float GetRoll()
{
  return Angle.roll;
}
float GetPitch()
{
  return Angle.pitch;
}
float GetYaw()
{
  return Angle.yaw;
}

float GetTemperature()
{
  return Temperature.temperature;
}

float GetAccX()
{
  return Accelerate.accelerate_x;
}
float GetAccY()
{
  return Accelerate.accelerate_y;
}
float GetAccZ()
{
  return Accelerate.accelerate_z;
}

float GetVeloX()
{
  return Velocity.velocity_x;
}
float GetVeloY()
{
  return Velocity.velocity_y;
}
float GetVeloZ()
{
  return Velocity.velocity_z;
}


/***************************************************/

void DecodeAngle()
{

  Angle.roll = (float)((jy62Message[3] << 8) | jy62Message[2]) / 32768 * 180;
  Angle.pitch = (float)((jy62Message[5] << 8) | jy62Message[4]) / 32768 * 180;
  Angle.yaw = (float)((jy62Message[7] << 8) | jy62Message[6]) / 32768 * 180;
}

void DecodeAccelerate()
{
  Accelerate.accelerate_x = (float)((jy62Message[3] << 8) | jy62Message[2]) / 32768 * 16 * g ;
  Accelerate.accelerate_y = (float)((jy62Message[5] << 8) | jy62Message[4]) / 32768 * 16 * g ;
  Accelerate.accelerate_z = (float)((jy62Message[7] << 8) | jy62Message[6]) / 32768 * 16 * g ;
}

void DecodeVelocity()
{
  Velocity.velocity_x = (float)((jy62Message[3] << 8) | jy62Message[2]) / 32768 * 2000 ;
  Velocity.velocity_y = (float)((jy62Message[5] << 8) | jy62Message[4]) / 32768 * 2000 ;
  Velocity.velocity_z = (float)((jy62Message[7] << 8) | jy62Message[6]) / 32768 * 2000 ;
}

void DecodeTemperature()
{
  Temperature.temperature = ((short)(jy62Message[9]) << 8 | jy62Message[8]) / 340 + 36.53;
}


void Decode()
{
  switch (jy62Message[1])
  {
      case 0x51: DecodeAccelerate(); break;
    case 0x52: DecodeVelocity(); break;
    case 0x53: DecodeAngle(); break;
  }
  DecodeTemperature();
}
