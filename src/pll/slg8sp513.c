
/*
 *  C Implementation: slg8sp513
 *
 * Description: slg8sp513
 *
 *
 * Author: Nikolay Kislitsa <deusexbeer@gmail.com>, (C) 2013
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
	unsigned int fsb;
	unsigned char ctrlb;
	float pci;
} PLL_t;
#define BYTECOUNT   25
#define CONTROLBYTE 14
#define CMD 0x00

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	{100, 0x95, 33.3},
	{103, 0xB5, 34.3},
	{105, 0xD5, 35.0},
	{108, 0xF5, 36.0},
	{133, 0x85, 33.3},
	{137, 0xA5, 34.3},
	{140, 0xC5, 35.0},
	{144, 0xE5, 36.0},
	{166, 0x8D, 33.3},
	{171, 0xAD, 34.3},
	{175, 0xCD, 35.0},
	{179, 0xED, 36.0},
	{200, 0x89, 33.3},
	{206, 0xA9, 34.3},
	{210, 0xC9, 35.0},
	{216, 0xE9, 36.0},
	{266, 0x81, 33.3},
	{274, 0xA1, 34.3},
	{279, 0xC1, 35.0},
	{287, 0xE1, 36.0},
	{300, 0x9D, 33.3},
	{309, 0xBD, 34.3},
	{315, 0xDD, 35.0},
	{324, 0xFD, 36.0},
	{333, 0x91, 33.3},
	{343, 0xB1, 34.3},
	{350, 0xD1, 35.0},
	{360, 0xF1, 36.0},
	{400, 0x99, 33.3},
	{412, 0xB9, 34.3},
	{420, 0xD9, 35.0},
	{432, 0xF9, 36.0},
	{0}
};

static int slg8sp513_unhide(const int file)
{
	int res;
	unsigned char buf[BYTECOUNT];

	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res < 0)
		return -1;
#ifdef DEBUG
	else
		printf("unhide DEBUG: %i bytes read : ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	if(res != 25)
	{
		buf[12] = 0x19;
		res = i2c_smbus_write_block_data(file, CMD, 13, buf);
		if(res)
			return -1;
#ifdef DEBUG
		printf("unhide DEBUG: %i bytes written : ", 13);
		for(int i=0; i<BYTECOUNT; i++)
			printf("%02X ", buf[i]);
		printf("\n");
#endif /* DEBUG */
	}
	else
		return 0;

	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res < 0)
		return -1;
#ifdef DEBUG
	printf("unhide DEBUG: %i bytes read : ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	if(res == 0x19 && buf[12] == 0x19)
		return 0;

	return -1;
}

int slg8sp513_SetFSB(int fsb)
{
	int i, file, res;
	unsigned char buf[BYTECOUNT], ctrl=0;

	if(fsb < 0)
		return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
		{
			ctrl = pll[i].ctrlb;
			break;
		}
	if(!ctrl)
		return -1;

	file = i2c_open();
	if(file < 0)
		return -1;

	slg8sp513_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
#ifdef DEBUG
	printf("SetFSB DEBUG: %i bytes read : ", res);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res != BYTECOUNT)
	{
		i2c_close();
		return -1;
	}

	buf[CONTROLBYTE] = ctrl;
	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	i2c_close();
	if(res)
		return -1;

#ifdef DEBUG
	printf("SetFSB DEBUG: %i bytes written : ", BYTECOUNT);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	return 0;
}

int slg8sp513_GetFSB()
{
	int i, file, res;
	unsigned char buf[BYTECOUNT];

	file = i2c_open();
	if(file < 0)
		return -1;

	slg8sp513_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

#ifdef DEBUG
	printf("GetFSB DEBUG: %i bytes read : ", res);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res < 0)
		return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].ctrlb == (buf[CONTROLBYTE] | 0x81))
			return pll[i].fsb;

	return -1;
}

int slg8sp513_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
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
		{
			if(pci)
				*pci = pll[i].pci;
			return 0;
		}

	return -1;
}

int slg8sp513_GetFirstFSB()
{
	FSBIndex = 0;
	if(pll[FSBIndex].fsb)
		return pll[FSBIndex].fsb;
	else
		return -1;
}

int slg8sp513_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb)
		return pll[FSBIndex].fsb;
	else
		return -1;
}
