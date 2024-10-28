#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "parameters.h"
#include "common_functions.h"

// unpacks the public key into C
void pk_to_C(const unsigned char* pk, __int128*** C)
{
	int total_bits = C1_BITS*M + C2_BITS*M*S + C3_BITS*M*N*S/2;
    bool C_bits[total_bits];
    
    // pk to C_bits
    int pkc = 0;
    for(int i=0; i<total_bits; i+=8)
    {
    	for(int j=0; j<8; j++)
    	   	C_bits[i+j] = (pk[pkc] >> j) & 1;
    	   	
    	pkc++;
	}
	
	int bitc = 0;
    	
    // C1
	for(int k=0; k<M; k++)
	{
		__int128 val = 0;
		
		for(int bit=0; bit<C1_BITS; bit++)
        	val |= (__int128)C_bits[bitc++] << bit;
        
        C[0][0][k] = val - C1_BOUND;
	}
	
	// C2
	for(int j=1; j<N; j++)
	{
		for(int k=0; k<M; k++)
		{
			__int128 val = 0;
		
			for(int bit=0; bit<C2_BITS; bit++)
	        	val |= (__int128)C_bits[bitc++] << bit;
	        
	        C[0][j][k] = val - C2_BOUND;
		}
	}
	
	//print_ring_matrix(N, N, C);
	
	// C3
	for(int i=1; i<N; i++)
	{
		for(int j=i; j<N; j++)
		{
			for(int k=0; k<M; k++)
			{
				__int128 val = 0;
		
				for(int bit=0; bit<C3_BITS; bit++)
			    	val |= (__int128)C_bits[bitc++] << bit;
			    
			    C[i][j][k] = val - C3_BOUND;
			}
		}
	}
	
	//print_ring_matrix(N, N, C);
	
	for(int i=0; i<N; i++)
        for(int j=0; j<i; j++)
			for(int k=0; k<M; k++)
				C[i][j][k] = C[j][i][k];
}

// unpacks the signature and message into y and m
void sm_to_my(const unsigned char* sm, unsigned long long smlen, unsigned char* m, unsigned long long* mlen, __int128** y)
{   
	int total_bits = Y_BITS*S*M;
	bool y_bits[total_bits];
    
    // sm to bits
    int smi = 0;
    for(int i=0; i<total_bits; i+=8)
    {
    	for(int j=0; j<8; j++)
    	   	y_bits[i+j] = (sm[smi] >> j) & 1;
    	   	
    	smi++;
	}
	
	int idx = 0;
	
	// y	
	for(int i=0; i<S; i++)
	{
		for(int k=0; k<M; k++)
		{
			__int128 val = 0;
		
			for(int bit=0; bit<Y_BITS; bit++)
	        	val |= (__int128)y_bits[idx++] << bit;
	        
	        y[i][k] = val - Y_BOUND;
		}
	}
    
    *mlen = smlen - smi;
    
    for(int i=0; i<*mlen; i++)
		m[i] = sm[smi+i];
}


// DEFIv2 signature verification
int sig_ver(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long smlen, const unsigned char *pk)
{
	__int128*** C = allocate_ring_matrix(N, N);
	pk_to_C(pk, C);
	
	__int128** y = allocate_ring_vector(S);
	sm_to_my(sm, smlen, m, mlen, y);
	
	if(valid_y(y) == false)
		return -1; // Verification Unsuccessfull
		
	__int128*** H = allocate_ring_matrix(2,2);
	hash_of_message(m, *mlen, H);
	
	__int128 v1v4[M];
	__int128 v2v3[M];
	product_in_ring(H[0][0], H[1][1], v1v4, true);
	product_in_ring(H[0][1], H[1][0], v2v3, true);
	
	free_ring_matrix(2, 2, H); H = NULL;
	
	__int128** z = allocate_ring_vector(N);	
	
	for(int k=0; k<M; k++)
		z[0][k] = v1v4[k] - v2v3[k];
			
	for(int i=0; i<S; i++)
		for(int k=0; k<M; k++)
			z[R+i][k] = y[i][k];
	
	free_ring_vector(S, y); y = NULL;
	
	__int128** Cz = allocate_ring_vector(N);
	rmv_multiply(N, N, C, z, Cz);

	__int128 zCz[M];
	zero_vector(M, zCz);

	for(int i=0; i<N; i++)
		product_in_ring(z[i], Cz[i], zCz, false);
	
	free_ring_matrix(N, N, C); C = NULL;
	free_ring_vector(N, z); z = NULL;
	free_ring_vector(N, Cz); Cz = NULL;
	
	for(int k=0; k<M; k++)
		if(zCz[k]!=0)
			return -1; // Verification Unsuccessfull

	return 0; // Verification Successfull	
}

