#include "zigbee_edc25.h"
extern uint8_t gameMap[64];
extern Position_edc25 now;
extern Position_edc25 goal;
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
