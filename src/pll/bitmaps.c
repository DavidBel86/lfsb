/*
 *  C Implementation: bitmaps.c
 *
 * Description: bitmapping utilities for PLL registers manipulations
 *
 *
 * Author: Andrey Zhornyak <darion76@gmail.com>, (C) 2014
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "bitmaps.h"

unsigned int get_bb (const BB_t *map,const unsigned char *buf)
{
    int i;
    unsigned int value;

    value = 0;
    for(i=0; map[i].bit[0]; i++)
    {
	value |= ((buf[map[i].byte] & (1<<map[i].bbit))?1:0) << i;
    }
    return value;
}


void set_bb (const BB_t *map, unsigned char *buf, const unsigned int value)
{
    int i;

    for(i=0; map[i].bit[0]; i++)
    {
	if (value & (1<<i))
	    buf[map[i].byte] |= 1 << map[i].bbit;
	else
	    buf[map[i].byte] &= ~(1 << map[i].bbit);
    }
}
