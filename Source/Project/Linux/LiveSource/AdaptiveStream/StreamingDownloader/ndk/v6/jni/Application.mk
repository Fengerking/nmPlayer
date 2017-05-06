# Build both ARMv5TE and ARMv6-A machine code.
APP_CFLAGS := -O3
APP_ABI := armeabi

# release/debug
APP_OPTIM?= release

# VisualOn Info
VOMODVER ?= 3.0.0
VOBRANCH ?= trunk 
VOSRCNO ?= 15300

VOBUILDTOOL ?= NDKr6b
VOBUILDNUM ?= 0000
VOGPVER ?= 3.3.18
