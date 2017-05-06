CFLAGS= -DMEP -DT4G -O3   -w -la -Xopt_madd -Xopt_mulr -Xopt_max -Xopt_clip -mm -g
SOURCE=	../source/radec_wrapper.c	\
		../source/ucifio.c


INCLUDE= ../include	

LIB   = ../../RA8LIB/lib/RealAudio.lib 

LNK_FILE = RealAudio.lnk	
	
	

RealAudio:$(SOURCE) $(INCLUDE)
	cc_asp.csh $(SOURCE) -I$(INCLUDE) $(CFLAGS) $(LNK_FILE) $(LIB) -o RealAudio8Dec.abs
	tuconv -Fs32 RealAudio8Dec.abs
	rm -rf *.o

run:
	sim_script RealAudio8Dec.abs

all:
	# making video , audio ,scalar libraries and then RealAudio8Dec.abs
	@echo '~'
	@echo '~'
	@echo '~'
	@echo '~'
	@echo '~'
	@echo '~'
	@echo 'Building RealAudio.lib .....'
	@echo '~'
	@make -C ../../RA8LIB/lib 
	@echo '~'
	@echo '~'
	@echo '~'
	@echo '~'
	@echo 'Building RealAudio8Dec.abs .....'
	@echo '~'
	@make -s
	@echo '~ '
	@echo '~'
	@echo '~'
	@echo '~'
clean:
	rm -rf *.o *.abs *.map *.s *.s32
clean_all:
	# cleaning video audio libraries and the RealAudio8Dec.abs
	rm -rf *.o *.abs *.map *.s *.s32
	make -C ../../library/RealAudio/lib clean


