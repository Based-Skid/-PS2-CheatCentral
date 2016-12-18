
#include "engine1.3.c"


void InstallEngine(void)
{
	u32 putoff = 0x80078910;
	u32 rdoff = (void*)enginev1o3;
	int i, data, jal, size;

	jal = putoff;

	ee_kmode_enter();
	for (i = 0; i < 868; i++)
	{
		*(u32*)putoff = *(u32*)rdoff;
		putoff += 4;
		rdoff += 4;
	}
	ee_kmode_exit();

	/*
	ee_kmode_enter();
	data = 0x00800000;
	for (i = 0x80030000; i < 0x8007FFFC; i += 4)
	{
		*(u32*)data = *(u32*)i;
	}
	ee_kmode_exit();

	// Debug dump
	i = fioOpen("mass:remoteps2_dump.raw", O_WRONLY | O_CREAT);
	if (i > 0)
	{
		fioWrite(i, 0x00800000, 0x4FFFC);
		fioClose(i);
	}
	*/

}

void InstallCodes(u32 ListAddr, int size)
{
	u32 Store = 0x80030000;
	u32 Points = 0x80078250;

	int i, n, j, p;

	u32 lAddr, cAddr, cData;
	u8 cCmd;

	ee_kmode_enter();

	for (i = 0; i < size; i += 8)
	{
		*(u32*)Store = 0; Store += 4;
		*(u32*)Store = 0; Store += 4;
	}

	Store = 0x80030000;

	j = 1;
	n = 1;
	lAddr = ListAddr;
	p = ((Store - 0x80030000) / 4) + 1;
	*(u16*)Points = p;
	Points += 4;
	*(u32*)Points = 0;
	for (i = 0; i < size; i += 8)
	{
		*(u32*)Store = 0;
		*(u32*)(Store + 4) = 0;

		cAddr = *(u32*)lAddr; lAddr += 4;
		cData = *(u32*)lAddr; lAddr += 4;
		cCmd = cAddr / 0x10000000;

		if ( ((cCmd == 0x7) | (cCmd == 0xD)) && (Store != 0x80030000) )
		{
			Store += 4;
			p = ((Store - 0x80030000) / 4) + 1;
			*(u16*)Points = p;
			Points += 4;
			*(u32*)Points = 0;
		}

		*(u32*)Store = cAddr; Store += 4;
		*(u32*)Store = cData; Store += 4;

		if ((cAddr == 0) && (cData == 0))
		{
			Store -= 8;
			cAddr = *(u32*)lAddr;

			if (cAddr != 0)
			{
				cCmd = cAddr / 0x10000000;
				if ((cCmd != 0x7) && (cCmd != 0xD))
				{
					Store += 4;
					p = ((Store - 0x80030000) / 4) + 1;
					*(u16*)Points = p;
					Points += 4;
					*(u32*)Points = 0;
				}
			}
		}


	}
	ee_kmode_exit();
}


void SetupEngine(u32 ListAddr, int size)
{
	InstallCodes(ListAddr, size);
	InstallEngine();
}

void HookEngine(void)
{
	ee_kmode_enter();
	_sw(0x0c01E244, 0x800002fc);
	ee_kmode_exit();
}


