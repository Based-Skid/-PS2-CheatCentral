
StackAddress = 0x01F00000
Load_Address = 0x00F00000
Stack___Size = 0x00100000

Param = -Wl,--defsym -Wl,_stack_size=$(Stack___Size) -Wl,--defsym -Wl,_stack=$(StackAddress)

ExtraOBJ = Obj/cdvdinit_irx.o Obj/USBD.o Obj/USBHDFSD.o \
	Obj/ps2ips.o Obj/ps2ip.o Obj/ps2dev9.o \
	Obj/poweroff.o Obj/freesio2.o Obj/dns.o \
	Obj/PS2SMAP.o Obj/freepad.o Obj/IOMANX.o \
	Obj/fileXio.o 

ProjectFiles = main.o 

EE_BIN = BIN/CL-HTTP_Server.elf
EE_OBJS = $(ProjectFiles) $(ExtraOBJ)
EE_LDFLAGS = -s -Ttext $(Load_Address) $(Param)
EE_LIBS = -lc -lmc -ldebug -lps2ip -lgraph -ldma -lpacket -lcdvd -lpatches -lkernel -lfileXio 


all: $(EE_BIN)
	ee-strip --strip-all $(EE_BIN)
	EXE/packer BIN/CL-HTTP_Server.elf BIN/CL-HTTP_Server_v1.0.elf
	rm -f main.o *.a *.s

Obj/IOMANX.s:
	bin2s IRX/IOMANX.IRX Obj/IOMANX.s IOMANX

Obj/fileXio.s:
	bin2s IRX/fileXio.IRX Obj/fileXio.s fileXio

Obj/freesio2.s:
	bin2s IRX/freesio2.irx Obj/freesio2.s freesio2

Obj/freepad.s:
	bin2s IRX/freepad.irx Obj/freepad.s freepad

Obj/USBD.s:
	bin2s IRX/USBD.IRX Obj/USBD.s USBD

Obj/USBHDFSD.s:
	bin2s IRX/USBHDFSD.IRX Obj/USBHDFSD.s USBHDFSD

Obj/poweroff.s:
	bin2s IRX/poweroff.irx Obj/poweroff.s poweroff

Obj/ps2dev9.s:
	bin2s IRX/ps2dev9.irx Obj/ps2dev9.s ps2dev9

Obj/ps2ip.s:
	bin2s IRX/ps2ip.irx Obj/ps2ip.s ps2ip

Obj/dns.s:
	bin2s IRX/dns.irx Obj/dns.s dns

Obj/ps2ips.s:
	bin2s IRX/ps2ips.irx Obj/ps2ips.s ps2ips

Obj/PS2SMAP.s:
	bin2s IRX/PS2SMAP.IRX Obj/PS2SMAP.s PS2SMAP


include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

