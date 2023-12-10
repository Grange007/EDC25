/*
 * utility.c
 *
 *  Created on: Dec 3, 2023
 *      Author: 86138
 */

#include "utility.h"
int rolling_mean_cnt = 0;
double rolling_mean_queue[ROLLING_MEAN_LEN];
void RollingMean(float *data, float *mean, uint8_t len)
{
    float sum = 0;
    for (int8_t i = 0; i < len; i++)
    {
        sum += data[i];
    }
    *mean = sum / len;
}


