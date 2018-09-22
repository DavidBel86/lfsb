/*
*  C Implementation: cy28331
*
* Description: cy28331
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

//#define WATCHDOG

#define BYTECOUNT   12
#define CMD 0x00

static int FSBIndex = 0;
static const unsigned int FSB_Min = 100;
static const unsigned int FSB_Max = 300;

int cy28331_SetFSB(int fsb)
{
	int file, res;
	unsigned char fs, N, M;
	unsigned int n, m;
	float p, mdn, min;
	unsigned char buf[32];

	if(fsb > FSB_Max || fsb < FSB_Min) return -1;

#ifdef DEBUG
#define write_block \
	do \
	{ \
		res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf); \
		if(res < 0) \
 	 	{ \
  	  i2c_close(); \
    	return -1; \
  	} \
  	else \
		{ \
			printf("DEBUG: %i bytes written : ", BYTECOUNT); \
  		for(int i=0; i<BYTECOUNT; i++) printf("%02X ", buf[i]); \
  		printf("\n"); \
		} \
	} while(0)
#else /* DEBUG */
#define write_block \
	do \
	{ \
		res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf); \
		if(res < 0) \
 	 	{ \
  	  i2c_close(); \
    	return -1; \
  	} \
	} while(0)
#endif /* DEBUG */

	file = i2c_open();
	if(file < 0) return -1;

	//Read registers data to calculate P
	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res != BYTECOUNT)
	{
#ifdef DEBUG
		printf("DEBUG: %i (should be %i) bytes read : ", res, BYTECOUNT);
  	for(int i=0; i<res; i++) printf("%02X ", buf[i]);
  	printf("\n");
#endif /* DEBUG */
		i2c_close();
		return -1;
	}
#ifdef DEBUG
  else
	{
		printf("DEBUG: %i bytes read : ", res);
  	for(int i=0; i<res; i++) printf("%02X ", buf[i]);
  	printf("\n");
	}
#endif /* DEBUG */

	//Write 1 to byte0:0, 0 to byte0:7 allowing writing to registers
	buf[0] |= 0x01;
	buf[0] &= 0x7F;
	write_block;

#ifdef WATCHDOG
	//Set WD(0:3) Bits = 0
	buf[6] &= 0xF0;
	write_block;

	//Set Frequency Revert Bit
	//Set WD(0:3) = 3 Sec
	buf[6] |= 0x23;
	write_block;
#endif /* WATCHDOG */

	//calculcate P
	fs = buf[0];
	fs >>= 1;
	fs &= 0x0F;
	if(fs == 1 || fs == 5) p = 127.994667;
	else if(fs == 4 || fs == 8 || fs == 9) p = 95.966;
	else p = 191.992;

	//calculate M/N
	mdn = p/(float)fsb;

	//find nearest N,M to M/N
	N = 0;
	M = 0;
	min = 1.0;
	for(n=21; n<126; n++)
	{
		m=20;
		if(m <= n/2)	m = n/2+1;
		while(m<n && m<59)
		{
			if(fabs((float)m/(float)n - mdn) < min)
			{
				min = fabs((float)m/(float)n - mdn);
				N = n;
				M = m;
			}
			m++;
		}
	}

	if(N<21 || N>125 || M<20 || M>58 || N<=M || M<=N/2)
	{
		i2c_close();
		return -1;
	}

	//Set N,M
	buf[9] = N;
	buf[10] = M;
	buf[10] <<= 1;
	buf[10] |= 1;

	//Write N,M
	write_block;
	i2c_close();

#ifdef WATCHDOG
	//Set WD(0:3) Bits = 0
	buf[6] &= 0xF0;
	write_block;
#endif /* WATCHDOG */

	return 0;
}

int cy28331_GetFSB()
{
	int file, res;
  unsigned char fs;
  unsigned int n, m;
	float p;
	unsigned char buf[32];

	file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

	if(res < 0) return -1;
#ifdef DEBUG
	else
	{
		printf("DEBUG: %i bytes read : ", res);
		for(int i=0; i<res; i++) printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	fs = buf[0];
  fs >>= 1;
	fs &= 0x0F;
  if(fs == 1 || fs == 5) p = 127.994667;
  else if(fs == 4 || fs == 8 || fs == 9) p = 959.66;
  else p = 191.992;

	n = buf[9];
	n &= 0x7F;

	m = buf[10];
	m >>= 1;
	m &= 0x3F;

	return (int)(p*(float)n/(float)m);
}

int cy28331_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{
	if(sdram) *sdram = -1.0;
	if(pci) *pci = -1.0;
	if(agp) *agp = -1.0;
	if(fsb <= FSB_Max && fsb >= FSB_Min) return 0;
	return -1;
}

int cy28331_GetFirstFSB()
{
	FSBIndex = FSB_Min;
  return FSBIndex;
}

int cy28331_GetNextFSB()
{
	FSBIndex++;
	if(FSBIndex <= FSB_Max) return FSBIndex;
	else return -1;
}
