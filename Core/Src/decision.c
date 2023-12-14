#include "decision.h"
#include "jy62.h"
#include "main.h"
#include <math.h>

#define MAX_POS_ERROR 0.1

Status status = init;

uint8_t minWool=8;
uint8_t bedMinHeight=16;
uint8_t emeraldCtrlLine=32;
MapType gameMap[64]
				= {1, 0, 0, 0, 0, 0, 0, 3,
				   0, 0, 0, 0, 0, 0, 0, 0,
				   0, 0, 2, 0, 0, 0, 0, 0,
				   0, 0, 0, 0, 1, 0, 0, 0,
				   0, 0, 0, 1, 0, 0, 0, 0,
				   0, 0, 0, 0, 0, 2, 0, 0,
				   0, 0, 0, 0, 0, 0, 0, 0,
				   3, 0, 0, 0, 0, 0, 0, 1};

Grid goalGrid = {0, 0};
Grid nowGrid = {0, 0};
Grid homeGrid = {0, 0};
Grid opHomeGrid = {0, 0};
Grid desGrid = {0, 0};
Grid opGrid = {0, 0};
Grid nearestDiamond;

Position_edc25 now = {0, 0};
Position_edc25 goal = {0, 0};
Position_edc25 home = {0, 0};
Position_edc25 opHome = {0, 0};
Position_edc25 des = {0, 0};
Position_edc25 op = {0, 0};

uint8_t mhtDst(Grid from, Grid to)
{
    return abs(from.x-to.x)+abs(from.y-to.y);
}
uint8_t grid2No(Grid grid)
{
    return grid.x+8*grid.y;
}
Grid no2Grid(uint8_t no)
{
    Grid tmp;
    tmp.x=no%8;
    tmp.y=no/8;
    return tmp;
}
Grid pos2Grid(Position_edc25 pos)
{
    Grid tmp;
    tmp.x=pos.posx+0.5;
    tmp.y=pos.posy+0.5;
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
    uint8_t dst=255;
    for(int i=0;i<64;i++)
    {
        if(gameMap[i] == type)
        {
            if(mhtDst(nowGrid,no2Grid(i))<dst)
            {
                nearest=no2Grid(i);
                dst=mhtDst(nowGrid,no2Grid(i));
            }
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

void ready_func()
{
	goal = now;
	goalGrid = nowGrid;

	home = now;
	homeGrid = nowGrid;

	if (homeGrid.x == 0)
	{
		opHomeGrid.x = 7;
		opHomeGrid.y = 7;
	}
	else
	{
		opHomeGrid.x = 0;
		opHomeGrid.y = 0;
	}
	opHome = grid2Pos(opHomeGrid);
	// upd map
}
void init_func()
{
	InitAngle();
    nearestDiamond = nearestBlock(diamond);
    uint8_t diamondDst = mhtDst(nowGrid, nearestDiamond);
    for(uint8_t i = 0; i < 8 - diamondDst; i++)
    {
        place_block_id(grid2No(nowGrid));
    }
    desGrid = nearestDiamond;
    status = move;
}
void move_func()
{
	goalGrid = getNext(nowGrid, desGrid);
	if(goalGrid.x == nowGrid.x && goalGrid.y == nowGrid.y)
	{
		if(nowGrid.x == nearestDiamond.x && nowGrid.y == nearestDiamond.y)
		{
			status = mine;
			return;
		}
		else if(nowGrid.x == homeGrid.x && nowGrid.y == homeGrid.y)
		{
			status = protect;
			return;
		}
	}
	if(getHeightOfId(grid2No(goalGrid)) == 0)
	{
		place_block_id(grid2No(goalGrid));
	}
	goal = grid2Pos(goalGrid);
}
void protect_bed_func()
{
    if (getEmeraldCount() >= 2)
    {
        trade_id(3);
    }
    if (getHeightOfId(grid2No(homeGrid)) < bedMinHeight && getWoolCount() > minWool)
    {
        place_block_id(grid2No(homeGrid));
    }
    else
    {
    	desGrid = nearestDiamond;
    	status = move;
    }
}
void mine_func()
{
    if (getEmeraldCount() > emeraldCtrlLine || (opGrid.x == nowGrid.x && opGrid.y == nowGrid.y))
	{
    	desGrid = homeGrid;
		status = move;
	}
}
void destroy_bed_func()
{

}
void attack_func()
{

}
