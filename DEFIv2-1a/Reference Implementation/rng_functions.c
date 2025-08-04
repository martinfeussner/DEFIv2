#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rng.h"

// Defines the buffer size and buffer holding the random values for the RNG.
#define RNG_BUFFER_SIZE 1024 
unsigned char rng_buffer[RNG_BUFFER_SIZE];
int rng_buffer_idx = RNG_BUFFER_SIZE;  // Trigger refill on first use.
int current_bit_idx = 8;  // Start at 8 to trigger byte fetch on first use.
unsigned char current_byte = 0;  // Holds the byte from which bits are being extracted.

/**
 * This function refills the rng_buffer with random bytes.
 * It uses the 'randombytes' function to fill the buffer.
 */
void refill_rng_buffer()
{
    randombytes(rng_buffer, RNG_BUFFER_SIZE);
    rng_buffer_idx = 0;
}

/**
 * This function provides a random bit.
 * 
 * When called, the function extracts the next bit from the current_byte.
 * If all bits from the current_byte have been used, it fetches the next byte from the rng_buffer.
 * When the rng_buffer is exhausted, it gets refilled with random bytes.
 *
 * @return A random bit.
 */
bool rng_bit()
{
    if(current_bit_idx == 8)  // All bits in the current byte are used
    {
        if(rng_buffer_idx == RNG_BUFFER_SIZE)  // If buffer is empty, refill
        	refill_rng_buffer();
        	
        current_byte = rng_buffer[rng_buffer_idx];
        current_bit_idx = 0;  // Reset bit index for the new byte
        rng_buffer_idx++;     // Move to the next byte
    }

    bool bit = (current_byte & (1 << current_bit_idx)) != 0;
    current_bit_idx++;

    return bit;
}

/**
 * This function provides a random byte.
 * Bytes are fetched from the rng_buffer.
 *
 * @return A random byte.
 */
unsigned char rng_byte()
{
    if(rng_buffer_idx == RNG_BUFFER_SIZE)  // If buffer is empty, refill
    	refill_rng_buffer();
    	
    return rng_buffer[rng_buffer_idx++];
}

/**
 * This function provides a random +-1
 */
int rng2()
{
	return (rng_bit()==0)?1:-1;
}

/**
 * This function provides a random number from [-hr, hr]/{0}. r is the range and hr is half of r.
 */
int rngr(int r, int hr)
{
    int a = rng_byte() & (r - 1);   // Efficient random number in [0, r-1]
    int adjustment = (a >= hr);  // This will be 1 if a >= r/2, 0 otherwise
    return a - hr + adjustment;  // Shift to [-r/2, -1] or [1, r/2], excludes 0
}

/**
 * This function initializes the RNG with a given seed.
 * It also resets the buffer and bit indices to trigger refilling the buffer and fetching a new byte on their next respective uses.
 *
 * @param seed        A pointer to the seed array.
 * @param seed_size   The size of the seed array.
 */
void initialize_rng(unsigned char* seed, int seed_size)
{
	randombytes_init(seed, NULL, 8*seed_size);
    rng_buffer_idx = RNG_BUFFER_SIZE;
	current_bit_idx = 8;
}

/**
 * This function zeros the rng_buffer and current_byte
 */
void clear_rng()
{
	for(int i=0; i<RNG_BUFFER_SIZE; i++)
		rng_buffer[i] = 0;
	
	current_byte = 0;
}



