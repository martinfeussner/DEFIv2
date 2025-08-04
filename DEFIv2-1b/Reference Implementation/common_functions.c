#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "parameters.h"
#include "keccak.h"

__int128** allocate_ring_vector(int n)
{
    __int128** A = malloc(n*sizeof(__int128*));
    
    if(A==NULL)
    {
    	printf("Memory Allocation Failure!");
        exit(2);
	}
    
    for(int i=0; i<n; i++)
    {
        A[i] = calloc(M, sizeof(__int128));
        
        if(A[i]==NULL)
        {
            for(int k=0; k<i; k++)
            	free(A[k]);
            
            free(A);
            printf("Memory Allocation Failure!");
            exit(2);
        }
    }
    
    return A;
}

__int128*** allocate_ring_matrix(int m, int n)
{
    __int128*** A = malloc(m*sizeof(__int128**));
    
    if(A==NULL)
    {
    	printf("Memory Allocation Failure!");
        exit(2);
	}
    
    for(int i=0; i<m; i++)
	{
        A[i] = malloc(n*sizeof(__int128*));
        
        if(A[i]==NULL)
		{
            for(int k=0; k<i; k++)
            	free(A[k]);
                
            free(A);
            printf("Memory Allocation Failure!");
            exit(2);
        }
        
        for(int j=0; j<n; j++)
		{
            A[i][j] = calloc(M, sizeof(__int128));
            
            if(A[i][j]==NULL)
			{
                for(int k=0; k<j; k++)
                	free(A[i][k]);
                    
                free(A[i]);
                
                for(int k=0; k<i; k++)
                {
                	for(int l=0; l<n; l++)
                    	free(A[k][l]);
                    
                	free(A[k]);
				}
                    
               	free(A);
	        	printf("Memory Allocation Failure!");
	            exit(2);
            }
        }
    }
    
    return A;
}

void free_ring_vector(int n, __int128** A)
{
    for(int i=0; i<n; i++)
    	free(A[i]);
    
    free(A);
}

void free_ring_matrix(int m, int n, __int128*** A)
{
    for(int i=0; i<m; i++)
    {
        for(int j=0; j<n; j++)
        	free(A[i][j]);
        
        free(A[i]);
    }
    
    free(A);
}

void zero_vector(int n, __int128* A)
{
	for(int i=0; i<n; i++)
		A[i] = 0;
}

void zero_ring_vector(int n, __int128** A)
{
	for(int i=0; i<n; i++)
		for(int j=0; j<M; j++)
			A[i][j] = 0;
}

void zero_ring_matrix(int m, int n, __int128*** A)
{
	for(int i=0; i<m; i++)
		for(int j=0; j<n; j++)
			for(int k=0; k<M; k++)
				A[i][j][k] = 0;
}

void identity_ring_matrix(int n, __int128*** A)
{
	zero_ring_matrix(n, n, A);		
	for(int i=0; i<n; i++)
		A[i][i][0] = 1;
}

void copy_ring_matrix(int m, int n, __int128*** A, __int128*** B)
{
	for(int i=0; i<m; i++)
		for(int j=0; j<n; j++)
			for(int k=0; k<M; k++)
				B[i][j][k] = A[i][j][k];
}

// performs polynomial multplication modulo x^m+x+1 and stores it in result_poly
void product_in_ring(__int128* poly1, __int128* poly2, __int128* result_poly, bool overwrite)
{
	if(overwrite==true)
		zero_vector(M, result_poly);
	
	for(int i=0; i<M; i++)
	{
		if(poly1[i]!=0)
		{
			for(int j=0; j<M; j++)
			{
				if(poly2[j]!=0)
				{
					int degree = (i+j)%M;
					__int128 val = poly1[i]*poly2[j];
					
					if(i+j<M)
					{
						result_poly[degree] += val;
					}
					else
					{
						result_poly[degree] -= val;
						result_poly[degree+1] -= val;
					}	
				}	
			}		
		}
	}
}

// performs matrix-vector multiplication in the ring
void rmv_multiply(int m, int l, __int128*** A, __int128** b, __int128** c)
{
	zero_ring_vector(m, c);
	
	for(int i=0; i<m; i++)
		for(int j=0; j<l; j++)
			product_in_ring(A[i][j], b[j], c[i], false);
}

// performs matrix-matrix multiplication in the ring
void rmm_multiply(int m, int l, int n, __int128*** A, __int128*** B, __int128*** C)
{
	zero_ring_matrix(m, n, C);
	
	for(int i=0; i<m; i++)
		for(int j=0; j<n; j++)
			for(int k=0; k<l; k++)
				product_in_ring(A[i][k], B[k][j], C[i][j], false);
}

// computes the hash of the message which is stored as a 2x2 matrix
void hash_of_message(const unsigned char* m, unsigned long long mlen, __int128*** h)
{
	int hash_length = HASHSECURITY / 8;
	int bits_per_entry = HASHSECURITY/(N*M/2);
    unsigned char hash_digest[hash_length];
	
    FIPS202_SHAKE256(m, mlen, hash_digest, hash_length);
    
    bool hash_bits[HASHSECURITY];
    int idx = 0;
    for(int i=0; i<hash_length; i++)
    	for(int j=0; j<8; j++)
    		hash_bits[idx++] = (hash_digest[i] >> j) & 1;
	
	idx = 0;
	
	for(int i=0; i<2; i++)
	{
		for(int k=0; k<M; k++)
		{
			__int128 val = 0;
			
			for(int l=0; l<bits_per_entry; l++)
        		val |= (__int128)hash_bits[idx++] << l;
        		
        	h[i][i][k] = val;
		}
}
	
	int sub_value = 1 << (bits_per_entry-1);

	for(int i=0; i<2; i++)
		for(int k=0; k<M; k++)
    		h[i][i][k] -= sub_value;
}

