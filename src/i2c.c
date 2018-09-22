/*
*  C Implementation: i2c
*
* Description: i2c
*
*
* Author: Nikolay Kislitsa <deusexbeer@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "pll/i2c-dev.h"

static int File;

int DeviceIndex = -1;

int i2c_open()
{
	int i;
	char devname[256];

	if(DeviceIndex >= 0)
	{
		snprintf(devname, sizeof(devname), "/dev/i2c-%i", DeviceIndex);
		if((File = open(devname, O_RDWR)) > 0)
		{
			if(!ioctl(File, I2C_SLAVE, 0x69))
			{
#ifdef DEBUG
				printf("DEBUG: Using device '%s'\n", devname);
#endif /* DEBUG */
				return File;
			}
			close(File);
		}
	}

	for(i=0; i<32; i++)
	{
		snprintf(devname, sizeof(devname), "/dev/i2c-%i", i);
		if((File = open(devname, O_RDWR)) > 0)
		{
			if(!ioctl(File, I2C_SLAVE, 0x69))
			{
#ifdef DEBUG
				printf("DEBUG: Using device '%s'\n", devname);
#endif /* DEBUG */
				return File;
			}
			close(File);
		}
	}

	printf("\n"
	       "Run as root?\n"
	       "Is module i2c_dev loaded?\n"
	       "Is i2c module for your chipset loaded?\n"
	       "Is i2c support enabled in kernel?\n");

	return -1;
}

int i2c_close()
{
	return close(File);
}
