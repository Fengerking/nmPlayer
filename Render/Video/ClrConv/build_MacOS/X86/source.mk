BRANCH_INCLUDE_PATH:=../../../../../Include
BRANCH_COMMON_PATH:=../../../../../Common
CMNSRC_PATH:=../../src

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
VOMSRC:=$(CMNSRC_PATH)/ccConstant.c \
	$(BRANCH_COMMON_PATH)/voLog.c \
    $(CMNSRC_PATH)/ccMain.c \
    $(CMNSRC_PATH)/ccRGBToRGB.c \
    $(CMNSRC_PATH)/ccRGBToRGBMB.c \
    $(CMNSRC_PATH)/ccYUVToRGB.c \
    $(CMNSRC_PATH)/ccYUVToRGBMB.c \
    $(CMNSRC_PATH)/ccYUVToYUV.c


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		$(BRANCH_INCLUDE_PATH) \
		$(BRANCH_COMMON_PATH)\
		$(CMNSRC_PATH)
