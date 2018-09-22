/*
 *  C Implementation: ics9lpr427a.c
 *
 * Description: ics9lpr427a PLL driver
 *
 *
 * Author: Andrey Zhornyak <darion76@gmail.com>, (C) 2015
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "alg1.h"

#ifdef DEBUG
#include <stdio.h>
#endif

// N bits definitions

const BB_t const N_ics9lpr427a[] =
{
    {"0",  15, 6},
    {"1",  15, 7},
    {"2",  16, 0},
    {"3",  16, 1},
    {"4",  16, 2},
    {"5",  16, 3},
    {"6",  16, 4},
    {"7",  16, 5},
    {"8",  16, 6},
    {"9",  16, 7},
    {""}
};

// M bits definitions

const BB_t const M_ics9lpr427a[] =
{
    {"0", 15, 0},
    {"1", 15, 1},
    {"2", 15, 2},
    {"3", 15, 3},
    {"4", 15, 4},
    {"5", 15, 5},
    {""}
};

// bc bits definitions

const BB_t const bc_ics9lpr427a[] =
{
    {"0", 8, 0},
    {"1", 8, 1},
    {"2", 8, 2},
    {"3", 8, 3},
    {"4", 8, 4},
    {""}
};

const BB_t const TME_ics9lpr427a[] =
{
    {"0", 1, 0},
    {""}
};

// Divide table set bits definitions

const BB_t const dt_ics9lpr427a[] =
{
    {"0", 29, 0},
    {"1", 29, 1},
    {"2", 29, 2},
    {"3", 29, 3},
    {""}
};

static const int const DivTable_ics9lpr427a[] =
{
    0x02, 0x03, 0x05, 0x07,
    0x04, 0x06, 0x0A, 0x0E,
    0x08, 0x0C, 0x14, 0x1C,
    0x10, 0x18, 0x28, 0x38
};

const alg1_t alg1_ics9lpr427a = 
{
    "ics9lpr427a",    // PLL name
    &N_ics9lpr427a[0],         // N bitmap
    0,                // N addition
    &M_ics9lpr427a[0],         // M bitmap
    0,                // M addition
    &bc_ics9lpr427a[0],        // byte count bitmap
    &dt_ics9lpr427a[0],        // divide table bitmap
    &TME_ics9lpr427a[0],       // TME bitmap
    1,                // TME unlock value
    28.7f,           // M, N ratio (for the real FSB value calculation)
    31,               // byte count when read in unhide mode / write
    15,               // byte count when read in hyde mode / write
    0,                // command
    1,                // Divide table enable or not
    &DivTable_ics9lpr427a[0],     // Divide table
    80,              // FSB_Min
    400               // FSB_Max
} ;


//-----------------------------------------------------------------------------------------------
// TODO: need rework main code to implement algorithm based architecture

int ics9lpr427a_unhide(const int file)
{
    return alg1_unhide(&alg1_ics9lpr427a, file);
}

int ics9lpr427a_CheckFSB(int fsb, float *ram, float *pci, float *agp)
{
    return alg1_CheckFSB(&alg1_ics9lpr427a, fsb, ram, pci, agp);
}

int ics9lpr427a_CalcFSB(const unsigned int n, const unsigned int m, const unsigned char * buf)
{
    return alg1_CalcFSB(&alg1_ics9lpr427a, n, m, buf);
}

int ics9lpr427a_SetFSB(int fsb)
{
    return alg1_SetFSB(&alg1_ics9lpr427a, fsb, 1);
}

int ics9lpr427a_GetFSB()
{
    return alg1_GetFSB(&alg1_ics9lpr427a);
}

int ics9lpr427a_GetFirstFSB()
{
    return alg1_GetFirstFSB(&alg1_ics9lpr427a);
}

int ics9lpr427a_GetNextFSB()
{
    return alg1_GetNextFSB(&alg1_ics9lpr427a);
}

int ics9lpr427a_CheckTME()
{
    return alg1_CheckTME(&alg1_ics9lpr427a);
}


//-----------------------------------------------------------------------------------------------
