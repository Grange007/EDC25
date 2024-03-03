#ifndef __DECISION_H
#define __DECISION_H

#include "zigbee_edc25.h"

#define	blank 0
#define	iron 1
#define gold 2
#define diamond 3

typedef struct{
    uint8_t x;
    uint8_t y;
} Grid;

typedef enum
{
    init = 0,
	dead,
	protect,
    destroy,
    attack,
	mine,
	get_wool,
	get_enhanced,
} Status;

extern uint8_t gameMap[64];
extern Status status;

extern uint8_t agility;
extern uint8_t health;
extern uint8_t maxHealth;
extern uint8_t wool;
extern uint8_t emerald;
extern uint8_t time;

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

Grid bellmanford(Grid source, Grid target, int *needBlock);

void homeProtect();
void statusChange();
void ready_func();
void init_func();
void dead_func();

void protect_func();
void destroy_func();
void attack_func();
void mine_func();
void get_wool_func();
void get_enhanced_func();

void place_and_move();
void find_optimal_mine();
#endif
