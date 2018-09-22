
/*
 *  C Implementation: cv184
 *
 * Description: cv184
 *
 *
 * Author: Andrey Zhornyak <darion76@gmail.com>, (C) 2014
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "i2c.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define BYTECOUNT   20
#define CONTROLBYTE 17
#define CMD 0x00
#define NPROGEN 0x08
#define MAXFSB  400
#define MINFSB  100

static int FSBIndex = 0;

static int cv184_unhide(const int file)
{
	int res;
	unsigned char buf[BYTECOUNT];

	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res < 0)
		return -1;
#ifdef DEBUG
	else
		printf("unhide DEBUG: %i bytes read :    ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	if(res != BYTECOUNT)
	{
		buf[12] = BYTECOUNT;
		res = i2c_smbus_write_block_data(file, CMD, 13, buf);
		if(res)
			return -1;
#ifdef DEBUG
		printf("unhide DEBUG: %i bytes written :    ", 13);
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
	printf("unhide DEBUG: %i bytes read :    ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	if(res == 0x19 && buf[12] == 0x19)
		return 0;

	return -1;
}

int cv184_SetFSB(int fsb)
{
#ifdef DEBUG
int i;
#endif
	int file, res;
	unsigned char buf[BYTECOUNT], ctrl=0, hctrl=0;

	if(fsb < 0)
		return -1;
	
	hctrl = (fsb>255?1:0);
	hctrl |= NPROGEN; // Enable N-Programming
	ctrl = (unsigned char)(fsb&0xff);
#ifdef DEBUG
	printf("SetFSB DEBUG: hctrl=%u, ctrl=%u\n", hctrl, ctrl);
#endif /* DEBUG */
	file = i2c_open();
	if(file < 0)
		return -1;

	cv184_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
#ifdef DEBUG
	printf("SetFSB DEBUG: %i bytes read :    ", res);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res != BYTECOUNT)
	{
#ifdef DEBUG
		printf("SetFSB DEBUG: read error. res=%i\n", res);
#endif /* DEBUG */
		i2c_close();
		return -1;
	}
	
	buf[CONTROLBYTE-1] |= hctrl;
	buf[CONTROLBYTE] = ctrl;
	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	i2c_close();
	if(res){
#ifdef DEBUG
	printf("SetFSB DEBUG: wrire error. res=%i\n", res);
#endif /* DEBUG */
		return -1;
	}

#ifdef DEBUG
	printf("SetFSB DEBUG: %i bytes written : ", BYTECOUNT);
	for(i=0; i<BYTECOUNT; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	return 0;
}

int cv184_GetFSB()
{
#ifdef DEBUG
	int i;
#endif
	int file, res;
	unsigned char buf[BYTECOUNT];
	unsigned int fsb;

	file = i2c_open();
	if(file < 0)
		return -1;

	cv184_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

#ifdef DEBUG
	printf("GetFSB DEBUG: %i bytes read :    ", res);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res < 0)
		return -1;

	fsb = (buf[CONTROLBYTE-1] & 1)*256 + buf[CONTROLBYTE];
#ifdef DEBUG
	printf("GetFSB DEBUG: fsb=%u\n", fsb);
#endif /* DEBUG */

	return fsb;
}

int cv184_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{

	if(sdram)
		*sdram = -1.0;
	if(pci)
		*pci = -1.0;
	if(agp)
		*agp = -1.0;

	if((fsb < MINFSB)||(fsb>MAXFSB))
		return -1;
	return 0;
}

int cv184_GetFirstFSB()
{
	FSBIndex = 0;
	return MINFSB;
}

int cv184_GetNextFSB()
{
	FSBIndex++;
	if((MINFSB+FSBIndex)<=MAXFSB)
		return (int)(MINFSB+FSBIndex);
	else
		return -1;
}
