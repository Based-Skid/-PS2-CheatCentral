
char ps2IP[20] = "192.168.0.55";
char ps2NM[20] = "255.255.255.0";
char ps2GW[20] = "192.168.0.1";

//================================================================================
// Server Command Setup

#define SEND_CODES 0xC0000001
#define START_GAME 0xC0000002
#define REBOOT_PS2 0xC0000003
#define CLOSE_SERV 0xC0000004
#define UPLOAD     0xC0000005
#define DOWNLOAD   0xC0000006
#define DIR        0xC0000007
#define DELETE     0xC0000008
#define EXEC       0xC0000009
#define UPLOADEXEC 0xC000000A
#define RMKDIR     0xC000000B
#define DUMPBIOS   0xC000000C
#define LOADBACKUP 0xC000000D
#define LOADBRWSR  0xC000000E
#define PASTECOPY  0xC000000F
#define PASTECUT   0xC0000010

//================================================================================
// 1.3 Device Settings
/*
typdef struct {

	char clbname[20];
	char clbpath[20];
	u8 clbdata[32768];
} mclb;

u8 *mcAlistindex;
u8 *mcBlistindex;

struct mclb mcAclb[10];
struct mclb mcBclb[10];
*/
//================================================================================

char recData[8000];
char senData[8000];

int isHeader = 1;
u32 TotalSize = 1;
u32 Command = 0xC0000000;
int BytesReceived = 0;
int ServerSocket;
int ClientSocket;
int BrCastSocket;
int BrCGetSocket;
struct sockaddr_in ServerAddr;
struct sockaddr_in ClientAddr;
struct sockaddr_in BCSockAddr;
struct sockaddr_in BCGetSAddr;
fd_set active_rd_set;
fd_set rd_set;

u32 wrAddr = 0x00800000;
int listSize = 0;

//int MyMCFreeSpace = 0;
//int MyMCType = 0;
//int MyMCFormat = 0;
//char MyDIR[100] = "";
mcTable DIRListings[100] __attribute__((aligned(64)));

char ClipBoard[100];

void EmptyData()
{
	int i;
	for (i = 0; i < 8000; i += 4)
	{
		*(u8*)&recData[i + 0] = 0x00;
		*(u8*)&recData[i + 1] = 0x00;
		*(u8*)&recData[i + 2] = 0x00;
		*(u8*)&recData[i + 3] = 0x00;

		*(u8*)&senData[i + 0] = 0x00;
		*(u8*)&senData[i + 1] = 0x00;
		*(u8*)&senData[i + 2] = 0x00;
		*(u8*)&senData[i + 3] = 0x00;
	}
}

int ProcessClient()
{
	int recSize, sentSize, totalSent, sendingBuffer;
	int retData, i, ret, sendingSize, n, mcSlot;
	char *args[1];
	char elf[1024];
	u8 *flLoad;

	EmptyData();
	retData = 0;

	recSize = recv(ClientSocket, recData, 8000, 0);
	if ( recSize <= 0 )
	{
		return -1;
	}
	else
	{
		if (isHeader == 1)
		{
			Command = *(u32*)&recData;
			BytesReceived = recSize - 8;
			isHeader = 0;



			if (Command == SEND_CODES) // Receive Codes From PC
			{

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Receiving Code List...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();
				wrAddr = 0x007FFFFC;
				for (i = 4; i < recSize; i++)
				{
					*(u8*)wrAddr = *(u8*)&recData[i];
					wrAddr++;
					
				}
				TotalSize = *(u32*)0x007FFFFC;
				listSize = TotalSize;
			}
			else if (Command == LOADBRWSR)  // Boot Browser
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Launching Browser...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x00800000;
				disconnect(ClientSocket);
				ResetIOP();
				//MoveCodeList(CheatsEv012, wrAddr);
				//MoveEngine(CheatsEv012);
				//InstallCodes(0x00800000, listSize);
				//InstallEngine();
				HookEngine();
				LoadExecPS2("", 0, NULL);

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Browser Failed To Launch.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();
			}
			else if (Command == START_GAME) // Start Game
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Reading Game Disc...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				ReadCNF(elf);
				if (elf[0] != '!')
				{
					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("Launching Game Disc...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();

					wrAddr = 0x00800000;
					disconnect(ClientSocket);
					ResetIOP();
					//MoveCodeList(CheatsEv012, wrAddr);
					//MoveEngine(CheatsEv012);
					//InstallCodes(0x00800000, listSize);
					//InstallEngine();
					HookEngine();
					LoadExecPS2(elf, 0, NULL);
				}

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Game Disc Failed To Launch.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

			}
			else if (Command == REBOOT_PS2) // Reset PS2
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Rebooting PS2...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				pause(2);

				disconnect(ClientSocket);
				ResetIOP();
				LoadExecPS2("", 0, NULL);
			}
			else if (Command == CLOSE_SERV) // Exit Server
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Exiting Remote PS2...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();


				// Need Exit Code


				return -15;
			}
			else if (Command == UPLOAD) // Save file to Memory Card
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Receiving File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x007FFFFC;
				for (i = 4; i < recSize; i++)
				{
					*(u8*)wrAddr = *(u8*)&recData[i];
					wrAddr++;
					
				}
				TotalSize = *(u32*)0x007FFFFC;

			}
			else if (Command == PASTECOPY) // Copy and Paste
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Copying File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x007FFFFC;
				for (i = 4; i < recSize; i++)
				{
					*(u8*)wrAddr = *(u8*)&recData[i];
					wrAddr++;
					
				}
				TotalSize = *(u32*)0x007FFFFC;

				i = 0x00800000;
				while ( *(u8*)i != 0x00) { i++; }
				i++;

				int f = fioOpen(0x00800000, O_RDONLY);
				if (f < 0)
				{
					*(u32*)0x00800000 = PASTECOPY;
					*(u32*)0x00800004 = 4;
					*(u32*)0x00800008 = 0;
					sentSize = send(ClientSocket, 0x00800000, 12, 0);

					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("File doesn't exist.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();
				}
				else
				{
					ret = fioLseek(f, 0, 2);
					fioLseek(f, 0, 0);

					fioRead(f, 0x00100000, ret);
					fioClose(f);

					f = fioOpen(i, O_WRONLY | O_CREAT);
					if (f < 0)
					{
						*(u32*)0x00800000 = PASTECOPY;
						*(u32*)0x00800004 = 4;
						*(u32*)0x00800008 = 0;
						sentSize = send(ClientSocket, 0x00800000, 12, 0);

						DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
						DrawString("Cannot save to path!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
						graph_wait_vsync();
					}
					else
					{

						fioWrite(f, 0x00100000, ret);
						fioClose(f);


						*(u32*)0x00800000 = PASTECOPY;
						*(u32*)0x00800004 = 4;
						*(u32*)0x00800008 = 1;
						sentSize = send(ClientSocket, 0x00800000, 12, 0);

						DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
						DrawString("File Copied!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
						graph_wait_vsync();
					}
				}

				wrAddr = 0x007FFFFC;
				for (i = 0; i < 100; i++)
					*(u8*)(wrAddr + i) = 0x00;
				isHeader = 1;
			}
			else if (Command == PASTECUT) // Cut and Paste
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Copying File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x007FFFFC;
				for (i = 4; i < recSize; i++)
				{
					*(u8*)wrAddr = *(u8*)&recData[i];
					wrAddr++;
					
				}
				TotalSize = *(u32*)0x007FFFFC;

				i = 0;
				while ( *(u8*)(0x00800000 + i) != 0x00 )
				{
					*(u8*)(0x007FFF00 + i) = *(u8*)(0x00800000 + i);
					i++;
					*(u8*)(0x007FFF00 + i) = 0x00;
				}

				i = 0x00800000;
				while ( *(u8*)i != 0x00) { i++; }
				i++;

				int fA = fioOpen(0x00800000, O_RDONLY);
				if (fA < 0)
				{
					*(u32*)0x00800000 = PASTECOPY;
					*(u32*)0x00800004 = 4;
					*(u32*)0x00800008 = 0;
					sentSize = send(ClientSocket, 0x00800000, 12, 0);

					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("File doesn't exist.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();
				}
				else
				{
					int fB = fioOpen(i, O_WRONLY | O_CREAT);
					if (fB < 0)
					{
						fioClose(fA);

						*(u32*)0x00800000 = PASTECOPY;
						*(u32*)0x00800004 = 4;
						*(u32*)0x00800008 = 0;
						sentSize = send(ClientSocket, 0x00800000, 12, 0);

						DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
						DrawString("Cannot save to path!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
						graph_wait_vsync();
					}
					else
					{
						i = fioLseek(fA, 0, 2);
						fioLseek(fA, 0, 0);

						fioRead(fA, 0x00800000, i);
						fioClose(fA);

						fioWrite(fB, 0x00800000, i);
						fioClose(fB);

						DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
						DrawString("Deleting File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
						graph_wait_vsync();

						if ( *(u8*)0x007FFF02 == 0x30 )
							mcDelete(0, 0, 0x007FFF04);
						else if ( *(u8*)0x007FFF02 == 0x31 )
							mcDelete(1, 0, 0x007FFF04);

						mcSync(0, NULL, &i);

						if (i == 0)
						{
							*(u32*)0x00800000 = PASTECOPY;
							*(u32*)0x00800004 = 4;
							*(u32*)0x00800008 = 1;
							sentSize = send(ClientSocket, 0x00800000, 12, 0);

							DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
							DrawString("Cut + Pasted Successfully!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
							graph_wait_vsync();
						}
						else
						{
							*(u32*)0x00800000 = PASTECOPY;
							*(u32*)0x00800004 = 4;
							*(u32*)0x00800008 = 2;
							sentSize = send(ClientSocket, 0x00800000, 12, 0);

							DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
							DrawString("Delete Failed!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
							graph_wait_vsync();
						}
					}
				}

				wrAddr = 0x007FFFFC;
				for (i = 0; i < 100; i++)
					*(u8*)(wrAddr + i) = 0x00;
				isHeader = 1;
			}
			else if (Command == EXEC) // Execute saved ELF
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Checking File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x007FFFFC;
				for (i = 4; i < recSize; i++)
				{
					*(u8*)wrAddr = *(u8*)&recData[i];
					wrAddr++;
					
				}
				TotalSize = *(u32*)0x007FFFFC;

				int f = fioOpen(0x00800000, O_RDONLY);
				if (f < 0)
				{
					*(u32*)0x00800000 = EXEC;
					*(u32*)0x00800004 = 4;
					*(u32*)0x00800008 = 0;
					sentSize = send(ClientSocket, 0x00800000, 12, 0);

					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("File doesn't exist.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();

				}
				else
				{
					fioRead(f, 0x00801000, 4);
					if ( *(u32*)0x00801000 == 0x464c457f )
					{
						DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
						DrawString("Reading ELF...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
						graph_wait_vsync();

						fioLseek(f, 0, 0);
						i = fioLseek(f, 0, 2);
						fioLseek(f, 0, 0);

						fioRead(f, 0x00800000, i);
						fioClose(f);

						DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
						DrawString("Loading ELF...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
						graph_wait_vsync();

						ret = ExtractELF(0x00800000);

						DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
						DrawString("Launching ELF...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
						graph_wait_vsync();

						args[0] = "remoteps2_processed.elf";
						ResetIOP();
						FlushCache(0);
						FlushCache(2);
						ExecPS2(ret, 0, 1, args);

					}
					else
					{
						fioClose(f);

						*(u32*)0x00800000 = EXEC;
						*(u32*)0x00800004 = 4;
						*(u32*)0x00800008 = 1;
						sentSize = send(ClientSocket, 0x00800000, 12, 0);

						DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
						DrawString("File isn't an ELF!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
						graph_wait_vsync();
					}
				}

				wrAddr = 0x007FFFFC;
				for (i = 0; i < 100; i++)
					*(u8*)(wrAddr + i) = 0x00;
				isHeader = 1;
			}
			else if (Command == DELETE) // Delete file from Memory Card
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Removing File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x007FFFFC;
				for (i = 4; i < recSize; i++)
				{
					*(u8*)wrAddr = *(u8*)&recData[i];
					wrAddr++;
					
				}
				TotalSize = *(u32*)0x007FFFFC;

				//i = remove(0x00800000);

				if ( *(u8*)0x00800002 == 0x30)
					mcDelete(0, 0, 0x00800004);
				else if ( *(u8*)0x00800002 == 0x31)
					mcDelete(1, 0, 0x00800004);

				mcSync(0, NULL, &i);

				if (i == 0)
				{
					*(u32*)0x00800000 = DELETE;
					*(u32*)0x00800004 = 4;
					*(u32*)0x00800008 = 1;
					sentSize = send(ClientSocket, 0x00800000, 12, 0);
					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("File Removed!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();
				}
				else
				{
					*(u32*)0x00800000 = DELETE;
					*(u32*)0x00800004 = 4;
					*(u32*)0x00800008 = 0;
					sentSize = send(ClientSocket, 0x00800000, 12, 0);
					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("Failed to Remove File!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();
				}

				wrAddr = 0x007FFFFC;
				for (i = 0; i < 100; i++)
					*(u8*)(wrAddr + i) = 0x00;
				isHeader = 1;
			}
			else if (Command == DOWNLOAD) // Send file
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Checking File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x007FFFFC;
				for (i = 4; i < recSize; i++)
				{
					*(u8*)wrAddr = *(u8*)&recData[i];
					wrAddr++;
					
				}
				TotalSize = *(u32*)0x007FFFFC;

				int f = fioOpen(0x00800000, O_RDONLY);
				if (f < 0)
				{
					*(u32*)0x00800000 = DOWNLOAD;
					*(u32*)0x00800004 = 4;
					*(u32*)0x00800008 = 0xFA1FA1FA;
					sentSize = send(ClientSocket, 0x00800000, 12, 0);

					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("File doesn't exist!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();
				}
				else
				{
					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("Loading File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();

					i = fioLseek(f, 0, 2);
					fioLseek(f, 0, 0);
					fioRead(f, 0x00800008, i);
					fioClose(f);

					*(u32*)0x00800000 = DOWNLOAD;
					*(u32*)0x00800004 = i;
					TotalSize = i;

					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("Sending File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();


					n = 0;
					ret = 0x00800000;
					TotalSize += 8;
					while (n < TotalSize)
					{
						totalSent = 512;
						if ( (TotalSize - n) < totalSent) { totalSent = (TotalSize - n); }

						sentSize = send(ClientSocket, ret, totalSent, 0);
						n += sentSize;
						ret += sentSize;
					}

					DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("File Sent!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();
				}

				wrAddr = 0x007FFFFC;
				for (i = 0; i < 100; i++)
					*(u8*)(wrAddr + i) = 0x00;
				isHeader = 1;
			}
			else if (Command == DIR) // Get list index
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Retrieving Listings...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x007FFFFC;
				for (i = 4; i < recSize; i++)
				{
					*(u8*)wrAddr = *(u8*)&recData[i];
					wrAddr++;
					
				}
				TotalSize = *(u32*)0x007FFFFC;

				for (i = 0; i < 90; i++)
				{
					DIRListings[i].name[0] = 0x00;
				}

				i = 0;
				while ( *(u8*)(0x00800000 + i) != 0x00 ) { i++; }
				*(u8*)(0x00800000 + i) = '*';
				*(u8*)(0x00800001 + i) = 0x00;

				//DIRListings[100] 
				if ( *(u8*)0x00800002 == 0x30)
					mcGetDir(0, 0, 0x00800004, 0, 90, DIRListings);
				else if ( *(u8*)0x00800002 == 0x31)
					mcGetDir(1, 0, 0x00800004, 0, 90, DIRListings);

				mcSync(0, NULL, &ret);

				wrAddr = 0x00800000;
				*(u32*)wrAddr = DIR; wrAddr += 8;

				TotalSize = 0;
				for (i = 0; i < ret; i++)
				{
					for (n = 0; n < strlen(DIRListings[i].name); n++)
					{
						*(u8*)wrAddr = DIRListings[i].name[n];
						wrAddr++;
						TotalSize++;
					}
					*(u8*)wrAddr = 0x00;
					wrAddr++;
					TotalSize++;
				}
				*(u32*)0x00800004 = TotalSize;

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Sending listings...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				n = 0;
				ret = 0x00800000;
				TotalSize += 8;
				while (n < TotalSize)
				{
					totalSent = 512;
					if ( (TotalSize - n) < totalSent) { totalSent = (TotalSize - n); }

					sentSize = send(ClientSocket, ret, totalSent, 0);
					n += sentSize;
					ret += sentSize;
				}

				wrAddr = 0x007FFFFC;
				for (i = 0; i < 100; i++)
					*(u8*)(wrAddr + i) = 0x00;
				isHeader = 1;

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Listings Sent.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();
			}
			else if (Command == UPLOADEXEC) // Receive ELF and launch it
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Receiving ELF...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();
				wrAddr = 0x007FFFFC;
				for (i = 4; i < recSize; i++)
				{
					*(u8*)wrAddr = *(u8*)&recData[i];
					wrAddr++;
					
				}
				TotalSize = *(u32*)0x007FFFFC;
			}
			else if (Command == DUMPBIOS) // Dump PS2 BIOS
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Dumping ROM 0...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x00800000;
				*(u32*)wrAddr = DUMPBIOS; wrAddr += 4;
				*(u32*)wrAddr = 0x0004FFFC; wrAddr += 4;
				//sentSize = send(ClientSocket, 0x00800000, 8, 0);

				for (i = 0x80030000; i < 0x8007FFFC; i += 4) //for (i = 0x1FC00000; i < 0x20000000; i += 4)
				{
					ee_kmode_enter();
					*(u32*)wrAddr = *(u32*)i;
					ee_kmode_exit();
					wrAddr += 4;
				}

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Sending ROM 0...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				n = 0;
				i = 0x00800000;
				ret = 0x0004FFFC; //0x00400008;
				while (n < ret) //0x00400008)
				{
					totalSent = 512;
					if ( (ret - n) < totalSent) { totalSent = (ret - n); }

					sentSize = send(ClientSocket, i, totalSent, 0);
					n += sentSize;
					i += sentSize;
				}

				for (i = 0; i < 50; i++)
					graph_wait_vsync();

				isHeader = 1;
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("BIOS Dump Complete.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();
			}
			else if (Command == LOADBACKUP) // Stop Disc for Backup Loading
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Waiting for disc to be ready...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				disconnect(ClientSocket);
				ResetIOP();

				loadbasemodules();

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Reading Disc...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				fioOpen("qwerty:r",1); // Wait disc ready
				ReadCNF(elf);

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Stopping Disc...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				ResetCNF();
				ResetIOP();
				SifInitRpc(0);
				loadbasemodules();
				fioOpen("qwerty:c",1); // Put LBA 0
				fioOpen("qwerty:s",1); // Stop Disk
				padInit(0);
				padPortOpen(0, 0, padBuf);
				initializePad(0, 0);

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Swap your disc and press START to play.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				while (1)
				{
					if (padRead(0, 0, &buttons));
					{
						paddata = 0xffff ^ buttons.btns;
						new_pad = paddata & ~old_pad;
						old_pad = paddata;

						if (new_pad & PAD_START)
						{
							DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
							DrawString("Reading Backup Disc...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
							graph_wait_vsync();

							fioOpen("qwerty:r",1); // Wait disc ready
							ReadCNF(elf);
							if (elf[0] != '!')
							{
								DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
								DrawString("Launching Backup Disc...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
								graph_wait_vsync();

								wrAddr = 0x00800000;
								//MoveCodeList(CheatsEv012, wrAddr);
								//MoveEngine(CheatsEv012);
								//InstallCodes(0x00800000, listSize);
								//InstallEngine();
								HookEngine();
								LoadExecPS2(elf, 0, NULL);
							}

							DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
							DrawString("The disc could not be read. Swap and press START.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
							graph_wait_vsync();
						}
					}
				}
			}
			else
			{
				CopyString("Command not supported.", senData, 0); retData = 1;
				isHeader = 1;
			}
		}
		else
		{
			for (i = 0; i < recSize; i++)
			{
				*(u8*)wrAddr = *(u8*)&recData[i];
				wrAddr++;
			}
			BytesReceived += recSize;

		}

		if (BytesReceived >= TotalSize)
		{
			isHeader = 1;

			if (Command == SEND_CODES)
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Code List Received!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();
				CopyString("Codes Received.", senData, 0); retData = 1;
				SetupEngine(0x00800000, TotalSize);
				TotalSize = 1;
			}
			else if (Command == UPLOADEXEC)
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Extracting ELF...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				//InstallLoader();
				//*(u32*)0x00080000 = 0x00800000;
				//__asm("jal 0x00080400");
				//__asm("nop\nnop");

				disconnect(ClientSocket);
				ResetIOP();

				ret = ExtractELF(0x00800000);

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Launching ELF...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				args[0] = "remoteps2_processed.elf";
				ResetIOP();
				FlushCache(0);
				FlushCache(2);
				ExecPS2(ret, 0, 1, args);

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Failed to execute the ELF. Please re-connect.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();
			}
			else if (Command == UPLOAD) // Save file to Memory Card
			{
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Saving File...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				wrAddr = 0x00800000;
				i = 0;

				while ( *(u8*)wrAddr != 0x00 ) { wrAddr++; i++; }
				wrAddr++;
				i++;

				int f = fioOpen(0x00800000, O_WRONLY | O_CREAT);
				if (f > 0)
				{
					fioWrite(f, wrAddr, (TotalSize - i) );
					fioClose(f);


					*(u32*)0x00800000 = UPLOAD;
					*(u32*)0x00800004 = 4;
					*(u32*)0x00800008 = 1;
					sentSize = send(ClientSocket, 0x00800000, 12, 0);

					DrawPhotoSpot(background, 0, 220, 640, 40, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("Saved Successfully!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();
				}
				else
				{

					*(u32*)0x00800000 = UPLOAD;
					*(u32*)0x00800004 = 4;
					*(u32*)0x00800008 = 0;
					sentSize = send(ClientSocket, 0x00800000, 12, 0);

					DrawPhotoSpot(background, 0, 220, 640, 40, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
					DrawString("Save Failed!", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
					graph_wait_vsync();

				}


			}
			BytesReceived = 0;
		}

		if (retData == 1) { sentSize = send(ClientSocket, senData, strlen(senData), 0); }
	}

	return 1;
}

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

	BrCastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (BrCastSocket < 0) { return -1; }
	memset( &BCSockAddr, 0, sizeof(BCSockAddr) );

	BCSockAddr.sin_family = AF_INET;
	BCSockAddr.sin_addr.s_addr = htonl(-1);
	BCSockAddr.sin_port = htons(101);

	ret = connect( BrCastSocket, (struct sockaddr *)&BCSockAddr, sizeof(BCSockAddr) );
	if (ret < 0) { return -1; }

	BrCGetSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (BrCGetSocket < 0) { return -1; }
	memset( &BCGetSAddr, 0, sizeof(BCGetSAddr) );

	BCGetSAddr.sin_family = AF_INET;
	BCGetSAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	BCGetSAddr.sin_port = htons(101);

	ret = bind( BrCGetSocket, (struct sockaddr *)&BCGetSAddr, sizeof(BCGetSAddr) );
	if (ret < 0) { return -1; }

	FD_ZERO(&active_rd_set);
	FD_SET(ServerSocket, &active_rd_set);
	FD_SET(BrCastSocket, &active_rd_set);

	return 1;
}

int ServerLoop(void)
{
	int i, ret, ClientLen, delay, sz, fc;
	int inSession = 0;
	int inSessionB = 0;
	char senData[100];


	DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
	DrawString("Waiting For Connection...", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
	graph_wait_vsync();

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
				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Connected to PC.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();
				FD_SET(ClientSocket, &active_rd_set);
				inSession = 1;
			}
		}
		else
		{
			i = ProcessClient();
			if (i == -1)
			{

				DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
				DrawString("Not Connected.", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
				graph_wait_vsync();

				inSession = 0;
				disconnect(ClientSocket);
				return -1;
			}
			else if (i == -15)
			{
				return -15;
			}
		}

	}

	return 0;
}

int BCHandle(void)
{
	int i, ret, ClientLen, delay, sz, fc;
	int inSession = 0;
	int inSessionB = 0;
	char sData[100];
	char rData[100];
	char tmp[20];
	u32 ip = 0;

	DrawPhotoSpot(background, 0, 220, 640, 20, 0, 220, 0, 640, 480, 0, 0, 0, -1, -1, 0, 0);
	DrawString("Not Connected", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
	graph_wait_vsync();

	while(1)
	{
		sz = recv(BrCGetSocket, rData, 100, 0);
		if (sz > 0)
		{
			if ((rData[3] == 'P') && (rData[4] == 'C'))
			{
				if ( IsEqual((rData + 7), "Connect") )
				{
					CopyString("CL-PS2IP: ", sData, 0);
					CopyString(ps2IP, sData, 10);
					sz = send(BrCastSocket, sData, strlen(sData), 0);
					fc = ServerLoop();
					if (fc == -15) { return -15; }
				}
			}
			else
			{
				// Debug Relay
				CopyString("Return: ", sData, 0);
				CopyString(rData, sData, 8);
				sz = send(BrCastSocket, sData, strlen(sData), 0);
			}
		}
	}

	return 0;
}

int EnterServer(void)
{
	int r;

	DrawString("Not Connected", 160, 110, 0, MicrosoftSansSerif, 1, 0xDC, 0xDC, 0xDC, 640, 0x00, 0x00, 0x00, 1, 1);
	graph_wait_vsync();

	r = ServerInit(100);
	if (r > 0) { BCHandle(); }

	return 0;
}

