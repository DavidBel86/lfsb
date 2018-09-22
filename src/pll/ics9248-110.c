/*
*  C Implementation: ics9248-110
*
* Description: ics9248-110
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
	float sdram;
	float pci;
	float agp;
} PLL_t;
#define BYTECOUNT   7
#define CONTROLBYTE 6
#define CMD 0x00

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	{ 90, 0x03,  90.00, 30.00, 60.00},
	{ 95, 0x13,  95.00, 31.67, 63.33},
	{100, 0x43, 100.70, 33.57, 67.13},
	{101, 0x23, 100.99, 33.66, 67.33},
	{102, 0x07, 102.00, 34.00, 68.00},
	{103, 0x53, 103.00, 34.33, 68.67},
	{104, 0x17, 104.00, 34.76, 69.33},
	{105, 0x63, 105.00, 35.00, 70.00},
	{106, 0x27, 106.00, 35.33, 70.67},
	{107, 0x37, 107.00, 35.67, 71.33},
	{108, 0x47, 108.00, 36.00, 72.00},
	{109, 0x57, 109.00, 36.33, 72.67},
	{110, 0x67, 110.00, 36.67, 73.33},
	{111, 0x77, 111.00, 37.00, 74.00},
	{112, 0x0B, 112.00, 37.33, 74.67},
	{113, 0x1B, 113.00, 37.67, 75.33},
	{114, 0x2B, 114.00, 38.00, 76.00},
	{115, 0x33, 115.00, 38.33, 76.67},
	{116, 0x3B, 116.00, 38.67, 77.33},
	{117, 0x4B, 117.00, 39.00, 78.00},
	{118, 0x5B, 118.00, 39.33, 78.67},
	{119, 0x6B, 119.00, 39.67, 79.33},
	{120, 0x7B, 120.00, 30.00, 60.00},
	{121, 0x0F, 121.00, 30.25, 60.50},
	{122, 0x1F, 122.00, 30.50, 61.00},
	{123, 0x2F, 123.00, 30.75, 61.50},
	{124, 0x3F, 124.00, 31.00, 62.00},
	{125, 0x4F, 124.00, 31.25, 62.50},
	{133, 0x5F, 133.33, 33.33, 66.67},
	{140, 0x6F, 140.00, 35.00, 70.00},
	{150, 0x7F, 150.00, 37.50, 75.00},
	{0}
};

int ics9248_110_SetFSB(int fsb)
{
	int i, file, res;
	unsigned char buf[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

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

int ics9248_110_GetFSB()
{
	int i, file, res;
	unsigned char buf[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

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

int ics9248_110_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{
	int i;

	if(sdram) *sdram = -1.0;
	if(pci) *pci = -1.0;
	if(agp) *agp = -1.0;

	if(fsb < 0) return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
		{
			if(sdram) *sdram = pll[i].sdram;
			if(pci) *pci = pll[i].pci;
			if(agp) *agp = pll[i].agp;
			return 0;
		}

	return -1;
}

int ics9248_110_GetFirstFSB()
{
	FSBIndex = 0;
  if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
  else return -1;
}

int ics9248_110_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
	else return -1;
}

