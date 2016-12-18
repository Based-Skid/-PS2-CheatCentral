

int EnterMemDump(void)
{
	int select, w, x, y, z;

	DrawImage(background, 0, 0, 0, 640, 480, -1);

	AnimateTitle(title_memdump, 376);
	DrawImage(memdump_menu, 152, 114, 0, 336, 236, -1);
	DrawImage(arrow, 117, 203, 0, 49, 29, -1);
	graph_wait_vsync();


	select = 0;
	while(1)
	{
		w = 0;

		if (padRead(0, 0, &buttons));
		{
			paddata = 0xffff ^ buttons.btns;
			new_pad = paddata & ~old_pad;
			old_pad = paddata;

			if (new_pad & PAD_UP)
			{
				select = MDAnimateSelect(select, (select - 1));
			}
			if (new_pad & PAD_DOWN)
			{
				select = MDAnimateSelect(select, (select + 1));
			}
			if (new_pad & PAD_CROSS)
			{
				w = MDAnimateClick(select);
				if (select == 2)
				{
					DrawImage(background, 0, 0, 0, 640, 480, -1);
					graph_wait_vsync();

					return 1;
				}
			}
		}
		RestartBGM();
	}

	return 0;
}

int MDAnimateSelect(int oldSelect, int newSelect)
{
	int x, y, z, i, n;
	int max = 2;

	if ( (newSelect >= 0) && (newSelect <= max) )
	{
		PlayADPCM(pcm_Hclick);

		x = 117;
		y = (27 * oldSelect) + 203;
		z = 0;
		n = 7;

		//if (oldSelect < 2) { y = (27 * oldSelect) + (y + 203); }
		if (oldSelect == 2) { y += 54; }

		//printf("%i, %i ", x, y);
		if (newSelect > oldSelect)
		{
			for (i = 0; i < 6; i++) //(i = 0; i < 27; i++)
			{
				DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
				y += n;
				if (newSelect == 2) { y += n; }
				if (newSelect == 2) { y += n; }
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
				if (newSelect == 1) { y -= n; }
				if (newSelect == 1) { y -= n; }
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

void MDAnimateClick(int select)
{
	int x, y, z, i, n, b, c, d;

	x = 117;
	y = (27 * select) + 203;
	z = 0;

	if (select == 2) { y += 54; }

	for (i = 0; i < 12; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x -= 2;
		DrawImage(arrow, x, y, z, 49, 29, -1);
		//DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 3, 1, 25);
		graph_wait_vsync();
	}

	for (i = 0; i < 5; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x += 4;
		DrawImage(arrow, x, y, z, 49, 29, -1);
		//DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 3, 1, 25);
		graph_wait_vsync();
	}

	for (i = 0; i < 5; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x -= 2;
		DrawImage(arrow, x, y, z, 49, 29, -1);
		//DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 3, 1, 25);
		graph_wait_vsync();
	}
	DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);

	for (i = 0; i < 17; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x++;
		DrawImage(arrow, x, y, z, 49, 29, (15*i));
		//DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, (15*i), 3, 1, 25);
		graph_wait_vsync();
	}

	i = MDDoClick(select);

	x = 117;
	y = (27 * select) + 203;
	z = 0;
	if (select == 2) { y += 54; }

	DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
	if (i != -2) { DrawImage(arrow, x, y, z, 49, 29, -1); }
	graph_wait_vsync();
}

int MDDoClick(int select)
{
	int i, w, f, rd;
	u8 kdump[1024];

	if (select == 0) // BIOS
	{
		rd = 0x1FC00000;

		f = fioOpen("mass:CL-MemDump_BIOS_ROM0.raw", O_WRONLY | O_CREAT);
		if (f > 0)
		{
			while (rd < 0x20000000)
			{
				fioWrite(f, rd, 0x1000);
				rd += 0x1000;
				MDAnimateSave(select, 0, 0);
			}
			fioClose(f);
			MDAnimateSave(select, 2, 1);
			MDAnimateSave(select, 2, 1);
			MDAnimateSave(select, 2, 1);
		}
		else
		{
			MDAnimateSave(select, 3, 1);
			MDAnimateSave(select, 3, 1);
			MDAnimateSave(select, 3, 1);
		}

		//DrawPhotoSpot(background, 250, 128, (w + 50), 17, 250, 128, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		return -1;
	}
	else if (select == 1) // Kernel
	{
		rd = 0x80000000;

		f = fioOpen("mass:CL-MemDump_KERNEL.raw", O_WRONLY | O_CREAT);
		if (f > 0)
		{
			while (rd < 0x80080000)
			{
				ee_kmode_enter();
				for (i = 0; i < 1024; i++)
				{
					kdump[i] = *(u8*)rd;
					rd++;
				}
				ee_kmode_exit();
				fioWrite(f, kdump, 1024);
				MDAnimateSave(select, 0, 0);
			}
			fioClose(f);
			MDAnimateSave(select, 2, 1);
			MDAnimateSave(select, 2, 1);
			MDAnimateSave(select, 2, 1);
		}
		else
		{
			MDAnimateSave(select, 3, 1);
			MDAnimateSave(select, 3, 1);
			MDAnimateSave(select, 3, 1);
		}

		//DrawPhotoSpot(background, 281, 185, (w + 50), 17, 281, 185, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		return -1;
	}
	else if (select == 2) // Browser
	{
		return -2;
	}

	return -1;
}

int MDAnimateSave(int select, int s, int vSync)
{
	int x, y, z, i, n, b, c, d;
	int loopC;

	if (s == 3) { PlayADPCM(pcm_Fpluck); }
		else { PlayADPCM(pcm_Spluck); }

	x = 117;
	y = (27 * select) + 203;
	z = 0;
	if (select == 2) { y += 54; }

	for (i = 0; i < 5; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x -= 4;
		DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, s, 1, 25);
		if (vSync) { graph_wait_vsync(); }
	}

	for (i = 0; i < 5; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x += 4;
		DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, s, 1, 25);
		if (vSync) { graph_wait_vsync(); }
	}

	return 0;
}

