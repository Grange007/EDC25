#ifndef __DECISION_H
#define __DECISION_H

#include "zigbee_edc25.h"

#define	blank 3
#define	iron 0
#define gold 1
#define diamond 2

typedef struct
{
    uint8_t x;
    uint8_t y;
} Grid;

typedef struct
{
    Grid pos;
    uint8_t type;
}  OreInfo;

typedef enum
{
    init = 0,
	dead,
    protect,
    purchase,
    Pmove,
    Pdestroy,
	Nmove,
	Ndestroy,
	recover,
	upgrade
} Status;

extern uint8_t gameMap[64];
extern Status status;

extern uint8_t agility;
extern uint8_t health;
extern uint8_t wool;
extern uint8_t emerald;
extern uint8_t time;
extern uint8_t strength;
extern uint8_t team;

extern uint8_t maxHealth;
extern int8_t lastTime;

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

extern OreInfo ore[64];
extern int oreNum;

uint8_t mhtDst(Grid from, Grid to);
uint8_t grid2No(Grid grid);
Grid no2Grid(uint8_t no);
Grid pos2Grid(Position_edc25 pos);
Position_edc25 grid2Pos(Grid grid);

Grid nearestBlock(uint8_t type);

int getAccumulatedNumberOfOre(Grid OrePos);
void getPositionOfAllOre();
Grid findMostValuableBlock(Grid source);

Grid bellmanford(Grid source, Grid target, int *needBlock);
int bellmanford_distance(Grid source, Grid target, int *needBlock);

uint8_t getStuck();
uint8_t if_op_inAttack();
uint8_t if_op_aroundHome();

void statusChange();
void ready_func();
void updInfo_func();
void init_func();
void dead_func();
void purchase_func();
void Pmove_func();
void Pdestroy_func();
void Nmove_func();
void Ndestroy_func();
void recover_func();
void upgrade_func();

#endif

/*目前策略：
    绿宝石的使用：先将生命值提升至29点，再升DPS（较优路线），剩2分钟时，继续升生命值
    中间设置三个进攻节点：到达这三个节点时，出发尝试杀死对方并毁掉对方的家
    三个进攻节点：生命力提升到29点时；DPS达到2时；DPS达到17时。
    未达到三个节点时，如果绿宝石不够就去采矿，绿宝石足够就回来升级
    时刻保证身上携带有至少16个羊毛，不足的话要回去补充，羊毛超过32个则不再购买羊毛；
    如果在采矿时碰到对方，也攻击，直接进入进攻节点
*/
