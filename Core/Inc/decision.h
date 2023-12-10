#include "zigbee_edc25.h"
extern uint8_t gameMap[64];
extern Position_edc25 now = {0, 0};
extern Position_edc25 goal = {0, 0};
typedef struct{
    uint8_t x;
    uint8_t y;
} Grid;
typedef enum{
    init=0,
    mine,
    move,
    protect,
    destroy,
    attack
} status;