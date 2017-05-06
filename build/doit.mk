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
VO_TOP :=  $(subst /build,,$(VO_BUILD))
MKOBJDIR := $(shell	if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi;)

GENVERSION:= $(shell if [ ! -f .version ]; then echo 0001 > .version; cat .version; fi;)
VOBUILDNUM:= $(shell cat .version;)


# where to place object files 
ifeq ($(VOMT), lib)
LIB_STATIC=$(OBJDIR)/$(VOTARGET).a
LIB_DYNAMIC=$(OBJDIR)/$(VOTARGET).so
LIB_SONAME=$(VOTARGET).so
endif

ifeq ($(VOMT), exe)
TARGET=$(OBJDIR)/$(VOTARGET)
endif

ifeq ($(VODBG), yes)
CFLAGS=-D__VODBG__ -DMODULE_NAME="\"$(VOTARGET)-(debug)\""
CPPFLAGS=-D__VODBG__ -DMODULE_NAME="\"$(VOTARGET)-(debug)\""
else 
CFLAGS=-DMODULE_NAME=\"$(VOTARGET)\"
CPPFLAGS=-DMODULE_NAME=\"$(VOTARGET)\" 
endif

ifeq ($(VOTT), pc)
VOTGTCPU:=X86
VOTGTOS:=linux
else
VOTGTCPU:=ARM
VOTGTOS:=$(VONJ)
endif

CFLAGS+=-DMODULE_VERSION=\"$(VOMODVER)\" -DTARGET_CPUARCH=\"$(VOTGTCPU)$(VOTT)\" -DTARGETOS=\"$(VOTGTOS)\" -DMODULE_BRANCH=\"$(VOBRANCH)\" -DSOURCE_REVISIONNO=\"$(VOSRCNO)\" -DMODULE_BUILD=\"$(VOBUILDNUM)\" -DGLOBALVER=\"$(VOGPVER)\" $(VOCFLAGS) $(addprefix -I, $(VOSRCDIR)) 
CPPFLAGS+=-DMODULE_VERSION=\"$(VOMODVER)\" -DTARGET_CPUARCH=\"$(VOTGTCPU)$(VOTT)\" -DTARGETOS=\"$(VOTGTOS)\" -DMODULE_BRANCH=\"$(VOBRANCH)\" -DSOURCE_REVISIONNO=\"$(VOSRCNO)\" -DMODULE_BUILD=\"$(VOBUILDNUM)\" -DGLOBALVER=\"$(VOGPVER)\" $(VOCPPFLAGS) $(addprefix -I, $(VOSRCDIR)) 

#For Log, Per Jason
ifeq ($(VOOPTIM), release)
CFLAGS+=-D_VONDBG
CPPFLAGS+=-D_VONDBG 
endif

VOSRCDIR+=$(VO_BUILD)
ifneq ($(VOTT), pc)
ASFLAGS=$(VOASFLAGS) $(addprefix -I, $(VOSRCDIR)) 
endif

LDFLAGS:=$(VOLDFLAGS)
#VOTEDEPS+=$(VODEPLIBS)
#VOTLDEPS+=$(VODEPLIBS)
VOSTCLIBS?=

vpath %.c $(VOSRCDIR)
vpath %.cpp $(VOSRCDIR)
ifneq ($(VOTT), pc)
vpath %.s $(VOSRCDIR)
vpath %.S $(VOSRCDIR)
endif

OBJS+=version.o
VOOBJS:=$(addprefix $(OBJDIR)/, $(OBJS)) 
OBJS:=$(VOOBJS) 


#VOMIDDEPS:=$($(VOOBJS):.o=.d)
VOMIDDEPS:=$(patsubst %.o,%.d,$(VOOBJS))

ifeq ($(VOTT), pc)

BLTDIRS=$(VORELDIR)/Linux/static
BLTDIRD=$(VORELDIR)/Linux/shared

else 

ifeq ($(findstring NNJ,$(VOPREDEF)), NNJ)
BLTDIRS=$(VORELDIR)/Customer/$(TCVENDOR)/lib/$(VOTT)
BLTDIRD=$(VORELDIR)/Customer/$(TCVENDOR)/so/$(VOTT)
else

ifeq ($(findstring SEUIC,$(VOPREDEF)), SEUIC)
BLTDIRS=$(VORELDIR)/Customer/$(TCVENDOR)/lib/$(VOTT)
BLTDIRD=$(VORELDIR)/Customer/$(TCVENDOR)/so/$(VOTT)
else
BLTDIRS=$(VORELDIR)/Customer/Google/$(VONJ)/lib/$(VOTT)
BLTDIRD=$(VORELDIR)/Customer/Google/$(VONJ)/so/$(VOTT)
endif

endif

endif

ifeq ($(VOMT), lib)

all: mkdirs $(LIB_STATIC) $(LIB_DYNAMIC)
mkdirs: $(BLTDIRS) $(BLTDIRD)

else
all: $(TARGET)
endif


ifeq ($(VOMT), lib)
$(BLTDIRS):
	@if test ! -d $@; then \
		mkdir -p $@; \
	fi;
$(BLTDIRD):
	@if test ! -d $@; then \
		mkdir -p $@; \
	fi;
endif

#.PRECIOUS: $(OBJS)

ifeq ($(VOMT), lib)

$(LIB_STATIC):$(OBJS)
	$(AR) cr $@ $^ $(VOSTCLIBS)
	$(RANLIB) $@
ifneq ($(VODBG), yes)
	#$(STRIP) $@
endif

$(LIB_DYNAMIC):$(OBJS)
	$(GG) $(LDFLAGS) -Wl,-soname,$(LIB_SONAME) -o $@ $(CCTCRTBEGIN) $^ -Wl,--whole-archive $(VOSTCLIBS) -Wl,--no-whole-archive $(CCRTEXTRAS) $(VODEPLIBS) $(VOTLDEPS) 

ifneq ($(VODBG), yes)
		$(STRIP) $@
endif
	#$(VERBOSE) touch $(VO_BUILD)/version.cpp
	#$(VERBOSE) expr $(VOBUILDNUM) + 1  > .version

else

$(TARGET):$(OBJS)
	$(GG) $(LDFLAGS) -o $@ $(CCTTECRTBEGIN) $^ -Wl,--whole-archive $(VOSTCLIBS) -Wl,--no-whole-archive $(CCRTEXTRAS) $(VODEPLIBS) $(VOTEDEPS) 

ifneq ($(VODBG), yes)
	$(STRIP) $@
endif
	#$(VERBOSE) touch $(VO_BUILD)/version.cpp
	#$(VERBOSE) expr $(VOBUILDNUM) + 1  > .version

endif

sinclude $(VOMIDDEPS)

.SUFFIXES: .c .cpp .s .S .d .o

#for building .c
#.c.o:
#	$(VERBOSE) $(CC) $(CFLAGS) -o $@ -c $<

#$(OBJDIR)/%.o:%.c
#	$(VERBOSE) $(CC) $(CFLAGS) -o $@ -c $<
#

$(OBJDIR)/%.d:%.c
	$(VERBOSE) $(CC) -MM $(CFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.o,g' > $@; \
	echo '$(VERBOSE) $(CC) $(CFLAGS) -o $@ -c $<' | sed 's,^$(VERBOSE),\t$(VERBOSE),g' | sed 's,\($*\)\.d,\1\.o,g' >> $@;

# for building .cpp
#.cpp.o:
#	$(VERBOSE) $(GG) $(CPPFLAGS) -o $@ -c $<

#$(OBJDIR)/%.o:%.cpp
#	$(VERBOSE) $(GG) $(CPPFLAGS) -o $@ -c $<

$(OBJDIR)/%.d:%.cpp
	$(VERBOSE) $(GG) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o,$(OBJDIR)/\1\.o,g' > $@; \
	echo '$(VERBOSE) $(GG) $(CPPFLAGS) -o $@ -c $<' | sed 's,^$(VERBOSE),\t$(VERBOSE),g' | sed 's,\($*\)\.d,\1\.o,g' >> $@;

ifneq ($(VOTT), pc)
# for building assembly
#.s.o:
#	$(VERBOSE) $(AS) $(ASFLAGS) -o $@ $<
$(OBJDIR)/%.o:%.s
	$(VERBOSE) $(CC) $(VOMM) -E $< | $(AS) $(ASFLAGS) -o $@ 
$(OBJDIR)/%.o:%.S
	$(VERBOSE) $(CC) $(VOMM) -E $< | $(AS) $(ASFLAGS) -o $@ 
endif


.PHONY: clean devel
clean:
ifeq ($(VOMT), lib)
	-rm -f $(OBJS) $(VOMIDDEPS);
	@if test -e "$(LIB_STATIC)"; then rm -f $(LIB_STATIC); fi;
	@if test -e "$(LIB_DYNAMIC)"; then rm -f $(LIB_DYNAMIC); fi;
else
	-rm -f $(OBJS) $(VOMIDDEPS) .*.sw* $(TARGET)
endif

devel:
	cp -a $(LIB_STATIC) $(BLTDIRS)
	cp -a $(LIB_DYNAMIC) $(BLTDIRD)

