/*
 *  C Implementation: bitmaps.h
 *
 * Description: bitmapping utilities for PLL registers manipulations
 *
 *
 * Author: Andrey Zhornyak <darion76@gmail.com>, (C) 2014
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */

#ifndef __BITMAPS_H_

#define __BITMAPS_H_

#include <math.h>

typedef struct
{
    char *bit; // Bit name, just for reference
    int byte;  // PLL Register byte number (starting from 0)
    int bbit;  // bit number
} BB_t; // bits definition main structure


unsigned int get_bb (const BB_t *map,const unsigned char *buf);

void set_bb (const BB_t *map, unsigned char *buf, const unsigned int value);


#endif //__BITMAPS_H_
