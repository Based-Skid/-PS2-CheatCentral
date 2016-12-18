
StackAddress = 0x01d00000
Load_Address = 0x00100000
Stack___Size = 0x00008000

Param = -Wl,--defsym -Wl,_stack_size=$(Stack___Size) -Wl,--defsym -Wl,_stack=$(StackAddress)

ExtraOBJ = Obj/cdvdinit_irx.o Obj/usbd.o Obj/usbhdfsd.o

ProjectFiles = main.o embeded.o

EE_BIN = Bin/CL-CheatsE_Public_v1.3.elf
EE_OBJS = $(ProjectFiles) $(ExtraOBJ)
EE_LDFLAGS = -s $(Param) -Ttext $(Load_Address)
EE_LIBS = -ldraw -lgraph -ldma -lc -lpacket -lpad -lcdvd



all: $(EE_BIN)
	ee-strip --strip-all $(EE_BIN)
	EXE/packer Bin/CL-CheatsE_Public_v1.3.elf Bin/CL-CheatsE_Public_v1.3r3.elf
	rm -f main.o *.a *.s # Bin/CL-CheatsE_Public_v1.3.elf

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

