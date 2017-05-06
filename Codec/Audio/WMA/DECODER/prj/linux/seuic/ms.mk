
# please list all objects needed by your target here
OBJS:=AutoProfile.o baseplusdecpro.o basepluspro.o chexdecpro.o chexpro.o commontables.o configcommon.o dectables.o dectablespro.o \
			dectablesstd.o dectablesstdpro.o downmix.o drccommonpro.o entropydec.o entropydecpro.o entropydecprolsl.o entropydecstd.o fex.o \
			fexdec.o fft.o float.o huffdec.o losslessdeclsl.o losslessdecpro.o losslessdecprolsl.o lowrate_common.o lowrate_commonstd.o \
			lpc.o lpclsl.o lpcprolsl.o lpcstd.o msaudio.o msaudiodec.o msaudiodectemplate.o msaudiolsl.o msaudiopro.o msaudioprotemplate.o \
			msaudiostd.o msaudiostdpro.o msaudiotemplate.o strmdec_wma.o wmabuffilt.o wmachmtx.o wmadec.o wmaerror.o \
			wmafmt.o wmaltrt.o wmamalloc.o wmaprodecS_api.o cmnMemory.o
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../src/chanmtx \
					../../../../src/common \
					../../../../src/decoder \
					../../../../src/fmthlpr \
					../../../../src/pcmfmt \
					../../../../src/primitive \
					../../../../inc/audio/common/include \
					../../../../inc/audio/v10/include \
					../../../../inc/audio/v10/common \
					../../../../inc/audio/v10/asfparse \
					../../../../inc/audio/v10/decoder \
					../../../../inc/audio/v10/win32 \
					../../../../inc/common/include \
					../../../../inc/common/logging \
					../../../../../../../../Include \
					../../../../../../../../Common \
					../../../../asm/linuxasm 


	
