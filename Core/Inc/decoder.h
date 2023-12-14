extern int (* map_ptr)[8];
void decode_init(int (*map)[8]); // the map to be changed
void decode_update(char str); // decode the message and update the given map