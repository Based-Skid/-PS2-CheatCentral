
char ps2IP[20] = "192.168.0.55";
char ps2NM[20] = "255.255.255.0";
char ps2GW[20] = "192.168.0.1";

int ServerSocket;
int ClientSocket;
struct sockaddr_in ServerAddr;
struct sockaddr_in ClientAddr;
fd_set active_rd_set;
fd_set rd_set;

u32 DataToSend = 0x00800000;
u32 DataSendSz = 0;

u32 DataToGet = 0x00100000;
u32 DataGetSz = 0;

u32 CodeSize = 0;

char PAGE[100];
char ACTION[100];

int ServerInit(int TCP_Port)
{
	char senData[100];
	int ret, sz;

	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket < 0) { return -1; }
	memset( &ServerAddr, 0, sizeof(ServerAddr) );

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ServerAddr.sin_port = htons(TCP_Port);

	ret = bind( ServerSocket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr) );
	if (ret < 0) { return -2; }

	ret = listen(ServerSocket, 2);
	if (ret < 0) { return -3; }

	FD_ZERO(&active_rd_set);
	FD_SET(ServerSocket, &active_rd_set);
	return 1;
}

int ServerLoop(void)
{
	int i, ret, ClientLen, delay, sz, fc;
	int inSession = 0;
	int inSessionB = 0;
	char senData[100];

	ClearSendBuffer();
	ClearRecBuffer();

	delay = 0;
	fc = 0;
	while(1)
	{
		ClientLen = sizeof(ClientAddr);
		rd_set = active_rd_set;
		ret = select(FD_SETSIZE, &rd_set, NULL, NULL, NULL);

		if (inSession == 0)
		{
			ClientSocket = accept( ServerSocket, (struct sockaddr *)&ClientAddr, &ClientLen );
			if (ClientSocket > 0)
			{
				FD_SET(ClientSocket, &active_rd_set);
				inSession = 1;
			}
		}
		else
		{
			i = ProcessClient();
			if (i < 0)
			{
				inSession = 0;
				disconnect(ClientSocket);
			}
		}

	}

	return 0;
}

int ProcessClient()
{
	int recSize, sentSize, totalSent, sendingBuffer;
	int retData, i, ret, sendingSize, n, mcSlot, html;
	char *args[1];
	char elf[1024];
	char GETSIZE[20];
	char POSTSIZE[20];
	char GETDATA[1024];
	char POSTDATA[1024];
	u8 ind;

	u8 recData[8000];
	u8 senData[20480];
	u32 GetDataSize = 0;
	u32 PostDataSize = 0;

	// Clear Data
	for (i = 0; i < 1024; i++)
		GETDATA[i] = 0x00;
	for (i = 0; i < 1024; i++)
		POSTDATA[i] = 0x00;
	for (i = 0; i < 1024; i++)
		elf[i] = 0x00;
	for (i = 0; i < 20; i++)
		GETSIZE[i] = 0x00;
	for (i = 0; i < 20; i++)
		POSTSIZE[i] = 0x00;

	retData = 0;

	recSize = recv(ClientSocket, recData, 8000, 0);
	recData[recSize] = 0x00;
	if ( recSize <= 0 )
	{
		return -1;
	}
	else
	{

		ACTION[0] = 0x00;
		GETDATA[0] = 0x00;
		POSTDATA[0] = 0x00;

		if (GETorPOST(recData) == 1) // GET
		{
			GetDataSize = strlen(recData);

			PostDataSize = 0;
			StrToHex(&GetDataSize, GETSIZE, 4);
			ReverseHexString(GETSIZE, 8);
			StrToHex(&PostDataSize, POSTSIZE, 4);
			ReverseHexString(POSTSIZE, 8);

			GrabGETData(recData, GETDATA);
		}
		else if (GETorPOST(recData) == 2) // POST
		{
			GetDataSize = strlen(recData);

			StrToHex(&GetDataSize, GETSIZE, 4);
			ReverseHexString(GETSIZE, 8);

			PostDataSize = GetPOSTSize(recData);

			StrToHex(&PostDataSize, POSTSIZE, 4);
			ReverseHexString(POSTSIZE, 8);

			GrabGETData(recData, GETDATA);

			DataGetSz = GetPOSTData(recData);

			while (DataGetSz < PostDataSize)
			{
				recSize = recv(ClientSocket, recData, 8000, 0);
				for (i = 0; i < recSize; i++)
				{
					*(u8*)DataToGet = recData[i];
					DataToGet++;

					DataGetSz = DataToGet - 0x00100000;
				}
				DataGetSz = DataToGet - 0x00100000;
			}
			DataGetSz = PostDataSize;
		}


		if ( GrabGETVar(GETDATA, "action", ACTION) < 0 )
			ACTION[0] = 0x00;

		html = 1;

		GrabPageURL(GETDATA, PAGE);

		if ( IsEqual(PAGE, "/") )
		{
			CopyString(template_html, senData, 0); //CopyString(index_html, senData, 0);
			ReplaceString(senData, "$TABLEDATA", "Select a link in the menu to continue.");
		}
		else if ( IsEqual(PAGE, "/index.gt") )
		{
			CopyString(template_html, senData, 0); //CopyString(index_html, senData, 0);
			ReplaceString(senData, "$TABLEDATA", "Select a link in the menu to continue.");
		}
		else if ( IsEqual(PAGE, "/codes.gt") )
		{
			CopyString(template_html, senData, 0); //CopyString(codes_html, senData, 0);
			ReplaceString(senData, "$TABLEDATA", codes_html);
		}
		else if ( IsEqual(PAGE, "/sendlist.gt") )
		{
			CopyString(template_html, senData, 0); //CopyString(codes_html, senData, 0);
			ReplaceString(senData, "$TABLEDATA", codes_html);
		}
		else if ( IsEqual(PAGE, "/info.gt") )
		{
			CopyString(template_html, senData, 0); //CopyString(codes_html, senData, 0);
			ReplaceString(senData, "$TABLEDATA", info_html);
		}
		else if ( IsEqual(PAGE, "/CLounge.css") )
		{
			CopyString(CLounge_css, senData, 0); //CopyString(codes_html, senData, 0);
		}
		else if ( IsEqual(PAGE, "/CLoungePlain.jpg") )
		{
			AddToOutput(CLoungePlain_jpg, 41292);
			html = 0;
		}
		else if ( IsEqual(PAGE, "/headergrunge.jpg") )
		{
			AddToOutput(headergrunge_jpg, 26808);
			html = 0;
		}
		else if ( IsEqual(PAGE, "/headergrunge2.jpg") )
		{
			AddToOutput(headergrunge2_jpg, 28698);
			html = 0;
		}

		if (html == 1)
		{

			if ( IsEqual(ACTION, "reset") )
			{
				CopyString(closed_html, senData, 0);
				ReplaceString(senData, "$TABLEDATA", "Server has been reset.");
				AddToOutput(senData, strlen(senData));
				SendOutput();
				ClearSendBuffer();
				ClearRecBuffer();
				disconnect(ClientSocket);
				ResetIOP();
				LoadExecPS2("", 0, NULL);
			}
			else if ( IsEqual(ACTION, "execgame") )
			{
				ReadCNF(elf);
				args[0] = elf;

				if (elf[0] != '!')
				{
					CopyString(closed_html, senData, 0);
					ReplaceString(senData, "$TABLEDATA", "The game disc has been launched.<br />If it does not load, please ensure the disc has been inserted and try again.<br />If it hasn't loaded and you cannot reconnect to the server, then double check the code list you sent and reload the application.");
					AddToOutput(senData, strlen(senData));
					SendOutput();
					ClearSendBuffer();
					ClearRecBuffer();
					disconnect(ClientSocket);
					ResetIOP();
					MoveCodeList(0x80030000, 0x00700000);
					MoveEngine(0x80030000);
					LoadExecPS2(elf, 1, args);
				}
				else
				{
					CopyString("Game disc failed to launch.", senData, 0);
				}
			}
			else if ( IsEqual(ACTION, "sendcodes") )
			{
				if ( *(u32*)0x00100008 != 0x00)
				{
					ClearCodeBuffer();
					RebuildCodes(0x00100000, PostDataSize, 0x00700000);
					ReplaceString(senData, "$RECEIVEDCODES", "<font color=30A030>Codes Received!</font>");

				}

			}
			/*
			else if ( IsEqual(ACTION, "debugcodes") )
			{
				CopyString(0x00750000, senData, 0);

			}
			*/

			ReplaceString(senData, "$GETDATA", GETDATA);
			ReplaceString(senData, "$GETSIZE", GETSIZE);
			ReplaceString(senData, "$POSTDATA", POSTDATA);
			ReplaceString(senData, "$POSTSIZE", POSTSIZE);
			ReplaceString(senData, "$PAGEURL", PAGE);
			ReplaceString(senData, "$ACTION", ACTION);
			ReplaceString(senData, "$RECDATA", recData);
			ReplaceString(senData, "$RECEIVEDCODES", "Nothing Received.");

			AddToOutput(senData, strlen(senData));

		}

		SendOutput();

		ClearSendBuffer();
		ClearRecBuffer();
	}

	return -1;
}

void RebuildCodes(u32 read, int sz, u32 write)
{
	int i, begwr;
	u32 rd, wr;
	u8 b1, b2, b3, b4, b5, b6, b7, b8;
	u32 addr, data;
	int j = 0;
	u32 debugAddr = 0x00750000;
	u32 tmpDebug = 0x00740000;
	int debugMode = 0;
	begwr = 0;
	rd = read;
	wr = write;
	CodeSize = 0;

	i = 0;
	//for (i = 0; i < sz; i++)
	while (*(u8*)rd != 0x00)
	{
		// Supposed to convert string to hex but for now debug to output
		if ( *(u8*)rd == '=' )
		{
			begwr = 1;
			rd++;
		}

		if (begwr == 1)
		{

			if ( *(u8*)rd == '%' ) {
			 if ( *(u8*)(rd + 1) == '0' ) {
			  if ( *(u8*)(rd + 2) == 'D' ) {
			   if ( *(u8*)(rd + 3) == '%' ) {
			    if ( *(u8*)(rd + 4) == '0' ) {
			     if ( *(u8*)(rd + 5) == 'A' ) {

				rd += 5;
				i += 6;

				if (j == 1)
				{
					*(u32*)wr = 0; wr += 4;
					*(u32*)wr = 0; wr += 4;
					j = 0;
				}

				if (debugMode == 1)
				{
					// Debug the shit
					*(u8*)debugAddr = 0x0D;
					debugAddr++;
					*(u8*)debugAddr = 0x0A;
					debugAddr++;
				}
				
			}}}}}}
			else {

			addr = 0;
			// Address
			b1 = *(u8*)rd; rd++;
			b2 = *(u8*)rd; rd++;
			b3 = *(u8*)rd; rd++;
			b4 = *(u8*)rd; rd++;
			b5 = *(u8*)rd; rd++;
			b6 = *(u8*)rd; rd++;
			b7 = *(u8*)rd; rd++;
			b8 = *(u8*)rd; rd++;
			rd++;

			i += 9;

			b1 -= 0x30;
			b2 -= 0x30;
			b3 -= 0x30;
			b4 -= 0x30;
			b5 -= 0x30;
			b6 -= 0x30;
			b7 -= 0x30;
			b8 -= 0x30;

			// Lower Case                          Upper Case
			if (b1 > 0x30) { b1 -= 0x27; } else if (b1 > 0x10) { b1 -= 0x07; }
			if (b2 > 0x30) { b2 -= 0x27; } else if (b2 > 0x10) { b2 -= 0x07; }
			if (b3 > 0x30) { b3 -= 0x27; } else if (b3 > 0x10) { b3 -= 0x07; }
			if (b4 > 0x30) { b4 -= 0x27; } else if (b4 > 0x10) { b4 -= 0x07; }
			if (b5 > 0x30) { b5 -= 0x27; } else if (b5 > 0x10) { b5 -= 0x07; }
			if (b6 > 0x30) { b6 -= 0x27; } else if (b6 > 0x10) { b6 -= 0x07; }
			if (b7 > 0x30) { b7 -= 0x27; } else if (b7 > 0x10) { b7 -= 0x07; }
			if (b8 > 0x30) { b8 -= 0x27; } else if (b8 > 0x10) { b8 -= 0x07; }


			addr += (b8 * 0x1);
			addr += (b7 * 0x10);
			addr += (b6 * 0x100);
			addr += (b5 * 0x1000);
			addr += (b4 * 0x10000);
			addr += (b3 * 0x100000);
			addr += (b2 * 0x1000000);
			addr += (b1 * 0x10000000);

			data = 0;
			// Data
			b1 = *(u8*)rd; rd++;
			b2 = *(u8*)rd; rd++;
			b3 = *(u8*)rd; rd++;
			b4 = *(u8*)rd; rd++;
			b5 = *(u8*)rd; rd++;
			b6 = *(u8*)rd; rd++;
			b7 = *(u8*)rd; rd++;
			b8 = *(u8*)rd; rd++;

			i += 8;

			b1 -= 0x30;
			b2 -= 0x30;
			b3 -= 0x30;
			b4 -= 0x30;
			b5 -= 0x30;
			b6 -= 0x30;
			b7 -= 0x30;
			b8 -= 0x30;

			// Lower Case                          Upper Case
			if (b1 > 0x30) { b1 -= 0x27; } else if (b1 > 0x10) { b1 -= 0x07; }
			if (b2 > 0x30) { b2 -= 0x27; } else if (b2 > 0x10) { b2 -= 0x07; }
			if (b3 > 0x30) { b3 -= 0x27; } else if (b3 > 0x10) { b3 -= 0x07; }
			if (b4 > 0x30) { b4 -= 0x27; } else if (b4 > 0x10) { b4 -= 0x07; }
			if (b5 > 0x30) { b5 -= 0x27; } else if (b5 > 0x10) { b5 -= 0x07; }
			if (b6 > 0x30) { b6 -= 0x27; } else if (b6 > 0x10) { b6 -= 0x07; }
			if (b7 > 0x30) { b7 -= 0x27; } else if (b7 > 0x10) { b7 -= 0x07; }
			if (b8 > 0x30) { b8 -= 0x27; } else if (b8 > 0x10) { b8 -= 0x07; }

			data += (b8 * 0x1);
			data += (b7 * 0x10);
			data += (b6 * 0x100);
			data += (b5 * 0x1000);
			data += (b4 * 0x10000);
			data += (b3 * 0x100000);
			data += (b2 * 0x1000000);
			data += (b1 * 0x10000000);



			if (j == 0)
			{
				if ((addr / 0x10000000) == 0x0D)
				{
					j = 1;
				}
			}
			else
			{
				if ((addr / 0x10000000) == 0x0D)
				{
					*(u32*)wr = 0; wr += 4;
					*(u32*)wr = 0; wr += 4;

					j = 1;
				}
			}

			if (debugMode == 1)
			{
				// Debug the shit
				StrToHex(&addr, tmpDebug, 4);
				ReverseHexString(tmpDebug, 8);
				CopyString(tmpDebug, debugAddr, 0);
				debugAddr += 8;
				*(u8*)debugAddr = 0x20;
				debugAddr++;
				StrToHex(&data, tmpDebug, 4);
				ReverseHexString(tmpDebug, 8);
				CopyString(tmpDebug, debugAddr, 0);
				debugAddr += 8;
				*(u8*)debugAddr = 0x0D;
				debugAddr++;
				*(u8*)debugAddr = 0x0A;
				debugAddr++;
			}

			*(u32*)wr = addr; wr += 4;
			*(u32*)wr = data; wr += 4;

			CodeSize += 8;

			// Return Mark
			rd += 5;
			i += 6;
			}
		}
		rd++;
	}
}

void ClearCodeBuffer(void)
{
	int i;
	for (i = 0x00700000; i < 0x00800000; i += 4)
		*(u32*)i = 0x00000000;

}

void ClearSendBuffer(void)
{
	int i;

	for (i = 0x00800000; i < 0x00EF0000; i += 0x10)
	{
		*(u32*)(i + 0x00) = 0;
		*(u32*)(i + 0x04) = 0;
		*(u32*)(i + 0x08) = 0;
		*(u32*)(i + 0x0C) = 0;
	}
	DataToSend = 0x00800000;
	DataSendSz = 0;
}

void ClearRecBuffer(void)
{
	int i;

	for (i = 0x00100000; i < 0x00700000; i += 0x10)
	{
		*(u32*)(i + 0x00) = 0;
		*(u32*)(i + 0x04) = 0;
		*(u32*)(i + 0x08) = 0;
		*(u32*)(i + 0x0C) = 0;
	}

	DataToGet = 0x00100000;
	DataGetSz = 0;
}

int GetPOSTData(char *header, char *dst)
{
	int i, p, sz;

	p = 0;
	for (i = 0; i < strlen(header); i++)
	{
		if ( (header[i] == 0x0D) && (header[i + 1] == 0x0A) ) {
			if ( (header[i + 2] == 0x0D) && (header[i + 3] == 0x0A) ) {
				i += 3; p = 1; } }

		if (p == 1)
		{
			while(i < strlen(header))
			{
				*(u8*)DataToGet = header[i];
				DataToGet++;
				i++;
			}
			sz = DataToGet - 0x00100000;

			return sz;
		}
	}

	return 0;
}

int GetPOSTSize(char *data)
{
	char strBit[50];

	u8 b;
	int i, n, c, d;
	int val;

	for (i = 0; i < strlen(data); i++)
	{
		GetStringBit(data, strBit, i, 16);

		if ( IsEqual(strBit, "Content-Length: ") )
		{
			i += 16;

			n = 0;
			while (data[i] != 0x0D)
			{
				strBit[n] = data[i];
				strBit[n + 1] = 0x00;
				n++;
				i++;
			}

			d = 0;
			val = 0;
			for (i = (strlen(strBit) - 1); i > -1; i--)
			{
				b = strBit[i] - 0x30;
				c = power(10, d);
				d++;

				val += (b * c);
			}

			return val;
		}

	}

	return 0;
}

int GETorPOST(char *data)
{
	char strBit[10];

	GetStringBit(data, strBit, 0, 4);

	if ( IsEqual(strBit, "GET ") )
		return 1;

	if ( IsEqual(strBit, "POST") )
		return 2;

	return 0;
}

int GrabGETData(char *src, char *dst)
{
	char strBit[100];
	int i, sz, f, n;

	sz = 1;
	n = 0;
	f = 0;
	for (i = 0; i < strlen(src); i++)
	{
		if (f == 0)
		{
			GetStringBit(src, strBit, i, 4);
			n = i + 4;
			i += 4;
			if ( IsEqual(strBit, "GET ") )
				f = 1;
			if ( IsEqual(strBit, "POST") )
			{
				n++;
				i++;
				f = 1;
			}
		}
		else
		{
			if (src[i] == ' ')
			{
				GetStringBit(src, dst, n, sz);
				return 1;
			}
			else
			{
				sz++;
			}
		}
	}

	return -1;
}

int AddToOutput(char *data, int sz)
{
	int i;

	for (i = 0; i < sz; i++)
	{
		*(u8*)DataToSend = data[i];
		DataToSend++;
	}

	DataSendSz = DataToSend - 0x00800000;
}

int SendOutput(void)
{
	int sent, totalSent, buffSize;
	u32 sData = 0x00800000;
	if (DataSendSz == 0) { return -1; }

	totalSent = 0;
	while (totalSent != DataSendSz)
	{
		buffSize = DataSendSz - totalSent;
		if (buffSize > 1024) { buffSize = 1024; }

		sent = send(ClientSocket, sData, buffSize, 0);
		totalSent += sent;
		sData += sent;
	}
	return 1;
}

int GrabPageURL(char *getData, char *ret)
{
	int i;

	ret[0] = 0x00;

	for (i = 0; i < strlen(getData); i++)
	{
		if (getData[i] == '?')
			return 0;

		if (getData[i] == '&')
			return 0;

		if (getData[i] == ' ')
			return 0;

		ret[i] = getData[i];
		ret[i + 1] = 0x00;
	}

	return 0;
}

int GrabGETVar(char *getData, char *var, char *ret)
{
	int i, v, n, c, nr;

	char AllVars[1024];
	char gVar[50];
	char gVal[50];

	c = 0;
	n = 0;
	for (i = 0; i < strlen(getData); i++)
	{
		if (getData[i] == '?')
		{
			c++;
			v = 1;
		}

		if (v == 1)
		{
			AllVars[n] = getData[i];

			if (getData[i] == '&')
			{
				c++;
				AllVars[n] = '?';
			}
			AllVars[n + 1] = 0x00;

			n++;
		}
	}

	n = 0;
	v = 0;
	for (i = 0; i < strlen(AllVars); i++)
	{
		if (v == 0)
		{
			if (AllVars[i] == '?')
			{
				n = 0;
				v = 1;
				i++;
			}
		}

		if (v == 1)
		{
			if (AllVars[i] == '=')
			{
				n = 0;
				v = 2;
				i++;
			}
			else
			{
				gVar[n] = AllVars[i];
				gVar[n + 1] = 0x00;
				n++;
			}
		}

		if (v == 2)
		{
			if (AllVars[i] == '?')
			{
				//scr_printf("'%s' --> '%s'\n", gVar, gVal);

				if ( IsEqual(gVar, var) )
				{
					CopyString(gVal, ret, 0);
					return 1;
				}

				n = 0;
				v = 1;
			}
			else
			{
				gVal[n] = AllVars[i];
				gVal[n + 1] = 0x00;
				n++;
			}
		}

	}
	//scr_printf("'%s' --> '%s'\n", gVar, gVal);

	if ( IsEqual(gVar, var) )
	{
		CopyString(gVal, ret, 0);
		return 1;
	}

	return -1;
}

int power(int num, int pow) // Why the fuck does the compiler not use ^ as the power operator... >:o
{
	int i, n;

	if (pow == 0) { return 1; }

	n = num;
	for (i = 1; i < pow; i++)
		n = num * n;

	return n;
}

