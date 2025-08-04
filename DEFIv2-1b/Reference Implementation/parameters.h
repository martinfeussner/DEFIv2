#ifndef parameters_h
#define parameters_h

// Parameters described in the paper
#define M 28 // $m$
#define N 4 // $n$
#define S 3 // $s$
#define R 1 // $r$
#define KB 13 // $k_B$
#define KA 10 // $k_{AD}$
#define RF 4 // $\delta_{F}$
#define RB 8 // $\delta_{B_{21}}$
#define G 112 // $\Omega_{B}$
#define C1_BOUND 2048 // $\gamma_{C_1}$
#define C2_BOUND 4096 // $\gamma_{C_2}$
#define C3_BOUND 32768 // $\gamma_{C_3}$
#define B22_BOUND 128 // $\gamma_{B_{22}}$
#define B22inv_BOUND 2048 // $\gamma_{B_{22}^{-1}}$
#define Y_BOUND 562949953421312LL // $\gamma_{y}$
#define HASHSECURITY 280 // $d$

// Calculated from the parameters above to keep as constants
#define SF 6 // $s!$
#define DRF 8 // double $\delta_{F}$
#define DRB 16 // double $\delta_{B_{21}}$
#define C1_BITS 12 // Computed: $log_2(2*\gamma_{C_1})$
#define C2_BITS 13 // Computed: $log_2(2*\gamma_{C_2})$
#define C3_BITS 16 // Computed: $log_2(2*\gamma_{C_3})$
#define B22inv_BITS 12 // Computed: $log_2(2*\gamma_{B_{22}^{-1}})$
#define Y_BITS 50 // Computed: $log_2(2*\gamma_{y})$

#endif
