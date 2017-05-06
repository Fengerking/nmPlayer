INC_PATH:=../../../../Include
CMNSRC_PATH:=../../../../Common
LOAD_PATH:=../../iOS/

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:=    $(LOAD_PATH)/voLoadLibControl.cpp

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		$(INC_PATH) \
	  $(LOAD_PATH)
	  
#		../../../../Source/File/XML \
