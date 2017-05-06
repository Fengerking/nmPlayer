CFLAGS= -DMEP -DT4G -O3 -w -Xopt_madd -Xopt_mulr -Xopt_max -Xopt_clip -mm -g

SOURCE= ../source/radec_bitpack.c   \
	../source/radec_category.c		\
	../source/radec_couple.c		\
	../source/radec_envelope.c  	\
	../source/radec_fft.c       	\
	../source/radec_gainctrl.c     	\
	../source/radec_gecko2codec.c  	\
	../source/radec_huffman.c      	\
	../source/radec_hufftabs.c     	\
	../source/radec_mlt.c          	\
	../source/radec_sqvh.c   		\
	../source/radec_trigtabs.c    	\
	../source/radec_dma_api.c


INCLUDE= ../include

RealAudio:$(SOURCE) $(INCLUDE)
	cc_asp.csh $(SOURCE) -I$(INCLUDE) $(CFLAGS) -c
	tulib -r RealAudio.lib *.o
	rm -rf *.o

clean:
	rm RealAudio.lib 

