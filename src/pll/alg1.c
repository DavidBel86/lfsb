/*
 *  C Implementation: algorithm 1
 *
 * Description: algorithm for the most PLLs
 *
 *
 * Author: Andrey Zhornyak <darion76@gmail.com>, (C) 2014
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "alg1.h"

#include <stdio.h>


static int FSBIndex = 0;


unsigned int calc_N(const alg1_t *alg, unsigned int fsb, const unsigned char *buf)
{
    return fsb * (get_bb(alg->M_map, buf)-alg->M_add) * (alg->dt_enable?alg->DivTable[get_bb(alg->dt_map, buf)]:1) / alg->mn_ratio - alg->N_add;
}

int alg1_unhide( const alg1_t *alg, const int file)
{
	int res;
	// Prevent buffer overflow when wrong chip. 
	//***TODO: need to change i2c_smbus_read_block_data to protect buffer when reading 
	// (add buffer size argument)***
	unsigned char buf[2*alg->bc];

	res = i2c_smbus_read_block_data(file, alg->cmd, buf);
	if(res < 0){
#ifdef DEBUG
		printf("unhide DEBUG: read error. code=%i\n", res);
#endif /* DEBUG */
		return -1;
	}
#ifdef DEBUG
	else
		printf("unhide DEBUG: %i bytes read :    ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
#endif /* DEBUG */

	if(res != alg->bc)
	{
		//buf[BC_BYTE] = BYTECOUNT | BC_MASK;
		set_bb(alg->bc_map, buf, alg->bc); // Set byte count
		res = i2c_smbus_write_block_data(file, alg->cmd, alg->bc_hide, buf);
		if(res){
#ifdef DEBUG
			printf("unhide DEBUG: write block error. code=%i \n ", res);
			printf("unhide DEBUG: buffer content: ");
			for(int i=0; i<alg->bc_hide; i++)
				printf("%02X ", buf[i]);
			printf("\n");
#endif /* DEBUG */
			return -1;
		}
#ifdef DEBUG
			printf("unhide DEBUG: %i bytes written : ", alg->bc_hide);
			for(int i=0; i<alg->bc_hide; i++)
				printf("%02X ", buf[i]);
			printf("\n");
#endif /* DEBUG */
	}
	else
		return 0;

	res = i2c_smbus_read_block_data(file, alg->cmd, buf);
	if(res < 0)
		return -1;
#ifdef DEBUG
	printf("unhide DEBUG: %i bytes read :    ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res == (alg->bc+1) && get_bb(alg->bc_map, buf) == alg->bc)
	return 0;

	return -1;
}


int alg1_CheckFSB(const alg1_t *alg, int fsb, float *ram, float *pci, float *agp)
{
	if(ram)
		*ram = -1.0f;
	if(pci)
		*pci = -1.0f;
	if(agp)
		*agp = -1.0f;

	if(fsb <= alg->FSB_Max && fsb >= alg->FSB_Min)
		return 0;

	return -1;
}


int alg1_CalcFSB(const alg1_t *alg, const unsigned int n, const unsigned int m, const unsigned char * buf)
{
	return alg->mn_ratio * (float)(n+alg->N_add)/ (float)(m+alg->M_add) / (float)(alg->dt_enable?alg->DivTable[get_bb(alg->dt_map, buf)]:1);
}

int alg1_SetFSB(const alg1_t *alg, int fsb, int test)
{
	int file, res;
	unsigned int n, m;
	unsigned char buf[2*alg->bc];

	if(fsb > alg->FSB_Max || fsb < alg->FSB_Min)
		return -1;

	file = i2c_open();
	if(file < 0)
		return -1;

	res = i2c_smbus_read_block_data(file, alg->cmd, buf);
	if((res != alg->bc) || test == 1)
	{
#ifdef DEBUG
		printf("SetFSB DEBUG: %i (should be %i) bytes read : ", res, alg->bc);
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
		printf("SetFSB DEBUG: %i bytes read :    ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	n = get_bb(alg->N_map, buf);
	m = get_bb(alg->M_map, buf);
	if(alg1_CheckFSB(alg, alg1_CalcFSB(alg, n, m, buf), NULL, NULL, NULL))
	{
#ifdef DEBUG
		printf("SetFSB DEBUG: Read FSB out of range.\nStopping.\n");
		printf("SetFSB DEBUG: n=%i, m=%i, dt=%i \n", n, m, alg->dt_enable?get_bb(alg->dt_map, buf):1);
#endif /* DEBUG */
		i2c_close();
		return -1;
	}

	n = calc_N(alg, fsb, buf);
	set_bb(alg->N_map, buf, n);
	set_bb(alg->TME_map, buf, alg->TME_unlock);
#ifdef DEBUG
		printf("SetFSB DEBUG: n=%i, m=%i, dt=%i \n", n, m, alg->dt_enable?get_bb(alg->dt_map, buf):1);
#endif /* DEBUG */

	if (test == 0) 
	    res = i2c_smbus_write_block_data(file, alg->cmd, alg->bc, buf);
	else printf("SetFSB simulated\n");
	i2c_close();

 if(res < 0)
	 return -1;
#ifdef DEBUG
  else
		printf("SetFSB DEBUG: %i bytes written : ", alg->bc);
  for(int i=0; i<alg->bc; i++)
		printf("%02X ", buf[i]);
  printf("\n");
#endif /* DEBUG */

	return 0;
}


int alg1_GetFSB(const alg1_t *alg)
{
	int file, res;
	unsigned int n, m;
	unsigned char buf[alg->bc*2];

	file = i2c_open();
	if(file < 0)
		return -1;
	alg1_unhide(alg, file);
	res = i2c_smbus_read_block_data(file, alg->cmd, buf);
	i2c_close();

	if(res < 0) return -1;
#ifdef DEBUG
	else
	{
		printf("GetFSB DEBUG: %i bytes read :    ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	n = get_bb(alg->N_map, buf);
	m = get_bb(alg->M_map, buf);

#ifdef DEBUG
	printf("GetFSB DEBUG: n=%i, m=%i, dt=%i \n", n, m, alg->dt_enable?get_bb(alg->dt_map, buf):1);
#endif /* DEBUG */


	return alg1_CalcFSB(alg, n, m, buf);
}

int alg1_GetFirstFSB(const alg1_t *alg)
{
	FSBIndex = alg->FSB_Min;

	return FSBIndex;
}

int alg1_GetNextFSB(const alg1_t *alg)
{
	FSBIndex++;

	if(FSBIndex <= alg->FSB_Max)
		return FSBIndex;

	return -1;
}

static int alg1_SetTME(const alg1_t *alg)
{
	int file, res;
	unsigned char buf[2*alg->bc];

	file = i2c_open();
	if(file < 0)
		return -1;

	res = i2c_smbus_read_block_data(file, alg->cmd, buf);
	if(res != alg->bc)
	{
#ifdef DEBUG
		printf("SetTME DEBUG: %i (should be %i) bytes read : ", res, alg->bc);
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
		printf("SetTME DEBUG: %i bytes read :    ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	set_bb(alg->TME_map, buf, alg->TME_unlock);
	res = i2c_smbus_write_block_data(file, alg->cmd, alg->bc, buf);
	i2c_close();

 if(res < 0){
#ifdef DEBUG
	printf("SetTME DEBUG: write error code=%i \n", res);
#endif /* DEBUG */

	 return -1;
 }
#ifdef DEBUG
  else
		printf("SetTME DEBUG: %i bytes written : ", alg->bc);
  for(int i=0; i<alg->bc; i++)
		printf("%02X ", buf[i]);
  printf("\n");
#endif /* DEBUG */

	return 0;
}


int alg1_CheckTME(const alg1_t *alg)
{
	int file, res;
	unsigned char buf[alg->bc];

	file = i2c_open();
	if(file < 0)
		return -1;

	alg1_unhide(alg, file);
	res = i2c_smbus_read_block_data(file, alg->cmd, buf);
	i2c_close();

#ifdef DEBUG
	printf("CheckTME DEBUG: %i bytes read :  ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res < 0)
		return -1;
	if(get_bb(alg->TME_map, buf)) 
	    alg1_SetTME(alg);

	if(get_bb(alg->TME_map, buf))
		return 1;

	return 0;
}


