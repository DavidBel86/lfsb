/*
*  C Implementation: lfsb
*
* Description: FSB speed manipulating programm.
*
*
* Author: Nikolay Kislitsa <deusexbeer@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <inttypes.h>
#include <strings.h>
#include <getopt.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <sched.h>

#define VERSION "0.4.5"
#define DATE    "16-Jul-2013"

#define CPUGOVERNOR
#define SYSLOGREPORT
#define CPUAFFINITY

#include "pll.h"

static const PLL_t *CurPLL = NULL;
extern int DeviceIndex;

static void Usage()
{
	printf("lfsb - linux FSB overclocking tool. Version "VERSION", "DATE"\n"
	       "\n"
	       "   Usage: lsfb [options] PLL [FSBFREQ]\n"
	       "\n"
	       "   -h --help         print this message\n"
	       "   -v --version      display software version & license\n"
	       "   -s --supported    print list of supported PLL\n"
	       "   -f --frequencies  print all supported freqs by PLL\n"
	       "   -y --yes          skip confirmation\n"
	       "   -d --device       force i2c device index\n"
	       "\n"
	       "example: lfsb -y ics9248-110 115\n");
	exit(0);
}

static void Version()
{
	printf("lfsb - linux FSB overclocking tool. Version "VERSION", "DATE"\n"
	       "\n"
	       "Copyright (C) 2013 by Nikolay Kislitsa <deusexbeer@gmail.com>\n"
	       "\n"
	       "This program is free software; you can redistribute it and/or modify\n"
	       "it under the terms set out in the LICENSE file, which is included\n"
	       "in the source distribution.\n"
	       "\n"
	       "This program is distributed in the hope that it will be useful,\n"
	       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	       "LICENSE file for more details.\n");
	exit(0);
}

static inline int64_t rdtsc(void)
{
	unsigned int i, j;
	__asm__ __volatile__("rdtsc" : "=a"(i), "=d"(j) : );
	return ((int64_t)j<<32) + (int64_t)i;
}

#ifdef CPUAFFINITY
static void SetAffinity(const int cpunum)
{
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpunum, &cpuset);
	sched_setaffinity(0, sizeof(cpuset), &cpuset);
}
#endif

static double GetCPUFreq()
{
	int64_t tsc_start, tsc_end;
	struct timespec ts_start, ts_end;
	int nsec_delay;

#ifdef CPUAFFINITY
	SetAffinity(0);
#endif

	tsc_start = rdtsc();
	clock_gettime(CLOCK_MONOTONIC, &ts_start);
	while(1)
	{
		clock_gettime(CLOCK_MONOTONIC, &ts_end);
		if(1000000000 * (ts_end.tv_sec - ts_start.tv_sec) + (ts_end.tv_nsec - ts_start.tv_nsec) > 300000000)
			break;
	}
	tsc_end = rdtsc();

	nsec_delay = 1000000000 * (ts_end.tv_sec - ts_start.tv_sec) + (ts_end.tv_nsec - ts_start.tv_nsec);

	return ((double)(tsc_end-tsc_start)*1000.0 / (double)nsec_delay);
}

static int SetPLL(const char *name)
{
	const PLL_t *pll;

	for(pll=PLL; pll->name[0]; pll++)
		if(!strcasecmp(name, pll->name))
		{
			CurPLL = pll;
			return 0;
		}

	return -1;
}

static const char *GetPLLFlags(const Test_t t)
{
	if(t == Tested)
		return "tested";
	else if(t == Testing)
		return "testing";

	return "untested";
}

static void Supported()
{
	int i;

	printf("Supported PLL:\n");
	for(i=0; PLL[i].name[0]; i++)
	{
		printf("%s (%s)\n", PLL[i].name, GetPLLFlags(PLL[i].flags));
	}
	printf("\n");
}

void PrintCPUInfo(const float cpuf)
{
	printf("CPU frequency : %.2f MHz (estimated)\n", cpuf);
}

void PrintFSBInfo(int fsb)
{
	float ram, pci, agp;

	if(!CurPLL->CheckFSB(fsb, &ram, &pci, &agp))
	{
		printf("FSB=%i MHz", fsb);
		if(ram > 0)
			printf(", RAM=%.2f MHz", ram);
		if(pci > 0)
			printf(", PCI=%.2f MHz", pci);
		if(agp > 0)
			printf(", AGP=%.2f MHz", agp);
		printf("\n");
	}
}

#ifdef CPUGOVERNOR
static char Governor[256];
static void GetGovernor(char *gov, size_t govlen)
{
	FILE *file;
	char *ch;

	file = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "rt");
	if(!file)
		return;
	memset(gov, 0, govlen);
	fread(gov, 1, govlen-1, file);
	fclose(file);
	if((ch = strrchr(gov, '\n')))
		*ch = 0;
}

static void SetGovernor(const char *gov)
{
	char cmd[256];

	if(!gov)
		return;

	snprintf(cmd, sizeof(cmd), "cpufreq-set -r -g %s &> /dev/null", gov);
	if(system(cmd))
		printf("cpufreq-set failed\n");
	else
		printf("CPU governor '%s' set.\n", gov);
}

static void Exit()
{
	printf("\n");
	SetGovernor(Governor);
	usleep(300000);
}
#endif

void PrintSupportedFrequencies()
{
	int fsb, prev, start;

	printf("\nSupported FSB frequencies (MHz):\n");
	fsb = CurPLL->GetFirstFSB();
	start = 0;
	prev = 0;
	while(fsb > 0)
	{
		if(fsb - prev == 1)
		{
			if(!start)
				start = prev;
		}
		else
		{
			if(start)
			{
				printf("-%i", prev);
				start = 0;
			}
			else
				printf(" %i", fsb);
		}
		prev = fsb;
		fsb = CurPLL->GetNextFSB();
	}
	if(start)
		printf("-%i", prev);
	printf("\n");
}

int main(int argc, char *argv[])
{
	int c, fsb, freqs=0, yes=0, fsbf, tme;
	char *pllname=NULL, *fsbfreq=NULL;
	float cpuf;
	struct option long_options[] =
	{
		{"help", 0, 0, 'h'},
		{"version", 0, 0, 'v'},
		{"supported", 0, 0, 's'},
		{"frequencies", 0, 0, 'f'},
		{"yes", 0, 0, 'y'},
		{"device", 1, 0, 'd'},
		{0, 0, 0, 0}
	};

	while((c = getopt_long(argc, argv, "hvsfyd:", long_options, NULL)) != -1)
	{
		switch(c)
		{
			case 'h': Usage(); break;
			case 'v': Version(); break;
			case 's': Supported(); break;
			case 'f': freqs = 1; break;
			case 'y': yes = 1; break;
			case 'd': DeviceIndex = atoi(optarg); break;
			default: Usage(); return 0; break;
		}
	}
	while(optind < argc)
	{
		if(!pllname)
			pllname = argv[optind];
		else
			fsbfreq = argv[optind];
		optind++;
	}

#ifdef CPUGOVERNOR
	GetGovernor(Governor, sizeof(Governor));
	SetGovernor("performance");
	atexit(Exit);
#endif

	printf("\n");
	PrintCPUInfo(GetCPUFreq());

	if(!pllname)
		return 0;

	if(SetPLL(pllname))
	{
		printf("PLL %s is not suppported\n", pllname);
		return 0;
	}
	else
		printf("PLL %s is supported (%s)\n", pllname, GetPLLFlags(CurPLL->flags));
	if(CurPLL->CheckTME)
	{
		tme = CurPLL->CheckTME();
		if(tme == 1)
			printf("\nTrusted Mode Enabled, PLL is TME locked.\n");
		else if(!tme)
			printf("\nTrusted Mode Disabled.\nOverclocking is possible.\n");
	}

	fsbf = CurPLL->GetFSB();
	PrintFSBInfo(fsbf);
	if(freqs)
		PrintSupportedFrequencies();

	if(fsbfreq)
	{
		printf("\n");
		fsb = atoi(fsbfreq);
		if(CurPLL->CheckFSB(fsb, NULL, NULL, NULL))
		{
			printf("FSB %i MHz is not supported.\n", fsb);
			return 0;
		}
		printf("Changing to:\n");
		PrintFSBInfo(fsb);
#ifdef SYSLOGREPORT
		syslog(LOG_INFO, "Changing FSB frequency from %i to %i MHz", fsbf, fsb);
		sync();
#endif

		if(!yes)
		{
			printf("WARNING: Too high FSB/PCI/AGP speed can DAMAGE your hardware!\nContinue [y/N]: ");
			yes = getc(stdin);
			if(yes != 'y' && yes != 'Y')
			{
				printf("Aborted.\n");
				return 0;
			}
		}

		c = CurPLL->SetFSB(fsb);
		cpuf = GetCPUFreq();

		if(c)
			perror("Can't change frequency, SetFSB error");
		else
		{
			printf("FSB frequency changed.\n");
#ifdef SYSLOGREPORT
			syslog(LOG_INFO, "Success: FSB %i MHz, CPU %.2f MHz", fsb, cpuf);
#endif
		}

		printf("\n");
		PrintCPUInfo(cpuf);
		PrintFSBInfo(fsb);
	}

	return 0;
}
