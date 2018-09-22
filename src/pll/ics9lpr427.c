/*
 *  C Implementation: ics9lpr427.c
 *
 * Description: ics9lpr427 PLL driver
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

const BB_t const N_bm[] =
{
    {"0",  11, 6},
    {"1",  11, 7},
    {"2",  12, 0},
    {"3",  12, 1},
    {"4",  12, 2},
    {"5",  12, 3},
    {"6",  12, 4},
    {"7",  12, 5},
    {"8",  12, 6},
    {"9",  12, 7},
    {""}
};

// M bits definitions

const BB_t const M_bm[] =
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

const BB_t const bc_bm[] =
{
    {"0", 8, 0},
    {"1", 8, 1},
    {"2", 8, 2},
    {"3", 8, 3},
    {"4", 8, 4},
    {""}
};

const BB_t const TME_bm[] =
{
    {"0", 1, 0},
    {""}
};

// Divide table set bits definitions

const BB_t const dt_bm[] =
{
    {"0", 28, 0},
    {"1", 28, 1},
    {"2", 28, 2},
    {"3", 28, 3},
    {""}
};

static const int const DivTable[] =
{
    0x02, 0x03, 0x05, 0x07,
    0x04, 0x06, 0x0A, 0x0E,
    0x08, 0x0C, 0x14, 0x1C,
    0x10, 0x18, 0x28, 0x38
};

const alg1_t alg1_ics9lpr427 = 
{
    "ics9lpr427",    // PLL name
    &N_bm[0],         // N bitmap
    0,                // N addition
    &M_bm[0],         // M bitmap
    0,                // M addition
    &bc_bm[0],        // byte count bitmap
    &dt_bm[0],        // divide table bitmap
    &TME_bm[0],       // TME bitmap
    1,                // TME unlock value
    125.0f,           // M, N ratio (for the real FSB value calculation)
    31,               // byte count when read in unhide mode / write
    15,               // byte count when read in hyde mode / write
    0,                // command
    1,                // Divide table enable or not
    &DivTable[0],     // Divide table
    80,              // FSB_Min
    400               // FSB_Max
} ;


//-----------------------------------------------------------------------------------------------
// TODO: need rework main code to implement algorithm based architecture

int ics9lpr427_unhide(const int file)
{
    return alg1_unhide(&alg1_ics9lpr427, file);
}

int ics9lpr427_CheckFSB(int fsb, float *ram, float *pci, float *agp)
{
    return alg1_CheckFSB(&alg1_ics9lpr427, fsb, ram, pci, agp);
}

int ics9lpr427_CalcFSB(const unsigned int n, const unsigned int m, const unsigned char * buf)
{
    return alg1_CalcFSB(&alg1_ics9lpr427, n, m, buf);
}

int ics9lpr427_SetFSB(int fsb)
{
    return alg1_SetFSB(&alg1_ics9lpr427, fsb, 0);
}

int ics9lpr427_GetFSB()
{
    return alg1_GetFSB(&alg1_ics9lpr427);
}

int ics9lpr427_GetFirstFSB()
{
    return alg1_GetFirstFSB(&alg1_ics9lpr427);
}

int ics9lpr427_GetNextFSB()
{
    return alg1_GetNextFSB(&alg1_ics9lpr427);
}

int ics9lpr427_CheckTME()
{
    return alg1_CheckTME(&alg1_ics9lpr427);
}


//-----------------------------------------------------------------------------------------------
