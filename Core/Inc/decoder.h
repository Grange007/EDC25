#ifndef DECODER_H
#define DECODER_H

extern int *map_ptr;
void decode_init(int *map); // the map to be changed
void decode_update(char *str); // decode the message and update the given map

#endif
