/*
 * C Implementation: ics932s421b
 *
 * Description: ics932s421b
 *
 *
 * Author: Andrey Zhornyak <darion76@gmail.com>, (C) 2014
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "alg1.h"

#ifdef DEBUG
#include <stdio.h>
#endif

// N bits definitions

const BB_t const N_ics932s421b[] =
{
{"0",  12, 0},
{"1",  12, 1},
{"2",  12, 2},
{"3",  12, 3},
{"4",  12, 4},
{"5",  12, 5},
{"6",  12, 6},
{"7",  12, 7},
{"8",  11, 7},
{"9",  11, 6},
{""}
};

// M bits definitions

const BB_t const M_ics932s421b[] =
{
{"0", 11, 0},
{"1", 11, 1},
{"2", 11, 2},
{"3", 11, 3},
{"4", 11, 4},
{"5", 11, 5},
{""}
};

// bc bits definitions

const BB_t const bc_ics932s421b[] =
{
{"0", 8, 0},
{"1", 8, 1},
{"2", 8, 2},
{"3", 8, 3},
{"4", 8, 4},
{"5", 8, 5},
{"6", 8, 6},
{"7", 8, 7},
{""}
};

const BB_t const TME_ics932s421b[] =
{
{"0", 10, 7},
{""}
};

// Divide table set bits definitions

const BB_t const dt_ics932s421b[] =
{
{"0", 19, 4},
{"1", 19, 5},
{"2", 19, 6},
{"3", 19, 7},
{""}
};

static const int const DivTable_ics932s421b[] =
{
	 2,  3,  5, 15,
	 4,  6, 10, 30,
	 8, 12, 20, 60,
	16, 24, 40,120
};


const alg1_t alg1_ics932s421b = 
{
    "ics932s421b",    // PLL name
    &N_ics932s421b[0],         // N bitmap
    8,                       // N addidion
    &M_ics932s421b[0],         // M bitmap
    2,                       // M addition
    &bc_ics932s421b[0],        // byte count bitmap
    &dt_ics932s421b[0],        // divide table bitmap
    &TME_ics932s421b[0],       // TME bitmap
    1,                // TME unlock value
    14.318f,           // M, N ratio (for the real FSB value calculation)
    21,               // byte count when read in unhide mode / write
    9,               // byte count when read in hyde mode / write
    0,                // command
    1,                // Divide table enable or not
    &DivTable_ics932s421b[0],     // Divide table
    100,              // FSB_Min
    400               // FSB_Max
} ;


//-----------------------------------------------------------------------------------------------
// TODO: need rework main code to implement algorithm based architecture

int ics932s421b_unhide(const int file)
{
    return alg1_unhide(&alg1_ics932s421b, file);
}

int ics932s421b_CheckFSB(int fsb, float *ram, float *pci, float *agp)
{
    return alg1_CheckFSB(&alg1_ics932s421b, fsb, ram, pci, agp);
}

int ics932s421b_CalcFSB(const unsigned int n, const unsigned int m, const unsigned char * buf)
{
    return alg1_CalcFSB(&alg1_ics932s421b, n, m, buf);
}

int ics932s421b_SetFSB(int fsb)
{
    return alg1_SetFSB(&alg1_ics932s421b, fsb, 0);
}

int ics932s421b_GetFSB()
{
    return alg1_GetFSB(&alg1_ics932s421b);
}

int ics932s421b_GetFirstFSB()
{
    return alg1_GetFirstFSB(&alg1_ics932s421b);
}

int ics932s421b_GetNextFSB()
{
    return alg1_GetNextFSB(&alg1_ics932s421b);
}

int ics932s421b_CheckTME()
{
    return alg1_CheckTME(&alg1_ics932s421b);
}


//-----------------------------------------------------------------------------------------------
