/*
 *  C Implementation: rtm875t-606
 *
 * Description: rtm875t-606
 *
 *
 * Author: Nikolay Kislitsa <deusexbeer@mail.ru>, (C) 2013
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "i2c.h"

#ifdef DEBUG
#include <stdio.h>
#endif

typedef struct
{
	unsigned char fsb;
	unsigned char ctrlb;
	unsigned char ctrlb1;
} PLL_t;

#define BYTECOUNT    32
#define CONTROLBYTE  0x11
#define CONTROLBYTE1 0x13
#define CMD 0x00

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	{ 100, 0x58, 0x23},
	{ 117, 0x88, 0x87},
	{ 125, 0x88, 0xA3},
	{ 133, 0x20, 0x33},
	{ 142, 0x88, 0xDB},
	{ 150, 0x88, 0xF7},
	{ 158, 0x48, 0x13},
	{ 167, 0xE8, 0x33},
	{ 183, 0x4C, 0x43},
	{ 200, 0xB0, 0x43},
	{0}
};


int rtm875t_606_SetFSB(int fsb)
{
	int file;
	int i, res;
	unsigned char buf[128];

	if(fsb < 0)
		return -1;

	file = i2c_open();
	if(file < 0)
		return -1;

	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res < 0)
		return res;
#ifdef DEBUG
	else
	{
		printf("SetFSB DEBUG: %i bytes read : ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	buf[0x00] |= 0x07;
	buf[0x01] = 0x89;
	buf[0x1D] = 0x32;
	buf[0x1E] = 0x30;
	buf[0x1F] = 0x57;
	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
		{
			buf[CONTROLBYTE] = pll[i].ctrlb;
			buf[CONTROLBYTE1] = pll[i].ctrlb1;
			break;
		}
	if(!buf[CONTROLBYTE])
		return -1;

	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	i2c_close();
	if(res < 0)
		return res;
#ifdef DEBUG
	else
		printf("DEBUG: %i bytes written : ", BYTECOUNT);
	for(i=0; i<BYTECOUNT; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	return 0;
}

int rtm875t_606_GetFSB()
{
	int file;
	int i, res;
	unsigned char buf[128];

	file = i2c_open();
	if(file < 0)
		return -1;
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

	if(res < 0)
		return -1;
#ifdef DEBUG
	else
		printf("DEBUG: %i bytes read : ", res);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	for(i=0; pll[i].fsb; i++)
		if(pll[i].ctrlb == buf[CONTROLBYTE] && pll[i].ctrlb1 == buf[CONTROLBYTE1])
			return pll[i].fsb;

	return -1;
}

int rtm875t_606_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{
	int i;

	if(sdram)
		*sdram = -1.0;
	if(pci)
		*pci = -1.0;
	if(agp)
		*agp = -1.0;

	if(fsb < 0)
		return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
			return 0;

	return -1;
}

int rtm875t_606_GetFirstFSB()
{
	FSBIndex = 0;
	if(pll[FSBIndex].fsb)
		return pll[FSBIndex].fsb;
	else
		return -1;
}

int rtm875t_606_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb)
		return pll[FSBIndex].fsb;
	else
		return -1;
}

int  rtm875t_606_CheckTME()
{
	int file, res;
	unsigned char buf[128];

	file = i2c_open();
	if(file < 0)
		return -1;

	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

#ifdef DEBUG
	printf("CheckTME DEBUG: %i bytes read : ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res < 0)
		return -1;

	if(buf[9] & 0x40)
		return 1;

	return 0;
}
