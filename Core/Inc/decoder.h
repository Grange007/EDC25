#ifndef DECODER_H
#define DECODER_H
#include "main.h"
extern uint8_t *map_ptr;
void decode_init(uint8_t *map); // the map to be changed
void decode_update(char *str); // decode the message and update the given map

#endif
