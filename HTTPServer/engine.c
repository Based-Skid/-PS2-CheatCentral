#include <kernel.h>

// --------------------------
// CheatsEngine v1.2 ~ Created By: Gtlcpimp

#define CheatsEv012 0x80038000

// --------------------------
// Controller Configuration

#define START 0xFFF7
#define SELECT 0xFFFE
#define CROSS 0xBFFF
#define SQUARE 0x7FFF
#define TRIANGLE 0xEFFF
#define CIRCLE 0xDFFF
#define LEFT 0xFF7F
#define UP 0xFFEF
#define RIGHT 0xFFDF
#define DOWN 0xFFBF
#define R1 0xF7FF
#define R2 0xFDFF
#define R3 0xFFFB
#define L1 0xFBFF
#define L2 0xFEFF
#define L3 0xFFFD
#define NOINPUT 0xFFFF

void CheatEngine()
{
	__asm("addiu $29, $29, 0xF000");
	__asm("sq $1, 0x0000($29)");
	__asm("sq $2, 0x0010($29)");
	__asm("sq $3, 0x0020($29)");
	__asm("sq $4, 0x0030($29)");
	__asm("sq $5, 0x0040($29)");
	__asm("sq $6, 0x0050($29)");
	__asm("sq $7, 0x0060($29)");
	__asm("sq $8, 0x0070($29)");
	__asm("sq $9, 0x0080($29)");
	__asm("sq $10, 0x0090($29)");
	__asm("sq $11, 0x00a0($29)");
	__asm("sq $12, 0x00b0($29)");
	__asm("sq $13, 0x00c0($29)");
	__asm("sq $14, 0x00d0($29)");
	__asm("sq $15, 0x00e0($29)");
	__asm("sq $16, 0x00f0($29)");
	__asm("sq $17, 0x0100($29)");
	__asm("sq $18, 0x0110($29)");
	__asm("sq $19, 0x0120($29)");
	__asm("sq $20, 0x0130($29)");
	__asm("sq $21, 0x0140($29)");
	__asm("sq $22, 0x0150($29)");
	__asm("sq $23, 0x0160($29)");
	__asm("sq $24, 0x0170($29)");
	__asm("sq $25, 0x0180($29)");
	__asm("sq $26, 0x0190($29)");
	__asm("sq $27, 0x01a0($29)");
	__asm("sq $28, 0x01b0($29)");
	__asm("sq $30, 0x01c0($29)");
	__asm("sq $31, 0x01d0($29)");
	__asm("nop");

	__asm("lui $2, 0x8007");
	__asm("sw $31, 0x0000($2)");

	int offset, data, codedata, cmd, codecmd;
	int Skips, Writes, Incs;
	int jokerAddress, jokerBoolean, jokerHalf, jokerData;
	int tmpAddr = 0x80078F00;
	int line = _lw(0x80070000); //0x80079000; //0x01C00000; // 0x80078250;
	_sw(0x00000000, 0x80070000);

	line -= 8;
	line = _lw(line);
	line -= 0x0C000000;
	line = line * 4;
	line -= 0x2010;
	line += 0x80000000;

	data = _lw(line);
	_sw(data, tmpAddr);
	_sb(0x00, (tmpAddr + 3));
	offset = _lw(tmpAddr);
	data = 0x00000000;

	while (offset != 0x00000000) // Engine Loop
	{
		data = _lw(line);
		_sw(data, tmpAddr);
		offset = _lw(tmpAddr);
		offset = offset * 0x10;
		_sw(offset, tmpAddr);
		offset = _lw(tmpAddr);
		offset = offset / 0x10;
		data = 0x00000000;

		cmd = _lb(line + 3);
		cmd = cmd / 0x10;

		if (cmd == 0x0)
		{
			data = _lb(line + 4);
			_sb(data, offset);
		}
		else if (cmd == 0x1)
		{
			data = _lh(line + 4);
			_sh(data, offset);
		}
		else if (cmd == 0x2)
		{
			data = _lw(line + 4);
			_sw(data, offset);
		}
		else if (cmd == 0x3)
		{
			codecmd = _lb(line + 2);
			if (codecmd == 0x00) // 8 Bit Increment
			{
				offset = _lw(line + 4);
				codedata = _lb(line);
				data = _lw(offset);
				data += codedata;
				_sw(data, offset);
			}
			else if (codecmd == 0x10) // 8 Bit Decrement
			{
				offset = _lw(line + 4);
				codedata = _lb(line);
				data = _lw(offset);
				data -= codedata;
				_sw(data, offset);
			}
			else if (codecmd == 0x20) // 16 Bit Increment
			{
				offset = _lw(line + 4);
				codedata = _lh(line);
				data = _lw(offset);
				data += codedata;
				_sw(data, offset);
			}
			else if (codecmd == 0x30) // 16 Bit Decrement
			{
				offset = _lw(line + 4);
				codedata = _lh(line);
				data = _lw(offset);
				data -= codedata;
				_sw(data, offset);
			}
			else if (codecmd == 0x40) // 32 Bit Increment
			{
				offset = _lw(line + 4);
				line += 8;
				codedata = _lw(line);
				data = _lw(offset);
				data += codedata;
				_sw(data, offset);
			}
			else if (codecmd == 0x50) // 32 Bit Decrement
			{
				offset = _lw(line + 4);
				line += 8;
				codedata = _lw(line);
				data = _lw(offset);
				data -= codedata;
				_sw(data, offset);
			}
		}
		else if (cmd == 0x4)
		{
			data = _lh(line + 6);
			codedata = _lh(line + 4);
			codedata = codedata * 4;
			line += 8;
			codecmd = _lw(line);
			while (data != 0x00000000)
			{
				_sw(codecmd, offset);
				data--;
				offset += codedata;
				codecmd += _lw(line + 4);
			}
		}
		else if (cmd == 0x5)
		{
			data = _lw(line + 4);
			codedata = _lw(line + 8);
			for (codecmd = 0x0; codecmd < data; codecmd++)
			{
				_sb(_lb(offset + codecmd), (codedata + codecmd));
			}
			line += 8;
		}
		else if (cmd == 0x6)
		{
			offset = _lw(offset);
			codecmd = _lb(line + 10);
			codedata = _lw(line + 12);
			Writes = _lh(line + 8);
			while (Writes != 0x0000)
			{
				offset = _lw(offset);
				Writes--;
			}

			offset += codedata;
			if (codecmd == 0x00)
			{
				data = _lb(line + 4);
				_sb(data, offset);
			}
			else if (codecmd == 0x01)
			{
				data = _lh(line + 4);
				_sh(data, offset);
			}
			else if (codecmd == 0x02)
			{
				data = _lw(line + 4);
				_sw(data, offset);
			}

			line += 8;
		}
		else if (cmd == 0xC)
		{
			data = _lw(line + 4);
			codedata = _lw(offset);
			if (data != codedata)
			{
				goto kill;
			}
		}
		else if (cmd == 0xD)
		{
			jokerAddress = offset;
			jokerHalf = _lh(line + 4);
			jokerBoolean = _lb(line + 6);
			jokerData = _lh(jokerAddress);

			line += 8;

			data = _lw(line);
			_sw(data, tmpAddr);
			offset = _lw(tmpAddr);
			offset = offset * 0x10;
			_sw(offset, tmpAddr);
			offset = _lw(tmpAddr);
			offset = offset / 0x10;
			data = 0x00000000;

			if (((jokerBoolean == 0x0) && (jokerHalf == jokerData)) || ((jokerBoolean == 0x10) && (jokerHalf != jokerData)))
			{
				data = _lw(line);
				_sw(data, tmpAddr);
				offset = _lw(tmpAddr);
				offset = offset * 0x10;
				_sw(offset, tmpAddr);
				offset = _lw(tmpAddr);
				offset = offset / 0x10;
				data = 0x00000000;

				while (offset != 0x00000000)
				{
					data = _lw(line);
					_sw(data, tmpAddr);
					offset = _lw(tmpAddr);
					offset = offset * 0x10;
					_sw(offset, tmpAddr);
					offset = _lw(tmpAddr);
					offset = offset / 0x10;
					data = 0x00000000;

					cmd = _lb(line + 3);
					cmd = cmd / 0x10;

					if (cmd == 0x0)
					{
						data = _lb(line + 4);
						_sb(data, offset);
					}
					else if (cmd == 0x1)
					{
						data = _lh(line + 4);
						_sh(data, offset);
					}
					else if (cmd == 0x2)
					{
						data = _lw(line + 4);
						_sw(data, offset);
					}
					else if (cmd == 0x3)
					{
						codecmd = _lb(line + 2);
						if (codecmd == 0x00) // 8 Bit Increment
						{
							offset = _lw(line + 4);
							codedata = _lb(line);
							data = _lw(offset);
							data += codedata;
							_sw(data, offset);
						}
						else if (codecmd == 0x10) // 8 Bit Decrement
						{
							offset = _lw(line + 4);
							codedata = _lb(line);
							data = _lw(offset);
							data -= codedata;
							_sw(data, offset);
						}
						else if (codecmd == 0x20) // 16 Bit Increment
						{
							offset = _lw(line + 4);
							codedata = _lh(line);
							data = _lw(offset);
							data += codedata;
							_sw(data, offset);
						}
						else if (codecmd == 0x30) // 16 Bit Decrement
						{
							offset = _lw(line + 4);
							codedata = _lh(line);
							data = _lw(offset);
							data -= codedata;
							_sw(data, offset);
						}
						else if (codecmd == 0x40) // 32 Bit Increment
						{
							offset = _lw(line + 4);
							line += 8;
							codedata = _lw(line);
							data = _lw(offset);
							data += codedata;
							_sw(data, offset);
						}
						else if (codecmd == 0x50) // 32 Bit Decrement
						{
							offset = _lw(line + 4);
							line += 8;
							codedata = _lw(line);
							data = _lw(offset);
							data -= codedata;
							_sw(data, offset);
						}
					}
					else if (cmd == 0x4)
					{
						data = _lh(line + 6);
						codedata = _lh(line + 4);
						codedata = codedata * 4;
						line += 8;
						codecmd = _lw(line);
						while (data != 0x00000000)
						{
							_sw(codecmd, offset);
							data--;
							offset += codedata;
							codecmd += _lw(line + 4);
						}
					}
					else if (cmd == 0x5)
					{
						data = _lw(line + 4);
						codedata = _lw(line + 8);
						for (codecmd = 0x0; codecmd < data; codecmd++)
						{
							_sb(_lb(offset + codecmd), (codedata + codecmd));
						}
						line += 8;
					}
					else if (cmd == 0x6)
					{
						offset = _lw(offset);
						codecmd = _lb(line + 10);
						codedata = _lw(line + 12);
						Writes = _lh(line + 8);
						while (Writes != 0x0000)
						{
							offset = _lw(offset);
							Writes--;
						}
						offset += codedata;
						if (codecmd == 0x00)
						{
							data = _lb(line + 4);
							_sb(data, offset);
						}
						else if (codecmd == 0x01)
						{
							data = _lh(line + 4);
							_sh(data, offset);
						}
						else if (codecmd == 0x02)
						{
							data = _lw(line + 4);
							_sw(data, offset);
						}
						line += 8;
					}
					else if (cmd == 0xC)
					{
						data = _lw(line + 4);
						codedata = _lw(offset);
						if (data != codedata)
						{
							goto kill;
						}
					}
					else if (cmd == 0xD)
					{
						line -= 8;
						goto NextJoker; //return line;
					}

					line += 8;
					data = _lw(line);
					_sw(data, tmpAddr);
					offset = _lw(tmpAddr);
					offset = offset * 0x10;
					_sw(offset, tmpAddr);
					offset = _lw(tmpAddr);
					offset = offset / 0x10;
					data = 0x00000000;
				}
			}
			else
			{
				data = _lw(line);
				_sw(data, tmpAddr);
				offset = _lw(tmpAddr);
				offset = offset * 0x10;
				_sw(offset, tmpAddr);
				offset = _lw(tmpAddr);
				offset = offset / 0x10;
				data = 0x00000000;

				while (offset != 0x00000000)
				{
					data = _lw(line);
					_sw(data, tmpAddr);
					offset = _lw(tmpAddr);
					offset = offset * 0x10;
					_sw(offset, tmpAddr);
					offset = _lw(tmpAddr);
					offset = offset / 0x10;
					data = 0x00000000;

					cmd = _lb(line + 3);
					cmd = cmd / 0x10;

					if (cmd == 0xD)
					{
						line -= 8;
						goto NextJoker; //return line;
					}

					line += 8;
					data = _lw(line);
					_sw(data, tmpAddr);
					offset = _lw(tmpAddr);
					offset = offset * 0x10;
					_sw(offset, tmpAddr);
					offset = _lw(tmpAddr);
					offset = offset / 0x10;
					data = 0x00000000;
				}
			}
		}

		line += 8;
		data = _lw(line);
		_sw(data, tmpAddr);
		offset = _lw(tmpAddr);
		offset = offset * 0x10;
		_sw(offset, tmpAddr);
		offset = _lw(tmpAddr);
		offset = offset / 0x10;
		data = 0x00000000;

		NextJoker:
	}

	kill:

	__asm("nop");
	__asm("lq $1, 0x0000($29)");
	__asm("lq $2, 0x0010($29)");
	__asm("lq $3, 0x0020($29)");
	__asm("lq $4, 0x0030($29)");
	__asm("lq $5, 0x0040($29)");
	__asm("lq $6, 0x0050($29)");
	__asm("lq $7, 0x0060($29)");
	__asm("lq $8, 0x0070($29)");
	__asm("lq $9, 0x0080($29)");
	__asm("lq $10, 0x0090($29)");
	__asm("lq $11, 0x00a0($29)");
	__asm("lq $12, 0x00b0($29)");
	__asm("lq $13, 0x00c0($29)");
	__asm("lq $14, 0x00d0($29)");
	__asm("lq $15, 0x00e0($29)");
	__asm("lq $16, 0x00f0($29)");
	__asm("lq $17, 0x0100($29)");
	__asm("lq $18, 0x0110($29)");
	__asm("lq $19, 0x0120($29)");
	__asm("lq $20, 0x0130($29)");
	__asm("lq $21, 0x0140($29)");
	__asm("lq $22, 0x0150($29)");
	__asm("lq $23, 0x0160($29)");
	__asm("lq $24, 0x0170($29)");
	__asm("lq $25, 0x0180($29)");
	__asm("lq $26, 0x0190($29)");
	__asm("lq $27, 0x01a0($29)");
	__asm("lq $28, 0x01b0($29)");
	__asm("lq $30, 0x01c0($29)");
	__asm("lq $31, 0x01d0($29)");
	__asm("addiu $29, $29, 0x1000");
	__asm("nop");
	__asm("jr $26");
	__asm("nop");
}

void EndEngine()
{
	printf("This is bullshit! ");
}

void MoveEngine(int StoreAddr)
{

	int putoff = StoreAddr;
	int i, data, jal, size, ok;

	jal = putoff;

	ok = 0;

	DI;
	ee_kmode_enter();
	for (i = (void*)CheatEngine; i < ((void*)EndEngine + 4); i += 4)
	{

		data = _lw(i);

		if (data == 0x27bdf000)
			ok = 1;

		if (ok == 1)
		{
			_sw(data, putoff);
			putoff += 4;
		}
	}
	ee_kmode_exit();
	EI;

	jal = (StoreAddr - 0x80000000);
	jal = jal / 4;
	jal += 0x0C000000;

	ee_kmode_enter();
	_sw(jal, 0x800002fc);
	ee_kmode_exit();
}

void MoveCodeList(int StoreAddr, int ListAddr)
{
	int i;

	ee_kmode_enter();
	for (i = 0; i < 8192; i++)
	{
		_sb( _lb(ListAddr + i), ((StoreAddr - 0x2010) + i));
	}
	ee_kmode_exit();
}
