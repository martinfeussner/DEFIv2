#ifndef rng_functions_h
#define rng_functions_h

#include <stdbool.h>

void refill_rng_buffer(void);
bool rng_bit(void);
unsigned char rng_byte(void);
int rng2(void);
int rngr(int r, int hr);
void initialize_rng(unsigned char* seed, int seed_size);
void clear_rng(void);

#endif
