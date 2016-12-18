#include <stdio.h>
#include <graph.h>
#include <packet.h>
#include <kernel.h>
#include <sifrpc.h>
#include <tamtypes.h>
#include <loadfile.h>
#include <sbv_patches.h>
#include <dma.h>

#include "ps2ip.h"

extern void IOMANX;
extern void fileXio;
extern void USBD;
extern void USBHDFSD;
extern void ps2ips;
extern void ps2ip;
extern void ps2dev9;
extern void poweroff;
extern void freesio2;
extern void freepad;
extern void dns;
extern void PS2SMAP;

extern u32 size_IOMANX;
extern u32 size_fileXio;
extern u32 size_USBD;
extern u32 size_USBHDFSD;
extern u32 size_ps2ips;
extern u32 size_ps2ip;
extern u32 size_ps2dev9;
extern u32 size_poweroff;
extern u32 size_freesio2;
extern u32 size_freepad;
extern u32 size_dns;
extern u32 size_PS2SMAP;

#include "cl-string.c"
#include "cl-gs.c"
#include "qwerty.c"
#include "public.c"
#include "engine.c"
#include "server.c"

int main(void)
{
	ResetIOP();

	//---------------------------------------------------------- Initialize Screen
	InitGraph();
	//init_scr();
	graph_set_clearbuffer(0x00, 0x00, 0x00);
	graph_wait_vsync();
	DrawString("Loading...", 320, 220, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 1, 1);
	graph_wait_vsync();

	//---------------------------------------------------------- Load Modules
	LoadModules();

	//---------------------------------------------------------- Initialize Server

	ServerInit(80);

	graph_set_clearbuffer(0x00, 0x00, 0x00);
	graph_wait_vsync();
	char myURL[100];
	CopyString("http://", myURL, 0);
	CopyString(ps2IP, myURL, strlen(myURL));
	CopyString("/", myURL, strlen(myURL));
	DrawString(myURL, 320, 220, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 1, 1);
	graph_wait_vsync();

	//---------------------------------------------------------- Launch Server
	ServerLoop();

	return 0;
}

int LoadConfig(void)
{
	char data[50];
	int f, i, sz;

	f = fioOpen("mc0:/CheatersLounge/ip.cfg", O_RDONLY);
	if (f < 0) { f = fioOpen("mc1:/CheatersLounge/ip.cfg", O_RDONLY); }
	if (f < 0) { return -1; }

	sz = fioLseek(f, 0, SEEK_END);
	i = fioLseek(f, 0, SEEK_SET);

	if (sz < 47) { fioClose(f); return -2; }

	i = fioRead(f, data, sz);

	if (i < 0) { fioClose(f); return -3; }

	for (i = 0; i < 15; i++)
		ps2IP[i] = data[i];
	for (i = 16; i < 31; i++)
		ps2NM[i-16] = data[i];
	for (i = 32; i < 47; i++)
		ps2GW[i-32] = data[i];

	fioClose(f);

	return 1;
}

int LoadModules(void)
{

	int rv, ret;

	//char* ps2IP = "192.168.0.32";
	//char* ps2NM = "255.255.255.0";
	//char* ps2GW = "192.168.0.1";

	char args[100];
	int a = 0;

	rv = 1;

	SifInitRpc(0);

	rv = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	rv = SifLoadModule("rom0:MCMAN", 0, NULL);
	rv = SifLoadModule("rom0:MCSERV", 0, NULL);

	if (LoadConfig() < 1)
	{
		CopyString("192.168.000.123", ps2IP, 0);
		CopyString("255.255.255.000", ps2NM, 0);
		CopyString("192.168.000.001", ps2GW, 0);
	}

	rv = SifExecModuleBuffer(&USBD, size_USBD, 0, NULL, &ret);
	rv = SifExecModuleBuffer(&USBHDFSD, size_USBHDFSD, 0, NULL, &ret);
	rv = SifExecModuleBuffer(&freesio2, size_freesio2, 0, NULL, &ret);
	rv = SifExecModuleBuffer(&freepad, size_freepad, 0, NULL, &ret);
	rv = SifExecModuleBuffer(&poweroff, size_poweroff, 0, NULL, &ret);
	rv = SifExecModuleBuffer(&IOMANX, size_IOMANX, 0, NULL, &ret);
	rv = SifExecModuleBuffer(&ps2dev9, size_ps2dev9, 0, NULL, &ret);
	rv = SifExecModuleBuffer(&fileXio, size_fileXio, 0, NULL, &ret);
	rv = SifExecModuleBuffer(&ps2ip, size_ps2ip, 0, NULL, &ret);
	rv = SifExecModuleBuffer(&ps2ips, size_ps2ips, 0, NULL, &ret);

	strcpy(&args[a], ps2IP); a += strlen(ps2IP) + 1;
	strcpy(&args[a], ps2NM); a += strlen(ps2NM) + 1;
	strcpy(&args[a], ps2GW); a += strlen(ps2GW) + 1;
	rv = SifExecModuleBuffer(&PS2SMAP, size_PS2SMAP, a, args, &ret);

	ps2ip_init();
}

void ReadCNF(char *elfnm)
{
	FILE *fd;
	char *systemcnf;
	int fsize;
	int OnFirstLine = 1;
	int IsElfName, ElfIndex;

	fd = fopen("cdrom0:\\SYSTEM.CNF;1", "r");
	if (fd < 0)
	{
		fd = fopen("cdrom0:\\system.cnf;1", "r");
	}

	if (fd > 0)
	{
		fseek(fd, 0, SEEK_END);
		fsize = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		systemcnf = memalign(128, fsize);
		fread(systemcnf, fsize, 1, fd);
		fclose(fd);

		//printf("SYSTEM.CNF: Contents ->\n %s ", systemcnf);
		printf("%i Bytes ", fsize);

		int i;

		IsElfName = 0;
		OnFirstLine = 1;

		for (i = 0; i < fsize; i++)
		{
			if (OnFirstLine == 1)
			{
				if (IsElfName == 0)
				{
					if (systemcnf[i] == 'c')
					{
						if (systemcnf[i + 1] == 'd')
						{
							if (systemcnf[i + 2] == 'r')
							{
								if (systemcnf[i + 3] == 'o')
								{
									if (systemcnf[i + 4] == 'm')
									{
										if (systemcnf[i + 5] == '0')
										{
											ElfIndex = 0;
											IsElfName = 1;
										}
									}
								}
							}
						}
					}
				}
				if (IsElfName == 1)
				{
					elfnm[ElfIndex] = systemcnf[i];
					ElfIndex += 1;
					if (systemcnf[i] == ';')
					{
						if (systemcnf[i + 1] == '1')
						{
							elfnm[ElfIndex] = systemcnf[i + 1];
							elfnm[ElfIndex + 1] = 0x00;
							OnFirstLine = 0;
						}
					}
				}
			}
		}

		//elfnme = elfname;

		printf("SYSTEM.CNF: ELF = %s ", elfnm);

	}
	else
	{
		elfnm[0] = '!';
		elfnm[1] = 0x00;
	}
}

void ResetIOP()
{
	SifIopReset("rom0:UDNL rom0:EELOADCNF", 0);
	while ( SifIopSync()) ;
	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();
	EI;
	FlushCache(0);
	FlushCache(2);

	SifIopReset("rom0:UDNL rom0:EELOADCNF", 0);
	while ( SifIopSync()) ;
	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();
	EI;
	FlushCache(0);
	FlushCache(2);

	SifInitRpc(0);
	SifInitCmd();
	SifLoadFileInit();
	fioInit();
	
}
