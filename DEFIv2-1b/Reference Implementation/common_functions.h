#ifndef common_functions_h
#define common_functions_h

#include <stdbool.h>

__int128** allocate_ring_vector(int n);
__int128*** allocate_ring_matrix(int m, int n);
void free_ring_vector(int n, __int128** A);
void free_ring_matrix(int m, int n, __int128*** A);
void zero_vector(int n, __int128* A);
void zero_ring_vector(int n, __int128** A);
void zero_ring_matrix(int m, int n, __int128*** A);
void identity_ring_matrix(int n, __int128*** A);
void copy_ring_matrix(int m, int n, __int128*** A, __int128*** B);
void product_in_ring(__int128* poly1, __int128* poly2, __int128* result_poly, bool overwrite);
void rmv_multiply(int m, int l, __int128*** A, __int128** b, __int128** c);
void rmm_multiply(int m, int l, int n, __int128*** A, __int128*** B, __int128*** C);
void hash_of_message(const unsigned char* m, unsigned long long mlen, __int128*** h);

#endif
