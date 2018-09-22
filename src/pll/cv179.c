
/*
 *  C Implementation: cv179
 *
 * Description: cv179
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

#define BYTECOUNT   25
#define CONTROLBYTE 17
#define CMD 0x00
#define MAXCTRL 0xff
#define MAXFSB  169
#define MINCTRL 0x78
#define MINFSB  80

static int FSBIndex = 0;

unsigned int cv179_ictrl2fsb(unsigned int ictrl)
{
    return (unsigned int)((MAXFSB-MINFSB)*(ictrl-MINCTRL)/(MAXCTRL-MINCTRL)+MINFSB);
}

unsigned int cv179_fsb2ictrl(unsigned int fsb)
{
    return (unsigned int)((fsb-MINFSB)*(MAXCTRL-MINCTRL)/(MAXFSB-MINFSB)+MINCTRL);
}

static int cv179_unhide(const int file)
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
		buf[12] = 0x19;
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

int cv179_SetFSB(int fsb)
{
#ifdef DEBUG
int i;
#endif
	int file, res;
	unsigned char buf[BYTECOUNT], ctrl=0, hctrl=0;
	unsigned int ictrl;

	if(fsb < 0)
		return -1;
	ictrl = cv179_fsb2ictrl(fsb);
	if((ictrl > MAXCTRL) || (ictrl < MINCTRL)){ 
#ifdef DEBUG
	printf("SetFSB DEBUG: error ictrl=%u", ictrl);
#endif /* DEBUG */
		return -1;
	}
	hctrl = (ictrl>255?1:0);
	ctrl = (unsigned char)(ictrl%255);
#ifdef DEBUG
	printf("SetFSB DEBUG: ictrl=%u, hctrl=%u, ctrl=%u\n", ictrl, hctrl, ctrl);
#endif /* DEBUG */
	file = i2c_open();
	if(file < 0)
		return -1;

	cv179_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
#ifdef DEBUG
	printf("SetFSB DEBUG: %i bytes read :    ", res);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if((res > 25) || (res<20))
	{
#ifdef DEBUG
		printf("SetFSB DEBUG: read error. res=%i\n", res);
#endif /* DEBUG */
		i2c_close();
		return -1;
	}

	buf[CONTROLBYTE-1] |= hctrl; // seems not working with cv179 (actualy there is no datasheet for cv179) must work with cv183
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

int cv179_GetFSB()
{
#ifdef DEBUG
	int i;
#endif
	int file, res;
	unsigned char buf[BYTECOUNT];
	unsigned int ictrl, fsb;

	file = i2c_open();
	if(file < 0)
		return -1;

	cv179_unhide(file);
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

	ictrl = (buf[CONTROLBYTE-1] & 1)*256 + buf[CONTROLBYTE];
	fsb = cv179_ictrl2fsb(ictrl);
#ifdef DEBUG
	printf("GetFSB DEBUG: ictrl=%u, fsb=%u\n", ictrl, fsb);
#endif /* DEBUG */

	return fsb;
}

int cv179_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
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

int cv179_GetFirstFSB()
{
	FSBIndex = 0;
	return MINFSB;
}

int cv179_GetNextFSB()
{
	FSBIndex++;
	if((MINFSB+FSBIndex)<=MAXFSB)
		return (int)(MINFSB+FSBIndex);
	else
		return -1;
}
