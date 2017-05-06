
# please list all objects needed by your target here
VOMSRC:=AutoProfile.c baseplusdecpro.c basepluspro.c chexdecpro.c chexpro.c commontables.c configcommon.c dectables.c dectablespro.c \
			dectablesstd.c dectablesstdpro.c downmix.c drccommonpro.c entropydec.c entropydecpro.c entropydecprolsl.c entropydecstd.c fex.c \
			fexdec.c fft.c float.c huffdec.c losslessdeclsl.c losslessdecpro.c losslessdecprolsl.c lowrate_common.c lowrate_commonstd.c \
			lpc.c lpclsl.c lpcprolsl.c lpcstd.c msaudio.c msaudiodec.c msaudiodectemplate.c msaudiolsl.c msaudiopro.c msaudioprotemplate.c \
			msaudiostd.c msaudiostdpro.c msaudiotemplate.c strmdec_wma.c wmabuffilt.c wmachmtx.c wmadec.c wmaerror.c \
			wmafmt.c wmaltrt.c wmamalloc.c wmaprodecS_api.c arm_stub.c
			
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../src/chanmtx \
					../../../src/common \
					../../../src/decoder \
					../../../src/fmthlpr \
					../../../src/pcmfmt \
					../../../src/primitive \
					../../../inc/audio/common/include \
					../../../inc/audio/v10/include \
					../../../inc/audio/v10/common \
					../../../inc/audio/v10/asfparse \
					../../../inc/audio/v10/decoder \
					../../../inc/audio/v10/win32 \
					../../../inc/common/include \
					../../../inc/common/logging \
					../../../../../../../Include \
					../../../../../../../Common \




ifeq ($(VOTT), v6)
VOMSRC+=fft_arm_v6.s msaudiopro_arm_v6.s msaudiostdpro_arm_v6.s msaudio_arm_v6.s \
      huffdec_arm.s losslessdeclsl_arm.s losslessdecprolsl_arm.s lowrate_arm.s lpclsl_arm.s  \
      lpcstd_arm.s msaudiodec_arm.s strmdec_arm.s voMemory_arm.s

VOSRCDIR+= ../../../asm/iOS/armv6 \
	   ../../../asm/iOS/armv7
endif      

ifeq ($(VOTT), v7)
VOMSRC+=fft_arm_v7.s msaudiopro_arm_v7.s msaudiostdpro_arm_v7.s msaudio_arm_v7.s \
      huffdec_arm.s losslessdeclsl_arm.s losslessdecprolsl_arm.s lowrate_arm.s lpclsl_arm.s  \
      lpcstd_arm.s msaudiodec_arm.s strmdec_arm.s voMemory_arm.s 
			  
VOSRCDIR+= ../../../asm/iOS/armv7 
endif			

ifeq ($(VOTT), v7s)
VOMSRC+=fft_arm_v7.s msaudiopro_arm_v7.s msaudiostdpro_arm_v7.s msaudio_arm_v7.s \
      huffdec_arm.s losslessdeclsl_arm.s losslessdecprolsl_arm.s lowrate_arm.s lpclsl_arm.s  \
      lpcstd_arm.s msaudiodec_arm.s strmdec_arm.s voMemory_arm.s 
			  
VOSRCDIR+= ../../../asm/iOS/armv7 
endif			
