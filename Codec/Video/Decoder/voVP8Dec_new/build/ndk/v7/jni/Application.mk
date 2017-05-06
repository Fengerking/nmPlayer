# Build both ARMv5TE and ARMv7-A machine code.
APP_CFLAGS := -O2
APP_ABI := armeabi-v7a

# release/debug
APP_OPTIM?= release

# VisualOn Info
VOMODVER ?= 3.0.0
VOBRANCH ?= trunk 
VOSRCNO ?= 18857

VOBUILDTOOL ?= NDKr7c
VOBUILDNUM ?= 0000
VOGPVER ?= 3.3.18
