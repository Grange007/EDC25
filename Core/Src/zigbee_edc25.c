#include "jy62.h"
#include "zigbee_edc25.h"

#include "main.h"
#include "usart.h"

#define MAX_SINGLE_MSG 95 // �?�?�?
#define MAX_MSG_LEN 150
#define MAX_STATUS_LEN 150

uint8_t zigbeeRaw[MAX_MSG_LEN];         // Raw zigbee data
uint8_t zigbeeMessage[MAX_MSG_LEN * 2]; // Double the size to save a complete message
int32_t memPtr = 0;
uint8_t cutavoid[4];//In case bytenum or time be wrong
// We separate the message in memory into 4 sections, with each length MAX_MSG_LEN/2.
// In each DMA half/full CpltCallback, memPtr will be updated and the data will be transferred
// from zigbeeRaw into the correct section in zigbeeMessage.
// This is set to improve the call back frequency and deal with different frame lengths.

uint8_t gameStatusMessage[MAX_STATUS_LEN];

UART_HandleTypeDef *zigbee_huart;

int32_t modularAdd(int32_t a, int32_t b, int32_t max) // In case modular does not work for negative numbers
{
    int32_t c;
    c = a + b;
    if (c >= max)
        c -= max;
    if (c < 0)
        c += max;
    return c;
}

static uint8_t calculateChecksum(uint8_t tempMessage[], int32_t start_idx, int32_t count)
{
    uint8_t checksum = 0;
    for (int32_t i = 0; i < count; ++i)
    {
        checksum ^= tempMessage[modularAdd(i, start_idx, MAX_MSG_LEN * 2)];
    }
    return checksum;
}

static float changeFloatData(uint8_t *dat)
{
    float float_data;
    float_data = *((float *)dat);
    return float_data;
}


void zigbee_Init(UART_HandleTypeDef *huart)
{
    memset(zigbeeMessage, 0x00, MAX_MSG_LEN);
    memset(zigbeeRaw, 0x00, MAX_MSG_LEN);
    memset(gameStatusMessage, 0x00, MAX_STATUS_LEN);
    zigbee_huart = huart;
    HAL_UART_Receive_DMA(zigbee_huart, zigbeeRaw, MAX_MSG_LEN);
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == jy62_huart)
    {
        jy62MessageRecord();
    }
    if (huart == zigbee_huart)
    {
        uint8_t *zigbeeMsgPtr = &zigbeeMessage[memPtr];
        uint8_t *rawPtr = &zigbeeRaw[0];
        memcpy(zigbeeMsgPtr, rawPtr, sizeof(uint8_t) * MAX_MSG_LEN / 2);
        memPtr = modularAdd(MAX_MSG_LEN / 2, memPtr, MAX_MSG_LEN * 2);
        zigbeeMessageRecord();
        // zigbeeMessageRecord is completed almost instantly in the callback function.
        // Please don't add u1_printf into the function.
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//    if (huart == jy62_huart)
//    {
//        jy62MessageRecord();
//    }
    if (huart == zigbee_huart)
    {
        uint8_t *zigbeeMsgPtr = &zigbeeMessage[memPtr];
        uint8_t *rawPtr = &zigbeeRaw[MAX_MSG_LEN / 2];
        memcpy(zigbeeMsgPtr, rawPtr, sizeof(uint8_t) * MAX_MSG_LEN / 2);
        memPtr = modularAdd(MAX_MSG_LEN / 2, memPtr, MAX_MSG_LEN * 2);
        zigbeeMessageRecord();
    }
}

uint8_t zigbeeMessageRecord()
{

    int32_t msgIndex = 0;
    uint8_t tempZigbeeMessage[MAX_MSG_LEN * 2];
    memcpy(tempZigbeeMessage, zigbeeMessage, MAX_MSG_LEN * 2);
    // In case zigbeeMessage updates in the interrupt during the loop

    int32_t prevMemPtr = memPtr; // In case memPtr changes in the interrupt during the loop
    int16_t byteNum;
    // find the first 0x55 of msgType
    for (msgIndex = modularAdd(prevMemPtr, -MAX_SINGLE_MSG, MAX_MSG_LEN * 2); msgIndex != prevMemPtr;)
    // A message is at most 30 bytes long. We find the header of the first full message
    {
        if (tempZigbeeMessage[msgIndex] == 0x55 &&
            tempZigbeeMessage[modularAdd(msgIndex, 1, MAX_MSG_LEN * 2)] == 0xAA)
        {
            
            
            cutavoid[0] = tempZigbeeMessage[modularAdd(msgIndex, 2, MAX_MSG_LEN * 2)];
            cutavoid[1] = tempZigbeeMessage[modularAdd(msgIndex, 3, MAX_MSG_LEN * 2)];
            byteNum = *((int16_t*)(cutavoid));
            
            uint8_t tmpchecksum;
            tmpchecksum = calculateChecksum(tempZigbeeMessage, modularAdd(msgIndex, 5, MAX_MSG_LEN * 2), byteNum);
            if (tmpchecksum == tempZigbeeMessage[modularAdd(msgIndex, 4, MAX_MSG_LEN * 2)])
            {
                break;
            }
        }
        msgIndex = modularAdd(msgIndex, -1, MAX_MSG_LEN * 2);
    }
    if (msgIndex == prevMemPtr)
    {
        return 1;
    }
    
    int32_t prevTime, newTime;
    prevTime = getGameTime();
    for(int32_t i = 0;i < 4;i++)
    {
        cutavoid[i] =  tempZigbeeMessage[modularAdd(msgIndex, 5 + 1 + i, MAX_MSG_LEN * 2)];
    }
    newTime = *((int32_t *)(cutavoid));
    if (newTime >= prevTime && newTime <= prevTime + 1000)
    {
        memset(gameStatusMessage, 0x00, MAX_STATUS_LEN);
        for (int32_t i = 0; i < byteNum; i++)
        {
            gameStatusMessage[i] = tempZigbeeMessage[modularAdd(msgIndex, 5 + i, MAX_MSG_LEN * 2)];
        }
    }
    return 0;
}

int32_t getGameTime()
{
    int32_t time;
    time = *((int32_t *)(&gameStatusMessage[1]));
    return time;
}

GameStage_edc25 getGameStage()
{
    uint8_t stage;
    stage = gameStatusMessage[0];
    return (GameStage_edc25)stage;
}

void getHeightOfAllChunks(uint8_t *dest)
{
    memcpy(dest, gameStatusMessage+5, 64);
    for(int i = 0;i<64;i++)
    {
        dest[i] = dest[i]&0x0F;
    }
}
void getOreKindOfAllChunks(uint8_t *dest)//0 mean Iron, 1 mean Gold, 2 mean Diamond, 3 mean None
{
    memcpy(dest, gameStatusMessage+5, 64);
    for(int i = 0;i<64;i++)
    {
        dest[i] = dest[i]>>4;
    }
}

uint8_t getHeightOfId(uint8_t id)
{  
    
    return gameStatusMessage[5 + id]&0x0F;
      
}
uint8_t getOreKindOfId(uint8_t id)//0 mean Iron, 1 mean Gold, 2 mean Diamond, 3 mean None
{
    
    return gameStatusMessage[5 + id]>>4;
   
}


bool hasBed()
{
    return (bool)gameStatusMessage[69];
}

bool hasBedOpponent()
{
    return (bool)gameStatusMessage[70];
}

void getPosition(Position_edc25 *Pos)
{
    Pos->posx = changeFloatData(gameStatusMessage + 71);
    Pos->posy = changeFloatData(gameStatusMessage + 75);
}

void getPositionOpponent(Position_edc25 *Pos)
{
    Pos->posx = changeFloatData(gameStatusMessage + 79);
    Pos->posy = changeFloatData(gameStatusMessage + 83);
}

uint8_t getAgility()
{
    return gameStatusMessage[87];
}

uint8_t getHealth()
{
    return gameStatusMessage[88];
}

uint8_t getMaxHealth()
{
    return gameStatusMessage[89];
}

uint8_t getStrength()
{
    return gameStatusMessage[90];
}

uint8_t getEmeraldCount()
{
    return gameStatusMessage[91];
}

uint8_t getWoolCount()
{
    return gameStatusMessage[92];
}

void attack_id(uint8_t chunk_id)
{
    uint8_t slaver_msg[7] = {0x55, 0xAA, 0x02, 0x00, (uint8_t)(0^chunk_id), 0, chunk_id};
    HAL_UART_Transmit(zigbee_huart, slaver_msg, 7, HAL_MAX_DELAY);
}

void place_block_id(uint8_t chunk_id)
{
    uint8_t slaver_msg[7] = {0x55, 0xAA, 0x02, 0x00, (uint8_t)(1^chunk_id), 1, chunk_id};
    HAL_UART_Transmit(zigbee_huart, slaver_msg, 7, HAL_MAX_DELAY);
}

void trade_id(uint8_t item_id)
{
    uint8_t slaver_msg[7] = {0x55, 0xAA, 0x02, 0x00, (uint8_t)(2^item_id), 2, item_id};
    HAL_UART_Transmit(zigbee_huart, slaver_msg, 7, HAL_MAX_DELAY);
}
