#ifndef ZIGBEE_H_
#define ZIGBEE_H_
#include "stdbool.h"
#include "stm32f1xx_hal.h"
#include "string.h"

typedef enum
{
    READY,
    RUNNING,
    BATTLING,
    FINISHED
} GameStage_edc25;

typedef struct
{
    float posx;
    float posy;
} Position_edc25;

void zigbee_Init(UART_HandleTypeDef *huart); // ��ʼ��,��ʼ������Ϣ
uint8_t zigbeeMessageRecord();                  // ˢ����Ϣ

// ��ȡ��Ϸʱ��
int32_t getGameTime();

// ��ȡ��Ϸ�׶�
GameStage_edc25 getGameStage();

// ��ȡ���п�ĸ߶���Ϣ
void getHeightOfAllChunks(uint8_t *dest);

// ����ID��ȡ��ĸ߶�
uint8_t getHeightOfId(uint8_t id);

// �������Ƿ��д�
bool hasBed();

// �������Ƿ��д�
bool hasBedOpponent();

// ��ȡ���λ��
void getPosition(Position_edc25 *Pos);

// ��ȡ����λ��
void getPositionOpponent(Position_edc25 *Pos);

// ��ȡ����
uint8_t getAgility();

// ��ȡ��ǰ����
uint8_t getHealth();

// ��ȡ�������
uint8_t getMaxHealth();

// ��ȡ����
uint8_t getStrength();

// ��ȡ�̱�ʯ����
uint8_t getEmeraldCount();

// ��ȡ��ë����
uint8_t getWoolCount();

// ����ָ��ID��λ��
void attack_id(uint8_t chunk_id);

// ��ָ��ID�Ŀ��Ϸ��÷���
void place_block_id(uint8_t chunk_id);

// ָ��ID����Ʒ����
void trade_id(uint8_t item_id);

#endif