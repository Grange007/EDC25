#include "decision.h"
#include "jy62.h"
#include "main.h"
#include <math.h>

#define MAX_POS_ERROR 0.1
#define MAX_EMERALD 40
#define RED_TEAM 1
#define BLUE_TEAM 2

MapType gameMap[64]
				= {1, 0, 0, 0, 0, 0, 0, 3,
				   0, 0, 0, 0, 0, 0, 0, 0,
				   0, 0, 2, 0, 0, 0, 0, 0,
				   0, 0, 0, 0, 1, 0, 0, 0,
				   0, 0, 0, 1, 0, 0, 0, 0,
				   0, 0, 0, 0, 0, 2, 0, 0,
				   0, 0, 0, 0, 0, 0, 0, 0,
				   3, 0, 0, 0, 0, 0, 0, 1};

Status status = init;

uint8_t agility;
uint8_t health;
uint8_t wool;
uint8_t emerald;
uint8_t time;

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

Position_edc25 now = {0, 0};
Position_edc25 goal = {0, 0};
Position_edc25 des = {0, 0};
Position_edc25 op = {0, 0};
Position_edc25 home = {0, 0};
Position_edc25 opHome = {0, 0};

uint8_t mhtDst(Grid from, Grid to)
{
    return abs(from.x - to.x) + abs(from.y - to.y);
}
uint8_t grid2No(Grid grid)
{
    return 8 * grid.x + grid.y;
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
Grid nearestBlock(MapType type)
{
    Grid nearest = {0, 0};
    uint8_t dst = 255;
    for(int i = 0 ; i < 64 ; i++)
        if(gameMap[i] == type)
        {
            if(mhtDst(nowGrid, no2Grid(i)) < dst)
            {
                nearest = no2Grid(i);
                dst = mhtDst(nowGrid, no2Grid(i));
            }
        }
    return nearest;
}
Grid getNext(Grid from, Grid to)
{
    if(from.x < to.x)
        from.x += 1;
    else if(from.x > to.x)
        from.x -= 1;
    else if(from.y < to.y)
        from.y += 1;
    else if(from.y > to.y)
        from.y -= 1;
    return from;
}

void statusChange()
{
	if (health == 0)
		status = dead;
	else
	{
		if (hasBedOpponent())
		{
			if (wool > 2 * mhtDst(nowGrid, opHomeGrid) - 2
			 && time - lastTime > agility)
			{
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
				status = Pmove;
			}
			else if (emerald > MAX_EMERALD)
				status = Pprotect;
			else
			{
				nearestDiamond = nearestBlock(diamond);
				if (wool > 2 * mhtDst(nowGrid, nearestDiamond))
					desGrid = nearestDiamond;
				else
					desGrid = homeGrid;
				status = Pmove;
			}
		}
		else
		{
			if (emerald > 64)
				status = Nprotect;
			else if (wool > 2 * mhtDst(nowGrid, opGrid) - 2
				  && time - lastTime > agility)
			{
				desGrid = opGrid;
				status = Nmove;
			}
			else
			{
				nearestDiamond = nearestBlock(diamond);
				if (wool > 2 * mhtDst(nowGrid, nearestDiamond))
					desGrid = nearestDiamond;
				else
					desGrid = homeGrid;
				status = Nmove;
			}
		}
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
	// upd map
}
void init_func()
{
	InitAngle();
	if (health == 0)
		status = dead;
	else
		status = Pprotect;
}
void dead_func()
{
	goal = home;
	goalGrid = homeGrid;
	statusChange();
}
void Pmove_func()
{
	goalGrid = getNext(nowGrid, desGrid);
	if(desGrid.x == nowGrid.x && desGrid.y == nowGrid.y)
	{
		if ((team == RED_TEAM && nowGrid.x == opHomeGrid.x - 1 && nowGrid.y == opHomeGrid.y - 1)
		 || (team == BLUE_TEAM && nowGrid.x == opHomeGrid.x + 1 && nowGrid.y == opHomeGrid.y + 1))
			status = Pdestroy;
		return;
	}
	if(getHeightOfId(grid2No(goalGrid)) == 0)
	{
		place_block_id(grid2No(goalGrid));
		HAL_Delay(300);
	}
	goal = grid2Pos(goalGrid);
	statusChange();
}
void Pprotect_func()
{
    if (emerald >= 2)
        trade_id(3);
    statusChange();
}
void Pdestroy_func()
{
	attack_id(grid2No(opHomeGrid));
	statusChange();
}
void Nmove_func()
{
	goalGrid = getNext(nowGrid, desGrid);
	if(desGrid.x == nowGrid.x && desGrid.y == nowGrid.y)
	{
		if (nowGrid.x == opGrid.x && nowGrid.y == opGrid.y)
			status = Ndestroy;
		return;
	}
	if(getHeightOfId(grid2No(goalGrid)) == 0)
	{
		place_block_id(grid2No(goalGrid));
		HAL_Delay(300);
	}
	goal = grid2Pos(goalGrid);
	statusChange();
}
void Nprotect_func()
{
	if (emerald >= 64)
		trade_id(/*攻击力*/0);
    if (emerald >= 2)
        trade_id(3);
    statusChange();
}
void Ndestroy_func()
{
	attack_id(grid2No(opGrid));
	statusChange();
}

