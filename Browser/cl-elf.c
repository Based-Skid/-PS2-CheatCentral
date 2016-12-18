
#define ELF_MAGIC	0x464c457f
#define ELF_PT_LOAD	1

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



int ExtractELF(u32 elfdata)
{
	int i, n, tmpA, tmpB;
	int nsize = 1024;
	u8 *boot_elf = (u8 *) 0;
	elf_header_t *eh = &elfh;
	elf_pheader_t *eph;
	void *pdata;

	eh = (elf_header_t*)elfdata;
	eph = (elf_pheader_t*)(elfdata + eh->phoff);

	for (i = 0; i < eh->phnum; i++)
	{
		if (eph[i].type != ELF_PT_LOAD)
			continue;
	
		pdata = (void *)(elfdata + eph[i].offset);
		memcpy2(eph[i].vaddr, pdata, eph[i].filesz);
	
		if (eph[i].memsz > eph[i].filesz)
			memset2(eph[i].vaddr + eph[i].filesz, 0, eph[i].memsz - eph[i].filesz);
	}

	return eh->entry;
}


int ExecData(u32 elfdata, char *elfname)
{
	int Entry = ExtractELF(elfdata);

	if (Entry > 0)
	{
		LaunchELF(Entry, "CheatersLounge-EMBEDED_ELF.elf");
	}

	return -1;
}

int LaunchELF(u32 elfentry, char *elfname)
{
	char *args[1];
	args[0] = elfname;

	ResetIOP();

	FlushCache(0);
	FlushCache(2);

	ExecPS2((void*)elfentry, 0, 1, args);
}

int ReadELF(char *elfname)
{
	int fd, size, i;
	u8 *boot_elf = (u8 *) 0;
	elf_header_t *eh = &elfh;
	elf_pheader_t *eph;
	void *pdata;

	fd = fioOpen(elfname, 1);
	if (fd < 0) 
	{
		return -1;
	}

	size = fioLseek(fd, 0,2);
	if (size <= 0)
	{
		fioClose(fd);
		return -2;
	}
	
	fioLseek(fd, 0, 0);
	fioRead(fd, eh, sizeof(elf_header_t));

	boot_elf = 0x01800000 - size - 256; 
	if( (eh->entry + size) >= boot_elf )
	{
		boot_elf = eh -> entry - size - 256;
	}

	boot_elf = (u8*)(((unsigned)boot_elf)&0xfffffff0);

	fioRead(fd, boot_elf+sizeof(elf_header_t), size-sizeof(elf_header_t));
	fioClose(fd);

	eph = (elf_pheader_t*)(boot_elf + eh->phoff);

	for (i = 0; i < eh->phnum; i++)
	{
		if (eph[i].type != ELF_PT_LOAD)
			continue;
	
		pdata = (void *)(boot_elf + eph[i].offset);
		memcpy2(eph[i].vaddr, pdata, eph[i].filesz);
	
		if (eph[i].memsz > eph[i].filesz)
			memset2(eph[i].vaddr + eph[i].filesz, 0, eph[i].memsz - eph[i].filesz);
	}

	return eh->entry;
}

int RunELF(char *elfname)
{
	int Entry = ReadELF(elfname);
	char *argv[1];

	if (Entry > 0)
	{
		argv[0] = elfname;

		ResetIOP();

		FlushCache(0);
		FlushCache(2);
		ExecPS2(Entry, 0, 1, argv);
	}

	return -1;
}

int LoadELF(char *elfname)
{
	t_ExecData ELFData;

	SifLoadElf(elfname, &ELFData);

	return ELFData.epc;
}

int BootELF(char *elfname, int Mode)
{
	if (Mode == 0)
	{
		t_ExecData ELFData;

		SifLoadElf(elfname, &ELFData);

		if (ELFData.epc > 0)
		{
			ExecPS2((void*)ELFData.epc, 0, 1, NULL);
		}
		else
		{
			return -1;
		}

	}
	if (Mode == 1)
	{
		LoadExecPS2(elfname, 0, NULL);
	}

	return -2;
}

void memcpy2(unsigned char *dest,unsigned char *org,int ndata)
{
	int n;
	for(n = 0; n < ndata; n++)
		dest[n] = org[n];
}

void memset2(unsigned char *dest,unsigned char val,int ndata)
{
	int n;
	for(n = 0; n < ndata; n++)
		dest[n] = val;
}
