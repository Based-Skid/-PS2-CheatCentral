#include <kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <fileio.h>
#include <debug.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include <string.h>
#include <tamtypes.h>
#include <graph.h>
#include <packet.h>
#include <dma.h>
#include <sbv_patches.h>
#include <libmc.h>

#include "ps2ip.h"
#include "libpad.h"


#include "cl-gs.c"
#include "engine.c"
#include "qwerty.c"
#include "pad.c"


extern u32 logo[];
extern u32 background[];

extern u32 progressbar[];
extern u32 pb_fill_left[];
extern u32 pb_fill_middle[];
extern u32 pb_fill_right[];

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


struct padButtonStatus buttons;
u32 paddata;
u32 old_pad = 0;
u32 new_pad;

#include "elf_loader.c"
#include "server.c"

int BarMax = 479;
int Progress = 0;

int BarTop = 270;
int BarLeft = 37;

void pause(int seconds)
{
	int x, y;

	for (x = 0; x < seconds; x++)
	{
		for (y = 0; y < 0x04400000; y++)
		{
			__asm("nop\nnop");
		}
	}
}

void ShowBar()
{
	DrawImage(progressbar, BarLeft, BarTop, 0, 567, 88, -1);
	DrawImage(pb_fill_left, (BarLeft + 28), (BarTop + 30), 0, 26, 27, -1);
}

void FillBar(int newProgress)
{
	int i;

	for (i = 0; i < newProgress; i++)
	{
		DrawImage(pb_fill_middle, ((BarLeft + 38) + i), (BarTop + 30), 0, 12, 27, -1);
	}
	DrawImage(pb_fill_right, ((BarLeft + 48) + i), (BarTop + 30), 0, 13, 27, -1);

	Progress = newProgress;
}

int main(void)
{
	int r, i;

	ResetIOP();

	InitGraph();
	graph_set_clearbuffer(0x00, 0x00, 0x00);
	graph_wait_vsync();

	//for (i = 0; i < 255; i++)
	//{
	//	DrawImage(logo, 63, 196, 0, 514, 49, (255-i));
	//	graph_wait_vsync();
	//}
	//pause(3);

	DrawImage(logo, 63, 196, 0, 514, 49, -1);
	ShowBar();

	//pause(1);
	FillBar(50);
	//DrawString("Loading...", 0, 0, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 0, 1);
	loadmodules();

	FillBar(479);
	//pause(1);


	//for (i = 255; i > 0; i--)
	//{
	//	DrawImage(logo, 63, 196, 0, 514, 49, (255-i));
	//	graph_wait_vsync();
	//}
	//pause(1);

	DrawImage(background, 0, 0, 0, 640, 480, -1);
	graph_wait_vsync();

	//DrawString("Starting Remote PS2...", 0, 10, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 0, 1);
	EnterServer();

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

void loadmodules(void)
{
	int rv, ret;

	//char* IP = "192.168.0.32";
	//char* NM = "255.255.255.0";
	//char* GW = "192.168.0.1";

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

	FillBar(130);
	//rv = SifExecModuleBuffer(&USBD, size_USBD, 0, NULL, &ret);

	FillBar(160);
	//rv = SifExecModuleBuffer(&USBHDFSD, size_USBHDFSD, 0, NULL, &ret);

	FillBar(190);
	//rv = SifExecModuleBuffer(&freesio2, size_freesio2, 0, NULL, &ret);

	FillBar(220);
	//rv = SifExecModuleBuffer(&freepad, size_freepad, 0, NULL, &ret);

	FillBar(250);
	rv = SifExecModuleBuffer(&poweroff, size_poweroff, 0, NULL, &ret);

	FillBar(280);
	rv = SifExecModuleBuffer(&IOMANX, size_IOMANX, 0, NULL, &ret);

	FillBar(310);
	rv = SifExecModuleBuffer(&ps2dev9, size_ps2dev9, 0, NULL, &ret);

	FillBar(330);
	//rv = SifExecModuleBuffer(&fileXio, size_fileXio, 0, NULL, &ret);

	FillBar(360);
	rv = SifExecModuleBuffer(&ps2ip, size_ps2ip, 0, NULL, &ret);

	FillBar(390);
	rv = SifExecModuleBuffer(&ps2ips, size_ps2ips, 0, NULL, &ret);

	FillBar(420);
	strcpy(&args[a], ps2IP); a += strlen(ps2IP) + 1;
	strcpy(&args[a], ps2NM); a += strlen(ps2NM) + 1;
	strcpy(&args[a], ps2GW); a += strlen(ps2GW) + 1;
	rv = SifExecModuleBuffer(&PS2SMAP, size_PS2SMAP, a, args, &ret);

	//ps2ip_getconfig(char* netif_name,t_ip_info* ip_info);

	FillBar(450);
	ps2ip_init();

	rv = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	rv = SifLoadModule("rom0:MCMAN", 0, NULL);
	rv = SifLoadModule("rom0:MCSERV", 0, NULL);
	//rv = SifLoadModule("rom0:XSIO2MAN", 0, NULL);
	//rv = SifLoadModule("rom0:XMCMAN", 0, NULL);
	//rv = SifLoadModule("rom0:XMCSERV", 0, NULL);

	mkdir("mc0:/CheatersLounge/Lists");
	mkdir("mc0:/CheatersLounge/Apps");
	mkdir("mc1:/CheatersLounge/Lists");
	mkdir("mc1:/CheatersLounge/Apps");

	mcInit(0);
} 

void loadbasemodules(void)
{
	int rv, ret;
	rv = 1;
	SifInitRpc(0);
	rv = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	rv = SifLoadModule("rom0:MCMAN", 0, NULL);
	rv = SifLoadModule("rom0:MCSERV", 0, NULL);
	rv = SifLoadModule("rom0:PADMAN", 0, NULL);
	rv = SifLoadModule("rom0:LIBSD", 0, NULL);
	//mcInit(MC_TYPE_MC);
	loadmodulemem(iopmod, 1865, 0, 0); // Initialize qwerty device
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

void ResetCNF()
{
	fioOpen("cdrom0:\\SYSTEM.CNF;1",1);
	SifIopReset("rom0:UDNL cdrom0:\\SYSTEM.CNF;1", 0);

	while ( SifIopSync()) ;

	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();
	EI;

	SifInitRpc(0);

	FlushCache(0);
	FlushCache(2);
}


void Reverse(u8 *src)
{
	int i, n;
	u8 *tmp;

	tmp = malloc(strlen(src));

	for (i = 0; i < strlen(src); i++)
	{
		tmp[i] = src[i];
	}

	n = strlen(src) - 1;
	for (i = 0; i < strlen(src); i += 2)
	{
		src[i] = tmp[n - 1];
		src[i + 1] = tmp[n];

		n -= 2;
	}

	free(tmp);
}

void StrToHex(u8 *src, u8 *dst, int len)
{
	int i, n;
	u8 b1, b2;

	n = 0;
	for (i = 0; i < len; i++)
	{
		b1 = src[i];
		b2 = src[i];

		b1 = b1 / 0x10;
		b1 = b1 * 0x10;
		b2 = b2 - b1;
		b1 = b1 / 0x10;

		dst[n] = CharToHex(b1); n++;
		dst[n] = CharToHex(b2); n++;
	}
}

u8 CharToHex(u8 chr)
{
	if (chr == 0x00) { return 0x30; }
	if (chr == 0x01) { return 0x31; }
	if (chr == 0x02) { return 0x32; }
	if (chr == 0x03) { return 0x33; }
	if (chr == 0x04) { return 0x34; }
	if (chr == 0x05) { return 0x35; }
	if (chr == 0x06) { return 0x36; }
	if (chr == 0x07) { return 0x37; }
	if (chr == 0x08) { return 0x38; }
	if (chr == 0x09) { return 0x39; }
	if (chr == 0x0A) { return 0x41; }
	if (chr == 0x0B) { return 0x42; }
	if (chr == 0x0C) { return 0x43; }
	if (chr == 0x0D) { return 0x44; }
	if (chr == 0x0E) { return 0x45; }
	if (chr == 0x0F) { return 0x46; }
}

void CopyString(char *src, char *dst, int index)
{
	int i;
	for (i = 0; i < strlen(src); i++)
	{
		dst[index + i] = src[i];
	}
	dst[index + i] = 0x00;
}

int IsEqual(char *strA, char *strB)
{
	int i;

	if (strlen(strA) != strlen(strB))
	{
		return 0;
	}

	for (i = 0; i < strlen(strA); i++)
	{
		if (strA[i] != strB[i])
		{
			return 0;
		}
	}

	return 1;
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
		//printf("%i Bytes ", fsize);

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

		//printf("SYSTEM.CNF: ELF = %s ", elfnm);

	}
	else
	{
		elfnm[0] = '!';
		elfnm[1] = 0x00;
	}
}
