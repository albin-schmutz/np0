CC      = gcc
CFLAGS  = -Wall

ifdef SystemRoot
	RM = del /Q *.o npx.exe
else
	RM = rm -f *.o npx
endif

NPX = npx.o run.o sys.o

all: npx

npx: $(NPX)
	$(CC) $(CFLAGS) -o npx $(NPX)

npx.o: npx.c run.h sys.h
	$(CC) $(CFLAGS) -c npx.c

run.o: run.c run.h sys.h np.h
	$(CC) $(CFLAGS) -c run.c

sys.o: sys.c sys.h
	$(CC) $(CFLAGS) -c sys.c

clean:
	$(RM)
