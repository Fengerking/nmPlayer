# release/debug
APP_OPTIM?= release
# highest optimized level
APP_CFLAGS := -O3
APP_ABI := x86
# VisualOn Info
VOMODVER ?= 3.0.0
VOBRANCH ?= trunk 
VOSRCNO ?= 11803 



VOBUILDTOOL ?= NDKx86
VOBUILDNUM ?= 0000
VOGPVER ?= 3.3.18