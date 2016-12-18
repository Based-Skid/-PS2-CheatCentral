EE_BIN = CL-Remote_PS2.ELF
EE_OBJS = main.o embed.o Objects/USBD.o Objects/USBHDFSD.o \
	Objects/ps2ips.o Objects/ps2ip.o Objects/ps2dev9.o \
	Objects/poweroff.o Objects/freesio2.o Objects/dns.o \
	Objects/PS2SMAP.o Objects/freepad.o Objects/IOMANX.o \
	Objects/fileXio.o 
EE_LDFLAGS = -s -Ttext 0x00F00000
EE_LIBS = -lc -lmc -lpad -ldebug -lps2ip -lgraph -ldma -lpacket -lpatches 

all: $(EE_BIN)
	ee-strip --strip-all $(EE_BIN)
	EXE/packer CL-Remote_PS2.ELF CL-Remote_PS2v1.0a.elf
	rm main.o # CL-Remote_PS2.ELF -f

Objects/IOMANX.s:
	bin2s IRX/IOMANX.IRX Objects/IOMANX.s IOMANX

Objects/fileXio.s:
	bin2s IRX/fileXio.IRX Objects/fileXio.s fileXio

Objects/freesio2.s:
	bin2s IRX/freesio2.irx Objects/freesio2.s freesio2

Objects/freepad.s:
	bin2s IRX/freepad.irx Objects/freepad.s freepad

Objects/USBD.s:
	bin2s IRX/USBD.IRX Objects/USBD.s USBD

Objects/USBHDFSD.s:
	bin2s IRX/USBHDFSD.IRX Objects/USBHDFSD.s USBHDFSD

Objects/poweroff.s:
	bin2s IRX/poweroff.irx Objects/poweroff.s poweroff

Objects/ps2dev9.s:
	bin2s IRX/ps2dev9.irx Objects/ps2dev9.s ps2dev9

Objects/ps2ip.s:
	bin2s IRX/ps2ip.irx Objects/ps2ip.s ps2ip

Objects/dns.s:
	bin2s IRX/dns.irx Objects/dns.s dns

Objects/ps2ips.s:
	bin2s IRX/ps2ips.irx Objects/ps2ips.s ps2ips

Objects/PS2SMAP.s:
	bin2s IRX/PS2SMAP.IRX Objects/PS2SMAP.s PS2SMAP


clean:
	rm -f *.elf *.o *.a $(EE_ELF) $(EE_ELF) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
