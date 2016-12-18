
StackAddress = 0x01f00000
Load_Address = 0x00100000
Stack___Size = 0x00008000

Param = -Wl,--defsym -Wl,_stack_size=$(Stack___Size) -Wl,--defsym -Wl,_stack=$(StackAddress)

ExtraOBJ = Obj/cdvdinit_irx.o Obj/usbd.o Obj/usbhdfsd.o obj/audsrv.o 

ProjectFiles = main.o embeded.o 

EE_BIN = Bin/CL-CheatCentral_Browser_ucp.elf
EE_OBJS = $(ProjectFiles) $(ExtraOBJ)
EE_LDFLAGS = -s $(Param) -Ttext $(Load_Address)
EE_LIBS = -ldraw -lgraph -ldma -lc -lpacket -lpad -lcdvd -lmf -laudsrv -ldebug -lkernel -lstdc++

all: $(EE_BIN)
	ee-strip --strip-all $(EE_BIN)
	EXE/packer Bin/CL-CheatCentral_Browser_ucp.elf Bin/CL-CheatCentral_Browser.elf
	rm -f main.o *.a *.s # EE_BIN
	# adpenc WAV/Click_high.wav WAV/click_high.adp
	# adpenc WAV/Click_low.wav WAV/click_low.adp
	# adpenc WAV/pluck_success.wav WAV/pluck_success.adp
	# adpenc WAV/pluck_failed.wav WAV/pluck_failed.adp
	# adpenc WAV/intro.wav WAV/intro.adp
	# adpenc WAV/background_music.wav WAV/background_music.adp

obj/audsrv.s:
	bin2s IRX/audsrv.irx obj/audsrv.s audsrv

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

