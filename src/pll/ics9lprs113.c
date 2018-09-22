/*
 *  C Implementation: ics9lprs113
 *
 * Description: ics9lprs113
 *
 *
 * Author: Nikolay Kislitsa <deusexbeer@gmail.com>, (C) 2013
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "i2c.h"
#include <math.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define BYTECOUNT 31
#define CMD 0x00

static int FSBIndex = 0;
static const unsigned int FSB_Min = 100;
static const unsigned int FSB_Max = 400;

static const int const DivTable[] =
{
	0x02, 0x03, 0x05, 0x01,
	0x04, 0x06, 0x0A, 0x01,
	0x08, 0x0C, 0x14, 0x01,
	0x10, 0x18, 0x28, 0x01
};

static int ics9lprs113_CalcFSB(const unsigned int n, const unsigned int m, const unsigned int dividx)
{
	return 48.0f * (float)n / (float)m / (float)DivTable[dividx & 0x0F];
}

static int ics9lprs113_unhide(const int file)
{
	int res;
	unsigned char buf[128];

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

	if(res != BYTECOUNT)
	{
		buf[8] = BYTECOUNT;
		res = i2c_smbus_write_block_data(file, CMD, 8, buf);
		if(res)
			return -1;
#ifdef DEBUG
		printf("unhide DEBUG: %i bytes written : ", 8);
		for(int i=0; i<8; i++)
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

	if(res == BYTECOUNT && BYTECOUNT)
		return 0;

	return -1;
}

int ics9lprs113_CheckFSB(int fsb, float *ram, float *pci, float *agp)
{
	if(ram)
		*ram = -1.0f;
	if(pci)
		*pci = -1.0f;
	if(agp)
		*agp = -1.0f;

	if(fsb <= FSB_Max && fsb >= FSB_Min)
		return 0;

	return -1;
}

int ics9lprs113_SetFSB(int fsb)
{
	int file, res;
	unsigned int n, m;
	unsigned char buf[128];

	file = i2c_open();
	if(file < 0)
		return -1;

	ics9lprs113_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res != BYTECOUNT)
	{
#ifdef DEBUG
		printf("SetFSB DEBUG: %i (should be %i) bytes read : ", res, BYTECOUNT);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
#endif /* DEBUG */
		i2c_close();
		return -1;
	}
#ifdef DEBUG
	else
	{
		printf("SetFSB DEBUG: %i bytes read : ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	n = fsb * 3;
	m = 0x24;

	buf[0x04] |= 0x80;
	buf[0x0C] = (n >> 2);
	buf[0x0B] = (m & 0x3F) | ((n & 0x03) << 6);
	buf[0x1D] = (buf[0x1D] & 0xF0) | 0x04;

	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	i2c_close();

 if(res < 0)
	 return -1;
#ifdef DEBUG
  else
		printf("SetFSB DEBUG: %i bytes written : ", BYTECOUNT);
  for(int i=0; i<BYTECOUNT; i++)
		printf("%02X ", buf[i]);
  printf("\n");
#endif /* DEBUG */

	return 0;
}

int ics9lprs113_GetFSB()
{
	int file, res;
	unsigned int n, m;
	unsigned char buf[128];

	file = i2c_open();
	if(file < 0)
		return -1;

	ics9lprs113_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

	if(res < 0) return -1;
#ifdef DEBUG
	else
	{
		printf("GetFSB DEBUG: %i bytes read : ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	n = (buf[0x0B] >> 6) | (buf[0x0C] << 2);
	m = buf[0x0B] & 0x3F;

	return ics9lprs113_CalcFSB(n, m, buf[0x1D]);
}

int ics9lprs113_GetFirstFSB()
{
	FSBIndex = FSB_Min;

	return FSBIndex;
}

int ics9lprs113_GetNextFSB()
{
	FSBIndex++;

	if(FSBIndex <= FSB_Max)
		return FSBIndex;

	return -1;
}

