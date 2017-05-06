# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../Include \
    ../../src/libpng \
    ../../src/zlib \
    ../../src/ \
    ../../src/libpng/neon/gnu/

# please list all objects needed by your target here
OBJS:= adler32.o compress.o crc32.o deflate.o gzio.o infback.o  inffast.o \
       inflate.o inftrees.o trees.o uncompr.o zutil.o voPNGDecFront.o voPNGMemmory.o \
       png.o pngerror.o pnggccrd.o pngget.o pngmem.o pngpread.o pngread.o pngrio.o pngrtran.o \
       pngrutil.o pngset.o pngtrans.o pngvcrd.o pngwio.o pngwrite.o pngwtran.o pngwutil.o
			
ifeq ($(VOTT), v4)
OBJS+= 
endif				

ifeq ($(VOTT), v6)
OBJS+= 
endif				

ifeq ($(VOTT), v7)
OBJS+= 
endif			
