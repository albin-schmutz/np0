CC      = gcc
CFLAGS  = -Wall

ifdef SystemRoot
	RM = del /Q *.o npa.exe
else
	RM = rm -f *.o npa
endif

NPA = npa.o asm.o

all: npa

npa: $(NPA)
	$(CC) $(CFLAGS) -o npa $(NPA)

npa.o: npa.c asm.h
	$(CC) $(CFLAGS) -c npa.c

asm.o: asm.c asm.h np.h
	$(CC) $(CFLAGS) -c asm.c

clean:
	$(RM)
