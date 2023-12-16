#include "decoder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "usart.h"
uint8_t *map_ptr;

void decode_init(uint8_t *map)
{
    map_ptr = map;
    for (uint8_t i = 0; i < 64; i++)
    {
    	*(map_ptr + i) = 0;
    }
//    for (int i = 0; i < 8; i++)
//	{
//		for (int j = 0; j < 8; j++)
//		{
//			u1_printf("%d ", *(map_ptr + 8 * i + j));
//		}
//		u1_printf("\n");
//	}
}

void decode_update(char *str)
{
//    if (strlen(str) != 5)
//        return;
//    if (str[1] != ',' || str[3] != ',')
//        return;
//    if (str[0] < '0' || str[0] > '7' || str[2] < '0' || str[2] > '7' || str[4] < '0' || str[4] > '3')
//        return;
//    int x = str[0] - '0';
//    int y = str[2] - '0';
//    int color = str[4] - '0';
//    *(map_ptr + 8 * x + y) = color;
	u1_printf("strlen: %d\n", strlen(str));
	for (int i = 0; i < strlen(str); i++)
	{
		u1_printf("str[i]: %c\n", str[i]);
		if (str[i] == '*')
		{
			if (i + 5 < strlen(str))
			{
				if (str[i + 1] >= '0' && str[i + 1] <= '8'
					&& str[i + 2] == ','
					&& str[i + 3] >= '0' && str[i + 3] <= '8'
				 	&& str[i + 4] == ','
				 	&& str[i + 5] >= '0' && str[i + 5] <= '8')
				{
					uint8_t x = str[i + 1] - '0';
					uint8_t y = str[i + 3] - '0';
					uint8_t color = str[i + 5] - '0';
					u1_printf("x: %d, y: %d, color, %d\n", x, y, color);
					*(map_ptr + 8 * x + y) = color;
					for (uint8_t i = 0; i < 8; i++)
					{
						for (uint8_t j = 0; j < 8; j++)
						{
							u1_printf("%d ", *(map_ptr + 8 * i + j));
						}
						u1_printf("\n");
					}
				}
			}
		}
	}
    //    for (int i = 0; i < 8; i++)
    //    {
    //        for (int j = 0; j < 8; j++)
    //        {
    //            printf("%d ", *(map_ptr[i]+j));
    //        }
    //        printf("\n");
    //    }
}
