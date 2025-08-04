#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "parameters.h"
#include "keccak.h"
#include "rng_functions.h"
#include "common_functions.h"

// defines all possible 2x2 elementary matrices with permutations: {a1,a2,a3,a4,a5,a6}
// {a1,a2} - position of the off diagonal non zero entry
// {a3,a4} - position of one of the 1's from the diagonal
// {a5,a6} - position of one of the 1's from the diagonal
const bool PA[4][6] = {{0,0,0,1,1,0}, {1,1,0,1,1,0}, {0,1,0,0,1,1}, {1,0,0,0,1,1}};


// unpacks B22^-1 from the secret key
void sk_to_B22inv(const unsigned char* sk, __int128*** B22inv)
{
	int total_bits = B22inv_BITS*S*S*M;
	bool B22inv_bits[total_bits];
    
    // pk to C_bits
    int ski = 48;
    for(int i=0; i<total_bits; i+=8)
    {
    	for(int j=0; j<8; j++)
    	   	B22inv_bits[i+j] = (sk[ski] >> j) & 1;
    	   	
    	ski++;
	}
	
	int bitc = 0;

	for(int i=0; i<S; i++)
	{
		for(int j=0; j<S; j++)
		{
			for(int k=0; k<M; k++)
			{
				__int128 val = 0;
		
				for(int bit=0; bit<B22inv_BITS; bit++)
			    	val |= (__int128)B22inv_bits[bitc++] << bit;
			    
			    B22inv[i][j][k] = val - B22inv_BOUND;
			}
		}
	}
}

// generates a random 2x2 unimodular matrix in the ring
void generate_random_A(__int128*** A)
{
	__int128*** PE = allocate_ring_matrix(2, 2);
	__int128*** T = allocate_ring_matrix(2, 2);
	identity_ring_matrix(2, A);

	for(int r=0; r<KA; r++)
	{
		copy_ring_matrix(2, 2, A, T);
		
		int i = rng_byte()&3; // mod 4
		int k = rng_byte()%M;
		
		zero_ring_matrix(2, 2, PE);
		
		PE[PA[i][0]][PA[i][1]][k] = rng2();
		PE[PA[i][2]][PA[i][3]][0] = 1;
		PE[PA[i][4]][PA[i][5]][0] = 1;

		rmm_multiply(2, 2, 2, T, PE, A);
	}
}

// checks if coefficients in y satisfy its bounds
bool valid_y(__int128** y)
{
	for(int i=0; i<S; i++)
	{
		for(int k=0; k<M; k++)
		{
			__int128 abs_val = y[i][k] < 0 ? -y[i][k] : y[i][k];
			
			if(abs_val>=Y_BOUND)
			{
				return false;
			}
		}
	}

	return true;
}

// packs the message (m) and signature (y) into the signed message (sm)
void my_to_sm(const unsigned char* m, unsigned long long mlen, __int128** y, unsigned char* sm, unsigned long long* smlen)
{
	int total_bits = Y_BITS*S*M;
	bool y_bits[total_bits];
	
	int idx = 0;
	
	// y
	for(int i=0; i<S; i++)
	{
		for(int k=0; k<M; k++)
		{
			__int128 val = y[i][k] + Y_BOUND;
		
			for(int bit=0; bit<Y_BITS; bit++)
        		y_bits[idx++] = (val >> bit) & 1;
		}
	}
	
	// y_bits to sm
	int smi = 0;
	for(int i=0; i<total_bits; i+=8)
	{
		unsigned char val = 0;
		
		for(int j=0; j<8; j++)
			val += y_bits[i+j] << j;
				
		sm[smi++] = val;
	}
	
	*smlen = mlen + smi;
	
	for(int i=0; i<mlen; i++)
		sm[smi+i] = m[i];
}


// DEFIv2 signature generation for a message
int sig_gen(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
{
	initialize_rng((unsigned char*)sk, 48);
	__int128** B21 = allocate_ring_vector(S);
	for(int i=0; i<S; i++)
		for(int k=0; k<M; k++)
	    	B21[i][k] = rngr(DRB, RB);
	
	__int128*** B22inv = allocate_ring_matrix(S, S);
	sk_to_B22inv(sk, B22inv);
	
	__int128*** H = allocate_ring_matrix(2,2);
	hash_of_message(m, mlen, H);
	
	__int128 h[M];
	product_in_ring(H[0][0], H[1][1], h, true);
	
	__int128** B21h = allocate_ring_vector(S);
	for(int i=0; i<S; i++)
		product_in_ring(B21[i], h, B21h[i], true);
	
	free_ring_vector(S, B21); B21 = NULL;
	
	// Initialize rng with entropy source (currently deterministic)
	//.....................................//
	unsigned char new_seed[48];
	FIPS202_SHAKE256(m, mlen, new_seed, 48);
	for(int i=0; i<48; i++)
		new_seed[i] += sk[i];
	initialize_rng(new_seed, 48);
	//.....................................//
	
	__int128*** A1 = allocate_ring_matrix(2, 2);  // Holds D
	__int128*** A2 = allocate_ring_matrix(2, 2);  // Holds A
	__int128*** HA2 = allocate_ring_matrix(2, 2); // Holds H*A
	__int128*** V = allocate_ring_matrix(2, 2); // Holds V = D*H*A
	__int128 V1V2[M];
	__int128 V1V4[M];
	__int128 V2V3[M];
	__int128 V3V4[M];
	__int128** T = allocate_ring_vector(S); // Temporary vector to hold: Z"- B21*H
	__int128** y = allocate_ring_vector(S);

	do
	{
		generate_random_A(A1);
		generate_random_A(A2);
		rmm_multiply(2, 2, 2, H, A2, HA2); 
		rmm_multiply(2, 2, 2, A1, HA2, V);
		
		product_in_ring(V[0][0], V[0][1], V1V2, true);
		product_in_ring(V[0][0], V[1][1], V1V4, true);
		product_in_ring(V[0][1], V[1][0], V2V3, true);
		product_in_ring(V[1][0], V[1][1], V3V4, true);

		for(int k=0; k<M; k++)
		{
			T[0][k] = V1V2[k] + V3V4[k] - B21h[0][k];
			T[1][k] = V1V2[k] - V3V4[k] - B21h[1][k];
			T[2][k] = V1V4[k] + V2V3[k] - B21h[2][k];
		}

		rmv_multiply(S, S, B22inv, T, y);
	}
	while(valid_y(y)==false);
	
	clear_rng();
	
	free_ring_matrix(S, S, B22inv); B22inv = NULL;
	free_ring_matrix(2, 2, H); H = NULL;
	free_ring_vector(S, B21h); B21h = NULL;
	free_ring_matrix(2, 2, A1); A1 = NULL;
	free_ring_matrix(2, 2, A2); A2 = NULL;
	free_ring_matrix(2, 2, HA2); HA2 = NULL;
	free_ring_matrix(2, 2, V); V = NULL;
	free_ring_vector(S, T); T = NULL;
	
	my_to_sm(m, mlen, y, sm, smlen);

	free_ring_vector(S, y); y = NULL;

	return 0;
}

