#include "decision.h"
#include "main.h"
#include <math.h>
#define DIAMOND 1
#define MAX_POS_ERROR 0.1
uint8_t minWool=8;
uint8_t bedMinHeight=16;
uint8_t emeraldCtrlLine=32;
status stat;
uint8_t gameMap[64];
Grid goalGrid={0,0},nowGrid={0,0},homeGrid={0,0};
Position_edc25 now = {0, 0};
Position_edc25 goal = {5, 5};
Grid nearestDiamond;
uint8_t mhtDst(Grid from, Grid to){
    return abs(from.x-to.x)+abs(from.y-to.y);
}
uint8_t grid2No(Grid grid){
    return grid.x+8*grid.y;
}
Grid no2Grid(uint8_t no){
    Grid tmp;
    tmp.x=no%8;
    tmp.y=no/8;
    return tmp;
}
Grid pos2Grid(Position_edc25 pos){
    Grid tmp;
    tmp.x=pos.posx+0.5;
    tmp.y=pos.posy+0.5;
    return tmp;
}
Position_edc25 grid2Pos(Grid grid){
    Position_edc25 tmp;
    tmp.posx=grid.x;
    tmp.posy=grid.y;
    return tmp;
}
Grid nearestBlock(uint8_t type){
    Grid nearest={0,0};
    uint8_t dst=255;
    for(int i=0;i<64;i++){
        if(gameMap[i]==type){
            if(mhtDst(nowGrid,no2Grid(i))<dst){
                nearest=no2Grid(i);
                dst=mhtDst(nowGrid,no2Grid(i));
            }
        }
    }
    return nearest;
}
Grid getNext(Grid from, Grid to){
    if(from.x<to.x){
        from.x+=1;
    }
    else if(from.x>to.x){
        from.x-=1;
    }
    else if(from.y<to.y){
        from.y+=1;
    }
    else if(from.y>to.y){
        from.y-=1;
    }
    return from;
}
void protect_bed_func(){
    while(getEmeraldCount()>=2){
        trade_id(3);
    }
    while(getHeightOfId(grid2No(homeGrid))<bedMinHeight&&getWoolCount()>minWool){
        place_block_id(grid2No(homeGrid));
    }
    goalGrid=nearestDiamond;
    stat=move;    
}
void mine_func(){
    bool gotEnough=false;
    bool conflict=false;
    Position_edc25 posop;
    while(!(gotEnough||conflict)){
        gotEnough=getEmeraldCount()>emeraldCtrlLine;
        getPositionOpponent(&posop);
        conflict=pos2Grid(posop).x==nowGrid.x&&pos2Grid(posop).y==nowGrid.y;
    }
    goalGrid=homeGrid;
    stat=move;
}
void destroy_bed_func(){

}
void move_func(){
    Grid nextGrid=getNext(nowGrid,goalGrid);
    if(nextGrid.x==nowGrid.x&&nextGrid.y==nowGrid.y){
        if(nowGrid.x==nearestDiamond.x&&nowGrid.y==nearestDiamond.y){
            stat=mine;
            return;
        }
        else if(nowGrid.x==homeGrid.x&&nowGrid.y==homeGrid.y){
            stat=protect;
            return;
        }
    }
    if(getHeightOfId(grid2No(nextGrid))==0){
        place_block_id(grid2No(nextGrid));
    }
    goal=grid2Pos(nextGrid);
    Position_edc25 actualPos;
    do{
        getPosition(&actualPos);
    }
    while((fabs(actualPos.posx-nextGrid.x)+fabs(actualPos.posy-nextGrid.y))>MAX_POS_ERROR);
}
void init_func(){
    nearestDiamond=nearestBlock(DIAMOND);
    uint8_t diamondDst=mhtDst(nowGrid,nearestDiamond);
    for(uint8_t i=0;i<8-diamondDst;i++){
        place_block_id(grid2No(nowGrid));
    }
    goalGrid=nearestDiamond;
    stat=move;
}
void attack_func(){

}
void mainLoop(){
    while(1){
        getPosition(&now);
        nowGrid=pos2Grid(now);
        switch(stat){
        case init:
            init_func();
            break;
        case move:
            move_func();
            break;
        case mine:
            mine_func();
            break;
        case protect:
            protect_bed_func();
            break;
        case destroy:
            destroy_bed_func();
            break;
        case attack:
            attack_func();
            break;
        }
    };
}
