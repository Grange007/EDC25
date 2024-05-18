#include "decision.h"
#include "jy62.h"
#include "main.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define RED_TEAM 1
#define BLUE_TEAM 2

#define HOME_HEIGHT 4

// enhancements
#define AGILITY 0
#define HEALTH 1
#define STRENGTH 2
#define WOOL 3
#define HEALING 4

// mine types
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
float weight[10]={0};

// maintained in the main loop
uint8_t agility;
uint8_t health;
uint8_t strength;
uint8_t maxHealth = 20;
uint8_t wool;
uint8_t emerald;
uint32_t time;
uint8_t cd=170;

int8_t team;
int32_t lastTime = -16;
int32_t lastAttack = -160;
Status lastStatus=init;

Grid nowGrid;
Grid goalGrid;   // 当前向哪走
Grid desGrid;    // 最终的目标
Grid opGrid;     // op是对面
Grid homeGrid;   // 家的位置
Grid opHomeGrid; // 对面家的位置
Grid redHomeGrid = {0, 0};
Grid blueHomeGrid = {7, 7};
Grid nearestDiamond;

Mine mineList[MAX_MINE];
uint8_t mineNum = 0;

Position_edc25 now = {0.5f, 0.5f};
Position_edc25 goal = {0.5f, 0.5f};
Position_edc25 des = {0, 0};
Position_edc25 op = {0, 0};
Position_edc25 home = {0, 0};
Position_edc25 opHome = {0, 0};

// used by bellmanford function
int needWool;
int dis[8][8];
int pre_pos[8][8][2];
int dx[4] = {0, 0, 1, -1};
int dy[4] = {1, -1, 0, 0};
int inf = 0x3f3f3f3f;

int needWool;

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
	if(pos.posx<0||pos.posx>8)
		tmp.x = 64;
	else
		tmp.x = (uint8_t)pos.posx;
	if(pos.posy<0||pos.posy>8)
		tmp.y = 0;
	else
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
Grid bellmanford(Grid source, Grid target, int *needBlock) // 找到从source到target的花费最少的路，needBlock是花费羊毛数
{
    if (source.x == target.x && source.y == target.y)
    {
        needBlock = 0;
        return target;
    }
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
                    int edge_w = 3;
                    if (getHeightOfId(grid2No((Grid){x, y})) == 0)
                    {
                        edge_w = 4;
                    }
                    if (getOreKindOfId(grid2No((Grid){x, y})) == iron || getOreKindOfId(grid2No((Grid){x, y})) == gold || getOreKindOfId(grid2No((Grid){x, y})) == diamond)
                    {
                        edge_w = 1;
                    }
                    else
                    {
                        edge_w = 3;
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

// calculate the weight of each normal status and choose the best
void statusChange()
{
	if (health == 0)
		status = dead;
	else
	{
		lastStatus=status;
		weight[protect]=calculate_weight_protect();
		weight[destroy]=calculate_weight_destroy();
		weight[attack]=calculate_weight_attack();
		weight[mine]=calculate_weight_mine();
		weight[get_wool]=calculate_weight_get_wool();
		weight[get_enhanced]=calculate_weight_get_enhanced();
		Status best=best_status(protect,get_enhanced);
		status = best;
		if(status!=lastStatus){
			lastTime=time;
		}
	}
}

// special status funcs
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
	mineNum=0;
	for (int i = 0; i < 64; i++){
		gameMap[i]=getOreKindOfId(i);
		if(getOreKindOfId(i)!=3&&mineNum<MAX_MINE){
			mineList[mineNum].grid=no2Grid(i);
			mineList[mineNum].type=getOreKindOfId(i);
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

// normal status funcs
void protect_func(){
	desGrid=homeGrid;
	if (desGrid.x != nowGrid.x || desGrid.y != nowGrid.y)
	{
		place_and_move();
		return;
	}
	if(getHeightOfId(grid2No(homeGrid))<8){
		place_block_id(grid2No(homeGrid));
		HAL_Delay(300);
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
	lastAttack=time;
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
	lastAttack=time;
	HAL_Delay(300);
}
void mine_func(){
	desGrid=find_optimal_mine().grid;
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
	if (emerald >= 2&&wool<32)
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
	if (emerald >= 32)
	{
		trade_id(find_optimal_enhancement());
		HAL_Delay(300);
	}	
}

float calculate_weight_protect(){
	bellmanford(nowGrid,homeGrid,&needWool);
	if(needWool>wool){
		return 0;
	}
	float weight=1;
	if(getHeightOfId(grid2No(homeGrid))<HOME_HEIGHT&&wool>8){
		weight=5;
	}
	else if(getHeightOfId(grid2No(homeGrid))<8&&wool>=16){
		weight=5;
	}
	if(lastStatus==protect){
		if(time-lastTime>=600){
			weight=weight-1<0?0:weight-1;
		}
		if(time-lastTime>=1200){
			weight=0;
		}
	}
	return weight;
}
float calculate_weight_destroy(){
	bellmanford(nowGrid,opHomeGrid,&needWool);
	if(needWool>wool){
		return 0;
	}
	if(time-lastAttack+20<cd){
		return 0;
	}
	float weight=1;
	if(opGrid.x!=opHomeGrid.x&&opGrid.y!=opHomeGrid.y){
		if(getHeightOfId(grid2No(opHomeGrid))<=(200/cd)){
			weight=4.5;
		}
		else if(cd<100){
			weight=2.1;
		}
	}
	if(lastStatus==destroy){
		if(time-lastTime>=600){
			weight=weight-1<0?0:weight-1;
		}
		if(time-lastTime>=1200){
			weight=0;
		}
	}
	return weight;	// on simulator only
}
float calculate_weight_attack(){
	if(opGrid.x==64&&opGrid.y==0){
		return 0;
	}
	bellmanford(nowGrid,opGrid,&needWool);
	if(needWool>wool){
		return 0;
	}
	if(!hasBedOpponent()){
		return 4.5;
	}
	if(time-lastAttack+20<cd){
		return 0;
	}
	float weight=1;
	if(mhtDst(nowGrid,opGrid)<=sqrt(strength)){
		weight=4;
	}
	else if(strength>9){
		weight=2;	
	}
	if(lastStatus==attack){
		if(time-lastTime>=600){
			weight=weight-1<0?0:weight-1;
		}
		if(time-lastTime>=1200){
			weight=0;
		}
	}
	return weight;		//due to a simulator bug
}
float calculate_weight_mine(){
	bellmanford(nowGrid,find_optimal_mine().grid,&needWool);
	// u1_printf("optimal: %d, %d\n",find_optimal_mine().grid.x,find_optimal_mine().grid.y);
	// u1_printf("needwool:%d\n",needWool);
	if(needWool>wool){
		return 0;
	}
	float weight=1.5;
	if(find_optimal_mine().score>64){
		weight=2.5;
	}
	if(lastStatus==mine){
		if(time-lastTime>=600){
			weight=weight-1<0?0:weight-1;
		}
		if(time-lastTime>=1200){
			weight=0;
		}
	}
	return weight;
}
float calculate_weight_get_wool(){
	float weight=1;
	if(emerald<2){
		weight=0;
	}
	else if(wool<16&&emerald>16){
		weight=3;
	}
	if(lastStatus==get_wool){
		if(wool<24&&emerald>16){
			weight=3;
		}
		if(time-lastTime>=300){
			weight=weight-1<0?0:weight-1;
		}
		if(time-lastTime>=600){
			weight=0;
		}
	}
	return weight;
}
float calculate_weight_get_enhanced(){
	float weight=1;
	bool enough_to_enhance=emerald>=64||(emerald>=32&&find_optimal_enhancement()!=STRENGTH);
	if(!enough_to_enhance){
		weight=0;
	}
	else if(emerald>96){
		weight=3;
	}
	if(lastStatus==get_enhanced){
		if(enough_to_enhance&&wool>8){
			weight=3;
		}
		if(time-lastTime>=300){
			weight=weight-1<0?0:weight-1;
		}
		if(time-lastTime>=600){
			weight=0;
		}
	}
	return weight;
}

Status best_status(Status from,Status to){
	int max=0;
	Status best;
	for(Status i=from;i<=to;i++){
		if(weight[i]>max){
			best=i;
			max=weight[i];
		}
	}
	return best;
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
Mine find_optimal_mine(){
	float best_score=-50;
	int8_t best_mine=0;
	for(int i=0;i<mineNum;i++){
		int8_t dst=mhtDst(nowGrid,mineList[i].grid);
		mineList[i].score=mineList[i].store*16/(16+dst+needWool);
		if(mineList[i].score>best_score){
			best_score=mineList[i].score;
			best_mine=i;
		}
	}
	return mineList[best_mine];
}
uint8_t find_optimal_enhancement(){
	if(time>10000){
		return HEALTH;
	}
	if(strength<10){
		return STRENGTH;
	}
	else if(agility<32){
		return AGILITY;
	}
	else if(strength<17){
		return STRENGTH;
	}
	else{
		return HEALTH;
	}
}
// calculate the value accumulated on each mine
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
