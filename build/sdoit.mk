# this script aims at generating modules 
# david 2009-05-22

VERBOSE:=@


VOMT ?= lib

ifeq ($(VOMT), lib)
LIB_STATIC=$(VOTARGET).a
endif

ifeq ($(VOMT), exe)
TARGET=$(VOTARGET)
endif

CFLAGS=$(VOCFLAGS) $(addprefix -I, $(VOSRCDIR)) 
CPPFLAGS=$(VOCPPFLAGS) $(addprefix -I, $(VOSRCDIR)) 
ifneq ($(VOTT), pc)
ASFLAGS=$(VOASFLAGS) $(addprefix -I, $(VOSRCDIR)) 
endif

LDFLAGS:=$(VOLDFLAGS)
VOTEDEPS+=$(VODEPLIBS)
VOTLDEPS+=$(VODEPLIBS)
VOSTCLIBS ?=

vpath %.c $(VOSRCDIR)
vpath %.cpp $(VOSRCDIR)
ifneq ($(VOTT), pc)
vpath %.s $(VOSRCDIR)
endif

ifeq ($(VOTT), pc)
BLTDIRS=$(VORELDIR)/Linux/static
BLTDIRD=$(VORELDIR)/Linux/shared
else
BLTDIRS=$(VORELDIR)/static
BLTDIRD=$(VORELDIR)/shared
endif


.PRECIOUS: $(OBJDIR)/%.o

ifeq ($(VOMT), lib)
all: mkdirs $(LIB_STATIC)
mkdirs: $(OBJDIR) $(BLTDIRS) $(BLTDIRD)
else
all: mkdirs $(TARGET)
mkdirs: $(OBJDIR)
endif

$(OBJDIR):
	@if test ! -d $@; then \
		mkdir -p $@; \
	fi;

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


ifeq ($(VOMT), lib)

$(LIB_STATIC):$(OBJS)
	$(AR) cr $@ $(OBJDIR)/*.o $(VOSTCLIBS)
	$(RANLIB) $@
ifneq ($(VODBG), yes)
	#$(STRIP) $@
endif

else

$(TARGET):$(OBJS)
	$(GG) $(LDFLAGS) -o $@ $(OBJDIR)/*.o $(VOSTCLIBS) $(VOTEDEPS)
ifneq ($(VODBG), yes)
	$(STRIP) $@
endif
endif


.SUFFIXES: .c .cpp .s .o
.c.o:
	$(VERBOSE) $(CC) $(CFLAGS) -o $(OBJDIR)/$@ -c $^
.cpp.o:
	$(VERBOSE) $(GG) $(CPPFLAGS) -o $(OBJDIR)/$@ -c $^
ifneq ($(VOTT), pc)
.s.o:
	$(VERBOSE) $(AS) $(ASFLAGS) -o $(OBJDIR)/$@ $^
endif

.PHONY: clean devel
clean:
ifeq ($(VOMT), lib)
	-rm -fr $(OBJDIR) .*.sw* $(VOTARGET).*
else
	-rm -fr $(OBJDIR) .*.sw* $(VOTARGET)
endif

devel:
	cp -a $(LIB_STATIC) $(BLTDIRS)

