INC_PATH:=
CMNSRC_PATH:=

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
VOMSRC:=    $(CMNSRC_PATH)/file1.cpp \
    $(CMNSRC_PATH)/file2.cpp \


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
    $(INC_PATH) \
    $(CMNSRC_PATH) \


# You may need to add some file when in difference target
ifeq ($(VOTT), v7)
VOMSRC+= ../file_asm.s
VOSRCDIR+= ../dir/ios/armv7
Endif

