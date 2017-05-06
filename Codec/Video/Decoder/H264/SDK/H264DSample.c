 

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		<limits.h>
#include 		<stdarg.h>
#include		"h264dec.h"

//#include		"sharedYUVMem.h"
int 	outTotalFrame = INT_MAX;
#define TEST_IMX31_DEBLOCK  0
void *voH264Memset(void * s, int d, size_t e)
{
	memset(s,d,e);
}
void voPrintf(const char * format,...)
{
	
}
void voH264Memcpy(void * s, const void * d, size_t e)
{

	//memcpy(s,d,e);
	char* dst=(char*)s;
	char* src=(char*)d;
	while((e--)>0)
	{
	   *dst++=*src++;
	}
}
void voH264Memmove(void * s, const void * d, size_t e)
{
	//memmove(s,d,e);
	char* dst=(char*)s;
	char* src=(char*)d;
	while((e--)>0)
	{
	   *dst++=*src++;
	}
}
void* voH264Calloc(size_t s, size_t s2)
{
	return calloc(s, s2);
}
void voH264Free(void* d)
{
	free(d);
}
void  voH264Qsort(void * a, size_t b, size_t c,
           int (*d)(const void *, const void *))
{
	//qsort(a,b,c,d);
	printf("\nqsort!!!!\n");
}
#define VO_H264_DUMP 	

#ifdef VO_H264_DUMP

#define DUMP_SEQUENCE 							(1<<1) // sequence level dump;
#define DUMP_SLICE     							(1<<3) // slice (frame by frame);
#define DUMP_DEBLOCK							(1<<5) // result after deblocking;

FILE *logSeqFp, *logSlcFp, *logDbkFp;
char logPath[256], logSlcDir[256], logDbkDir[256];

void VOH264DumpInit()
{
	/*
	char fName[256];
	sprintf(logPath, "c:/H264Test/Dump/"); 
	//_mkdir(dmpPath);
	sprintf(fName,"%s/slice.txt",logPath);
	logSlcFp = fopen(fName, "w");
	sprintf(fName,"%s/deblock.txt",logPath);
	logDbkFp = fopen(fName, "w");

	sprintf(fName,"%s/sequence.txt",logPath);
	logSeqFp = fopen(fName, "w");
	*/
}

void VOH264DumpUninit()
{
	//fclose(logSeqFp);
}

void AvdLog(int logId, char *format, ...)
{
	FILE *fp = NULL;
	va_list arg;
	if (logId != DUMP_SEQUENCE && logId != DUMP_SLICE && 
		logId != DUMP_DEBLOCK)
		return;

	switch (logId){
		case DUMP_SEQUENCE:
			fp = logSeqFp;
			break;

		case DUMP_SLICE:
			fp = logSlcFp;
			break;

		case DUMP_DEBLOCK:
			fp = logDbkFp;
			break;

		default: 
			return;
	}
	if (!fp)
		return;

	va_start(arg, format);
	vfprintf(fp, format, arg);
	va_end(arg);
	fflush(fp);
}
#else// VO_H264_DUMP
#define VOH264DumpInit
#define VOH264DumpUninit
#endif // VO_H264_DUMP
static char configfile[256];
static FILE* fconfig;
static int nDecodeNextFile=0;
static void ParseConfig(char* inFile,char* outFile,char* speedFile,int* loopCount,int* outFrameNumber)
{

	if(fconfig)
	{
		fscanf(fconfig,"%*[^\"]");
		fscanf(fconfig,"\"%[^\"]",inFile);
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",outFile);               // RAW (YUV/RGB) output file
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",speedFile);               // speed file
		fscanf(fconfig,"%*[^\n]");


		fscanf(fconfig,"%d",outFrameNumber);           
		fscanf(fconfig,"%*[^\n]");
		
		fscanf(fconfig,"%d",loopCount);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&nDecodeNextFile);          
		fscanf(fconfig,"%*[^\n]");

		
	}
}
int GetFrameType(char * buffer)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len,value;
	int info_bit;
	int totbitoffset = 0;
	int naluType = buffer[0]&0x0f;
	if(naluType==5)
		return 2;
	if(naluType==1)//need continuous check
		buffer++;
	else//the nalu type is params info
		return naluType;
	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (buffer[byteoffset] & (0x01<<bitoffset));   // set up control bit

		len=1;
		while (ctr_bit==0)
		{                 // find leading 1 bit
			len++;
			bitoffset-=1;           
			bitcounter++;
			if (bitoffset<0)
			{                 // finish with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			ctr_bit=buffer[byteoffset] & (0x01<<(bitoffset));
		}
		// make infoword
		inf=0;                          // shortest possible code is 1, then info is always 0
		for(info_bit=0;(info_bit<(len-1)); info_bit++)
		{
			bitcounter++;
			bitoffset-=1;
			if (bitoffset<0)
			{                 // finished with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}

			inf=(inf<<1);
			if(buffer[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return -1;
	}
	//(int)pow(2,(bitsUsed/2))+info-1;//pow(2,x)==1<<x
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if (inf>=5)
	{
		inf-=5;
	}
	
	return inf;
}

FILE* outFile=NULL;
int frameNumber=0;
#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
#define MIN_FRAME_NUM 0
static char* GetNextFrame(char* currPos,int size)
{
	char* p = currPos+3;  
	char* endPos = currPos+size-8;
	for (; p < endPos; p++)
	{
		
		
		if (XRAW_IS_ANNEXB(p)||XRAW_IS_ANNEXB2(p))
		{
			break;
		}
	
	}
	if(p>=endPos)
		p = NULL;
	
	return p;
}
int VOCODECAPI CheckLicenseFile(void* handle)
{
	return 0;
}
#define MEM_OP_TEST 0
int VOCODECAPI OutputOneFrame(VOCODECVIDEOBUFFER *par,int width,int height)
{
	int i;
	char* buf;
	
	if (!outFile)
		return 0;
#if MEM_OP_TEST
	return SharedMemLock(par->data_buf[0]);
#endif
	frameNumber++;
	//printf("output frame %d(%dx%d),0x%X\n",frameNumber,width,height,par->data_buf[0]);
	//return 0;
	// write Y, U, V;
	buf  = par->data_buf[0];
	for(i=0;i<height;i++,buf+=par->stride[0])
	{
		fwrite(buf, 1, width, outFile);
	}	
	width	/=2;
	height	/=2;
	buf	=  par->data_buf[1];
	for(i=0;i<height;i++,buf+=par->stride[1])
	{
		fwrite(buf, 1, width, outFile); 
	}	
	
	buf	=  par->data_buf[2];
	for(i=0;i<height;i++,buf+=par->stride[2])
	{
		fwrite(buf, 1, width, outFile); 
	}	
	fflush(outFile);
	return 0;
}


#define READ_LEN 1024
#define INBUFFER_SIZE (READ_LEN*1024)
FILE *inFile, *outFile, *speedFile;
int frameNumber;
int main(int argc, char **argv) 
{
	char *inputBuf;
	HVOCODEC hCodec;
	VOH264DECRETURNCODE	returnCode;
	VOCODECDATABUFFER inData;
	VOCODECVIDEOBUFFER  outData;
	VOCODECVIDEOFORMAT outFormat;
	VOMEMORYOPERATOR customMem={0};
	char inFileName[256], outFileName[256], outSpeedName[256];
	double duration;
	int outTotalFrame = INT_MAX;
	int format = VOH264_ANNEXB;
	int loopCount=1;
	int inSize = 0;
	int inFrame = 0;
	int	endOfFile = 0;
	int IsUseCallBack = !MEM_OP_TEST;
	int frameNum3[3]={0};
	int frameType;
	char* nextFrame,*currFrame;
	int leftSize=0;
	int size;
	int naluType;
	char* sliceType[]={"P","B","I"};//,"I","I","I"};
	int i;
	
	clock_t start, finish;
	
	
	//return TestDeblcok();
	//strcpy(configfile,argv[1]); 
	//PORT_TODO:set the correct config file on your platform.The following is for windows
	//strcpy(configfile,"D:\\source_safe_project\\H264Codec\\utility\\new\\IA32\\h264test_cfg.txt");
#ifdef _WIN32_WCE
	sprintf(configfile,"/My Documents/h264test_cfg_s.txt");
#else//_WIN32_WCE
	strcpy(configfile,"D:\\SVN_PROJECT\\Numen\\trunk\\Codec\\Video\\Decoder\\H264\\testh264\\h264test_cfg_PC.txt");
#endif
	fconfig = fopen(configfile,"r");
	if(fconfig==NULL)
	{
		printf("The config file does not exist\n");
		return 0;
	}
	//TestCPP();return;
	VOH264DumpInit();
do
{
	int disableOutput;
	ParseConfig(inFileName,outFileName,outSpeedName,&loopCount,&outTotalFrame);
	if (outSpeedName[0] != '\0')
		speedFile = fopen(outSpeedName, "wb");
	else
		speedFile = NULL;
	
	if (!speedFile){
		printf("\nWarning: no output speed file!");
	}
	disableOutput = loopCount>1;
	frameNum3[0]=frameNum3[1]=frameNum3[1]=0;
	do 
	{
		endOfFile = 0;
		inFile= fopen (inFileName, "rb");
		if (!inFile){
			printf("\nError: cannot open input H.264 file!");
			exit(0);
		}
		else
		{
			printf("%s\n",inFileName);
		}

		if (outFileName[0] != '\0'&&!disableOutput)
			outFile = fopen(outFileName, "wb");
		else
			outFile = NULL;
		
		if (!outFile){
			printf("\nWarning: no output video file!");
		}


	
		

		// allocate input buffer;
		// initialization;
		returnCode = voH264DecInit(&hCodec);
		if(VORC_FAILED(returnCode))
		{
			printf("\nError %d: Decoder start failed!", returnCode);
			exit(0); 
		}
		frameNumber = 0;
		if(format==VOH264_AVC)
		{
			//inData.buffer=spsBuffer;
			//inData.length=sizeof(spsBuffer);
			//voH264DecSetParameter(hCodec, VOID_H264_STREAMFORMAT, format);
			//voH264DecSetParameter(hCodec, VOID_COM_HEAD_DATA,&inData);
		}
		else
		{
			format =  strstr(inFileName+strlen(inFileName)-4,".avc")?VOH264_14496_15:VOH264_ANNEXB;//VOH264_ANNEXB;//VOH264_14496_15;// 
			voH264DecSetParameter(hCodec, VOID_H264_STREAMFORMAT, format);
		}
#if MEM_OP_TEST
		if(MEM_OP_TEST)//
		{
			VOMEM_VIDEO_OPERATOR memOP;
			if(GetSharedMemOP(&memOP))
			{
				voH264DecSetParameter(hCodec, VOID_COM_VIDEO_SHAREDMEMORYOPERATOR, &memOP);
			}
		}
#endif	
	if(IsUseCallBack)
		voH264DecSetParameter(hCodec, VOID_H264_CALLBACK_OUT, (int)OutputOneFrame);
	//voH264DecSetParameter(hCodec, VOID_H264_DISABLEDEBLOCK, 1);
	//voH264DecSetParameter(hCodec, VOID_H264_DEBLOCKFLAG, 1);	

	if(format == VOH264_14496_15||format==VOH264_AVC)
	{
		// allocate input buffer;
		inputBuf = (char *)calloc(INBUFFER_SIZE * sizeof (char),1);
		//i = fread(inputBuf, 1, READ_LEN*50, inFile);
	}
	else
	{
		// allocate input buffer;
		inputBuf = (char *)calloc(INBUFFER_SIZE ,1);
	}
	currFrame = inputBuf;
	if(0)//fseek(inFile,4096*1024,SEEK_SET))
	{
		//printf("seek error :%s\n",strerror(errno));
	}
	leftSize = fread(currFrame,1,INBUFFER_SIZE,inFile);

	start = clock();
	while(frameNumber < outTotalFrame&&leftSize>4){
		if(format != VOH264_ANNEXB)
		{
			int size;
			if(format==VOH264_AVC)
			{
				int temp;
				static int totalSize=0;
				unsigned char* tempB=inData.buffer;
				size=fread(tempB,1,6,inFile);
				//inSize=(tempB[0]<<24)|(tempB[1]<<16)|(tempB[2]<<8)|(tempB[3]);
				tempB[6]='\0';
				inSize=atoi(tempB);
				printf("%d:size=%X\n",frameNumber ,inSize);
				inData.length = fread(inData.buffer, 1, inSize, inFile);
			

			}
			else
			{
				size = fread(&inSize,1,4,inFile);
				if(size<4)
					break;
				inData.length = fread(inData.buffer, 1, inSize, inFile);
			}
		
		}
		else
		{
			do 
			{
				nextFrame = GetNextFrame(currFrame,leftSize);
				if(nextFrame)
				{
					inSize = nextFrame-currFrame;
					break;
				}
				else
				{
					if(feof(inFile))
					{
						endOfFile = 1;
						inSize=leftSize;
						break;
					}
					else
					{
#if 0
						int readSize;
						int adjust = ((int)currFrame)&7;
						char* begin=inputBuf;
						leftSize+=adjust;
						memmove(begin,currFrame-adjust,leftSize);
						
						readSize=fread(inputBuf+leftSize,1,INBUFFER_SIZE-leftSize,inFile);
						currFrame=inputBuf+adjust;
						leftSize+=readSize-adjust;
#else
						int readSize;
						voH264Memmove(inputBuf,currFrame,leftSize);
						readSize=fread(inputBuf+leftSize,1,INBUFFER_SIZE-leftSize,inFile);
						currFrame=inputBuf;
						leftSize+=readSize;	
#endif
						if(readSize==0)
						{
							endOfFile = 1;
							inSize=leftSize;
							break;
						}
								
					}
				}
			} while(1);
		
		}	
	
		inFrame++;
		naluType=currFrame[4]&0x0f;
		frameType=GetFrameType(currFrame+4);
		if (naluType==5||naluType==1)
		{
			printf("Input: %d(size=%d) type=%s \n",inFrame,inSize,sliceType[frameType]);
		}
		else
		{
			//printf("Input: %d(size=%d) type=%d \n",inFrame,inSize,naluType);
		}
		inData.buffer         = currFrame;
		inData.length			 = inSize;
		leftSize-=inSize;
		currFrame=nextFrame;

#define DROP_SLICE_BEGIN	 5
#define DROP_SLICE_END		 12438
			//drop slice
			if (0)//inFrame>=DROP_SLICE_BEGIN&&inFrame<=DROP_SLICE_END)
			{
				//printf("drop pack=%d\n",inFrame);
				returnCode = 0;
			}
			else
				returnCode = voH264DecProcess(hCodec, &inData,&outData, &outFormat);
			if(VORC_FAILED(returnCode))
			{
#if MEM_OP_TEST
				printf("\nError %d: voH264DecProcess failed!\n", returnCode);
				while(returnCode==VOMEMRC_NO_YUV_BUFFER)
				{
					WaitForUnlockedBuf(10);
					returnCode=voH264DecGetParameter(hCodec,VOID_COM_VIDEO_SHAREDMEMWAITOUTPUT,&outData);
				}
#endif//
				if(outFormat.frame_type!=B_FRAME)//||returnCode==-109)
				{
					//printf("error: flush buffer\n");
					//voH264DecSetParameter(hCodec, VOID_H264_FLUSH, 0);
				}
				if (returnCode==-109)
				{
					//break;
				}
			}
			
		if(outData.data_buf[0]!=NULL&&IsUseCallBack==0)
		{
			OutputOneFrame(&outData,outFormat.width,outFormat.height);
		}
		
	}

	voH264DecUninit(hCodec);


	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("\nH.264 decoder speed = %2.3f (fps)", frameNumber/duration);
	printf("\n\ttotal frame = %d, total time = %2.3f seconds\n", frameNumber,duration);


	if (speedFile!=NULL)
	{
		fprintf(speedFile, "\nH.264 decoder speed = %2.3f (fps) (%dx%d)", frameNumber/duration,outFormat.width,outFormat.height);
		fprintf(speedFile, "\n\ttotal frame = %d, total time = %2.3f seconds\n", 
			frameNumber, duration);
		
	}

	free(inputBuf);
	fclose(inFile);
	if (outFile)
		fclose(outFile);
	inFile=outFile=NULL;
	} while(--loopCount>0);
	if(speedFile)
		fclose (speedFile);
	speedFile=NULL;
}while (nDecodeNextFile);
	fclose(fconfig);
	fconfig= NULL;
	VOH264DumpUninit();
	return 0;
}