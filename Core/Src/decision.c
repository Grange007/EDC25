#include "decision.h"
#include "main.h"
#include <math.h>
int minWool=8;
int bedMinHeight=16;
Grid goalGrid={0,0},nowGrid={0,0};
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
Grid nearestBlock(int type){
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

void protectBed(){

}
void seekMine(){

}
void destroyBed(){

}
void PauseWhileMoving(){

}
void initial(){

}