#include <stdio.h>
#include <stdlib.h>
#include <tamtypes.h>
#include <math.h>
#include <kernel.h>
#include <debug.h>
#include <audsrv.h>
#include <loadfile.h>

#include "libpad.h"


extern u32 background[];
extern u32 title_browser[];
extern u32 title_ipconfig[];
extern u32 title_memdump[];
extern u32 menu_text[];
extern u32 ipconfig_menu[];
extern u32 memdump_menu[];
extern u32 arrow[];
extern u32 n_arrow[];
extern u32 spark[];
extern u32 num_0[];
extern u32 num_1[];
extern u32 num_2[];
extern u32 num_3[];
extern u32 num_4[];
extern u32 num_5[];
extern u32 num_6[];
extern u32 num_7[];
extern u32 num_8[];
extern u32 num_9[];
extern u32 num_dot[];
extern u32 logo[];

extern u32 pcm_intro[];
extern u32 pcm_background_music[];
extern u32 pcm_Hclick[];
extern u32 pcm_Lclick[];
extern u32 pcm_Spluck[];
extern u32 pcm_Fpluck[];

extern u32 remoteps2[];
extern u32 httpserver[];
extern u32 cheatse13r3[];

extern void mod_usbd;
extern void mod_usb_mass;
extern void cdvdinit_irx;
extern void audsrv;

extern u32 size_mod_usbd;
extern u32 size_mod_usb_mass;
extern u32 size_cdvdinit_irx;
extern u32 size_audsrv;

struct padButtonStatus buttons;
u32 paddata;
u32 old_pad = 0;
u32 new_pad;

/*

MenuText = (152, 114)
Arrow = (117, 121)
Title = (71, 69)

*/

audsrv_adpcm_t audio_bgm;
audsrv_adpcm_t audio_sfx;

/*
audsrv_adpcm_t sound_click_high;
audsrv_adpcm_t sound_click_low;
audsrv_adpcm_t sound_pluck_high;
audsrv_adpcm_t sound_pluck_low;
*/

#include "cl-gs.c"
#include "pad.c"
#include "ipconfig.c"
#include "memdump.c"
#include "cl-elf.c"

u32 bgm_counter = 0;
u32 bgm_time = 0;
u32 bgm_length = 55;

struct audsrv_fmt_t bgm_format;

void RestartBGM(void)
{
	bgm_counter++;

	if (bgm_counter >= 0x0001C000)
	{
		bgm_time++;
		bgm_counter = 0;
	}
	if (bgm_time >= bgm_length)
	{
		printf("Restarting BGM... ");
		bgm_time = 0;
		bgm_counter = 0;
		PlayADPCM(pcm_background_music);
	}
}

int main(void)
{
	int i, n, c, f, x, y, select;

	LoadModules();

	InitGraph();
	graph_set_clearbuffer(0x00, 0x00, 0x00);
	graph_wait_vsync();


	//PlayADPCM(pcm_intro);

	for (i = 0; i < 255; i += 5)
	{
		DrawImage(logo, 63, 196, 0, 514, 49, (255-i));
		graph_wait_vsync();
	}
	pause(3);
	for (i = 255; i > 0; i -= 5)
	{
		DrawImage(logo, 63, 196, 0, 514, 49, (255-i));
		graph_wait_vsync();
	}
	pause(1);


	DrawImage(background, 0, 0, 0, 640, 480, 240);
	DrawImage(background, 0, 0, 0, 640, 480, 127);
	DrawImage(background, 0, 0, 0, 640, 480, 60);
	DrawImage(background, 0, 0, 0, 640, 480, -1);
	graph_wait_vsync();
	//pause(40);

	DrawImage(background, 0, 0, 0, 640, 480, -1);
	AnimateTitle(title_browser, 216);
	DrawImage(menu_text, 152, 114, 0, 244, 209, -1);

	padInit(0);
	padPortOpen(0, 0, padBuf);
	initializePad(0, 0);

	DrawImage(arrow, 117, 122, 0, 49, 29, -1);
	graph_wait_vsync();

	x = 0;
	y = 0;
	i = 0;

	select = 0;

	audsrv_adpcm_init();
	audsrv_set_volume(MAX_VOLUME);
	PlayADPCM(pcm_background_music);
	while (1)
	{
		if (padRead(0, 0, &buttons));
		{
			paddata = 0xffff ^ buttons.btns;
			new_pad = paddata & ~old_pad;
			old_pad = paddata;

			if (new_pad & PAD_UP)
			{
				select = AnimateSelect(select, (select - 1));
			}
			if (new_pad & PAD_DOWN)
			{
				select = AnimateSelect(select, (select + 1));
			}
			if (new_pad & PAD_SQUARE)
			{
				printf("%08X; %08X ", bgm_counter, bgm_time);
			}
			if (new_pad & PAD_CROSS)
			{
				AnimateClick(select);
			}
		}
		RestartBGM();
	}

	return 0;
}

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

int PlayWAV(char *wavPath, int bits, int frequency, int channel, int volume)
{
	char buffer[8192];
	int ret, played, err, f, sz;
	struct audsrv_fmt_t format;

	format.bits = bits; //16;
	format.freq = frequency; //44100;
	format.channels = channel; //2;
	err = audsrv_set_format(&format);
	audsrv_set_volume(volume); //MAX_VOLUME);

	f = fioOpen(wavPath, O_RDONLY);

	if (f > 0)
	{
		fioLseek(f, 0, SEEK_SET);

		while (1)
		{
			ret = fioRead(f, buffer, 8192);

			if (ret > 0)
			{
				audsrv_wait_audio(ret);
				audsrv_play_audio(buffer, ret);
			}
			else
			{
				break;
			}
		}
		fioClose(f);
	}
}

int AnimateClick(int select)
{
	int x, y, z, i, n, b, c, d;

	x = 117;
	y = (27 * select) + 122;
	z = 0;

	for (i = 0; i < 10; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x -= 2;
		DrawImage(arrow, x, y, z, 49, 29, -1);
		graph_wait_vsync();
	}

	for (i = 0; i < 5; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x += 4;
		DrawImage(arrow, x, y, z, 49, 29, -1);
		graph_wait_vsync();
	}

	for (i = 0; i < 5; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x -= 2;
		DrawImage(arrow, x, y, z, 49, 29, -1);
		graph_wait_vsync();
	}
	DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
	DrawPhotoSpot(background, 152, 114, 244, 209, 152, 114, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);

	for (i = 0; i < 17; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x += 2;
		DrawImage(arrow, x, y, z, 49, 29, (15*i));
		graph_wait_vsync();
	}

	OpenMenu(select);

	DrawImage(background, 0, 0, 0, 640, 480, -1);

	x = 117;
	y = (27 * select) + 122;
	z = 0;

	AnimateTitle(title_browser, 216);
	DrawImage(menu_text, 152, 114, 0, 244, 209, -1);
	DrawImage(arrow, x, y, z, 49, 29, -1);
	graph_wait_vsync();

}

int OpenMenu(int select)
{
	int f;

	if (select == 0) // CheatsE. v1.3
	{
		ExecData(cheatse13r3, "CL-CheatsEv1_3r03.ELF");
	}
	else if (select == 1) // Device Manager
	{
		
	}
	else if (select == 2) // Memory / BIOS Dump
	{
		EnterMemDump();
	}
	else if (select == 3) // Remote PS2
	{
		ExecData(remoteps2, "CL-Remote_PS2.ELF");
	}
	else if (select == 4) // HTTP Server
	{
		ExecData(httpserver, "CL-HTTP_Server.ELF");
	}
	else if (select == 5) // Configure PS2 IP
	{
		EnterIPConfig();
	}
	else if (select == 6) // Exit
	{
		LoadExecPS2("", 0, NULL);
	}

	return 0;
}

int AnimateSelect(int oldSelect, int newSelect)
{
	int x, y, z, i, n;
	int max = 6;

	if ( (newSelect >= 0) && (newSelect <= max) )
	{
		PlayADPCM(pcm_Hclick);

		x = 117;
		y = (27 * oldSelect) + 122;
		z = 0;
		n = 7;

		//printf("%i, %i ", x, y);
		if (newSelect > oldSelect)
		{
			for (i = 0; i < 6; i++) //(i = 0; i < 27; i++)
			{
				DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
				y += n;
				DrawImage(arrow, x, y, z, 49, 29, -1);
				graph_wait_vsync();
				n--;
			}
		}
		else
		{
			for (i = 0; i < 6; i++) //(i = 0; i < 27; i++)
			{
				DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
				y -= n;
				DrawImage(arrow, x, y, z, 49, 29, -1);
				graph_wait_vsync();
				n--;
			}
		}
		//printf("%i, %i ", x, y);
		
	}
	else if (newSelect < 0)
	{
		return 0;
	}
	else if (newSelect > max)
	{
		return max;
	}

	return newSelect;
}

int AnimateTitle(u32 titleAddr, int w)
{
	int x, y, n, f, c, i;

	x = 281;
	n = 20;
	f = 0;
	y = 1;
	DrawPhotoSpot(background, 71, 69, 569, 30, 71, 69, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
	for (i = 0; i < 20; i++)
	{
		for (c = 0; c < 0x00100000; c++) { __asm("nop\nnop"); }
		DrawPhotoSpot(background, x, 69, w, 30, x, 69, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x -= n;
		n--;
		f += y;
		y += 3;
		if (f >= 255) { f = 256; }
		//DrawPhotoSpot(titleAddr, 0, 0, w, 30, x, 69, 0, w, 30, 0x00, 0x00, 0x00, (255-f), 0, 1, 0);
		DrawImage(titleAddr, x, 69, 0, w, 30, (255-f));
		graph_wait_vsync();
	}

	return 0;
}

int PlayADPCM(u32 pcmData)
{
	int i;

	if (pcmData == pcm_intro)
	{
		audsrv_load_adpcm(&audio_bgm, pcm_intro, 814160);
		audsrv_play_adpcm(&audio_bgm);
		return 0;
	}
	if (pcmData == pcm_background_music)
	{
		audsrv_load_adpcm(&audio_bgm, pcm_background_music, 672048);
		audsrv_play_adpcm(&audio_bgm);
		return 0;
	}
	if (pcmData == pcm_Hclick)
	{
		i = audsrv_load_adpcm(&audio_sfx, pcm_Hclick, 80);
	}
	if (pcmData == pcm_Lclick)
	{
		i = audsrv_load_adpcm(&audio_sfx, pcm_Lclick, 96);
	}
	if (pcmData == pcm_Spluck)
	{
		i = audsrv_load_adpcm(&audio_sfx, pcm_Spluck, 3168);
	}
	if (pcmData == pcm_Fpluck)
	{
		i = audsrv_load_adpcm(&audio_sfx, pcm_Fpluck, 2080);
	}

	audsrv_play_adpcm(&audio_sfx);

}

int LoadModules()
{
	int err;
	int ret = 1;

	SifLoadFileInit();
	SifInitRpc(0);

	ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	ret = SifLoadModule("rom0:MCMAN", 0, NULL);
	ret = SifLoadModule("rom0:MCSERV", 0, NULL);
	ret = SifLoadModule("rom0:PADMAN", 0, NULL);
	ret = SifLoadModule("rom0:LIBSD", 0, NULL);

	SifExecModuleBuffer(&audsrv, size_audsrv, 0, NULL, &ret);
	SifExecModuleBuffer(&cdvdinit_irx, size_cdvdinit_irx, 0, NULL, &ret);
	SifExecModuleBuffer(&mod_usbd, size_mod_usbd, 0, NULL, &ret);
	SifExecModuleBuffer(&mod_usb_mass, size_mod_usb_mass, 0, NULL, &ret);

	mkdir("mc0:/CheatersLounge");
	mkdir("mc0:/CheatersLounge/Lists");
	mkdir("mc1:/CheatersLounge");
	mkdir("mc1:/CheatersLounge/Lists");

	ret = audsrv_init();

	bgm_format.bits = 16;
	bgm_format.freq = 8000;
	bgm_format.channels = 2;
	err = audsrv_set_format(&bgm_format);

	audsrv_adpcm_init();
	audsrv_set_volume(MAX_VOLUME);

	return ret;
}

void ClearWindow(int x, int y, int z, int w, int h, int trans, int tol)
{
//	DrawPhotoSpotMergeBG(window, background, x, y, w, h, (x+10), (y+5), z, window_w, window_h, 
//				(x+10), (y+5), w, h, 640, 480);

	DrawPhotoSpot(background, x, y, w, h, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, trans, tol);
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

