#include "zigbee_edc25.h"
extern uint8_t gameMap[64];

typedef struct{
    uint8_t x;
    uint8_t y;
} Grid;

typedef enum
{
	blank = 0,
	iron,
	gold,
	diamond
} MapType;

typedef enum
{
    init = 0,
	dead,
	poverty,
	move,
    mine,
    protect,
    destroy,
    attack
} Status;

extern Status status;

extern Grid nowGrid;
extern Grid goalGrid;
extern Grid desGrid;
extern Grid opGrid;
extern Grid homeGrid;
extern Grid opHomeGrid;

extern Position_edc25 now;
extern Position_edc25 des;
extern Position_edc25 goal;
extern Position_edc25 op;
extern Position_edc25 home;
extern Position_edc25 opHome;

uint8_t mhtDst(Grid from, Grid to);
uint8_t grid2No(Grid grid);

Grid no2Grid(uint8_t no);
Grid pos2Grid(Position_edc25 pos);
Position_edc25 grid2Pos(Grid grid);

Grid nearestBlock(uint8_t type);
Grid getNext(Grid from, Grid to);

void statusCheck();
void init_func();
void move_func();
void ready_func();
void protect_func();
void mine_func();
void destroy_func();
void attack_func();
void mainLoop();
