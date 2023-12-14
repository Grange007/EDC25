#include "decoder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int (*map_ptr)[8];

void decode_init(int (*map)[8])
{
    map_ptr = map;
}

void decode_update(char* str)
{
   int x = str[0] - '0';
   int y = str[2] - '0';
   int color = str[4] - '0';
   *(map_ptr[x]+y) = color;
//    for (int i = 0; i < 8; i++)
//    {
//        for (int j = 0; j < 8; j++)
//        {
//            printf("%d ", *(map_ptr[i]+j));
//        }
//        printf("\n");
//    }
}


