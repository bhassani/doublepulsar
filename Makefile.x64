#
# Builds:
#	pe_util.c hs_util.c main.c (x64)
#
CC      := x86_64-w64-mingw32-gcc
SRCS    := $(wildcard src/*.c)
OBJS    := $(SRCS:%.c=%.o)
CFLAGS  := -Os -D_KM_UTILS -ffunction-sections -fno-asynchronous-unwind-tables -nostdlib -fno-ident -falign-functions
INCS    := -Iinclude -Iinclude/subhook
LFLAGS  := -Wl,-s,--no-seh,-Tinclude/linked.x64.ld

all: payload.x64.exe payload.x64.bin

payload.x64.bin: third-party/syscall_hook.x64.bin
	python2.7 pedump.py payload.x64.exe $@
	cat third-party/syscall_hook.x64.bin $@ > payload_irql_hop.x64.bin

payload.x64.exe: $(OBJS) src/StackAdjust.o 
	$(CC) -o $@ $^ $(INCS) $(CFLAGS) $(LFLAGS)

.c.o: 
	$(CC) -o $@ -c $< $(INCS) $(CFLAGS)

src/StackAdjust.o:
	nasm -f win64 src/StackAdjust.asm -o $@

third-party/syscall_hook.x64.bin:
	nasm -f bin third-party/syscall_hook.x64.asm -o $@

clean:
	rm -rf $(OBJS) payload.x64.exe payload.x64.bin src/StackAdjust.o third-party/syscall_hook.x64.bin payload_irql_hop.x64.bin
