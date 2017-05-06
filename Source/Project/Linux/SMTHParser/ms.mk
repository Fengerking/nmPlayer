
# please list all objects needed by your module here
OBJS:=  CvoBaseObject.o \
	voOSFunc.o \
	voCMutex.o \
	voThread.o \
	base64.o \
	strutil.o \
	fortest.o \
	vo_socket.o \
	vo_stream.o \
	vo_mem_stream.o \
	vo_thread.o	\
	vo_http_stream.o \
	CvoBaseDrmCallback.o \
	voDrmCallback.o	\
	CSMTHParser.o \
	CSMTHParserCtrl.o \
	voSmthParser.o   
	

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	../../../../../../Include \
		../../../../../../Common \
		../../../../../../Source/SMTHParser \
		../../../../../../Source/File/Common	\
		../../../../../../Source/MTV/LiveSource/ATSCMH/Common \
		../../../../../../Source/IISSmoothStreaming \
		../../../../../../Common/NetWork
