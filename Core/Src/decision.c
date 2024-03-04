#include "decision.h"
#include "jy62.h"
#include "main.h"
#include <math.h>
#include <string.h>

#define RED_TEAM 1
#define BLUE_TEAM 2
#define HOME_HEIGHT 4

#define MAX_MINE 32

#define AGILITY 0
#define HEALTH 1
#define STRENGTH 2
#define WOOL 3
#define HEALING 4

#define IRON 0
#define GOLD 1
#define DIAMOND 2
#define EMPTY 3

uint8_t gameMap[64] = {0, 0, 0, 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0, 0, 0, 0};

Status status = init;

uint8_t agility;
uint8_t health;
uint8_t maxHealth = 20;
uint8_t wool;
uint8_t emerald;
uint32_t time;

int8_t team;
int8_t lastTime = -16;

Grid nowGrid;
Grid goalGrid;
Grid desGrid;
Grid opGrid;
Grid homeGrid;
Grid opHomeGrid;
Grid redHomeGrid = {0, 0};
Grid blueHomeGrid = {7, 7};
Grid nearestDiamond;

Mine mineList[MAX_MINE];
uint8_t mineNum=0;

Position_edc25 now = {0.5f, 0.5f};
Position_edc25 goal = {0.5f, 0.5f};
Position_edc25 des = {0, 0};
Position_edc25 op = {0, 0};
Position_edc25 home = {0, 0};
Position_edc25 opHome = {0, 0};

int needWool;
int dis[8][8];
int pre_pos[8][8][2];
int dx[4] = {0, 0, 1, -1};
int dy[4] = {1, -1, 0, 0};
int inf = 0x3f3f3f3f;

uint8_t mhtDst(Grid from, Grid to)
{
	return abs(from.x - to.x) + abs(from.y - to.y);
}
uint8_t grid2No(Grid grid)
{
	return grid.x + 8 * grid.y;
}
Grid no2Grid(uint8_t no)
{
	Grid tmp;
	tmp.x = no % 8;
	tmp.y = no / 8;
	return tmp;
}
Grid pos2Grid(Position_edc25 pos)
{
	Grid tmp;
	tmp.x = (uint8_t)pos.posx;
	tmp.y = (uint8_t)pos.posy;
	return tmp;
}
Position_edc25 grid2Pos(Grid grid)
{
	Position_edc25 tmp;
	tmp.posx = (float)grid.x + 0.5;
	tmp.posy = (float)grid.y + 0.5;
	return tmp;
}
Grid nearestBlock(uint8_t type)
{
	Grid nearest = {0, 0};
	uint8_t dst = 255;
	for (int i = 0; i < 64; i++)
		if (gameMap[i] == type)
		{
			if (mhtDst(nowGrid, no2Grid(i)) < dst)
			{
				nearest = no2Grid(i);
				dst = mhtDst(nowGrid, no2Grid(i));
			}
		}
	return nearest;
}

Grid bellmanford(Grid source, Grid target, int *needBlock)
{
    memset(dis, 63, sizeof(dis));
	memset(pre_pos, 0, sizeof(pre_pos));
	(*needBlock) = 0;
    dis[source.x][source.y] = 0;
    int flag; // 判断一轮循环过程中是否发生松弛操作
    for (int i = 0; i < 64; i++)
    {
        flag = 0;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (dis[i][j] == inf)
                    continue;
                // 无穷大与常数加减仍然为无穷大
                // 因此最短路长度为 inf 的点引出的边不可能发生松弛操作
                for (int k = 0; k < 4; k++)
                {
                    int x = i + dx[k], y = j + dy[k];
                    // printf("(%d, %d); (%d, %d)\n", i, j, x, y);
                    if (x < 0 || x >= 8 || y < 0 || y >= 8)
                        continue;
                    int edge_w = 2;
                    if (getHeightOfId(grid2No((Grid){x, y})) == 0)
                    {
                        edge_w = 2;
                    }
                    else
                    {
                        edge_w = 1;
                    }
                    if (dis[x][y] > dis[i][j] + edge_w)
                    {
                        dis[x][y] = dis[i][j] + edge_w;
                        pre_pos[x][y][0] = i;
                        pre_pos[x][y][1] = j;
                        flag = 1;
                    }
                }
            }
        }
        if (!flag)
            break;
    }
    Grid cur = target;
    while (cur.x != source.x || cur.y != source.y)
    {
        if (getHeightOfId(grid2No((Grid){cur.x, cur.y})) == 0)
        {
            (*needBlock)++;
        }
        // printf("(%d, %d) ", cur.x, cur.y);
        int xx = cur.x;
        int yy = cur.y;
        cur.x = pre_pos[xx][yy][0];
        cur.y = pre_pos[xx][yy][1];
        if (cur.x == source.x && cur.y == source.y)
        {
            // printf("nextPos: (%d, %d)\n", xx, yy);
            // printf("needWool: %d\n", *needBlock);
            return (Grid){xx, yy};
        }
    }
}


void statusChange()
{
	if (health == 0)
		status = dead;
	else
	{
		
	}
}
void ready_func()
{
	uint8_t redDis = mhtDst(nowGrid, redHomeGrid);
	uint8_t blueDis = mhtDst(nowGrid, blueHomeGrid);
	if (redDis < blueDis)
	{
		homeGrid = redHomeGrid;
		home = grid2Pos(redHomeGrid);
		opHomeGrid = blueHomeGrid;
		opHome = grid2Pos(blueHomeGrid);
		team = RED_TEAM;
	}
	else
	{
		homeGrid = blueHomeGrid;
		home = grid2Pos(blueHomeGrid);
		opHomeGrid = redHomeGrid;
		opHome = grid2Pos(redHomeGrid);
		team = BLUE_TEAM;
	}
	goal = home;
	goalGrid = homeGrid;
	for (int i = 0; i < 64; i++){
		gameMap[i]=getTypeOfId(i);
		if(getTypeOfId(i)!=3&&mineNum<MAX_MINE){
			mineList[mineNum].grid=no2Grid(i);
			mineList[mineNum].type=getTypeOfId(i);
			mineList[mineNum].last_visit_tick=0;
			mineNum++;
		}
	}
	// upd map
}
void init_func()
{
	InitAngle();
}
void dead_func()
{
	goal = home;
	goalGrid = homeGrid;
	statusChange();
}

void protect_func(){
	desGrid=homeGrid;
	if (desGrid.x != nowGrid.x || desGrid.y != nowGrid.y)
	{
		place_and_move();
		return;
	}
	// protect the bed
}
void destroy_func(){
	if (team == RED_TEAM)
	{
		desGrid.x = opHomeGrid.x - 1;
		desGrid.y = opHomeGrid.y - 1;
	}
	if (team == BLUE_TEAM)
	{
		desGrid.x = opHomeGrid.x + 1;
		desGrid.y = opHomeGrid.y + 1;
	}
	if (desGrid.x != nowGrid.x || desGrid.y != nowGrid.y)
	{
		place_and_move();
		return;
	}
	attack_id(grid2No(opHomeGrid));
	HAL_Delay(300);
}
void attack_func(){
	desGrid=opGrid;
	if (abs(desGrid.x-nowGrid.x)>1||abs(desGrid.y-nowGrid.y)>1)
	{
		place_and_move();
		return;
	}
	attack_id(grid2No(opGrid));
	HAL_Delay(300);
}
void mine_func(){
	desGrid=find_optimal_mine();
	if (desGrid.x != nowGrid.x || desGrid.y != nowGrid.y)
	{
		place_and_move();
	}
}
void get_wool_func(){
	desGrid=homeGrid;
	if (desGrid.x != nowGrid.x || desGrid.y != nowGrid.y)
	{
		place_and_move();
		return;
	}
	if (emerald >= 2||wool<16)
	{
		trade_id(3);
		HAL_Delay(300);
	}
}
void get_enhanced_func(){
	desGrid=homeGrid;
	if (desGrid.x != nowGrid.x || desGrid.y != nowGrid.y)
	{
		place_and_move();
		return;
	}
	if (emerald >= 64||wool<16)
	{
		trade_id(find_optimal_enhancement());
		HAL_Delay(300);
	}	
}

void place_and_move()
{
	if (desGrid.x == nowGrid.x && desGrid.y == nowGrid.y)
	{
		return;
	}
	goalGrid = bellmanford(nowGrid, desGrid, &needWool);
	if (getHeightOfId(grid2No(goalGrid)) == 0)
	{
		place_block_id(grid2No(goalGrid));
		HAL_Delay(300);
	}
	goal = grid2Pos(goalGrid);
}
Grid find_optimal_mine(){

}
uint8_t find_optimal_enhancement(){

}
void update_mine(){
	for(int i=0;i<mineNum;i++){
		if((mineList[i].grid.x==nowGrid.x&&mineList[i].grid.y==nowGrid.y)||(mineList[i].grid.x==opGrid.x&&mineList[i].grid.y==opGrid.y)){
			mineList[i].store=0;
			mineList[i].last_visit_tick=time;
		}
		else{
			uint8_t base=1;
			if(mineList[i].type==1){
				base=4;
			}
			else if(mineList[i].type==2){
				base=16;
			}
			mineList[i].store=base*(time/200-mineList[i].last_visit_tick/200);
		}
	}
}
// void homeProtect()
// {

// }
// void statusChange()
// {
// 	if (health == 0)
// 		status = dead;
// 	else
// 	{
// 		if (hasBedOpponent())
// 		{
// 			bellmanford(nowGrid, opHomeGrid, &needWool);
// 			if (wool > needWool && time - lastTime > agility)
// 			{
// 				if (team == RED_TEAM)
// 				{
// 					desGrid.x = opHomeGrid.x - 1;
// 					desGrid.y = opHomeGrid.y - 1;
// 				}
// 				if (team == BLUE_TEAM)
// 				{
// 					desGrid.x = opHomeGrid.x + 1;
// 					desGrid.y = opHomeGrid.y + 1;
// 				}
// 				status = Pmove;
// 			}
// 			else if (emerald > 2)
// 			{
// 				if(nowGrid.x == homeGrid.x && nowGrid.y == homeGrid.y)
// 					status = Pprotect;
// 				else
// 				{
// 					desGrid = homeGrid;
// 					status = Pmove;
// 				}
// 			}
// 			else
// 			{
// 				nearestDiamond = nearestBlock(diamond);
// 				bellmanford(nowGrid, nearestDiamond, &needWool);
// 				if (wool > needWool)
// 					desGrid = nearestDiamond;
// 				else
// 					desGrid = homeGrid;
// 				status = Pmove;
// 			}
// 		}
// 		else
// 		{
// 			bellmanford(nowGrid, opGrid, &needWool);
// 			if (emerald > 64)
// 			{
// 				if(nowGrid.x == homeGrid.x && nowGrid.y == homeGrid.y)
// 					status = Nprotect;
// 				else{
// 					desGrid = homeGrid;
// 					status = Nmove;
// 				}
// 			}
// 			else if (wool > needWool && time - lastTime > agility)
// 			{
// 				desGrid = opGrid;
// 				status = Nmove;
// 			}
// 			else
// 			{
// 				bellmanford(nowGrid, nearestDiamond, &needWool);
// 				nearestDiamond = nearestBlock(diamond);
// 				if (wool > needWool)
// 					desGrid = nearestDiamond;
// 				else
// 					desGrid = homeGrid;
// 				status = Nmove;
// 			}
// 		}
// 	}
// //	else
// //	{
// //		if (emerald >= 4)
// //		{
// //			if(nowGrid.x == homeGrid.x && nowGrid.y == homeGrid.y)
// //				status = recover;
// //			else
// //			{
// //				desGrid = homeGrid;
// //				status = Pmove;
// //			}
// //		}
// //		else
// //		{
// //			nearestDiamond = nearestBlock(diamond);
// //			bellmanford(nowGrid, nearestDiamond, &needWool);
// //			if (wool > needWool)
// //				desGrid = nearestDiamond;
// //			else
// //				desGrid = homeGrid;
// //			status = Pmove;
// //		}
// //	}
// }
// void ready_func()
// {
// 	uint8_t redDis = mhtDst(nowGrid, redHomeGrid);
// 	uint8_t blueDis = mhtDst(nowGrid, blueHomeGrid);
// 	if (redDis < blueDis)
// 	{
// 		homeGrid = redHomeGrid;
// 		home = grid2Pos(redHomeGrid);
// 		opHomeGrid = blueHomeGrid;
// 		opHome = grid2Pos(blueHomeGrid);
// 		team = RED_TEAM;
// 	}
// 	else
// 	{
// 		homeGrid = blueHomeGrid;
// 		home = grid2Pos(blueHomeGrid);
// 		opHomeGrid = redHomeGrid;
// 		opHome = grid2Pos(redHomeGrid);
// 		team = BLUE_TEAM;
// 	}
// 	goal = home;
// 	goalGrid = homeGrid;
// 	// upd map
// }
// void init_func()
// {
// 	InitAngle();
// 	if (health == 0)
// 		status = dead;
// 	else
// 		status = Pprotect;
// }
// void dead_func()
// {
// 	goal = home;
// 	goalGrid = homeGrid;
// 	statusChange();
// }
// void Pmove_func()
// {
// 	goalGrid = bellmanford(nowGrid, desGrid, &needWool);
// 	if (desGrid.x == nowGrid.x && desGrid.y == nowGrid.y)
// 	{
// 		if ((team == RED_TEAM && nowGrid.x == opHomeGrid.x - 1 && nowGrid.y == opHomeGrid.y - 1)
// 		 || (team == BLUE_TEAM && nowGrid.x == opHomeGrid.x + 1 && nowGrid.y == opHomeGrid.y + 1))
// 		{
// 			status = Pdestroy;
// 			return;
// 		}
// 	}
// 	if (getHeightOfId(grid2No(goalGrid)) == 0)
// 	{
// 		place_block_id(grid2No(goalGrid));
// 		HAL_Delay(300);
// 	}
// 	goal = grid2Pos(goalGrid);
// 	statusChange();
// }
// void Pprotect_func()
// {
// 	if (emerald > 2)
// 	{
// 		trade_id(3);
// 		HAL_Delay(300);
// 	}
// 	statusChange();
// }
// void Pdestroy_func()
// {
// 	attack_id(grid2No(opHomeGrid));
// 	HAL_Delay(300);
// 	lastTime = time;
// 	statusChange();
// }
// void Nmove_func()
// {
// 	goalGrid = bellmanford(nowGrid, desGrid, &needWool);
// 	if(desGrid.x == nowGrid.x && desGrid.y == nowGrid.y)
// 	{
// 		if (nowGrid.x == opGrid.x && nowGrid.y == opGrid.y)
// 		{
// 			status = Ndestroy;
// 			return;
// 		}
// 	}
// 	if(getHeightOfId(grid2No(goalGrid)) == 0)
// 	{
// 		place_block_id(grid2No(goalGrid));
// 		HAL_Delay(300);
// 	}
// 	goal = grid2Pos(goalGrid);
// 	statusChange();
// }
// void Nprotect_func()
// {
// 	if (emerald >= 64)
// 	{
// 		trade_id(2);
// 		HAL_Delay(300);
// 	}
//     if (emerald >= 2)
//     {
// 		trade_id(3);
// 		HAL_Delay(300);
// 	}
//     statusChange();
// }
// void Ndestroy_func()
// {
// 	attack_id(grid2No(opGrid));
// 	HAL_Delay(300);
// 	lastTime = time;
// 	statusChange();
// }
// void recover_func()
// {
// 	if (emerald > 4)
// 	{
// 		trade_id(4);
// 		HAL_Delay(300);
// 	}
// 	statusChange();
// }