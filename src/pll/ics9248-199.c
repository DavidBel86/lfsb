/*
*  C Implementation: ics9248-199
*
* Description: ics9248-199
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
	float sdram;
	float agp;
} PLL_t;
#define BYTECOUNT   1
#define CONTROLBYTE 4
#define CMD 0x00

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	{ 67, 0x0A, 33.33, 66.66, 66.66},
//	{ 67, 0x4A, 33.33, 100.00, 66.66},
	{ 90, 0x0E, 30.00, 90.00, 60.00},
	{100, 0x1A, 33.33, 100.00, 66.66},
//	{100, 0x5A, 33.33, 66.66, 66.66},
//	{100, 0x6A, 33.33, 133.33, 66.66},
//	{100, 0xCA, 31.25, 166.66, 62.50},
	{101, 0x1E, 33.63, 100.90, 67.27},
//	{101, 0x6E, 33.48, 133.90, 66.95},
	{103, 0x2E, 34.33, 103.00, 68.67},
	{107, 0x8E, 35.66, 107.00, 71.33},
//	{107, 0x9E, 35.66, 142.66, 71.33},
	{110, 0xBE, 36.66, 110.00, 73.33},
//	{110, 0xCE, 36.66, 146.66, 73.33},
	{112, 0x8A, 33.60, 112.00, 67.20},
	{124, 0x9A, 31.00, 124.00, 62.00},
	{133, 0x3A, 33.33, 133.33, 66.66},
//	{133, 0x7A, 33.33, 100.00, 66.66},
//	{133, 0xDA, 33.33, 166.66, 66.66},
	{134, 0x3E, 33.48, 133.90, 68.67},
//	{134, 0x7E, 33.48, 100.90, 66.95},
//	{137, 0x4E, 34.33, 103.00, 66.95},
	{137, 0x5E, 34.33, 137.33, 68.67},
	{138, 0xAA, 34.50, 138.00, 69.00},
	{143, 0xAE, 35.66, 142.66, 71.33},
	{147, 0xDE, 36.66, 146.66, 73.33},
	{150, 0xBA, 30.00, 150.00, 60.00},
//	{150, 0xEA, 30.00, 100.00, 60.00},
//	{160, 0xFA, 30.00, 120.00, 60.00},
	{167, 0x2A, 33.33, 166.66, 66.66},
//	{167, 0xEE, 31.25, 125.00, 66.68},
	{200, 0xFE, 33.33, 200.00, 66.66},
	{0}
};

int ics9248_199_SetFSB(int fsb)
{
	int i, file, res;
	int buf=0;

	if(fsb < 0) return -1;

	file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_read_word_data(file, CONTROLBYTE);
	if(res < 0) return -1;
	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
			buf = pll[i].ctrlb * 0x100 + (res & 0xFF);
	if(!buf) return -1;

	res = i2c_smbus_write_word_data(file, CONTROLBYTE, buf);
	i2c_close();

	if(res < 0) return -1;

#ifdef DEBUG
	printf("DEBUG: %i word written : ", BYTECOUNT);
	for(i=0; i<BYTECOUNT; i++) printf("%04X ", buf);
	printf("\n");
#endif /* DEBUG */

	return 0;
}

int ics9248_199_GetFSB()
{
	int i, file, res;
	unsigned char buf;

	file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_read_word_data(file, CONTROLBYTE);
	i2c_close();

	if(res < 0) return -1;
		else buf = res / 0x100;

#ifdef DEBUG
	printf("DEBUG: %i word read : ", BYTECOUNT);
	for(i=0; i<BYTECOUNT; i++) printf("%04X ", res);
	printf("\n");
#endif /* DEBUG */

	for(i=0; pll[i].fsb; i++)
		if(pll[i].ctrlb == buf) return pll[i].fsb;

	return -1;
}

int ics9248_199_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
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

int ics9248_199_GetFirstFSB()
{
	FSBIndex = 0;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
  else return -1;
}

int ics9248_199_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
	else return -1;
}

