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

MAKEFILE := $(lastword $(MAKEFILE_LIST))
VO_BUILD := $(abspath $(dir $(abspath $(MAKEFILE))))
VO_TOP := $(subst /build,,$(VO_BUILD))

# where to place object files 
ifeq ($(VOMT), lib)
LIB_STATIC=$(VOTARGET).a
LIB_DYNAMIC=$(VOTARGET).so
endif

ifeq ($(VOMT), exe)
TARGET=$(VOTARGET)
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
VOTGTOS:=iOS
else
VOTGTCPU:=ARM
VOTGTOS:=bada
endif

#VOSRCDIR+=$(dir $(VOMSRC))
CFLAGS+=-DMODULE_VERSION=\"$(VOMODVER)\" -DTARGET_CPUARCH=\"$(VOTGTCPU)$(VOTT)\" -DTARGETOS=\"$(VOTGTOS)\" -DMODULE_BRANCH=\"$(VOBRANCH)\" -DSOURCE_REVISIONNO=\"$(VOSRCNO)\" -DMODULE_BUILD=\"$(VOBUILDNUM)\" $(VOCFLAGS) $(addprefix -I, $(VOSRCDIR)) 
CPPFLAGS+=-DMODULE_VERSION=\"$(VOMODVER)\" -DTARGET_CPUARCH=\"$(VOTGTCPU)$(VOTT)\" -DTARGETOS=\"$(VOTGTOS)\" -DMODULE_BRANCH=\"$(VOBRANCH)\" -DSOURCE_REVISIONNO=\"$(VOSRCNO)\" -DMODULE_BUILD=\"$(VOBUILDNUM)\" $(VOCPPFLAGS) $(addprefix -I, $(VOSRCDIR)) 

#For Log, Per Jason
ifeq ($(VOOPTIM), release)
CFLAGS+=-D_VONDBG
CPPFLAGS+=-D_VONDBG 
endif

#VOSRCDIR+=$(VO_BUILD)
ifneq ($(VOTT), pc)
ASFLAGS=$(VOASFLAGS) $(addprefix -I, $(VOSRCDIR)) 
endif

LDFLAGS:=$(VOLDFLAGS)
VOTEDEPS+=$(VODEPLIBS)
VOTLDEPS+=$(VODEPLIBS)
VOSTCLIBS ?=

VOMSRC+=$(VO_BUILD)/version.cpp
VOSRCDIR+=$(VO_BUILD)
vpath %.c $(VOSRCDIR)
vpath %.cpp $(VOSRCDIR)
ifneq ($(VOTT), pc)
vpath %.s $(VOSRCDIR)
vpath %.S $(VOSRCDIR)
endif

VOSRC:=$(notdir $(VOMSRC))
OBJS:=$(addprefix $(OBJDIR)/, $(addsuffix .o, $(VOSRC)))
VOMIDDEPS:=$(patsubst %.o,%.d,$(OBJS))

ifeq ($(VOTT), pc)

BLTDIRS=$(VORELDIR)/Linux/static
BLTDIRD=$(VORELDIR)/Linux/shared

else 

ifeq ($(findstring NNJ,$(VOPREDEF)), NNJ)
BLTDIRS=$(VORELDIR)/Customer/$(TCVENDOR)/lib/$(VOTT)
BLTDIRD=$(VORELDIR)/Customer/$(TCVENDOR)/so/$(VOTT)
else

ifeq ($(findstring SEUIC,$(VOPREDEF)), __BADA__)
BLTDIRS=$(VORELDIR)/Customer/$(TCVENDOR)/lib/$(VOTT)
BLTDIRD=$(VORELDIR)/Customer/$(TCVENDOR)/so/$(VOTT)
else
BLTDIRS=$(VORELDIR)/Customer/Google/$(VONJ)/lib/$(VOTT)
BLTDIRD=$(VORELDIR)/Customer/Google/$(VONJ)/so/$(VOTT)
endif
endif
endif

ifeq ($(VOMT), lib)

all: mkdirs $(LIB_STATIC)
mkdirs: $(BLTDIRS)

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
	$(LIBTOOL) -static $(IOS_LFLAGS) $^ $(VOSTCLIBS) -o $@

ifneq ($(VODBG), yes)
	#$(STRIP) $@
endif

$(LIB_DYNAMIC):$(OBJS)
	$(GG) $(LDFLAGS) -o $@ $^ $(CCTCRTBEGIN) -Wl,--whole-archive $(VOSTCLIBS) -Wl,--no-whole-archive $(VOTLDEPS) 

ifneq ($(VODBG), yes)
	$(STRIP) $@
endif

else

$(TARGET):$(OBJS)
	$(GG) $(LDFLAGS) -o $@ $^ -Wl,--whole-archive $(VOSTCLIBS) -Wl,--no-whole-archive $(VOTEDEPS) 

endif

sinclude $(VOMIDDEPS)

.SUFFIXES: .c .cpp .s .S .d .h .o


$(OBJDIR)/%.c.d:%.c
	$(VERBOSE) $(CC) -MM $(CFLAGS) $< | sed 's,\($*\)\.o,obj/\1\.c\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) -x c $(CFLAGS) -o $@ -c $<' | sed 's,\($*\)\.c\.d,\1\.c\.o,g' >> $@; 
	
$(OBJDIR)/%.cpp.d:%.cpp
	$(VERBOSE) $(CC) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o,obj/\1\.cpp\.o,g' > $@; \
	echo '\t$(VERBOSE) $(CC) -x c++ $(CPPFLAGS) -o $@ -c $<' | sed 's,\($*\)\.cpp\.d,\1\.cpp\.o,g' >> $@; 

ifneq ($(VOTT), pc)

$(OBJDIR)/%.s.d:%.s
	$(VERBOSE) $(CC) -MM $(ASFLAGS) $< | sed 's,\($*\)\.o,obj/\1\.s\.o,g' > $@; \
	echo '\t$(VERBOSE) $(VOGAS_PRE) $(CC) -x assembler-with-cpp $(ASFLAGS) -o $@ -c $<' | sed 's,\($*\)\.s\.d,\1\.s\.o,g' >> $@;

$(OBJDIR)/%.S.d:%.S
	$(VERBOSE) $(CC) -MM $(ASFLAGS) $< | sed 's,\($*\)\.o,obj/\1\.S\.o,g' > $@; \
	echo '\t$(VERBOSE) $(VOGAS_PRE) $(CC) -x assembler-with-cpp $(ASFLAGS) -o $@ -c $<' | sed 's,\($*\)\.S\.d,\1\.S\.o,g' >> $@;
endif

.PHONY: clean devel
clean:
ifeq ($(VOMT), lib)
	-rm -f $(OBJS) $(VOMIDDEPS);
	@if test -e "$(VOTARGET).a"; then rm -f $(VOTARGET).a; fi;
	@if test -e "$(VOTARGET).so"; then rm -f $(VOTARGET).so; fi;
else
	-rm -f $(OBJS) $(VOMIDDEPS) .*.sw* $(VOTARGET)
endif

devel:
	cp -a -f $(LIB_STATIC) $(BLTDIRS)
#cp -a $(LIB_DYNAMIC) $(BLTDIRD)

