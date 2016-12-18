
/*

Content: (152, 114)

PS2IP: (250, 128)
NetMask: (281, 185)
GateWay: (272, 239)
*/

u8 IPString[9486];

char ps2IP[15] = "192.168.000.032";
char ps2NM[15] = "255.255.255.000";
char ps2GW[15] = "192.168.000.001";

int EnterIPConfig(void)
{
	int select, w, x, y, z;
	u8 ip[4];

	if (LoadConfig() < 1)
	{
		CopyString("192.168.000.032", ps2IP, 0);
		CopyString("255.255.255.000", ps2NM, 0);
		CopyString("192.168.000.001", ps2GW, 0);
	}

	DrawImage(background, 0, 0, 0, 640, 480, -1);

	AnimateTitle(title_ipconfig, 383);
	DrawImage(ipconfig_menu, 152, 114, 0, 132, 236, -1);
	DrawImage(arrow, 117, 122, 0, 49, 29, -1);
	graph_wait_vsync();

	ClearIPString();
	IPStrToHex(ps2IP, ip);
	w = BuildIPString(ip);
	DrawImage(IPString, 250, 128, 0, w, 17, -1);

	ClearIPString();
	IPStrToHex(ps2NM, ip);
	w = BuildIPString(ip);
	DrawImage(IPString, 281, 185, 0, w, 17, -1);

	ClearIPString();
	IPStrToHex(ps2GW, ip);
	w = BuildIPString(ip);
	DrawImage(IPString, 272, 239, 0, w, 17, -1);

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
				select = IPAnimateSelect(select, (select - 1));
			}
			if (new_pad & PAD_DOWN)
			{
				select = IPAnimateSelect(select, (select + 1));
			}
			if (new_pad & PAD_CROSS)
			{
				w = IPAnimateClick(select);
				if (select == 4)
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

int EditIPData(u8 ip[4], int ipN)
{
	int i, l, x, y, w, r, n, n2, ax, ay;
	u8 tmpIP[4];
	char strIP[15];

	tmpIP[0] = ip[0];
	tmpIP[1] = ip[1];
	tmpIP[2] = ip[2];
	tmpIP[3] = ip[3];

	if (ipN == 0) { x = 250; y = 128; }
	if (ipN == 1) { x = 281; y = 185; }
	if (ipN == 2) { x = 272; y = 239; }

	i = 0;
	r = 0;

	IPToString(tmpIP, strIP);
	ax = x + GetWidth(strIP[0] - 0x30);
	ay = y + 17;
	DrawImage(n_arrow, ax, ay, 0, 29, 30, -1);

	while(1)
	{
		if (padRead(0, 0, &buttons));
		{
			paddata = 0xffff ^ buttons.btns;
			new_pad = paddata & ~old_pad;
			old_pad = paddata;

			if (new_pad & PAD_UP)
			{
				PlayADPCM(pcm_Lclick);
				tmpIP[i]++;
				IPToString(tmpIP, strIP);
				r = 1;
			}
			if (new_pad & PAD_DOWN)
			{
				PlayADPCM(pcm_Lclick);
				tmpIP[i]--;
				IPToString(tmpIP, strIP);
				r = 1;
			}
			if (new_pad & PAD_LEFT)
			{
				if (i > 0)
				{
					PlayADPCM(pcm_Hclick);
					i--;
					r = 1;
				}
			}
			if (new_pad & PAD_RIGHT)
			{
				if (i < 3)
				{
					PlayADPCM(pcm_Hclick);
					i++;
					r = 1;
				}
			}
			if (new_pad & PAD_TRIANGLE)
			{
				DrawPhotoSpot(background, ax, ay, 29, 30, ax, ay, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
				graph_wait_vsync();
				return 0;
			}
			if (new_pad & PAD_CROSS)
			{
				ip[0] = tmpIP[0];
				ip[1] = tmpIP[1];
				ip[2] = tmpIP[2];
				ip[3] = tmpIP[3];
				DrawPhotoSpot(background, ax, ay, 29, 30, ax, ay, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
				graph_wait_vsync();
				return 0;
			}
		}

		if (r == 1)
		{
			IPToString(tmpIP, strIP);
			w = BuildIPString(tmpIP);
			DrawPhotoSpot(background, x, y, (w + 50), 17, x, y, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
			DrawImage(IPString, x, y, 0, w, 17, -1);

			l = 0;
			if (i > -1) { l += (GetWidth(strIP[0] - 0x30) / 2); }
			if (i > 0) {
					l += (GetWidth(strIP[0] - 0x30) / 2);
					l += GetWidth(strIP[1] - 0x30);
					l += GetWidth(strIP[2] - 0x30);
					l += 6;
					l += (GetWidth(strIP[4] - 0x30) / 2); }
			if (i > 1) {
					l += (GetWidth(strIP[4] - 0x30) / 2);
					l += GetWidth(strIP[5] - 0x30);
					l += GetWidth(strIP[6] - 0x30);
					l += 6;
					l += (GetWidth(strIP[8] - 0x30) / 2); }
			if (i > 2) {
					l += (GetWidth(strIP[8] - 0x30) / 2);
					l += GetWidth(strIP[9] - 0x30);
					l += GetWidth(strIP[10] - 0x30);
					l += 6;
					l += (GetWidth(strIP[12] - 0x30) / 2); }

			DrawPhotoSpot(background, ax, ay, 29, 30, ax, ay, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
			ax = x + l;
			DrawImage(n_arrow, ax, ay, 0, 29, 30, -1);

			r = 0;
		}
		RestartBGM();
	}

}

void IPToString(u8 ip[4], char strIp[15])
{
	int i, n, c;
	u8 num[16], bt[3];

	for (i = 0; i < 15; i++)
		num[i] = 0x30;

	n = 0;
	for (c = 0; c < 4; c++)
	{
		bt[0] = 0;
		bt[1] = 0;
		bt[2] = 0;
		for (i = 0; i < ip[c]; i++)
		{
			bt[0]++;
			if (bt[0] == 10) { bt[1]++; bt[0] = 0; }
			if (bt[1] == 10) { bt[2]++; bt[1] = 0; }
		}

		num[n] = IPHexToStr(bt[2]); n++;
		num[n] = IPHexToStr(bt[1]); n++;
		num[n] = IPHexToStr(bt[0]); n++;
		num[n] = IPHexToStr(10); n++;
	}

	for (i = 0; i < 15; i++)
		strIp[i] = num[i];
}

int BuildIPString(u8 ip[4])
{
	int i, c, n, x, y, w, wC;
	int num[16], bt[3];
	u32 numAddr, numWidth;
	u32 ipAddr;

	for (i = 0; i < 15; i++)
		num[i] = 0;

	n = 0;
	for (c = 0; c < 4; c++)
	{
		bt[0] = 0;
		bt[1] = 0;
		bt[2] = 0;
		for (i = 0; i < ip[c]; i++)
		{
			bt[0]++;
			if (bt[0] == 10) { bt[1]++; bt[0] = 0; }
			if (bt[1] == 10) { bt[2]++; bt[1] = 0; }
		}

		num[n] = bt[2]; n++;
		num[n] = bt[1]; n++;
		num[n] = bt[0]; n++;
		num[n] = 10; n++;
	}

	w = 0;
	wC = 0;
	c = 0;
	ipAddr = &IPString;

	for (y = 0; y < 17; y++)
	{
		for (i = 0; i < 15; i++)
		{
			numWidth = GetWidth(num[i]);
			numAddr = GetNum(num[i]) + ((numWidth * 3) * y);
			for (x = 0; x < numWidth; x++)
			{
				*(u8*)ipAddr = *(u8*)numAddr; numAddr++; ipAddr++; // R
				*(u8*)ipAddr = *(u8*)numAddr; numAddr++; ipAddr++; // G
				*(u8*)ipAddr = *(u8*)numAddr; numAddr++; ipAddr++; // B
			}
			if (wC < 15)
			{
				w += GetWidth(num[i]);
				wC++;
			}
		}
	}

	return w;
}

void IPStrToHex(char *ipStr, u8 ip[4])
{
	int i, n, c, btnum;
	u8 spc[4];

	ip[0] = 0;
	ip[1] = 0;
	ip[2] = 0;
	ip[3] = 0;

	spc[0] = 0;
	spc[1] = 0;
	spc[2] = 0;
	spc[3] = 0;

	btnum = 0;
	n = 0;
	for (i = 0; i < strlen(ipStr); i++)
	{
		if (ipStr[i] != '.')
			spc[n] = ipStr[i];
		n++;
		if (n < 4) { spc[n] = 0x00; }

		if ( (ipStr[i] == '.') || (i == (strlen(ipStr)-1)) )
		{
			c = 0;

			if (spc[1] == 0x00) // $x
			{
				ip[btnum] = HexToStr(spc[0]);
			}
			else if (spc[2] == 0x00) // $xx
			{
				spc[0] = HexToStr(spc[0]) * 10;
				spc[1] = HexToStr(spc[1]);
				ip[btnum] = spc[0] + spc[1];
			}
			else 			// $xxx
			{
				spc[0] = HexToStr(spc[0]) * 100;
				spc[1] = HexToStr(spc[1]) * 10;
				spc[2] = HexToStr(spc[2]);
				ip[btnum] = (spc[0] + spc[1]) + spc[2];
			}

			spc[0] = 0x00;
			spc[1] = 0x00;
			spc[2] = 0x00;
			spc[3] = 0x00;
			n = 0;
			btnum++;
		}
	}
}

int HexToStr(u8 hex)
{
	if (hex == 0x30) { return 0x00; }
	if (hex == 0x31) { return 0x01; }
	if (hex == 0x32) { return 0x02; }
	if (hex == 0x33) { return 0x03; }
	if (hex == 0x34) { return 0x04; }
	if (hex == 0x35) { return 0x05; }
	if (hex == 0x36) { return 0x06; }
	if (hex == 0x37) { return 0x07; }
	if (hex == 0x38) { return 0x08; }
	if (hex == 0x39) { return 0x09; }

	return 0;
}

u8 IPHexToStr(u8 hex)
{
	if (hex == 0x00) { return 0x30; }
	if (hex == 0x01) { return 0x31; }
	if (hex == 0x02) { return 0x32; }
	if (hex == 0x03) { return 0x33; }
	if (hex == 0x04) { return 0x34; }
	if (hex == 0x05) { return 0x35; }
	if (hex == 0x06) { return 0x36; }
	if (hex == 0x07) { return 0x37; }
	if (hex == 0x08) { return 0x38; }
	if (hex == 0x09) { return 0x39; }
	if (hex == 0x0A) { return 0x2E; }

	return 0x30;
}

void ClearIPString(void)
{
	int i;
	u32 addr;

	addr = &IPString;

	for (i = 0; i < 9486; i++) {
		*(u8*)addr = 0x00;
	}
}

int IPAnimateSelect(int oldSelect, int newSelect)
{
	int x, y, z, i, n;
	int max = 4;

	if ( (newSelect >= 0) && (newSelect <= max) )
	{
		PlayADPCM(pcm_Hclick);

		x = 117;
		y = (27 * oldSelect);
		z = 0;
		n = 7;

		if (oldSelect < 4) { y = (27 * oldSelect) + (y + 122); }
		if (oldSelect == 4) { y = (27 * 3) + (y + 122); }

		//printf("%i, %i ", x, y);
		if (newSelect > oldSelect)
		{
			for (i = 0; i < 6; i++) //(i = 0; i < 27; i++)
			{
				DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
				y += n;
				if (newSelect < 4) { y += n; }
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
				if (newSelect < 3) { y -= n; }
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

void IPAnimateClick(int select)
{
	int x, y, z, i, n, b, c, d;

	x = 117;
	y = (27 * select);
	z = 0;

	if (select < 4) { y = (27 * select) + (y + 122); }
	if (select == 4) { y = (27 * 3) + (y + 122); }

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

	i = DoClick(select);

	x = 117;
	y = (27 * select);
	z = 0;
	if (select < 4) { y = (27 * select) + (y + 122); }
	if (select == 4) { y = (27 * 3) + (y + 122); }

	DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
	if (i != -2) { DrawImage(arrow, x, y, z, 49, 29, -1); }
	graph_wait_vsync();
}

int DoClick(int select)
{
	int i, w;
	u8 ip[4];

/*

Content: (152, 114)

PS2IP: (250, 128)
NetMask: (281, 185)
GateWay: (272, 239)
*/

	if (select == 0) // PS2 IP
	{
		IPStrToHex(ps2IP, ip);
		EditIPData(ip, 0);
		IPToString(ip, ps2IP);
		w = BuildIPString(ip);
		DrawPhotoSpot(background, 250, 128, (w + 50), 17, 250, 128, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		DrawImage(IPString, 250, 128, 0, w, 17, -1);
		return -1;
	}
	else if (select == 1) // Net Mask
	{
		IPStrToHex(ps2NM, ip);
		EditIPData(ip, 1);
		IPToString(ip, ps2NM);
		w = BuildIPString(ip);
		DrawPhotoSpot(background, 281, 185, (w + 50), 17, 281, 185, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		DrawImage(IPString, 281, 185, 0, w, 17, -1);
		return -1;
	}
	else if (select == 2) // Gateway
	{
		IPStrToHex(ps2GW, ip);
		EditIPData(ip, 2);
		IPToString(ip, ps2GW);
		w = BuildIPString(ip);
		DrawPhotoSpot(background, 272, 239, (w + 50), 17, 272, 239, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		DrawImage(IPString, 272, 239, 0, w, 17, -1);
		return -1;
	}
	else if (select == 3) // Save
	{
		i = SaveConfig();
		AnimateSave(select, i);
		return -1;
	}
	else if (select == 4) // Browser
	{
		return -2;
	}

	return -1;
}

int SaveConfig(void)
{
	char data[50];
	int f, i;

	f = fioOpen("mc0:/CheatersLounge/ip.cfg", O_WRONLY | O_CREAT);
	if (f < 0) { f = fioOpen("mc1:/CheatersLounge/ip.cfg", O_WRONLY | O_CREAT); }
	if (f < 0) { return 3; }

	CopyString(ps2IP, data, 0);
	CopyString(ps2NM, data, 16);
	CopyString(ps2GW, data, 32);

	i = fioWrite(f, data, 48);
	if (i < 0) { return 3; }

	fioClose(f);

	return 2;
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

int AnimateSave(int select, int s)
{
	int x, y, z, i, n, b, c, d;
	int loopC;

	loopC = 0;

	if (s == 3) { PlayADPCM(pcm_Fpluck); }
		else { PlayADPCM(pcm_Spluck); }

	anisaveloop:
	if (loopC == 5) { goto anisavekill; }

	x = 117;
	y = (27 * select);
	z = 0;

	if (select < 4) { y = (27 * select) + (y + 122); }
	if (select == 4) { y = (27 * 3) + (y + 122); }


	for (i = 0; i < 5; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x -= 4;
		DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, s, 1, 25);
		graph_wait_vsync();
	}

	for (i = 0; i < 5; i++)
	{
		DrawPhotoSpot(background, x, y, 49, 29, x, y, z, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
		x += 4;
		DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, s, 1, 25);
		graph_wait_vsync();
	}

	loopC++;
	goto anisaveloop;

	anisavekill:

	return 0;
}

int GetWidth(int chr)
{
	if (chr == 0) { return 14; }
	if (chr == 1) { return 8; }
	if (chr == 2) { return 13; }
	if (chr == 3) { return 13; }
	if (chr == 4) { return 14; }
	if (chr == 5) { return 12; }
	if (chr == 6) { return 13; }
	if (chr == 7) { return 13; }
	if (chr == 8) { return 13; }
	if (chr == 9) { return 13; }
	if (chr == 10) { return 6; }
}

u32 GetNum(int chr)
{
	if (chr == 0) { return num_0; }
	if (chr == 1) { return num_1; }
	if (chr == 2) { return num_2; }
	if (chr == 3) { return num_3; }
	if (chr == 4) { return num_4; }
	if (chr == 5) { return num_5; }
	if (chr == 6) { return num_6; }
	if (chr == 7) { return num_7; }
	if (chr == 8) { return num_8; }
	if (chr == 9) { return num_9; }
	if (chr == 10) { return num_dot; }
}
