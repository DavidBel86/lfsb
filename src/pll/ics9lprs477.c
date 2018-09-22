/*
 *  C Implementation: ics9lprs477
 *
 * Description: ics9lprs477
 *
 *
 * Author: Nikolay Kislitsa <deusexbeer@mail.ru>, (C) 2005
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "i2c.h"
#include <math.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define BYTECOUNT 32
#define CMD 0x00

static int FSBIndex = 0;
static const unsigned int FSB_Min = 200;
static const unsigned int FSB_Max = 500;

int ics9lprs477_CheckFSB(int fsb, float *ram, float *pci, float *agp)
{
	if(ram)
		*ram = fsb;
	if(pci)
		*pci = -1.0f;
	if(agp)
		*agp = -1.0f;

	if(fsb <= FSB_Max && fsb >= FSB_Min)
		return 0;

	return -1;
}

static const int const DivTable[] =
{
	0x02, 0x03, 0x05, 0x09,
	0x04, 0x06, 0x0A, 0x12,
	0x08, 0x0C, 0x14, 0x24,
	0x10, 0x18, 0x28, 0x48
};

static int ics9lprs477_CalcFSB(const unsigned int n, const unsigned int m, const unsigned int dividx)
{
	return 50.0f * (float)n / (float)m / (float)DivTable[dividx & 0x0F];
}

int ics9lprs477_SetFSB(int fsb)
{
	int file, res;
	unsigned int n, m;
	unsigned char buf[32];

	if(fsb > FSB_Max || fsb < FSB_Min)
		return -1;

	file = i2c_open();
	if(file < 0)
		return -1;

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

	n = (buf[17] << 3) | ((buf[16] & 0xC0) >> 5);
	m = buf[16] & 0x3F;
	if(ics9lprs477_CheckFSB(ics9lprs477_CalcFSB(n, m, buf[20]), NULL, NULL, NULL))
	{
#ifdef DEBUG
		printf("SetFSB DEBUG: Read FSB out of range.\nStopping.\n");
#endif /* DEBUG */
		i2c_close();
		return -1;
	}

	n = fsb << 1;
	m = 50;

	buf[16] = m & 0x3F;
	buf[16] |= (n & 0x06) << 5;
	buf[17] = (n >> 3) & 0xFF;
	buf[20] = 0x00;

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

int ics9lprs477_GetFSB()
{
	int file, res;
	unsigned int n, m;
	unsigned char buf[32];

	file = i2c_open();
	if(file < 0)
		return -1;
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

	n = (buf[17] << 3) | ((buf[16] & 0xC0) >> 5);
	m = buf[16] & 0x3F;

	return ics9lprs477_CalcFSB(n, m, buf[20]);
}

int ics9lprs477_GetFirstFSB()
{
	FSBIndex = FSB_Min;

	return FSBIndex;
}

int ics9lprs477_GetNextFSB()
{
	FSBIndex++;

	if(FSBIndex <= FSB_Max)
		return FSBIndex;

	return -1;
}
