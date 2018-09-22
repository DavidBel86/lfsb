/*
*  C Implementation: ics9248-141
*
* Description: ics9248-141
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
} PLL_t;
#define BYTECOUNT   6
#define CONTROLBYTE 0
#define CMD 0x00

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	{ 90, 0x0A, 30.00},
	{ 95, 0x1A, 31.67},
	{100, 0x7E, 33.33},
	{101, 0x4A, 33.57},
	{102, 0x3A, 34.00},
	{103, 0x5A, 34.33},
	{105, 0x6A, 35.00},
	{107, 0x8A, 35.67},
	{109, 0x9A, 36.33},
	{110, 0xAA, 36.67},
	{111, 0xBA, 37.00},
	{113, 0xCA, 37.67},
	{115, 0xDA, 38.33},
	{117, 0xEA, 39.00},
	{120, 0x0E, 40.00},
	{125, 0x1E, 31.25},
	{130, 0x2E, 32.50},
	{133, 0xFE, 33.33},
	{134, 0x3E, 33.43},
	{135, 0x4E, 33.75},
	{137, 0x5E, 34.25},
	{139, 0x6E, 34.75},
	{140, 0x8E, 35.00},
	{143, 0x9E, 35.75},
	{145, 0xAE, 36.25},
	{148, 0xBE, 37.00},
	{150, 0xCE, 37.50},
	{155, 0xDE, 38.75},
	{167, 0xEE, 41.67},
	{0}
};

int ics9248_141_SetFSB(int fsb)
{
	int i, file, res;
	unsigned char buf[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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

int ics9248_141_GetFSB()
{
	int i, file, res;
	unsigned char buf[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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

int ics9248_141_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
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
		return 0;
	}

	return -1;
}

int ics9248_141_GetFirstFSB()
{
	FSBIndex = 0;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
  else return -1;
}

int ics9248_141_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
	else return -1;
}

