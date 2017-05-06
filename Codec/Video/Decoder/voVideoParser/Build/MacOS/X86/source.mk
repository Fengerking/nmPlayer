INC_PATH:=../../../Inc
CMNSRC_PATH:=../../../Src

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
VOMSRC:=    $(CMNSRC_PATH)/voH263Parser.c \
    $(CMNSRC_PATH)/voH264Parser.c \
    $(CMNSRC_PATH)/voMPEG2Parser.c \
    $(CMNSRC_PATH)/voMPEG4Parser.c \
    $(CMNSRC_PATH)/voReadBits.c \
    $(CMNSRC_PATH)/voVideoParserFront.c \
    $(CMNSRC_PATH)/voVP6Parser.c \
    $(CMNSRC_PATH)/voVP8Parser.c \
    $(CMNSRC_PATH)/voWMVParser.c \


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../Include\
    $(INC_PATH) \
    $(CMNSRC_PATH) \



