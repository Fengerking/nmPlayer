# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../../Include \
    ../../../src/libpng \
    ../../../src/zlib \
    ../../../src/ \
    ../../../src/libpng/neon/ios/

# please list all objects needed by your target here
VOMSRC:= adler32.c compress.c crc32.c deflate.c gzio.c infback.c  inffast.c \
       inflate.c inftrees.c trees.c uncompr.c zutil.c voPNGDecFront.c voPNGMemmory.c \
       png.c pngerror.c pnggccrd.c pngget.c pngmem.c pngpread.c pngread.c pngrio.c pngrtran.c \
       pngrutil.c pngset.c pngtrans.c pngvcrd.c pngwio.c pngwrite.c pngwtran.c pngwutil.c
			
ifeq ($(VOTT), v4)
VOMSRC+= 
endif				

ifeq ($(VOTT), v6)
VOMSRC+= 
endif				

ifeq ($(VOTT), v7)
VOMSRC+= 
endif			

ifeq ($(VOTT), v7s)
VOMSRC+= 
endif			
