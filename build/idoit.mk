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

ifeq ($(VOGAS), yes)
VOGAS_PRE := $(VO_BUILD)/gas-preprocessor.pl
else
VOGAS_PRE := 
endif

# where to place object files 
ifeq ($(VOMT), lib)
LIB_STATIC=$(OBJDIR)/$(VOTARGET).a
endif

ifeq ($(IDBG), yes)
CFLAGS=-D__VODBG__ -DMODULE_NAME="\"$(VOTARGET)-(debug)\""
CPPFLAGS=-D__VODBG__ -DMODULE_NAME="\"$(VOTARGET)-(debug)\""
else 
CFLAGS=-DMODULE_NAME=\"$(VOTARGET)\"
CPPFLAGS=-DMODULE_NAME=\"$(VOTARGET)\" 
endif

ifeq ($(VOTT), pc)
VOTGTCPU:=X86
VOTGTOS:=iOS
else
VOTGTCPU:=ARM
VOTGTOS:=iOS
endif

#VOSRCDIR+=$(dir $(VOMSRC))
CFLAGS+=-DMODULE_VERSION=\"$(VOMODVER)\" -DTARGET_CPUARCH=\"$(VOTGTCPU)$(VOTT)\" -DTARGETOS=\"$(VOTGTOS)\" -DMODULE_BRANCH=\"$(VOBRANCH)\" -DSOURCE_REVISIONNO=\"$(VOSRCNO)\" -DMODULE_BUILD=\"$(VOBUILDNUM)\" -DGLOBALVER=\"$(VOGPVER)\" $(VOCFLAGS) $(addprefix -I, $(VOSRCDIR)) 
CPPFLAGS+=-DMODULE_VERSION=\"$(VOMODVER)\" -DTARGET_CPUARCH=\"$(VOTGTCPU)$(VOTT)\" -DTARGETOS=\"$(VOTGTOS)\" -DMODULE_BRANCH=\"$(VOBRANCH)\" -DSOURCE_REVISIONNO=\"$(VOSRCNO)\" -DMODULE_BUILD=\"$(VOBUILDNUM)\" -DGLOBALVER=\"$(VOGPVER)\" $(VOCPPFLAGS) $(addprefix -I, $(VOSRCDIR)) 

#For Log, Per Jason
ifeq ($(VOOPTIM), release)
CFLAGS+=-D_VONDBG
CPPFLAGS+=-D_VONDBG 
endif

#VOSRCDIR+=$(VO_BUILD)
ifneq ($(VOTT), pc)
ASFLAGS=$(VOASFLAGS) $(addprefix -I, $(VOSRCDIR)) 
endif

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
endif

VOSRC:=$(notdir $(VOMSRC))
OBJS:=$(addprefix $(OBJDIR)/, $(addsuffix .o, $(VOSRC)))
VOMIDDEPS:=$(patsubst %.o,%.d,$(OBJS))

ifeq ($(VOMT), lib)

all: $(LIB_STATIC)

endif

#.PRECIOUS: $(OBJS)

ifeq ($(VOMT), lib)

$(LIB_STATIC):$(OBJS)
	$(LIBTOOL) -static $(IOS_LFLAGS) $^ $(VOSTCLIBS) -o $@

ifneq ($(VODBG), yes)
	#$(STRIP) $@
endif

	#$(VERBOSE) touch $(VO_BUILD)/version.cpp
	#$(VERBOSE) expr $(VOBUILDNUM) + 1  > .version
endif

sinclude $(VOMIDDEPS)

.SUFFIXES: .c .m .mm .cpp .s .S .d .h .o


$(OBJDIR)/%.c.d:%.c
	$(VERBOSE) $(CC) -w -MM $(CFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.c\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) -x c $(CFLAGS) -o $@ -c $<' | sed 's,\($*\)\.c\.d,\1\.c\.o,g' >> $@;
	
$(OBJDIR)/%.m.d:%.m
	$(VERBOSE) $(CC) -w -MM $(CFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.m\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) -x objective-c $(OBJC_MFLAGS) $(CFLAGS) -o $@ -c $<' | sed 's,\($*\)\.m\.d,\1\.m\.o,g' >> $@;
	
$(OBJDIR)/%.mm.d:%.mm
	$(VERBOSE) $(CC) -w -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.mm\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) -x objective-c++ $(OBJC_MMFLAGS) $(CPPFLAGS) -o $@ -c $<' | sed 's,\($*\)\.mm\.d,\1\.mm\.o,g' >> $@;

$(OBJDIR)/%.cpp.d:%.cpp
	$(VERBOSE) $(CC) -w -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.cpp\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) -x c++ $(CPPFLAGS) -o $@ -c $<' | sed 's,\($*\)\.cpp\.d,\1\.cpp\.o,g' >> $@; 

ifneq ($(VOTT), pc)

$(OBJDIR)/%.s.d:%.s
	$(VERBOSE) $(CC) -w -MM $(ASFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.s\.o,g' > $@; \
	echo '\t$(VERBOSE) $(VOGAS_PRE) $(CC) -x assembler-with-cpp $(ASFLAGS) -o $@ -c $<' | sed 's,\($*\)\.s\.d,\1\.s\.o,g' >> $@;

$(OBJDIR)/%.S.d:%.S
	$(VERBOSE) $(CC) -w -MM $(ASFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.S\.o,g' > $@; \
	echo '\t$(VERBOSE) $(VOGAS_PRE) $(CC) -x assembler-with-cpp $(ASFLAGS) -o $@ -c $<' | sed 's,\($*\)\.S\.d,\1\.S\.o,g' >> $@;
endif

.PHONY: clean devel
clean:
ifeq ($(VOMT), lib)
	-rm -f $(OBJS) $(VOMIDDEPS);
	@if test -e "$(LIB_STATIC)"; then rm -f $(LIB_STATIC); fi;
else
	-rm -f $(OBJS) $(VOMIDDEPS) .*.sw* $(VOTARGET)
endif

devel:
	cp -a -f $(LIB_STATIC) $(BLTDIRS)

