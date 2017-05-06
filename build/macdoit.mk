# this script aims at generating modules 
# david 2009-05-22

VERBOSE?=@

VOMT ?= lib

# SDK version 
VOMODVER?=3.0.0


# svn source NO.
VOSRCNO?=8888


# build number
VOBUILDNUM?=0000


# branch name
VOBRANCH?=master


# global product version
VOGPVER?=3.3.18


MAKEFILE := $(lastword $(MAKEFILE_LIST))
VO_BUILD := $(abspath $(dir $(abspath $(MAKEFILE))))
VO_TOP := $(subst /build,,$(VO_BUILD))
MKOBJDIR := $(shell	if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi;)

GENVERSION:= $(shell if [ ! -f .version ]; then echo 0001 > .version; cat .version; fi;)
VOBUILDNUM:= $(shell cat .version;)

ifeq ($(VOGAS), yes)
VOGAS_PRE := $(VO_BUILD)/gas-preprocessor.pl
else
VOGAS_PRE := 
endif

# where to place object files 
ifeq ($(VOMT), lib)
LIB_NAME=$(OBJDIR)/$(VOTARGET).dylib
endif

ifeq ($(VOMT), static_lib)
LIB_NAME=$(OBJDIR)/$(VOTARGET).a
endif

ifeq ($(IDBG), yes)
VOCPPFLAGS=-D__VODBG__ -DMODULE_NAME="\"$(VOTARGET)-(debug)\""
else 
VOCPPFLAGS=-DMODULE_NAME=\"$(VOTARGET)\" 
endif

ifeq ($(VOTT), x86)
VOTGTCPU:=X86
else
VOTGTCPU:=X86_64
endif

VOTGTOS:="Mac OS X"

LOCAL_INCLUDE:=$(addprefix -I, $(VOSRCDIR)) 
VOCPPFLAGS+=-DMODULE_VERSION=\"$(VOMODVER)\" -DTARGET_CPUARCH=\"$(VOTGTCPU)\" -DTARGETOS=\"$(VOTGTOS)\" -DMODULE_BRANCH=\"$(VOBRANCH)\" -DSOURCE_REVISIONNO=\"$(VOSRCNO)\" -DMODULE_BUILD=\"$(VOBUILDNUM)\" -DGLOBALVER=\"$(VOGPVER)\" $(LOCAL_INCLUDE)

#For Log, Per Jason
ifeq ($(VOOPTIM), release)
CPPFLAGS+=-D_VONDBG 
endif

MAC_CFLAGS+=$(LOCAL_INCLUDE)

# X86 C++ flags
MAC_CPPFLAGS+=$(VOCPPFLAGS) 

# X86 objective-c flags
MAC_OBJCFLAGS+=$(LOCAL_INCLUDE)

# X86 objective-c++ flags
MAC_OBJCPPFLAGS+=$(LOCAL_INCLUDE)

ifneq ($(VOTT), pc)
ASFLAGS=$(VOASFLAGS) $(addprefix -I, $(VOSRCDIR))
ASMFLAGS =-DHIGH_BIT_DEPTH=0 -DBIT_DEPTH=8 -DARCH_X86_64=0 -DHAVE_CPUNOP=0 -DARCH_X86_32=1  -DHAVE_ALIGNED_STACK=1 -f macho
endif

LDFLAGS:=$(VOLDFLAGS)
VOTEDEPS+=$(VODEPLIBS)
VOTLDEPS+=$(VODEPLIBS)
VOSTCLIBS ?=

VOMSRC+=$(VO_BUILD)/version.cpp
VOSRCDIR+=$(VO_BUILD)
vpath %.c $(VOSRCDIR)
vpath %.m $(VOSRCDIR)
vpath %.mm $(VOSRCDIR)
vpath %.cpp $(VOSRCDIR)
ifneq ($(VOTT), pc)
vpath %.s $(VOSRCDIR)
vpath %.S $(VOSRCDIR)
vpath %.asm $(VOSRCDIR)
endif

VOSRC:=$(notdir $(VOMSRC))
OBJS:=$(addprefix $(OBJDIR)/, $(addsuffix .o, $(VOSRC)))
VOMIDDEPS:=$(patsubst %.o,%.d,$(OBJS))

BLTDIRS=$(VORELDIR)

all: mkdirs $(LIB_NAME)
mkdirs: $(BLTDIRS)

$(BLTDIRS):
	@if test ! -d $@; then \
		mkdir -p $@; \
	fi;

#.PRECIOUS: $(OBJS)

ifeq ($(VOMT), lib)
$(LIB_NAME):$(OBJS)
	$(GG) $(VOLDFLAGS) -read_only_relocs suppress $^ $(VOSTCLIBS) -install_name /usr/local/lib/$(VOTARGET).dylib -o $@
endif

ifeq ($(VOMT), static_lib)
$(LIB_NAME):$(OBJS)
	$(LIBTOOL) $(VOLDFLAGS) $^ $(VOSTCLIBS) -o $@
endif

sinclude $(VOMIDDEPS)

.SUFFIXES: .c .m .mm .cpp .s .S .asm .d .h .o


$(OBJDIR)/%.c.d:%.c
	$(VERBOSE) $(CC) -w -MM $(MAC_CFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.c\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) $(MAC_CFLAGS) -o $@ -c $<' | sed 's,\($*\)\.c\.d,\1\.c\.o,g' >> $@; 
	
$(OBJDIR)/%.m.d:%.m
	$(VERBOSE) $(CC) -w -MM $(MAC_OBJCFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.m\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) $(MAC_OBJCFLAGS) -o $@ -c $<' | sed 's,\($*\)\.m\.d,\1\.m\.o,g' >> $@; 
	
$(OBJDIR)/%.mm.d:%.mm
	$(VERBOSE) $(CC) -w -MM $(MAC_OBJCPPFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.mm\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) $(MAC_OBJCPPFLAGS) -o $@ -c $<' | sed 's,\($*\)\.mm\.d,\1\.mm\.o,g' >> $@; 

$(OBJDIR)/%.cpp.d:%.cpp
	$(VERBOSE) $(CC) -w -MM $(MAC_CPPFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.cpp\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) $(MAC_CPPFLAGS) -o $@ -c $<' | sed 's,\($*\)\.cpp\.d,\1\.cpp\.o,g' >> $@; 

$(OBJDIR)/%.s.d:%.s
	$(VERBOSE) $(CC) -w -MM $(ASFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.s\.o,g' > $@; \
	echo '\t$(VERBOSE) $(VOGAS_PRE) $(CC) -x assembler-with-cpp $(ASFLAGS) -o $@ -c $<' | sed 's,\($*\)\.s\.d,\1\.s\.o,g' >> $@;

$(OBJDIR)/%.S.d:%.S
	$(VERBOSE) $(CC) -w -MM $(ASFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.S\.o,g' > $@; \
	echo '\t$(VERBOSE) $(VOGAS_PRE) $(CC) -x assembler-with-cpp $(ASFLAGS) -o $@ -c $<' | sed 's,\($*\)\.S\.d,\1\.S\.o,g' >> $@;
	
$(OBJDIR)/%.asm.d:%.asm
	$(VERBOSE) $(ASM) -w -MM $(ASMFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.asm\.o,g' > $@; \
	echo '\t$(VERBOSE) $(VOGAS_PRE) $(ASM) $(ASMFLAGS) -o $@ -c $<' | sed 's,\($*\)\.asm\.d,\1\.asm\.o,g' >> $@;

.PHONY: clean devel
clean:
	-rm -f $(OBJDIR)/*

devel:
	cp -a -f $(LIB_NAME) $(BLTDIRS)

