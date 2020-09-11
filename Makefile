# Names of the compiler and friends
AS 			= as.exe
CC 			= gcc.exe
LD 			= ld.exe
OBJCOPY 	= objcopy.exe
STRIP 		= strip.exe

# Where the exe ends up
DEST		= A:\L

# libraries and paths
LIBS	 		= -lm
INCLUDES 	=          

# Compiler flags
ASM_FLAGS 		= -mcpu=i486 -march=i486
ASM_FLAGS_386 	= -mcpu=i386 -march=i386
LDFLAGS 			=
CFLAGS 			= -mcpu=i486 -march=i486 -fomit-frame-pointer -ffast-math #-O2
CFLAGS_386		= -mcpu=i386 -march=i386 -fomit-frame-pointer -ffast-math #-O2
CFLAGS_DEBUG		= -g
LDSCRIPT 		=
OCFLAGS			=

# What our application is named
TARGET	= launcher.exe

all: $(TARGET)

OBJFILES = bmp.o data.o filter.o fstools.o gfx.o ini.o input.o main.o palette.o ui.o utils.o 

$(TARGET):  $(OBJFILES)
	gcc.exe $(LDFLAGS) $(OBJFILES) $(LIBS) -o $(TARGET)
	copy $(TARGET) $(DEST)

bmp.o: bmp.c
	gcc.exe $(CFLAGS) -c bmp.c bmp.o
	
data.o: data.c
	gcc.exe $(CFLAGS) -c data.c -o data.o

filter.o: filter.c
	gcc.exe $(CFLAGS) -c filter.c -o filter.o
	
fstools.o: fstools.c
	gcc.exe $(CFLAGS) -c fstools.c -o fstools.o

gfx.o: gfx.c
	gcc.exe $(CFLAGS) -c gfx.c -o gfx.o
	
ini.o: ini.c
	gcc.exe $(CFLAGS) -c ini.c -o ini.o

input.o: input.c
	gcc.exe $(CFLAGS) -c input.c -o input.o
	
main.o: main.c
	gcc.exe $(CFLAGS) -c main.c -o main.o
	
palette.o: palette.c
	gcc.exe $(CFLAGS) -c palette.c -o palette.o	
	
ui.o: ui.c	
	gcc.exe $(CFLAGS) -c ui.c -o ui.o
	
utils.o: utils.c
	gcc.exe $(CFLAGS) -c utils.c -o utils.o
	
clean:
	del *.o
	del $(TARGET)
