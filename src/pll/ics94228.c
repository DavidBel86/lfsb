/*
*  C Implementation: ics94228
*
* Description: ics94228
*
*
* Author: Nikolay Kislitsa <deusexbeer@mail.ru>, (C) 2005
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
	float pci;
	float agp;
} PLL_t;
#define BYTECOUNT    17
#define CONTROLBYTE   0
#define CMD 0x00

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	{ 67, 0x4B, 33.33, 66.67},
	{100, 0x4E, 33.33, 66.67},
	{110, 0x4A, 33.00, 66.00},
	{120, 0x49, 30.00, 60.00},
	{133, 0x4F, 33.33, 66.67},
	{140, 0x48, 35.00, 70.00},
	{150, 0x47, 37.50, 75.00},
	{167, 0x4D, 33.33, 66.67},
	{170, 0x46, 34.00, 68.00},
	{180, 0x45, 36.00, 72.00},
	{190, 0x44, 38.00, 76.00},
	{200, 0x43, 33.33, 66.67},
	{210, 0x42, 35.00, 70.00},
	{220, 0x41, 36.67, 73.33},
	{233, 0x40, 38.88, 77.78},
	{0}
};

int ics94228_SetFSB(int fsb)
{
	int i, file, res;
	unsigned char buf[] = {0x00, 0xFF, 0xFF, 0xFF, 0x08, 0x5F, 0x11, 0x0C, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x20, 0x08, 0x20};

	if(fsb < 0) return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
			buf[CONTROLBYTE] = pll[i].ctrlb;

	if(!buf[CONTROLBYTE]) return -1;

	file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	i2c_close();

	if(res < 0) return -1;
#ifdef DEBUG
	else printf("DEBUG: %i bytes written : ", BYTECOUNT);
	for(i=0; i<BYTECOUNT; i++) printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	return 0;
}

int ics94228_GetFSB()
{
	int i, file, res;
	unsigned char buf[] = {0x00, 0xFF, 0xFF, 0xFF, 0x08, 0x5F, 0x11, 0x0C, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x20, 0x08, 0x20};

	file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

	if(res < 0) return -1;
#ifdef DEBUG
	else printf("DEBUG: %i bytes read : ", res);
	for(i=0; i<res; i++) printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	for(i=0; pll[i].fsb; i++)
		if(pll[i].ctrlb == buf[CONTROLBYTE]) return pll[i].fsb;

	return -1;
}

int ics94228_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{
	int i;

	if(sdram) *sdram = -1.0;
	if(pci) *pci = -1.0;
	if(agp) *agp = -1.0;

	if(fsb < 0) return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
		{
			if(pci) *pci = pll[i].pci;
			if(agp) *agp = pll[i].agp;
			return 0;
		}

	return -1;
}

int ics94228_GetFirstFSB()
{
	FSBIndex = 0;
  if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
  else return -1;
}

int ics94228_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
	else return -1;
}

