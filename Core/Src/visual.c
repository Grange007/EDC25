#include "visual.h"
#include "decoder.h"
#include "string.h"
#include "decision.h"
#include "main.h"
#include "usart.h"
char visual_ch;
char visual_buffer[10];
void visual_receive(void)
{
    HAL_Delay(1000);
    for (int i = 0; i < 200; i++)
    {
        HAL_UART_Receive(&huart5, &visual_ch, 1, HAL_MAX_DELAY);
        u1_printf("%c", visual_ch);
        if (visual_ch == '\n')
        {
            decode_update(visual_buffer);
            memset(visual_buffer, 0, sizeof(visual_buffer));
            for (int i = 0; i < 64; i++)
            {
            	u1_printf("%d ", gameMap[i]);
            	if ((i + 1) % 8 == 0)
            		u1_printf("\n");
            }
        }
        else
        {
            strncat(visual_buffer, &visual_ch, 1);
        }
    }
}
