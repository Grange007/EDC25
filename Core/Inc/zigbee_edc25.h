#ifndef ZIGBEE_H_
#define ZIGBEE_H_
#include "stdbool.h"
#include "stm32f1xx_hal.h"
#include "string.h"

#define MAX_SINGLE_MSG 95 // 可修正
#define MAX_MSG_LEN 150
#define MAX_STATUS_LEN 150

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

extern uint8_t zigbeeRaw[MAX_MSG_LEN];
extern uint8_t zigbeeMessage[MAX_MSG_LEN * 2];
extern int32_t memPtr;
extern uint8_t gameStatusMessage[MAX_STATUS_LEN];
extern UART_HandleTypeDef *zigbee_huart;

void zigbee_Init(UART_HandleTypeDef *huart); // 初始化,开始接收消息
uint8_t zigbeeMessageRecord();                  // 刷新消息

// 获取游戏时间
int32_t getGameTime();

// 获取游戏阶段
GameStage_edc25 getGameStage();

// 获取所有块的高度信息
void getHeightOfAllChunks(uint8_t *dest);

// 根据ID获取块的高度
uint8_t getHeightOfId(uint8_t id);

// 检查玩家是否有床
bool hasBed();

// 检查对手是否有床
bool hasBedOpponent();

// 获取玩家位置
void getPosition(Position_edc25 *Pos);

// 获取对手位置
void getPositionOpponent(Position_edc25 *Pos);

// 获取敏捷
uint8_t getAgility();

// 获取当前生命
uint8_t getHealth();

// 获取最大生命
uint8_t getMaxHealth();

// 获取力量
uint8_t getStrength();

// 获取绿宝石数量
uint8_t getEmeraldCount();

// 获取羊毛数量
uint8_t getWoolCount();

// 攻击指定ID的位置
void attack_id(uint8_t chunk_id);

// 在指定ID的块上放置方块
void place_block_id(uint8_t chunk_id);

// 指定ID的物品交易
void trade_id(uint8_t item_id);

#endif
