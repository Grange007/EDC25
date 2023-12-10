/*
 * utility.h
 *
 *  Created on: Dec 3, 2023
 *      Author: 86138
 */

#ifndef INC_UTILITY_H_
#define INC_UTILITY_H_

#define ROLLING_MEAN_LEN 10
void RollingMean(float *data, float *mean, uint8_t len);

int rolling_mean_cnt = 0;


#endif /* INC_UTILITY_H_ */
