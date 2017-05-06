# Build both ARMv5TE and ARMv7-A machine code.
APP_CFLAGS := -O3
APP_ABI := x86

# release/debug
APP_OPTIM?= release

# VisualOn Info
VOMODVER ?= 3.1.0.11
VOBRANCH ?= trunk 
VOSRCNO ?= 11803 

VOBUILDTOOL ?= NDKx86
VOBUILDNUM ?= 0000
VOGPVER ?= 3.3.18
