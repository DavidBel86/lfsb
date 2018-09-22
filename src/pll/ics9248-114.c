/*
*  C Implementation: ics9248-114
*
* Description: ics9248-114
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
	{ 67, 0x3A, 33.40},
	{ 75, 0x1A, 37.50},
	{ 83, 0x2A, 41.65},
	{ 90, 0x0E, 30.00},
	{ 92, 0x1E, 30.83},
	{ 95, 0x2E, 31.67},
	{ 98, 0x3E, 32.50},
	{100, 0x7A, 33.33},
	{102, 0x4E, 33.83},
	{103, 0x4A, 34.33},
	{105, 0xBA, 35.00},
	{108, 0xBE, 35.83},
	{110, 0xAA, 36.67},
	{112, 0x5A, 37.33},
	{115, 0x9A, 38.33},
	{118, 0x9E, 39.17},
	{120, 0x8A, 40.00},
	{122, 0xAE, 40.67},
	{124, 0xEA, 31.00},
	{127, 0x5E, 42.33},
	{130, 0xEE, 32.50},
	{133, 0xFA, 33.33},
	{136, 0x6E, 34.13},
	{140, 0xCA, 35.00},
	{145, 0xCE, 36.25},
	{150, 0xDA, 37.50},
	{155, 0xDE, 38.75},
	{0}
};

int ics9248_114_SetFSB(int fsb)
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

int ics9248_114_GetFSB()
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

int ics9248_114_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
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

int ics9248_114_GetFirstFSB()
{
	FSBIndex = 0;
  if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
  else return -1;
}

int ics9248_114_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
	else return -1;
}

