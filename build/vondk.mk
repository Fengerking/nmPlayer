
VOOPTIM?=unknown
ifeq ($(VOOPTIM),debug)
  APP_OPTIM:=debug
else
ifeq ($(VOOPTIM),release)
  APP_OPTIM:=release
endif
endif

LOCAL_SRC_FILES += $(VOTOP)/Common/ndkversion.cpp


VOMINFO:=-DMODULE_VERSION=\"$(strip $(VOMODVER))\" -DCPU_ARCH=\"$(strip $(APP_ABI))\" -DMODULE_BRANCH=\"$(strip $(VOBRANCH))\" -DSOURCE_REVISIONNO=\"$(strip $(VOSRCNO))\" -DBUILD_TOOL=\"$(strip $(VOBUILDTOOL))\" -DMODULE_BUILD=\"$(strip $(VOBUILDNUM))\" -DGLOBALVER=\"$(strip $(VOGPVER))\"

ifeq ($(APP_OPTIM),debug)
VOABOUT:=-DMODULE_NAME=\"$(strip $(LOCAL_MODULE))\(debug\)\" $(VOMINFO)

ifeq ($(VOOPTIM),debug)
LOCAL_CFLAGS+=-DNDEBUG -DLINUX -D_LINUX -D_LINUX_ANDROID -D__VO_NDK__ -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO
else
LOCAL_CFLAGS+=-DLINUX -D_LINUX -D_LINUX_ANDROID -D__VO_NDK__ -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO
endif

else
VOABOUT:=-DMODULE_NAME=\"$(strip $(LOCAL_MODULE))\" $(VOMINFO)
LOCAL_CFLAGS+=-DNDEBUG 
endif

# about info option, do not need to care it
LOCAL_CFLAGS+=-fPIC $(VOABOUT) $(VOMM) -fsigned-char -Wno-error=format-security -Wno-format-security -Wno-format -Wno-unused-but-set-variable  -Wno-unused-but-set-parameter 
LOCAL_CPPFLAGS+=-fPIC $(VOABOUT) $(VOMM) -fsigned-char -Wno-error=format-security -Wno-format-security -Wno-format -Wno-unused-but-set-variable  -Wno-unused-but-set-parameter -Wno-conversion-null

#For Log, Per Jason
ifeq ($(VOOPTIM), release)
LOCAL_CFLAGS+=-D_VONDBG
LOCAL_CPPFLAGS+=-D_VONDBG 
endif

