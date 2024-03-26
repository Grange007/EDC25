#include "decision.h"
#include "jy62.h"
#include "main.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define RED_TEAM 1
#define BLUE_TEAM 2
#define HOME_HEIGHT 4
/*目前策略：
    绿宝石的使用：先将生命值提升至29点，再升DPS（较优路线），剩2分钟时，继续升生命值
    中间设置三个进攻节点：到达这三个节点时，出发尝试杀死对方并毁掉对方的家
    三个进攻节点：生命力提升到29点时；DPS达到2时；DPS达到17时。
    未达到三个节点时，如果绿宝石不够就去采矿，绿宝石足够就回来升级
    时刻保证身上携带有至少8个羊毛，不足的话要回去补充，羊毛超过32个则不再购买羊毛；
    如果在采矿时碰到对方，也攻击，直接进入进攻节点
*/
uint8_t gameMap[64] =
    {1, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 1};
Status status = init;

uint8_t agility;        // 急迫值
uint8_t emerald;        // 绿宝石
uint8_t health;         // 当前生命
uint8_t maxHealth;      // 最大生命
uint8_t strength;       // 攻击力
uint8_t wool;           // 羊毛
uint8_t team;           // 代表哪一方

int32_t time;           // 目前时间
int32_t lastTime = -16;

Grid nowGrid;
Grid goalGrid;   // 当前向哪走
Grid desGrid;    // 最终的目标
Grid opGrid;     // op是对面
Grid homeGrid;   // 家的位置
Grid opHomeGrid; // 对面家的位置
Grid redHomeGrid = {0, 0};
Grid blueHomeGrid = {7, 7};

Grid nearestDiamond;  // 最近钻石矿
Grid mostValuableOre; // 最有价值的矿物

Position_edc25 now = {0.5f, 0.5f};
Position_edc25 goal = {0.5f, 0.5f};
Position_edc25 des = {0, 0};
Position_edc25 op = {0, 0};
Position_edc25 home = {0, 0};
Position_edc25 opHome = {0, 0};


int dis[8][8];
int pre_pos[8][8][2];
int dx[4] = {0, 0, 1, -1};
int dy[4] = {1, -1, 0, 0};
int inf = 0x3f3f3f3f;

int needWool;

uint16_t accmulatedOre[8][8] =
        {{0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}};
OreInfo ore[64];
int oreNum = 0;
int oreUpdCnt = 1;

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

uint16_t getAccumulatedNumberOfOre(Grid OrePos)
{
    return accmulatedOre[OrePos.x][OrePos.y];
}

void getPositionOfAllOre()
{
    for (uint8_t i = 0; i < 64; i++)
        gameMap[i] = getOreKindOfId(i);

    for (int i = 0; i < 64; i++)
    {
        if (gameMap[i] == iron)
        {
            ore[oreNum].pos = no2Grid(i);
            ore[oreNum].type = iron;
            oreNum++;
        }
        else if (gameMap[i] == gold)
        {
            ore[oreNum].pos = no2Grid(i);
            ore[oreNum].type = gold;
            oreNum++;
        }
        else if (gameMap[i] == diamond)
        {
            ore[oreNum].pos = no2Grid(i);
            ore[oreNum].type = diamond;
            oreNum++;
        }
    }
}

Grid findMostValuableBlock(Grid source)
{
    int mostValueBlockIndex = 0;
    int needBlock;
    int money;
    float totalValue;
    int maxnValue = -1;
    int maxnIndex = 0;
    for (int i = 0; i < oreNum; i++)
    {
        int distance = bellmanford_distance(source, ore[i].pos, &needBlock);
        switch (ore[i].type)
        {
            case iron:
                money = 1;
                break;

            case gold:
                money = 4;
                break;

            case diamond:
                money = 16;
                break;

            default:
                break;
        }
        totalValue =
                (distance) == 0 ?
                (float)(money * 10 * getAccumulatedNumberOfOre(ore[i].pos) / 0.5) - 5 * needBlock :
                (float)(money * 10 * getAccumulatedNumberOfOre(ore[i].pos) / (distance)) - 5 * needBlock;
        if (totalValue > maxnValue)
        {
            maxnValue = totalValue;
            maxnIndex = i;
        }
    }
    return ore[maxnIndex].pos;
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
                    else if (getOreKindOfId(grid2No((Grid){x, y})) == iron || getOreKindOfId(grid2No((Grid){x, y})) == gold || getOreKindOfId(grid2No((Grid){x, y})) == diamond)
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
int bellmanford_distance(Grid source, Grid target, int *needBlock)
{
    if (source.x == target.x && source.y == target.y)
    {
        *needBlock = 0;
        return 0;
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
                    else if (getOreKindOfId(grid2No((Grid){x, y})) == iron || getOreKindOfId(grid2No((Grid){x, y})) == gold || getOreKindOfId(grid2No((Grid){x, y})) == diamond)
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
    return dis[target.x][target.y];
}

uint8_t getStuck()
{
    return getHeightOfId(grid2No((Grid){nowGrid.x - 1, nowGrid.y})) == 0
        && getHeightOfId(grid2No((Grid){nowGrid.x + 1, nowGrid.y})) == 0
        && getHeightOfId(grid2No((Grid){nowGrid.x, nowGrid.y - 1})) == 0
        && getHeightOfId(grid2No((Grid){nowGrid.x, nowGrid.y + 1})) == 0
        && wool == 0;
}
uint8_t if_op_inAttack()
{
    return abs(opGrid.x - nowGrid.x) <= 1
        && abs(opGrid.y - nowGrid.y) <= 1;
}
uint8_t if_op_aroundHome()
{
    return abs(opGrid.x - homeGrid.x) <= 1
        && abs(opGrid.y - homeGrid.y) <= 1;
}

void statusChange()
{
    if (health == 0 || getStuck())
        status = dead;
    else
    {
        if (health == 29 /*|| if_op_inAttack()*/) // 攻击节点：生命力提升至29或DPS达到2和17或遇到对方
        {
            if (health < 20) // 检查状态怎样，补充生命和羊毛
            {
                bellmanford(nowGrid, opGrid, &needWool);
                if (nowGrid.x == homeGrid.x && nowGrid.y == homeGrid.y)
                    status = recover;
                else
                {
                    desGrid = homeGrid;
                    status = Pmove;
                }
            }
            else if (wool < 16)
            {
                bellmanford(nowGrid, opGrid, &needWool);
                if (nowGrid.x == homeGrid.x && nowGrid.y == homeGrid.y)
                {
                    status = purchase;
                }
                else
                {
                    desGrid = homeGrid;
                    status = Pmove;
                }
            }
            else if (health >= 20 && wool >= 16) // 状态完备，再去攻击
            {
                if (getHeightOfId(grid2No(opHomeGrid)) > 0) // 有家先干家
                {
                    bellmanford(nowGrid, opHomeGrid, &needWool);
                    if (wool > needWool)
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
                } // 去干家
                else
                { // 对面没家就干人
                    bellmanford(nowGrid, opGrid, &needWool);
                    if (wool > needWool)
                    {
                        desGrid = opGrid;
                        status = Nmove;
                    }
                }
            }
        }
        else if (getHeightOfId(grid2No(homeGrid)) < 4)//家的高度小于4就去保家
        {
            bellmanford(nowGrid, opGrid, &needWool);
            if(nowGrid.x == homeGrid.x && nowGrid.y == homeGrid.y)
                status= protect;
            else
            {
                desGrid = homeGrid;
                status = Pmove;
            }
        }
        else if (emerald >= 70 - wool)
        {
            if (nowGrid.x == homeGrid.x && nowGrid.y == homeGrid.y)
            {
                if (wool < 32)
                    status = purchase;
                else
                    status = upgrade;
            }
            else
            {
                desGrid = homeGrid;
                status = Pmove;
            }
        }
        else // 没钱就去采矿
        {
            // nearestDiamond = nearestBlock(diamond);
            // bellmanford(nowGrid, nearestDiamond, &needWool);
            mostValuableOre = findMostValuableBlock(nowGrid);
            bellmanford(nowGrid, mostValuableOre, &needWool);
            if (wool > needWool)
            {
                desGrid = mostValuableOre;
            }
            else
            {
                desGrid = homeGrid;
            }
            status = Pmove;
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
    oreNum = 0;
    getPositionOfAllOre();
}
void updInfo_func()
{
    agility = getAgility();
    emerald = getEmeraldCount();
    health = getHealth();
    maxHealth = getMaxHealth();
    strength = getStrength();
    wool = getWoolCount();

    time = getGameTime();

    if (time > 200 * oreUpdCnt)
    {
        oreUpdCnt++;
        for (uint8_t i = 0; i < oreNum; i++)
        {
            switch (ore[i].type)
            {
                case iron:
                    accmulatedOre[ore[i].pos.x][ore[i].pos.y] += 1;
                    break;
                case gold:
                    accmulatedOre[ore[i].pos.x][ore[i].pos.y] += 4;
                    break;
                case diamond:
                    accmulatedOre[ore[i].pos.x][ore[i].pos.y] += 16;
                    break;
                default:
                    break;
            }
        }
    }
    accmulatedOre[nowGrid.x][nowGrid.y] = 0;
    accmulatedOre[opGrid.x][opGrid.y] = 0;
}
void init_func()
{
    if (health == 0)
        status = dead;
    else
        status = purchase;
}
void dead_func()
{
    goal = home;
    goalGrid = homeGrid;
    statusChange();
}
void protect_func()
{
    while (getHeightOfId(grid2No(homeGrid)) < 4 && wool > 0)
    {
        place_block_id(grid2No(homeGrid));
        HAL_Delay(300);
    }
    while (wool < 8 && emerald > 2)
    {
        trade_id(3);
        HAL_Delay(300);
    }
    desGrid = opGrid;
    status = Nmove;
}
void purchase_func()
{
    if (wool < 32 && emerald > 2)
    {
        trade_id(3);
        HAL_Delay(300);
    }
    statusChange();
}
void Pmove_func() // 打床；移动
{
    goalGrid = bellmanford(nowGrid, desGrid, &needWool);
    if (desGrid.x == nowGrid.x && desGrid.y == nowGrid.y)
    {
        if ((team == RED_TEAM && nowGrid.x == opHomeGrid.x - 1 && nowGrid.y == opHomeGrid.y - 1) || (team == BLUE_TEAM && nowGrid.x == opHomeGrid.x + 1 && nowGrid.y == opHomeGrid.y + 1))
        {
            status = Pdestroy;
            return;
        }
    }
    if (getHeightOfId(grid2No(goalGrid)) == 0)
    {
        place_block_id(grid2No(goalGrid));
        HAL_Delay(300);
    }
    goal = grid2Pos(goalGrid);
    statusChange();
}
void Pdestroy_func() // 干床
{
    while (getHeightOfId(grid2No(opHomeGrid)) > 0)
    {
        attack_id(grid2No(opHomeGrid));
        HAL_Delay(300);
        HAL_Delay(agility);
        lastTime = time;
    }
    statusChange();
}
void Nmove_func() // 打人||移动
{
    goalGrid = bellmanford(nowGrid, desGrid, &needWool);
    if (desGrid.x == nowGrid.x && desGrid.y == nowGrid.y)
    {
        if (nowGrid.x == opGrid.x && nowGrid.y == opGrid.y)
        {
            status = Ndestroy;
            return;
        }
    }
    if (getHeightOfId(grid2No(goalGrid)) == 0)
    {
        place_block_id(grid2No(goalGrid));
        HAL_Delay(300);
    }
    goal = grid2Pos(goalGrid);
    statusChange();
}
void Ndestroy_func() // 干人
{
    attack_id(grid2No(opGrid));
    HAL_Delay(300);
    lastTime = time;
    statusChange();
}
void recover_func()
{
    while (health < 20 && emerald > 4)
    {
        trade_id(4);
        HAL_Delay(300);
    }
    statusChange();
}
void upgrade_func()
{
    if (maxHealth < 29)
    {
        trade_id(1);
        HAL_Delay(300);
    }
    else if (strength < 17)
    {
        trade_id(2);
        HAL_Delay(300);
    }
    else
    {
        trade_id(0);
        HAL_Delay(300);
    }
    statusChange();
}
