
typedef struct {
	u8	ident[16];
	u16	type;
	u16	machine;
	u32	version;
	u32	entry;
	u32	phoff;
	u32	shoff;
	u32	flags;
	u16	ehsize;
	u16	phentsize;
	u16	phnum;
	u16	shentsize;
	u16	shnum;
	u16	shstrndx;
} elf_header_t;

typedef struct {
	u32	type;
	u32	offset;
	void	*vaddr;
	u32	paddr;
	u32	filesz;
	u32	memsz;
	u32	flags;
	u32	align;
} elf_pheader_t;

elf_header_t elfh;


void LoadELF(void)
{
	u32 elfdata = *(u32*)0x00080000;

	int i, n;

	elf_header_t *eh = 0x00080010;
	elf_pheader_t *eph;
	void *pdata;

	eh = (elf_header_t*)elfdata;
	eph = (elf_pheader_t*)(elfdata + eh->phoff);

	for (i = 0; i < eh->phnum; i++)
	{
		if (eph[i].type != 1)
			continue;
	
		pdata = (void *)(elfdata + eph[i].offset);
		//memcpy2(eph[i].vaddr, pdata, eph[i].filesz);

		for (n = 0; n < eph[i].filesz; n++)
		{
			*(u8*)(eph[i].vaddr + n) = *(u8*)(pdata + n);
		}

	
		if (eph[i].memsz > eph[i].filesz)
		{
			//memset2(eph[i].vaddr + eph[i].filesz, 0, eph[i].memsz - eph[i].filesz);

			for (n = 0; n < (eph[i].memsz - eph[i].filesz); n++)
			{
				*(u8*)((eph[i].vaddr + eph[i].filesz) + n) = 0x00;
			}
		}
	}

	//FlushCache(0);
	//FlushCache(2);
/*
	__asm("addiu $4, $0, 0x0000");
	__asm("addiu $2, $0, 0x0064");
	__asm("syscall (00000)");
	__asm("nop");

	__asm("addiu $4, $0, 0x0002");
	__asm("addiu $2, $0, 0x0064");
	__asm("syscall (00000)");
	__asm("nop");
*/
	*(u32*)0x00080004 = eh->entry;
	//ExecPS2(eh->entry, 0, 1, NULL);

	__asm("lui $25, 0x0008");
	__asm("lw $25, 0x0004($25)");
	__asm("jr $25");
	__asm("nop\nnop");
}

void InstallLoader(void)
{
	u32 addr = 0x00080400;
	u32 l = (void*)LoadELF;

	while (l < (void*)InstallLoader)
	{
		*(u32*)addr = *(u32*)l;
		l += 4;
		addr += 4;
	}
}

int ExtractELF(u32 elfdata)
{
	int i, n;

	elf_header_t *eh = &elfh;
	elf_pheader_t *eph;
	void *pdata;

	eh = (elf_header_t*)elfdata;
	eph = (elf_pheader_t*)(elfdata + eh->phoff);

	for (i = 0; i < eh->phnum; i++)
	{
		if (eph[i].type != 1)
			continue;
	
		pdata = (void *)(elfdata + eph[i].offset);
		//memcpy2(eph[i].vaddr, pdata, eph[i].filesz);

		for (n = 0; n < eph[i].filesz; n++)
		{
			*(u8*)(eph[i].vaddr + n) = *(u8*)(pdata + n);
		}

	
		if (eph[i].memsz > eph[i].filesz)
		{
			//memset2(eph[i].vaddr + eph[i].filesz, 0, eph[i].memsz - eph[i].filesz);

			for (n = 0; n < (eph[i].memsz - eph[i].filesz); n++)
			{
				*(u8*)((eph[i].vaddr + eph[i].filesz) + n) = 0x00;
			}
		}
	}

	return eh->entry;
}
