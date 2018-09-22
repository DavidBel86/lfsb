/*
 *  C Implementation: ics9ums9610bl
 *
 * Description: ics9ums9610bl
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

const BB_t const N_ics9ums9610bl[] =
{
{"0",   5, 0},
{"1",   5, 1},
{"2",   5, 2},
{"3",   5, 3},
{"4",   5, 4},
{"5",   5, 5},
{"6",   5, 6},
{"7",   5, 7},
{"8",   4, 7},
{"9",   4, 6},
{"10", 21, 7},
{"11", 21, 6},
{""}
};

// M bits definitions

const BB_t const M_ics9ums9610bl[] =
{
{"0", 4, 0},
{"1", 4, 1},
{"2", 4, 2},
{"3", 4, 3},
{"4", 4, 4},
{"5", 4, 5},
{""}
};

// bc bits definitions

const BB_t const bc_ics9ums9610bl[] =
{
{"0", 15, 0},
{"1", 15, 1},
{"2", 15, 2},
{"3", 15, 3},
{"4", 15, 4},
{"5", 15, 5},
{"6", 15, 6},
{""}
};

const BB_t const TME_ics9ums9610bl[] =
{
{"0", 16, 7},
{""}
};

// Divide table set bits definitions

const BB_t const dt_ics9ums9610bl[] =
{
{"0", 0, 0},
{"1", 0, 1},
{"2", 0, 2},
{"3", 0, 3},
{""}
};

static const int const DivTable_ics9ums9610bl[] =
{
	0x02, 0x03, 0x05, 0x07,
	0x04, 0x06, 0x0A, 0x0E,
	0x08, 0x0C, 0x14, 0x1C,
	0x10, 0x18, 0x28, 0x38
};


const alg1_t alg1_ics9ums9610bl = 
{
    "ics9ums9610bl",    // PLL name
    &N_ics9ums9610bl[0],         // N bitmap
    0,                           // N addition
    &M_ics9ums9610bl[0],         // M bitmap
    0,                           // M addition
    &bc_ics9ums9610bl[0],        // byte count bitmap
    &dt_ics9ums9610bl[0],        // divide table bitmap
    &TME_ics9ums9610bl[0],       // TME bitmap
    1,                // TME unlock value
    4.773f,           // M, N ratio (for the real FSB value calculation)
    23,               // byte count when read in unhide mode / write
    16,               // byte count when read in hyde mode / write
    0,                // command
    0,                // Divide table enable or not
    &DivTable_ics9ums9610bl[0],     // Divide table
    100,              // FSB_Min
    400               // FSB_Max
} ;


//-----------------------------------------------------------------------------------------------
// TODO: need rework main code to implement algorithm based architecture

int ics9ums9610bl_unhide(const int file)
{
    return alg1_unhide(&alg1_ics9ums9610bl, file);
}

int ics9ums9610bl_CheckFSB(int fsb, float *ram, float *pci, float *agp)
{
    return alg1_CheckFSB(&alg1_ics9ums9610bl, fsb, ram, pci, agp);
}

int ics9ums9610bl_CalcFSB(const unsigned int n, const unsigned int m, const unsigned char * buf)
{
    return alg1_CalcFSB(&alg1_ics9ums9610bl, n, m, buf);
}

int ics9ums9610bl_SetFSB(int fsb)
{
    return alg1_SetFSB(&alg1_ics9ums9610bl, fsb, 0);
}

int ics9ums9610bl_GetFSB()
{
    return alg1_GetFSB(&alg1_ics9ums9610bl);
}

int ics9ums9610bl_GetFirstFSB()
{
    return alg1_GetFirstFSB(&alg1_ics9ums9610bl);
}

int ics9ums9610bl_GetNextFSB()
{
    return alg1_GetNextFSB(&alg1_ics9ums9610bl);
}

int ics9ums9610bl_CheckTME()
{
    return alg1_CheckTME(&alg1_ics9ums9610bl);
}


//-----------------------------------------------------------------------------------------------
