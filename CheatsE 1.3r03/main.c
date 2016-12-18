#include <stdio.h>
#include "libpad.h"

#include "cl-gs.c"
#include "cl-engine.c"
#include "pad.c"

extern u32 logo[];
extern u32 background[];
extern u32 listmanwin[];
//extern u32 window[];
extern u32 button1[];
extern u32 button2[];

//u32 window_w = 620;
//u32 window_h = 440;

extern u32 title_listman[];
extern u32 title_codeman[];
extern u32 title_deviceman[];
extern u32 title_mainmenu[];

extern u32 sub_codeeditor[];
extern u32 sub_deviceman[];
extern u32 sub_editor[];
extern u32 sub_main[];
extern u32 sub_selectdrive[];
extern u32 sub_selector[];
extern u32 sub_startgame[];

extern void mod_usbd;
extern void mod_usb_mass;
extern void cdvdinit_irx;

extern u32 size_mod_usbd;
extern u32 size_mod_usb_mass;
extern u32 size_cdvdinit_irx;

typedef struct {

	char filepath[40];
	char filename[40];
	char caption[40];
	char action[40];
	int nmenu;

} mbutton;

typedef struct {

	char codename[40];
	u32 address[20];
	u32 data[20];
	u32 lines;

} mcode;

typedef struct {

	char listname[40];
	char listpath[40];
	int codecount;

} mlistinfo;

int mymenu = 0;
int menucount = 0;
int selected = 0;
char mylistname[40];
char mylistpath[40];
char bigtitle[40];
char subtitle[40];
u32 flushedcodelist;

struct padButtonStatus buttons;
u32 paddata;
u32 old_pad = 0;
u32 new_pad;

int main(void)
{
	int i, x, y, n;
	mbutton MenuButton[100];
	int Hold = 0;

	LoadModules();

	InitGraph();
	graph_set_clearbuffer(0x00, 0x00, 0x00);
	graph_wait_vsync();

	for (i = 255; i > 0; i--)
	{
		DrawImage(logo, 63, 215, 0, 514, 49, i);
		graph_wait_vsync();
	}
	for (i = 0; i < 50; i++)
		graph_wait_vsync();
	for (i = 0; i < 255; i++)
	{
		DrawImage(logo, 63, 215, 0, 514, 49, i);
		graph_wait_vsync();
	}

	for (n = 255; n > 0; n -= 3)
	{
		x = 0;
		y = 0;
		for (i = 0; i < 480; i += 20)
		{
			DrawPhotoSpot(background, x, y, 640, 20, x, y, 0, 640, 20, 0, 0, 0, n, 0);
			y += 20;
		}
	}

	padInit(0);
	padPortOpen(0, 0, padBuf);
	initializePad(0, 0);

	graph_wait_vsync();

	CopyString("Main Menu", bigtitle, 0);
	CopyString("Main", subtitle, 0);
	WriteTitle();

	selected = 0;
	SetMenu(0, MenuButton);

	while (1)
	{
		if (padRead(0, 0, &buttons));
		{
			paddata = 0xffff ^ buttons.btns;
			new_pad = paddata & ~old_pad;
			old_pad = paddata;

			if (new_pad & PAD_UP)
			{
				MenuBrowse(0, MenuButton);
			}
			if (new_pad & PAD_DOWN)
			{
				MenuBrowse(1, MenuButton);
			}
			if (new_pad & PAD_CROSS)
			{
				DoAction(MenuButton);
			}
		}
		graph_wait_vsync();
	}

	return 0;
}

int LoadModules()
{
	int ret = 1;

	SifLoadFileInit();
	SifInitRpc(0);

	ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	ret = SifLoadModule("rom0:MCMAN", 0, NULL);
	ret = SifLoadModule("rom0:MCSERV", 0, NULL);
	ret = SifLoadModule("rom0:PADMAN", 0, NULL);

	//SifExecModuleBuffer(&cdvdinit_irx, size_cdvdinit_irx, 0, NULL, &ret);
	SifExecModuleBuffer(&mod_usbd, size_mod_usbd, 0, NULL, &ret);
	SifExecModuleBuffer(&mod_usb_mass, size_mod_usb_mass, 0, NULL, &ret);
	//LoadModuleMem(iopmod, size_iopmod, 0, 0);

	return ret;
}

void ClearWindow(int x, int y, int z, int w, int h)
{
//	DrawPhotoSpotMergeBG(window, background, x, y, w, h, (x+10), (y+5), z, window_w, window_h, 
//				(x+10), (y+5), w, h, 640, 480);

	DrawPhotoSpot(background, x, y, w, h, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0);
}

void DrawFrame(int x, int y, int z, int w, int h, int bthick, int r, int g, int b, int br, int bg, int bb)
{
	DrawBox(x, y, z, w, h, br, bg, bb);
	DrawBox((x + bthick), (y + bthick), z, (w - (bthick * 2)), (h - (bthick * 2)), r, g, b);
}

int ListSelector(int drv)
{
	int x, y, i, sel, mn, ti, row, col, dls, chose, n;
	int listc, frmR, frmG, frmB, FR, FG, FB;
	char tmpPath[40];

	mlistinfo lists[100];

	Reload:

	listc = ReadIndex(drv, lists);

/*
	CopyString("List - 1 ", lists[0].listname, 0);
	CopyString("List - 2 ", lists[1].listname, 0);
	CopyString("List - 3 ", lists[2].listname, 0);
	CopyString("List - 4 ", lists[3].listname, 0);
	CopyString("List - 5 ", lists[4].listname, 0);
	CopyString("List - 6 ", lists[5].listname, 0);
	CopyString("List - 7 ", lists[6].listname, 0);
	CopyString("List - 8 ", lists[7].listname, 0);
	CopyString("List - 9 ", lists[8].listname, 0);
	CopyString("List - 10", lists[9].listname, 0);
	CopyString("List - 11", lists[10].listname, 0);
	CopyString("List - 12", lists[11].listname, 0);
	CopyString("List - 13", lists[12].listname, 0);
	CopyString("List - 14", lists[13].listname, 0);
	CopyString("List - 15", lists[14].listname, 0);
	CopyString("List - 16", lists[15].listname, 0);
	CopyString("List - 17", lists[16].listname, 0);
	CopyString("List - 18", lists[17].listname, 0);
	CopyString("List - 19", lists[18].listname, 0);
	listc = 19;
*/

	ClearWindow(20, 40, 0, 580, 360);
	//ClearWindow(230, 90, 0, 180, 240);

	CopyString("List Manager", bigtitle, 0);
	CopyString("Selector", subtitle, 0);
	WriteTitle();

	DrawImage(listmanwin, 40, 90, 0, 560, 300, -1);
	graph_wait_vsync();

	FR = 0xDC;
	FG = 0xDC;
	FB = 0xDC;

	x = 190;
	y = 85;
	DrawString("List Options",   230, 65, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 1, 1);
	DrawString("Add",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Edit",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Remove",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("De-Select",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Use List",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Exit",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	graph_wait_vsync();

	chose = -1;
	sel = 0;
	mn = 0;
	ti = 0;
	col = 0;
	dls = 1;
	row = 0;

	frmR = 0x87;
	frmG = 0xCE;
	frmB = 0xEB;

	while (1)
	{
		if (padRead(0, 0, &buttons));
		{
			paddata = 0xffff ^ buttons.btns;
			new_pad = paddata & ~old_pad;
			old_pad = paddata;


			if (new_pad & PAD_UP)
			{
				if (mn == 0)
				{
					if ((sel-ti) > 0)
					{
						i = sel - ti;
						i = i * 16;

						DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
						sel--;
						y = sel - ti;
						y = y * 8;
						DrawString(">>", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						y = chose - ti;
						if ((y > -1) && (y < 16))
						{
							y = y * 8;
							DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
						}
					}
					else
					{
						if (ti > 0)
						{
							ti--;
							dls = 1;
						}
					}
				}
				else if (mn == 1)
				{
					if (row > 0) { row--; dls = 1; }
				}
			}
			if (new_pad & PAD_DOWN)
			{
				if (mn == 0)
				{
					if (((sel - ti) < 15) && ((sel - ti) < (listc - 1)))
					{
						i = sel - ti;
						i = i * 16;
						DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
						sel++;
						y = sel - ti;
						y = y * 8;
						DrawString(">>", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						y = chose - ti;
						if ((y > -1) && (y < 16))
						{
							y = y * 8;
							DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
						}
						//if (sel == chose)
						//	DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
						//if ((sel - 1) == chose)
						//	DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

					}
					else
					{
						if ((sel + 1) < listc)
						{
							ti++;
							dls = 1;
						}
					}
				}
				else if (mn == 1)
				{
					if (row < 5) { row++; dls = 1; }
				}
			}
			if (new_pad & PAD_LEFT)
			{
				if (mn == 1)
				{
					mn = 0;
					dls = 1;
				}
			}
			if (new_pad & PAD_RIGHT)
			{
				if (mn == 0)
				{
					mn = 1;
					dls = 1;
				}
			}
			if (new_pad & PAD_CIRCLE)
			{
				if (mn == 1)
				{
					mn = 0;
				}
				chose = -1;
				dls = 1;
			}
			if (new_pad & PAD_CROSS)
			{
				if (mn == 0)
				{
					chose = sel;
					DrawPhotoSpotMergeBG(listmanwin, background, 270, 0, 100, 30, 310, 90, 0, 560, 300, 350, 90, 100, 30, 640, 480);
					DrawString(lists[sel].listname, 162, 48, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 0, 1);
					mn = 1;
					dls = 1;
				}
				else if (mn == 1)
				{
					if (row == 0) // Add
					{
						
					}
					if (row == 1) // Edit
					{
						if (chose > -1)
						{
							CopyString(mylistpath, tmpPath, 0);
							CopyString(lists[chose].listpath, mylistpath, 0);
							i = ListEditor(lists[chose].listname);
							CopyString(tmpPath, mylistpath, 0);
							mn = 0;
							if (i == 1)
							{
								CopyString(lists[chose].listpath, mylistpath, 0);
								ClearWindow(0, 0, 0, 640, 400);
								return 0;
							}
							goto Reload;
						}
					}
					if ((row == 2) && (chose > -1)) // Delete
					{
						
					}
					if (row == 3) // De-Select
					{
						DrawPhotoSpotMergeBG(listmanwin, background, 270, 0, 100, 30, 310, 90, 0, 560, 300, 350, 90, 100, 30, 640, 480);
						CopyString("none", mylistpath, 0);
						chose = -1;
						dls = 1;
					}
					if (row == 4) // Use
					{
						CopyString(lists[sel].listpath, mylistpath, 0);
						ClearWindow(20, 40, 0, 580, 360);
						return 0;
					}
					if (row == 5) // Exit
					{
						ClearWindow(20, 40, 0, 580, 360);
						return 0;
					}
				}
			}
		}


		if (dls == 1)
		{
			if (mn == 0)
			{
				DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 280, 300, 40, 90, 0, 560, 300, 40, 90, 280, 300, 640, 480);
				y = 50;
				for (i = 0; i < 16; i++)
				{
					if ( (ti + i) < listc)
					{
						if ((sel - ti) == i)
							DrawString(">>", 25, ((i*8) + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						if ((ti + i) == chose)
							DrawString(" >", 25, ((i*8) + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						DrawString(lists[ti + i].listname, 35, y, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 0, 1);
						y += 8;
					}
				}
				DrawPhotoSpotMergeBG(listmanwin, background, 310, 40, 30, 110, 350, 155, 0, 560, 300, 350, 90, 30, 110, 640, 480);
			}
			if (mn == 1)
			{
				DrawPhotoSpotMergeBG(listmanwin, background, 310, 40, 30, 110, 350, 155, 0, 560, 300, 350, 90, 30, 110, 640, 480);
				DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
				DrawString(">>", 180, ((row*8) + 85), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

				y = chose - ti;
				if ((y > -1) && (y < 16))
				{
					y = y * 8;
					DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
				}

			}

			dls = 0;
		}

	}
}

int ListEditor(char *lstname)
{
	int x, y, i, sel, mn, ti, row, col, dls, chose;
	int codec, frmR, frmG, frmB, FR, FG, FB;

	mcode Codes[100];

	codec = ReadList(Codes);

	reload:

/*
	CopyString("Code Test - 1 ", Codes[0].codename, 0);
	CopyString("Code Test - 2 ", Codes[1].codename, 0);
	CopyString("Code Test - 3 ", Codes[2].codename, 0);
	CopyString("Code Test - 4 ", Codes[3].codename, 0);
	CopyString("Code Test - 5 ", Codes[4].codename, 0);
	CopyString("Code Test - 6 ", Codes[5].codename, 0);
	CopyString("Code Test - 7 ", Codes[6].codename, 0);
	CopyString("Code Test - 8 ", Codes[7].codename, 0);
	CopyString("Code Test - 9 ", Codes[8].codename, 0);
	CopyString("Code Test - 10", Codes[9].codename, 0);
	CopyString("Code Test - 11", Codes[10].codename, 0);
	CopyString("Code Test - 12", Codes[11].codename, 0);
	CopyString("Code Test - 13", Codes[12].codename, 0);
	CopyString("Code Test - 14", Codes[13].codename, 0);
	CopyString("Code Test - 15", Codes[14].codename, 0);
	CopyString("Code Test - 16", Codes[15].codename, 0);
	CopyString("Code Test - 17", Codes[16].codename, 0);
	CopyString("Code Test - 18", Codes[17].codename, 0);
	CopyString("Code Test - 19", Codes[18].codename, 0);
	codec = 19;
*/

	ClearWindow(20, 40, 0, 580, 360);
	//ClearWindow(230, 90, 0, 180, 240);

	CopyString("List Manager", bigtitle, 0);
	CopyString("Editor", subtitle, 0);
	WriteTitle();

	DrawImage(listmanwin, 40, 90, 0, 560, 300, -1);
	graph_wait_vsync();

	FR = 0xDC;
	FG = 0xDC;
	FB = 0xDC;

	x = 190;
	y = 85;
	DrawString("Code Options",   230, 65, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 1, 1);
	DrawString("Add",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Edit",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Remove",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Save Changes",	x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Use List (Saves)",	x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Cancel",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	graph_wait_vsync();

	if (codec < 0) { return -1; }

	chose = -1;
	sel = 0;
	mn = 0;
	ti = 0;
	col = 0;
	dls = 1;
	row = 0;

	frmR = 0x87;
	frmG = 0xCE;
	frmB = 0xEB;

	DrawPhotoSpotMergeBG(listmanwin, background, 270, 0, 100, 30, 310, 90, 0, 560, 300, 350, 90, 100, 30, 640, 480);
	DrawString(lstname, 162, 48, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 0, 1);
	graph_wait_vsync();


	while (1)
	{
		if (padRead(0, 0, &buttons));
		{
			paddata = 0xffff ^ buttons.btns;
			new_pad = paddata & ~old_pad;
			old_pad = paddata;


			if (new_pad & PAD_UP)
			{
				if (mn == 0)
				{
					if ((sel-ti) > 0)
					{
						i = sel - ti;
						i = i * 16;

						DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
						sel--;
						y = sel - ti;
						y = y * 8;
						DrawString(">>", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						y = chose - ti;
						if ((y > -1) && (y < 16))
						{
							y = y * 8;
							DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
						}
					}
					else
					{
						if (ti > 0)
						{
							ti--;
							dls = 1;
						}
					}
				}
				else if (mn == 1)
				{
					if (row > 0) { row--; dls = 1; }
				}
			}
			if (new_pad & PAD_DOWN)
			{
				if (mn == 0)
				{
					if (((sel - ti) < 15) && ((sel - ti) < (codec - 1)))
					{
						i = sel - ti;
						i = i * 16;
						DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
						sel++;
						y = sel - ti;
						y = y * 8;
						DrawString(">>", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						y = chose - ti;
						if ((y > -1) && (y < 16))
						{
							y = y * 8;
							DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
						}
					}
					else
					{
						if ((sel + 1) < codec)
						{
							ti++;
							dls = 1;
						}
					}
				}
				else if (mn == 1)
				{
					if (row < 5) { row++; dls = 1; }
				}
			}
			if (new_pad & PAD_LEFT)
			{
				if (mn == 1)
				{
					mn = 0;
					dls = 1;
				}
			}
			if (new_pad & PAD_RIGHT)
			{
				if (mn == 0)
				{
					mn = 1;
					dls = 1;
				}
			}
			if (new_pad & PAD_CIRCLE)
			{
				if (mn == 1)
				{
					mn = 0;
				}
				chose = -1;
				dls = 1;
			}
			if (new_pad & PAD_CROSS)
			{
				if (mn == 0)
				{
					chose = sel;
					mn = 1;
					dls = 1;
				}
				else if (mn == 1)
				{
					if (row == 0) // Add
					{
						
					}
					if ((row == 1) && (chose > -1)) // Edit
					{
						EditCode(&Codes[sel], &Codes[sel].address, &Codes[sel].data);
						mn = 0;
						goto reload;
					}
					if ((row == 2) && (chose > -1)) // Remove
					{
						
					}
					if (row == 3) // Save Changes
					{
						SaveList(Codes, mylistpath, codec);
						return 0;
					}
					if (row == 4) // Use List
					{
						return 1;
					}
					if (row == 5) // Go Back
					{
						ClearWindow(20, 40, 0, 580, 360);
						return 0;
					}
				}
			}
		}

		if (dls == 1)
		{
			if (mn == 0)
			{
				DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 280, 300, 40, 90, 0, 560, 300, 40, 90, 280, 300, 640, 480);
				y = 50;
				for (i = 0; i < 16; i++)
				{
					if ( (ti + i) < codec)
					{
						if ((sel - ti) == i)
							DrawString(">>", 25, ((i*8) + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						if ((ti + i) == chose)
							DrawString(" >", 25, ((i*8) + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						DrawString(Codes[ti + i].codename, 35, y, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 0, 1);
						y += 8;
					}
				}
				DrawPhotoSpotMergeBG(listmanwin, background, 310, 40, 30, 110, 350, 155, 0, 560, 300, 350, 90, 30, 110, 640, 480);
			}
			if (mn == 1)
			{
				DrawPhotoSpotMergeBG(listmanwin, background, 310, 40, 30, 110, 350, 155, 0, 560, 300, 350, 90, 30, 110, 640, 480);
				DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
				DrawString(">>", 180, ((row*8) + 85), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

				y = chose - ti;
				if ((y > -1) && (y < 16))
				{
					y = y * 8;
					DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
				}

			}

			dls = 0;
		}
	}
}


int EditCode(mcode Code, u32 AddressMark, u32 DataMark)
{
	int x, y, i, sel, mn, ti, row, col, dls, chose, ind;
	int codec, line, spacex, frmR, frmG, frmB, FR, FG, FB;
	mcode TempCode;
	char str[18];

	u32 tmpAddr, tmpData;

	codec = 0;
	for (i = 0; i < 20; i++)
	{
		if (Code.address[i] != 0) { codec++; }
		if (Code.address[i] == 0) { Code.data[i] = 0; }
	}

	if (codec < 1) { return -1; }


	ClearWindow(20, 40, 0, 580, 360);
	//ClearWindow(230, 90, 0, 180, 240);

	CopyString("Code Manager", bigtitle, 0);
	CopyString("Editor", subtitle, 0);
	WriteTitle();

	DrawImage(listmanwin, 40, 90, 0, 560, 300, -1);
	graph_wait_vsync();

	FR = 0xDC;
	FG = 0xDC;
	FB = 0xDC;

	x = 190;
	y = 85;
	DrawString("Code Options",   230, 65, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 1, 1);
	DrawString("Add Line",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Edit Line",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Remove Line",	x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Save Changes",	x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	DrawString("Cancel",		x, y, 0, MicrosoftSansSerif, 1, FR, FG, FB, 600, 0x00, 0x00, 0x00, 0, 1); y += 8;
	graph_wait_vsync();

	if (codec < 0) { return -1; }

	chose = -1;
	sel = 0;
	mn = 0;
	ti = 0;
	col = 0;
	dls = 1;
	row = 0;
	ind = 0;
	line = 0;

	frmR = 0x87;
	frmG = 0xCE;
	frmB = 0xEB;

	DrawPhotoSpotMergeBG(listmanwin, background, 270, 0, 100, 30, 310, 90, 0, 560, 300, 350, 90, 100, 30, 640, 480);
	DrawString(Code.codename, 162, 48, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 0, 1);
	graph_wait_vsync();

	for (i = 0; i < 20; i++)
	{
		TempCode.address[i] = Code.address[i];
		TempCode.data[i] = Code.data[i];
	}

	while (1)
	{
		if (padRead(0, 0, &buttons));
		{
			paddata = 0xffff ^ buttons.btns;
			new_pad = paddata & ~old_pad;
			old_pad = paddata;


			if (new_pad & PAD_UP)
			{
				if (mn == 0)
				{
					if ((sel-ti) > 0)
					{
						i = sel - ti;
						i = i * 16;

						DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
						sel--;
						y = sel - ti;
						y = y * 8;
						DrawString(">>", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						y = chose - ti;
						if ((y > -1) && (y < 16))
						{
							y = y * 8;
							DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
						}
/*
						i = sel - ti;
						i = i * 16;

						//DrawBox(42, (i + 120), 0, 30, 20, frmR, frmG, frmB);
						sel--;
						y = sel - ti;
						y = y * 8;
						DrawString(">>", 25, (y + 60), 0, MicrosoftSansSerif, 1, 0x00, 0x00, 0x9C, 600, 0x00, 0x00, 0x00, 0, 1);

						if (sel == chose)
							DrawString(" >", 25, (y + 60), 0, MicrosoftSansSerif, 1, 0x00, 0x9C, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
						if ((sel + 1) == chose)
							DrawString(" >", 25, (y + 68), 0, MicrosoftSansSerif, 1, 0x00, 0x9C, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
*/
					}
					else
					{
						if (ti > 0)
						{
							ti--;
							dls = 1;
						}
					}
				}
				else if (mn == 1)
				{
					if (row > 0) { row--; dls = 1; }
				}
				else if (mn == 2)
				{
					if (ind == 0) { tmpAddr += 0x10000000; }
					if (ind == 1) { tmpAddr += 0x01000000; }
					if (ind == 2) { tmpAddr += 0x00100000; }
					if (ind == 3) { tmpAddr += 0x00010000; }
					if (ind == 4) { tmpAddr += 0x00001000; }
					if (ind == 5) { tmpAddr += 0x00000100; }
					if (ind == 6) { tmpAddr += 0x00000010; }
					if (ind == 7) { tmpAddr += 0x00000001; }

					if (ind == 8)  { tmpData += 0x10000000; }
					if (ind == 9)  { tmpData += 0x01000000; }
					if (ind == 10) { tmpData += 0x00100000; }
					if (ind == 11) { tmpData += 0x00010000; }
					if (ind == 12) { tmpData += 0x00001000; }
					if (ind == 13) { tmpData += 0x00000100; }
					if (ind == 14) { tmpData += 0x00000010; }
					if (ind == 15) { tmpData += 0x00000001; }

					dls = 1;
				}
			}
			if (new_pad & PAD_DOWN)
			{
				if (mn == 0)
				{
					if (((sel - ti) < 15) && ((sel - ti) < (codec - 1)))
					{
						i = sel - ti;
						i = i * 16;
						DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
						sel++;
						y = sel - ti;
						y = y * 8;
						DrawString(">>", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						y = chose - ti;
						if ((y > -1) && (y < 16))
						{
							y = y * 8;
							DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
						}
/*
						i = sel - ti;
						i = i * 16;
						//DrawBox(42, (i + 120), 0, 30, 20, frmR, frmG, frmB);
						sel++;
						y = sel - ti;
						y = y * 8;
						DrawString(">>", 25, (y + 60), 0, MicrosoftSansSerif, 1, 0x00, 0x00, 0x9C, 600, 0x00, 0x00, 0x00, 0, 1);

						if (sel == chose)
							DrawString(" >", 25, (y + 60), 0, MicrosoftSansSerif, 1, 0x00, 0x9C, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
						if ((sel - 1) == chose)
							DrawString(" >", 25, (y + 52), 0, MicrosoftSansSerif, 1, 0x00, 0x9C, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
*/
					}
					else
					{
						if ((sel + 1) < codec)
						{
							ti++;
							dls = 1;
						}
					}
				}
				else if (mn == 1)
				{
					if (row < 4) { row++; dls = 1; }
				}
				else if (mn == 2)
				{
					if (ind == 0) { tmpAddr -= 0x10000000; }
					if (ind == 1) { tmpAddr -= 0x01000000; }
					if (ind == 2) { tmpAddr -= 0x00100000; }
					if (ind == 3) { tmpAddr -= 0x00010000; }
					if (ind == 4) { tmpAddr -= 0x00001000; }
					if (ind == 5) { tmpAddr -= 0x00000100; }
					if (ind == 6) { tmpAddr -= 0x00000010; }
					if (ind == 7) { tmpAddr -= 0x00000001; }

					if (ind == 8)  { tmpData -= 0x10000000; }
					if (ind == 9)  { tmpData -= 0x01000000; }
					if (ind == 10) { tmpData -= 0x00100000; }
					if (ind == 11) { tmpData -= 0x00010000; }
					if (ind == 12) { tmpData -= 0x00001000; }
					if (ind == 13) { tmpData -= 0x00000100; }
					if (ind == 14) { tmpData -= 0x00000010; }
					if (ind == 15) { tmpData -= 0x00000001; }

					dls = 1;
				}
			}
			if (new_pad & PAD_LEFT)
			{
				if (mn == 1)
				{
					mn = 0;
					dls = 1;
				}
				if (mn == 2)
				{
					if (ind > 0) { ind--; dls = 1; }
				}
			}
			if (new_pad & PAD_RIGHT)
			{
				if (mn == 0)
				{
					mn = 1;
					dls = 1;
				}
				if (mn == 2)
				{
					if (ind < 15) { ind++; dls = 1; }
				}
			}
			if (new_pad & PAD_CIRCLE)
			{
				if (mn == 1)
				{
					mn = 0;
				}
				//DrawFrame(318, 110, 0, 280, 30, 2, frmR, frmG, frmB, 0x01, 0x01, 0x01);
				chose = -1;
				dls = 1;
			}
			if (new_pad & PAD_TRIANGLE)
			{
				if (mn == 2)
				{
					//DrawFrame(330, 300, 0, 268, 40, 2, frmR, frmG, frmB, 0x01, 0x01, 0x01);
					ind = 0;
					mn = 1;
					dls = 1;
				}
			}
			if (new_pad & PAD_CROSS)
			{
				if (mn == 0)
				{
					i = chose - ti;
					if ((i > -1) && (i < 16))
					{
						i = i * 16;
						DrawBox(42, (i + 120), 0, 30, 20, frmR, frmG, frmB);
					}
					chose = sel;
					//DrawFrame(330, 300, 0, 268, 40, 2, frmR, frmG, frmB, 0x01, 0x01, 0x01);
					//DrawBox(42, (((sel - ti)*16) + 120), 0, 30, 20, frmR, frmG, frmB);

					MemToString(str, TempCode.address[chose], TempCode.data[chose]);
					DrawString(str, 180, 152, 0, MicrosoftSansSerif, 1, 0x00, 0x64, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

					mn = 1;
					dls = 1;
				}
				else if (mn == 1)
				{
					if (row == 0) // Add Line
					{
						if (codec < 20)
						{
							tmpAddr = 0x00000000;
							tmpData = 0x00000000;
							chose = codec;
							mn = 2;
							dls = 1;
						}
					}
					if (row == 1) // Edit Line
					{
						tmpAddr = TempCode.address[chose];
						tmpData = TempCode.data[chose];
						mn = 2;
						dls = 1;
					}
					if (row == 2) // Remove Line
					{
						
					}
					if (row == 3) // Save Changes
					{
						for (i = 0; i < 20; i++)
						{
							*(u32*)&Code.address[i] = TempCode.address[i];
							*(u32*)&Code.data[i] = TempCode.data[i];
							*(u32*)(AddressMark + (i * 4)) = TempCode.address[i];
							*(u32*)(DataMark + (i * 4)) = TempCode.data[i];
						}
						ClearWindow(20, 40, 0, 580, 360);
						return 0;
					}
					if (row == 4) // Go Back
					{
						ClearWindow(20, 40, 0, 580, 360);
						return 0;
					}
				}
				else if (mn == 2)
				{
					//DrawFrame(330, 300, 0, 268, 40, 2, frmR, frmG, frmB, 0x01, 0x01, 0x01);
					TempCode.address[chose] = tmpAddr;
					TempCode.data[chose] = tmpData;
					mn = 0;
					ind = 0;
					dls = 1;

					codec = 0;
					for (i = 0; i < 20; i++)
					{
						if (TempCode.address[i] != 0) { codec++; }
						if (TempCode.address[i] == 0) { Code.data[i] = 0; }
					}
				}
			}
		}

		if (dls == 1)
		{
			if (mn == 0)
			{
				DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 280, 300, 40, 90, 0, 560, 300, 40, 90, 280, 300, 640, 480);
				y = 50;
				for (i = 0; i < 16; i++)
				{
					if ( (ti + i) < codec)
					{
						if ((sel - ti) == i)
							DrawString(">>", 25, ((i*8) + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						if ((ti + i) == chose)
							DrawString(" >", 25, ((i*8) + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

						if (TempCode.address[i] != 0)
						{
							MemToString(str, TempCode.address[i], TempCode.data[i]);
							DrawString(str, 35, y, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 0, 1);
							y += 8;
						}
						//DrawString(Codes[ti + i].codename, 35, y, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 0, 1);
						//y += 8;
					}
				}
				DrawPhotoSpotMergeBG(listmanwin, background, 310, 40, 30, 110, 350, 155, 0, 560, 300, 350, 90, 30, 110, 640, 480);
			}
			if (mn == 1)
			{
				DrawPhotoSpotMergeBG(listmanwin, background, 310, 40, 30, 110, 350, 155, 0, 560, 300, 350, 90, 30, 110, 640, 480);
				DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
				DrawString(">>", 180, ((row*8) + 85), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);

				y = chose - ti;
				if ((y > -1) && (y < 16))
				{
					y = y * 8;
					DrawString(" >", 25, (y + 50), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 0, 1);
				}

			}
			if (mn == 2)
			{
				DrawPhotoSpotMergeBG(listmanwin, background, 310, 40, 30, 110, 350, 155, 0, 560, 300, 350, 90, 30, 110, 640, 480);
				DrawPhotoSpotMergeBG(listmanwin, background, 0, 0, 30, 300, 40, 90, 0, 560, 300, 40, 90, 30, 300, 640, 480);
				DrawPhotoSpotMergeBG(listmanwin, background, 300, 200, 220, 50, 350, 290, 0, 560, 300, 350, 305, 200, 50, 640, 480);
				//DrawFrame(330, 300, 0, 268, 40, 2, frmR, frmG, frmB, 0x01, 0x01, 0x01);

				MemToString(str, tmpAddr, tmpData);
				DrawString(str, 180, 152, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 0, 1);

				spacex = 0;
				for (i = 0; i < ind; i++)
					spacex += GetSpacing(str[i]);
				if (ind > 7) { spacex += 10; }

				spacex = spacex / 2;

				DrawString("^", (181 + spacex), 160, 0, MicrosoftSansSerif, 1, 0x00, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 0, 1);
			}

			dls = 0;
		}

	}
}

int ReadIndex(int drv, mlistinfo lists[100])
{
	int f, fs, i, n, it, ind, cp, lc;
	u8 *data;
	char *path[3];
	char indexPath[50];
	char tmp[40];


	path[0] = "mc0:/CheatersLounge/Lists/";
	path[1] = "mc1:/CheatersLounge/Lists/";
	path[2] = "mass:/CheatersLounge/Lists/";

	CopyString(path[drv - 1], indexPath, 0);
	CopyString("index.ini", indexPath, strlen(path[drv - 1]));

	f = fioOpen(indexPath, O_RDONLY);

	ind = -1;
	lc = 0;

	if (f > 0)
	{
		fs = fioLseek(f, 0, SEEK_END);
		i = fioLseek(f, 0, SEEK_SET);
		data = malloc(fs);
		i = fioRead(f, data, fs);
		fioClose(f);

		n = 0;
		it = 1;
		ind = 0;
		cp = 0;
		for (i = 0; i < fs; i++)
		{
			if (it == 1)
			{
				tmp[n] = data[i];
				tmp[n + 1] = 0x00;
				//lists[ind].listname[n] = data[i];
				//lists[ind].listname[n + 1] = 0x00;
				if (data[i] == 0x00)
				{
					n = -1;
					it = 0;
					CopyString(tmp, lists[ind].listname, 0);
					CopyString(path[drv - 1], lists[ind].listpath, 0);
					cp = strlen(path[drv - 1]);
					lists[ind].listname[22] = 0x00;
					printf("lists[%i].listname: %s ", ind, lists[ind].listname);
				}
			}
			else
			{
				tmp[n] = data[i];
				tmp[n + 1] = 0x00;
				//lists[ind].listpath[n] = data[i];
				//lists[ind].listpath[n + 1] = 0x00;
				if (data[i] == 0x00)
				{
					CopyString(tmp, lists[ind].listpath, cp);
					it = 1;
					n = -1;
					ind++;
					lc++;
					printf("lists[%i].listpath: %s ", ind, lists[ind].listpath);
				}
			}
			if ( (data[i] == 0x00) && (data[i + 1] == 0x00) ) { free(data); return lc; }
			n++;
		}
		ind++;
		free(data);
	}

	printf("ListCount: %i ", lc);
	return lc;
}

int ReadList(mcode Codes[100])
{
	int f, fs, i, n, fz, ind, valid;
	u32 *data;
	u32 dAddr;
	char tmp[40];
	u32 CodeSize, CodeStart, fzAddr, blockSize;

	f = fioOpen(mylistpath, O_RDONLY);

	ind = -1;

	if (f > 0)
	{
		fs = fioLseek(f, 0, SEEK_END);
		i = fioLseek(f, 0, SEEK_SET);
		data = malloc(fs);
		i = fioRead(f, data, fs);
		fioClose(f);

		dAddr = *(u32*)(&data);

		// Check Header Validity
		valid = 1;
		if (_lw(dAddr + 0x00) != 0x61656843) { valid = -1; }
		if (_lw(dAddr + 0x04) != 0x73726574) { valid = -1; }
		if (_lw(dAddr + 0x08) != 0x6E756F4C) { valid = -1; }
		if (_lw(dAddr + 0x0C) != 0x31006567) { valid = -1; }
		if (_lw(dAddr + 0x10) != 0x00003300) { valid = -1; }
		if (_lw(dAddr + 0x14) != 0x004E4942) { valid = -1; }
		if (_lw(dAddr + 0x18) != 0x00320031) { valid = -1; }
		if (_lw(dAddr + 0x1C) != 0x00000000) { valid = -1; }

		if (valid == -1) { free(data); return -2; }

		// Parse Code List
		fz = 1;
		fzAddr = 0;
		blockSize = 0x24;
		for (i = 0; i < 100; i++)
		{
			CodeSize = _lw(((dAddr + 0x20) + (i * 8)) + 0x00);
			CodeStart = dAddr + _lw(((dAddr + 0x20) + (i * 8)) + 0x04);
			blockSize += 0x08;
			blockSize += (CodeSize * 8);

			if (CodeSize == 0) { goto SkipHundred; }


			for (n = 0; n < 20; n++)
			{
				if (n < CodeSize)
				{
					*(u32*)(&Codes[i].address[n]) = *(u32*)( ((CodeStart + (n*8)) + 0x00) );
					*(u32*)(&Codes[i].data[n]) = *(u32*)( ((CodeStart + (n*8)) + 0x04) );
					*(u32*)(&Codes[i].address[n + 1]) = 0x00000000;
					*(u32*)(&Codes[i].data[n + 1]) = 0x00000000;
				}
				else
				{
					if (fz == 1) { fz = 0; fzAddr = ((CodeStart + (n*8)) + 0x00); Codes[i].lines = (n-1); }
					*(u32*)(&Codes[i].address[n]) = 0x00000000;
					*(u32*)(&Codes[i].data[n]) = 0x00000000;
				}
			}

		}
		SkipHundred:
		ind = i;

		fzAddr = dAddr + blockSize;

		CodeStart = fzAddr;

		for (i = 0; i < ind; i++)
		{
			n = 0;
			while (_lb(CodeStart) != 0x00)
			{
				Codes[i].codename[n] = _lb(CodeStart);
				CodeStart++;
				n++;
			}
			Codes[i].codename[n] = _lb(CodeStart);
			if (_lb(CodeStart) == 0x00) { CodeStart++; }
			Codes[i].codename[22] = 0x00;
		}

		free(data);
	}

	return ind;
}

int SaveList(mcode Codes[100], char *listpath, int codecount)
{
	u8 chdata[100];
	u8 data[800];
	u8 header[32];
	int fd, c, i;
	int codec, totalsize, csize, ts;
	u32 sizes[100];
	u32 locations[100];
	u32 addr;

	codec = codecount;

	fd = fioOpen(listpath, O_WRONLY | O_CREAT);
	if (fd > 0)
	{
		printf("Build header.. ");
		addr = &header;
		*(u32*)(addr + 0x00) = 0x61656843;
		*(u32*)(addr + 0x04) = 0x73726574;
		*(u32*)(addr + 0x08) = 0x6E756F4C;
		*(u32*)(addr + 0x0C) = 0x31006567;
		*(u32*)(addr + 0x10) = 0x00003300;
		*(u32*)(addr + 0x14) = 0x004E4942;
		*(u32*)(addr + 0x18) = 0x00320031;
		*(u32*)(addr + 0x1C) = 0x00000000;

		// Write the file Header
		printf("Write header.. ");
		fioWrite(fd, &header, 32);

		// Build Lead Information
		csize = 0;
		totalsize = 0;
		printf("Build Lead.. ");
		for (c = 0; c < codec; c++)
		{
			csize = 0;
			for (i = 0; i < 20; i++)
			{
				if (*(u32*)&Codes[c].address[i] != 0)
				{
					csize++;
				}
				else
				{
					goto savelist_skip_rest_code_data;
				}
			}
savelist_skip_rest_code_data:
			sizes[c] = csize;
			printf("%i->%08X.. ", c, csize);
		}
		totalsize = 0x20;
		ts = codec * 8;
		totalsize += ts;
		totalsize += 8;
		for (i = 0; i < codec; i++)
		{
			locations[i] = totalsize;
			ts = sizes[i] * 8;
			totalsize += ts;
		}

		// Write Lead Information
		ts = 0;
		printf("Write Lead.. ");
		addr = &data;
		for (i = 0; i < codec; i++)
		{
			*(u32*)(addr + ts) = sizes[i]; ts += 4;
			*(u32*)(addr + ts) = locations[i]; ts += 4;
		}
		*(u32*)(addr + ts) = 0; ts += 4;
		*(u32*)(addr + ts) = 0; ts += 4;
		fioWrite(fd, data, ts);

		// Write Code Data
		ts = 0;
		printf("Write Code Data.. ");
		for (c = 0; c < codec; c++)
		{
			for (i = 0; i < sizes[c]; i++)
			{
				*(u32*)(addr + ts) = *(u32*)&Codes[c].address[i]; ts += 4;
				*(u32*)(addr + ts) = *(u32*)&Codes[c].data[i]; ts += 4;
			}
		}
		*(u32*)(addr + (ts)) = 0; ts += 4;
		fioWrite(fd, data, ts);

		// Write Code Labels
		ts = 0;
		printf("Write Labels.. ");
		for (c = 0; c < codec; c++)
		{
			CopyString(Codes[c].codename, chdata, 0);
			fioWrite(fd, chdata, (strlen(chdata) + 1));
		}

		// Finally done
		fioClose(fd);
		return 1;
	}
	else
	{
		return -1;
	}
}

void MenuBrowse(int Direct, mbutton MenuButton[100])
{
	int x, y, b;

	if (Direct == 0) // Up
	{
		//ClearWindow(70, 115, 0, 20, 200);
		//DrawFrame(80, 120, 0, 20, 200, 0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
		if (selected > 0)
		{
			selected--;
		}
		x = 221;
		y = selected * 50; //8;
		b = selected * 24;
		y += 101; // 60;
		b += 55;

		//DrawPhotoSpot(background, 230, (y-5), 230, 100, 230, (y-5), 0, 640, 480, 0x00, 0x00, 0x00, -1, 0);

		DrawImage(button1, x, (y+50), 0, 197, 41, -1);
		DrawImage(button2, x, y, 0, 197, 41, -1);
		DrawString(MenuButton[selected].caption, 158, (b+selected), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 1, 1);
		b += 24;
		DrawString(MenuButton[selected+1].caption, 158, (b+(selected+1)), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 1, 1);
	}
	else // Down
	{
		//ClearWindow(70, 115, 0, 20, 200);
		//DrawFrame(80, 120, 0, 20, 200, 0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
		if (selected < (menucount - 1))
		{
			selected++;
		}
		x = 221;
		y = selected * 50; //8;
		b = selected * 24;
		y += 51; // 60;
		b += 55;

		//DrawPhotoSpot(background, 230, (y-5), 230, 100, 230, (y-5), 0, 640, 480, 0x00, 0x00, 0x00, -1, 0);

		DrawImage(button1, x, y, 0, 197, 41, -1);
		DrawImage(button2, x, (y+50), 0, 197, 41, -1);
		DrawString(MenuButton[selected].caption, 158, (b+selected), 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 1, 1);
		b -= 24;
		DrawString(MenuButton[selected-1].caption, 158, (b+(selected-1)), 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 1, 1);
	}
}

void WriteTitle(void)
{
	ClearWindow(0, 0, 0, 640, 75);
	//DrawFrame(40, 45, 0, 560, 54, 0, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00);

	if ( IsEqual("Main Menu", bigtitle) )
	{
		DrawImage(title_mainmenu, 197, 0, 0, 247, 29, -1);
	}
	if ( IsEqual("Code Manager", bigtitle) )
	{
		DrawImage(title_codeman, 142, 0, 0, 356, 29, -1);
	}
	if ( IsEqual("List Manager", bigtitle) )
	{
		DrawImage(title_listman, 160, 0, 0, 321, 29, -1);
	}

	if ( IsEqual("Main", subtitle) )
	{
		DrawImage(sub_main, 279, 30, 0, 81, 24, -1);
	}
	if ( IsEqual("Device Manager", subtitle) )
	{
		DrawImage(sub_deviceman, 172, 30, 0, 296, 23, -1);
	}
	if ( IsEqual("Code Editor", subtitle) )
	{
		DrawImage(sub_codeeditor, 206, 30, 0, 229, 24, -1);
	}
	if ( IsEqual("Editor", subtitle) )
	{
		DrawImage(sub_editor, 258, 30, 0, 124, 24, -1);
	}
	if ( IsEqual("Select Drive", subtitle) )
	{
		DrawImage(sub_selectdrive, 200, 30, 0, 241, 24, -1);
	}
	if ( IsEqual("Selector", subtitle) )
	{
		DrawImage(sub_selector, 232, 30, 0, 176, 23, -1);
	}
	if ( IsEqual("Start Game", subtitle) )
	{
		DrawImage(sub_startgame, 209, 30, 0, 222, 24, -1);
	}
	//DrawString(bigtitle, 160, 21, 0, MicrosoftSansSerif, 2, 0x01, 0x01, 0x01, 600, 0x00, 0x00, 0x00, 1, 2);
	//DrawString(subtitle, 160, 40, 0, MicrosoftSansSerif, 1, 0x01, 0x01, 0x01, 600, 0x00, 0x00, 0x00, 1, 1);
}

void WriteMenuText(int menu, mbutton MenuButton[100])
{
	int x, y, i, b, n;
	char btitle[40];

	x = 221;
	y = 101; //60;
	b = 55;
	n = 1;

	ClearWindow(0, 100, 0, 640, 340);

	for (i = 0; i < menucount; i++)
	{
		if (i == selected)
		{
			DrawImage(button2, x, y, 0, 197, 41, -1);
			DrawString(MenuButton[i].caption, 158, b, 0, MicrosoftSansSerif, 1, 0xDC, 0x00, 0x00, 600, 0x00, 0x00, 0x00, 1, 1);
			y += 50; //8;
			b += 24 + n;
			n++;
		}
		else
		{
			DrawImage(button1, x, y, 0, 197, 41, -1);
			DrawString(MenuButton[i].caption, 158, b, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 600, 0x00, 0x00, 0x00, 1, 1);
			y += 50; //8;
			b += 22 + n;
			n++;
		}
	}

}


int SetMenu(int mnu, mbutton MenuButton[100])
{
	int btn;

	btn = 0;
	if (mnu == 0) // Main
	{
		CopyString("Main Menu", bigtitle, 0);
		CopyString("Main", subtitle, 0);

		CopyString("Start Game", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 3;
		btn++;
		CopyString("Code Editor", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 1;
		btn++;
		CopyString("Device Manager", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 2;
		btn++;
		CopyString("Options", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 5;
		btn++;
		CopyString("Exit", MenuButton[btn].caption, 0);
		CopyString("reboot", MenuButton[btn].action, 0);
		btn++;

		MenuButton[btn].caption[0] = '!';
		MenuButton[btn].action[0] = '!';
		menucount = btn;

	}
	if (mnu == 1) // Code Editor
	{
		CopyString("Main Menu", bigtitle, 0);
		CopyString("Code Editor", subtitle, 0);

		CopyString("Games", MenuButton[btn].caption, 0);
		CopyString("gamesel", MenuButton[btn].action, 0);
		btn++;
		CopyString("Code Lists", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 4;
		btn++;
		CopyString("Edit List", MenuButton[btn].caption, 0);
		CopyString("elist", MenuButton[btn].action, 0);
		btn++;
		CopyString("Go Back", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 0;
		btn++;

		MenuButton[btn].caption[0] = '!';
		MenuButton[btn].action[0] = '!';
		menucount = btn;

	}
	if (mnu == 2) // Device Manager
	{
		CopyString("Main Menu", bigtitle, 0);
		CopyString("Device Manager", subtitle, 0);

		CopyString("Mem Card 1", MenuButton[btn].caption, 0);
		CopyString("dmmc1", MenuButton[btn].action, 0);
		btn++;
		CopyString("Mem Card 2", MenuButton[btn].caption, 0);
		CopyString("dmmc2", MenuButton[btn].action, 0);
		btn++;
		CopyString("USB Drive", MenuButton[btn].caption, 0);
		CopyString("dmusb", MenuButton[btn].action, 0);
		btn++;
		CopyString("Go Back", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 0;
		btn++;

		MenuButton[btn].caption[0] = '!';
		MenuButton[btn].action[0] = '!';
		menucount = btn;

	}
	if (mnu == 3) // With / Without Codes
	{
		CopyString("Main Menu", bigtitle, 0);
		CopyString("Start Game", subtitle, 0);

		CopyString("With Codes", MenuButton[btn].caption, 0);
		CopyString("starty", MenuButton[btn].action, 0);
		btn++;
		CopyString("No Codes", MenuButton[btn].caption, 0);
		CopyString("startn", MenuButton[btn].action, 0);
		btn++;
		CopyString("Go Back", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 0;
		btn++;

		MenuButton[btn].caption[0] = '!';
		MenuButton[btn].action[0] = '!';
		menucount = btn;

	}
	if (mnu == 4) // Select List Drive Selector
	{
		CopyString("List Manager", bigtitle, 0);
		CopyString("Select Drive", subtitle, 0);

		CopyString("Mem Card 1", MenuButton[btn].caption, 0);
		CopyString("slistmc1", MenuButton[btn].action, 0);
		btn++;
		CopyString("Mem Card 2", MenuButton[btn].caption, 0);
		CopyString("slistmc2", MenuButton[btn].action, 0);
		btn++;
		CopyString("USB Drive", MenuButton[btn].caption, 0);
		CopyString("slistusb", MenuButton[btn].action, 0);
		btn++;
		CopyString("Go Back", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 1;
		btn++;

		MenuButton[btn].caption[0] = '!';
		MenuButton[btn].action[0] = '!';
		menucount = btn;

	}
	if (mnu == 5) // Options
	{
		CopyString("Main Menu", bigtitle, 0);
		CopyString("Options", subtitle, 0);

		CopyString("IP Config", MenuButton[btn].caption, 0);
		CopyString("ipconfig", MenuButton[btn].action, 0);
		btn++;
		CopyString("Restart Server", MenuButton[btn].caption, 0);
		CopyString("restartserv", MenuButton[btn].action, 0);
		btn++;
		CopyString("Go Back", MenuButton[btn].caption, 0);
		CopyString("goto", MenuButton[btn].action, 0);
		MenuButton[btn].nmenu = 1;
		btn++;

		MenuButton[btn].caption[0] = '!';
		MenuButton[btn].action[0] = '!';
		menucount = btn;

	}

	mymenu = mnu;
	WriteTitle();
	WriteMenuText(mnu, MenuButton);
	return 0;
}

int DoAction(mbutton MenuButton[100])
{
	int tmp, i, n, ib;
	u32 ListAddr;
	char elfname[1024];
	char *args[1];
	mcode Codes[100];

	u32 tA, tB;

	if ( IsEqual("starty", MenuButton[selected].action) )
	{
		ListAddr = 0x80078250;


		ReadCNF(elfname);

		if (elfname[0] != '!')
		{
			tmp = ReadList(Codes);
			if (tmp > 0)
			{
				for (i = 0; i < tmp; i++)
				{
					n = 0;
					while (*(u32*)&Codes[i].address[n] != 0)
					{
						ee_kmode_enter();
						_sw(Codes[i].address[n], ListAddr); ListAddr += 4;
						_sw(Codes[i].data[n], ListAddr); ListAddr += 4;
						//*(u32*)&ListAddr = *(u32*)&Codes[i].address[n]; ListAddr += 4;
						//*(u32*)&ListAddr =*(u32*)&Codes[i].data[n]; ListAddr += 4;
						ee_kmode_exit();
						if ((Codes[i].address[n] / 0x10000000) == 0xD) { ib = 1; }
						n++;
						if ((ib == 1) && ((Codes[i].address[n] / 0x10000000) == 0xD))
						{
							ee_kmode_enter();
							_sw(0x00000000, ListAddr); ListAddr += 4;
							_sw(0x00000000, ListAddr); ListAddr += 4;
							//*(u32*)&ListAddr = 0x00000000; ListAddr += 4;
							//*(u32*)&ListAddr = 0x00000000; ListAddr += 4;
							ee_kmode_exit();
							ib = 1;
						}
					}
					if (ib == 1)
					{
						ee_kmode_enter();
						_sw(0x00000000, ListAddr); ListAddr += 4;
						_sw(0x00000000, ListAddr); ListAddr += 4;
						//*(u32*)&ListAddr = 0x00000000; ListAddr += 4;
						//*(u32*)&ListAddr = 0x00000000; ListAddr += 4;
						ee_kmode_exit();
						ib = 0;
					}
				}

				for (i = 0; i < 40; i += 2)
				{
					ee_kmode_enter();
					tA = _lw(0x80078250 + (i*4));
					tB = _lw(0x80078254 + (i*4));
					ee_kmode_exit();
					printf("%08X %08X ", tA, tB);
				}
				args[0] = elfname;

				MoveEngine(0x80030000);
				LoadExecPS2(elfname, 1, &args);
			}
		}

		return -1;
	}
	if ( IsEqual("startn", MenuButton[selected].action) )
	{
		ReadCNF(elfname);
		args[0] = elfname;

		LoadExecPS2(elfname, 1, &args);
		return -1;
	}
	if ( IsEqual("reboot", MenuButton[selected].action) )
	{
		LoadExecPS2("", 0, NULL);
		return -1;
	}
	if ( IsEqual("goto", MenuButton[selected].action) )
	{
		tmp = selected;
		mymenu = MenuButton[tmp].nmenu;
		selected = 0;
		SetMenu(MenuButton[tmp].nmenu, MenuButton);
		return 1;
	}
	if ( IsEqual("slistmc1", MenuButton[selected].action) )
	{
		ListSelector(1);

		selected = 0;
		SetMenu(1, MenuButton);
		return 1;
	}
	if ( IsEqual("slistmc2", MenuButton[selected].action) )
	{
		ListSelector(2);

		selected = 0;
		SetMenu(1, MenuButton);
		return 1;
	}
	if ( IsEqual("slistusb", MenuButton[selected].action) )
	{
		ListSelector(3);

		selected = 0;
		SetMenu(1, MenuButton);
		return 1;
	}
	if ( IsEqual("elist", MenuButton[selected].action) )
	{
		ListEditor(mylistname);

		selected = 1;
		SetMenu(1, MenuButton);
		return 1;
	}

	return -1;

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

void MemToString(char str[18], u32 MemAddr, u32 MemData)
{

	int Addr, Temp, TmpA, TmpD, TmpT, TextCP;
	int offset, data, i, byte, txt, dest, n, jokr;

	Addr = 0x00080000;
	Temp = 0x00080004;
	TmpA = 0x00080010;
	TmpD = 0x00080014;
	TmpT = 0x00080020;
	TextCP = 0x00080060;
	dest = 0x00080F00;

	offset = MemAddr;

	_sw(offset, Addr);
	offset = _lw(Addr);

	data = MemData;

	_sw(offset, TmpA);
	_sw(data, TmpD);

	n = 0;

	for (i = 0; i < 8; i++)
	{
		byte = _lb(TmpA + i);
		byte = byte * 0x10;
		_sh(byte, Temp);
		byte = _lb(Temp);
		byte = byte / 0x10;
		_sb(byte, Temp);

		byte = _lb(Temp);
		_sb(byte, (TmpT + i + n));

		byte = _lb(Temp + 1);
		_sb(byte, (TmpT + i + n + 1));
		n += 1;
	}


	for (i = 0; i < 16; i++)
	{
		offset = (TmpT + i);
		data = _lb(offset);

		if (data == 0x00) { data = 0x30; }
		else if (data == 0x01) { data = 0x31; }
		else if (data == 0x02) { data = 0x32; }
		else if (data == 0x03) { data = 0x33; }
		else if (data == 0x04) { data = 0x34; }
		else if (data == 0x05) { data = 0x35; }
		else if (data == 0x06) { data = 0x36; }
		else if (data == 0x07) { data = 0x37; }
		else if (data == 0x08) { data = 0x38; }
		else if (data == 0x09) { data = 0x39; }
		else if (data == 0x0A) { data = 0x41; }
		else if (data == 0x0B) { data = 0x42; }
		else if (data == 0x0C) { data = 0x43; }
		else if (data == 0x0D) { data = 0x44; }
		else if (data == 0x0E) { data = 0x45; }
		else if (data == 0x0F) { data = 0x46; }

		_sb(data, offset);
	}

	n = 15;

	for (i = 0; i < 16; i++)
	{
		_sb(_lb(TmpT + n), (TextCP + i));
		n--;
	}

	_sw(_lw(TextCP + 8), (dest + 0));
	_sw(_lw(TextCP + 12), (dest + 4));
	_sw(_lw(TextCP + 0), (dest + 8));
	_sw(_lw(TextCP + 4), (dest + 12));


	for (n = 0; n < 8; n++)
		str[n] = _lb(dest + n);

	str[8] = 0x20;

	for (n = 9; n < 17; n++)
		str[n] = _lb(dest + (n - 1));

	str[17] = 0x00;


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
