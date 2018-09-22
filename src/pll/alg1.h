/*
 *  C Implementation: algorithm 1
 *
 * Description: algorithm for the most PLLs
 *
 *
 * Author: Andrey Zhornyak <darion76@gmail.com>, (C) 2014
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#ifndef __ALG1_H_

#define __ALG1_H_

#include "i2c.h"
#include <math.h>
#include "bitmaps.h"

typedef struct
{
    char *name;              // PLL chip name
    const BB_t * N_map;      // bitmap for N
    int   N_add;             // N addition
    const BB_t * M_map;      // bitmap for M
    int   M_add;             // M addition
    const BB_t * bc_map;     // bitmap for bc
    const BB_t * dt_map;     // bitmap for dt
    const BB_t * TME_map;    // bitmap for TME
    unsigned int TME_unlock; // TME_unlock value
    float mn_ratio;          // M, N ratio (for the real FSB value calculation)
    int bc;                  // byte count when read in unhide mode / write
    int bc_hide;             // byte count when read in hyde mode / write
    int cmd;
    int dt_enable;           // Divide table enable or not
    const int* DivTable;
    unsigned int FSB_Min;
    unsigned int FSB_Max;
} alg1_t;                    // algorithm 1 structure definition


unsigned int calc_N(const alg1_t *alg, unsigned int fsb, const unsigned char *buf);

int alg1_unhide( const alg1_t *alg, const int file);

int alg1_CheckFSB(const alg1_t *alg, int fsb, float *ram, float *pci, float *agp);

int alg1_CalcFSB(const alg1_t *alg, const unsigned int n, const unsigned int m, const unsigned char * buf);

int alg1_SetFSB(const alg1_t *alg, int fsb, int test);

int alg1_GetFSB(const alg1_t *alg);

int alg1_GetFirstFSB(const alg1_t *alg);

int alg1_GetNextFSB(const alg1_t *alg);

int alg1_CheckTME(const alg1_t *alg);

#endif // __ALG1_H_
