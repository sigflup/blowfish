CC=m68k-elf-gcc

CFLAGS= -O6 -m68000 -I../libmega/ -w
LDFLAGS=-T ldscript -nostdlib -L../libmega -lmega

OBJS = sega.o main.o pal.o sprites.o z80_ctrl.o songs.o stdlib.o

rom.bin: rom.
	m68k-elf-objcopy -O binary rom. rom.bin
	checksum rom.bin

rom.: ${OBJS}
	m68k-elf-gcc ${CFLAGS} ${OBJS} -o rom. ${LDFLAGS} -Wl,-Map=output.map

songs.o: songs.s
	m68k-elf-gcc -c ${CFLAGS} -o songs.o songs.s

main.s: main.c
	m68k-elf-gcc ${CFLAGS} -S main.c -o main.s

vert.s: vert.c
	m68k-elf-gcc ${CFLAGS} -S vert.c -o vert.s
%.o: %.c
	m68k-elf-gcc -c ${CFLAGS} -o $@ $<


sega.o: sega.s
	m68k-elf-gcc -c ${CFLAGS} -o sega.o sega.s


clean:
	rm -rf *.o rom. rom.bin output.map 
