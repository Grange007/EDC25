#include "visual.h"
#include "decoder.h"
#include "string.h"
#include "main.h"
#include "usart.h"
char visual_ch;
char visual_buffer[10];
void visual_receive(void)
{
    HAL_Delay(1000);
    for (int i = 0; i < 30; i++)
    {
        HAL_UART_Receive(&huart5, &visual_ch, sizeof(visual_ch), HAL_MAX_DELAY);
        if (visual_ch == '\n')
        {
            decode_update(visual_buffer);
            memset(visual_buffer, 0, sizeof(visual_buffer));
        }
        else
        {
            strncat(visual_buffer, &visual_ch, 1);
        }
    }
}
