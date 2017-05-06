#ifdef _WIN32
#include "ParseSPS.h"
#endif // _WIN32

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		<limits.h>
#include		"voH264.h"
#include		"viMem.h"

#include "voOSFunc.h"

int	outTotalFrame = INT_MAX;
static char configfile[256];
static FILE* fconfig;
static int nDecodeNextFile=0;

static int inputWidth,inputHeight,cpuNumber = 1;
static int test_type=4;


#define ERROR_TEST 0
#if ERROR_TEST
static void  MakeDataDirty(VO_CODECBUFFER *inBuffer,int nErrorSize)
{
	int i ;
	int nErrorPos = 0;
	nErrorSize = inBuffer->Length * nErrorSize / 1000;
#ifdef _WIN32
	srand(clock());
#else
	srand(time(NULL));
#endif
	for (i = 0; i < nErrorSize; i++)
	{

		nErrorPos = rand () % inBuffer->Length;
		VO_U8 vu = inBuffer->Buffer[nErrorPos];
		inBuffer->Buffer[nErrorPos] = rand () % 256;
		if(nErrorPos<0)
			printf(" vu %d; 222 =%d; %d;--;inBuffer->Length=%d\r\n", vu , inBuffer->Buffer[nErrorPos], nErrorPos,inBuffer->Length );
		
	}

}
#endif//ERROR_TEST
typedef struct  
{
	int id;
	int value;
}TParamItem;
typedef struct{
	TParamItem item[64];
	int		   itemNum;
}TParams;
static TParams voH264Params;
static char tmpBuf[1024];
static void ParseParams()
{
#if defined(VOARMV6) || defined(VOARMV7) || defined(WIN32)
    voH264Params.itemNum = 0;
    TParamItem *item = voH264Params.item + voH264Params.itemNum;
	item->id = VO_ID_H264_STREAMFORMAT;
	item->value = 0;
	voH264Params.itemNum++;

	item = voH264Params.item + voH264Params.itemNum;
	item->id = VO_ID_H264_MULTICORE_NUM;
	item->value = cpuNumber;
	voH264Params.itemNum++;
	
#else
	if(fconfig)
	{
		voH264Params.itemNum = 0;
		fscanf(fconfig,"%[^\r\n]",tmpBuf);
		
		while (strstr(tmpBuf,"TestFileList")==NULL) 
		{
			int id, value;
			int ret = sscanf(tmpBuf,"id=%X  value=%d",&id,&value);
			if(ret==2)
			{
				TParamItem *item = voH264Params.item + voH264Params.itemNum;
				item->id = id;
				item->value = value;
				voH264Params.itemNum++;
			}
			ret = fscanf(fconfig,"\n%[^\r\n]",tmpBuf);
		} 

	}
#endif	
}
static void ParseConfig(char* inFile,char* outFile,char* speedFile,int* loopCount,int* outFrameNumber)
{
	
	nDecodeNextFile = 0;//default is 0
	if(fconfig){
		//printf("file pos=%d\n",ftell(fconfig));
		fscanf(fconfig,"%*[^\"]");
		fscanf(fconfig,"\"%[^\"]",inFile);
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",outFile);               // RAW (YUV/RGB) output file
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",speedFile);               // speed file
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&inputWidth);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&inputHeight);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&test_type);          
		fscanf(fconfig,"%*[^\n]");
		
		fscanf(fconfig,"%d",outFrameNumber);           
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",loopCount);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&cpuNumber);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&nDecodeNextFile);          
		fscanf(fconfig,"%*[^\n]");

	}
}
FILE* outFile=NULL;
int frameNumber=0;

#define MEM_OP_TEST	0
static char* sFrameType[]={"I","P","B"};
//#define SAVE_BITSTREAM_TOOUT
int VO_API OutputOneFrame(VO_VIDEO_BUFFER *par,int width,int height,int frametype)
{
	int i;
	char* buf;
	frameNumber++;
	if (!outFile)
		return 0;
#ifdef SAVE_BITSTREAM_TOOUT
	return 0;
#endif
#if MEM_OP_TEST1
	return SharedMemLock(par->Buffer[0]);
#endif

	//printf("output frame %d:(ts=%d)(%dx%d)\n",frameNumber,(long)par->Time,width,height);
	//printf("output frame %d:(ts=%d)(%s)\n",frameNumber,(long)par->Time,sFrameType[frametype]);

	//return 0;
	// write Y, U, V;
	buf  = (char*)par->Buffer[0];
	for(i=0;i<height;i++,buf+=par->Stride[0])
	{
		fwrite(buf, 1, width, outFile);
	}	
	width	/=2;
	height	/=2;
	buf	=  (char*)par->Buffer[1];
	for(i=0;i<height;i++,buf+=par->Stride[1])
	{
		fwrite(buf, 1, width, outFile); 
	}	

	buf	=  (char*)par->Buffer[2];
	for(i=0;i<height;i++,buf+=par->Stride[2])
	{
		fwrite(buf, 1, width, outFile); 
	}	
	fflush(outFile);
	return 0;
}

#define MAX_SINGLE_INPUT  (200*1024)

#define READ_LEN 4096
#define INBUFFER_SIZE (READ_LEN*1024)

VO_U32 VO_API  Alloc4 (VO_S32 uID, VO_MEM_INFO * pMemInfo)
{
	pMemInfo->VBuffer=malloc(pMemInfo->Size);
	printf("%X: allocate %X size=%d\n",uID,pMemInfo->VBuffer,pMemInfo->Size);
	return 0;
}
VO_U32 VO_API  Free4 (VO_S32 uID, VO_PTR pBuff)
{
	free(pBuff);
	printf("%X: Free %X\n",uID,pBuff);
	return 0;
}

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))

//#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#//define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
static int currentFrameType = -1;
static int nextFrameType = -1;
int fileFormat=0;//default is ANNEXB
int isRawbitstream = 1;
static char* GetNextFrame(char* currPos,int size)
{
	char* p = currPos+3;  
	char* endPos = currPos+size-8;
	for (; p < endPos; p++)
	{
		if (XRAW_IS_ANNEXB(p)||XRAW_IS_ANNEXB2(p))
		{
			if(fileFormat==0)
				break;
		}
	}
	if(p>=endPos)
		p = NULL;

	return p;
}
FILE *inFile,  *speedFile;
static char frameTypeStr[]={'I','P','B','S','N'};
typedef VO_S32  (VO_API *voGetH264DecAPIFunc) (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);
voGetH264DecAPIFunc pvoGetH264DecAPI=NULL;
#ifdef LINUX//
#define USE_DLL 0
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif//
#if USE_DLL
void* h264Moudle = NULL;
#if  defined(VOARMV6)
#define H264DLLPATH "/data/local/libvoH264Dec_v6.so"
#elif defined(VOARMV7)
#define H264DLLPATH "/data/local/libvoH264Dec_v7.so"
#else
#define H264DLLPATH "/data/local/libvoH264Dec.so"
#endif
int LoadH264DLL()
{
	h264Moudle = dlopen (H264DLLPATH, RTLD_NOW);
	if(h264Moudle==NULL)
	{
		printf("load %s fail,%s\n",H264DLLPATH,strerror(errno));
		return -1;
	}
	return 0;
}
int GetGetH264DecAPI()
{
	pvoGetH264DecAPI=(voGetH264DecAPIFunc)dlsym(h264Moudle, "voGetH264DecAPI");
	if(pvoGetH264DecAPI==NULL)
	{
		printf("Get voGetH264DecAPI fail,%s\n",H264DLLPATH,strerror(errno));
		return -1;
	}
	return 0;
}
void FreeH264DLL()
{
	printf("before free dll\n");
	dlclose (h264Moudle);
	printf("after free dll\n");
}
#else//USE_DLL
int LoadH264DLL()
{
	return 0;
}
int GetGetH264DecAPI()
{
	pvoGetH264DecAPI=voGetH264DecAPI;
	return 0;
}
void FreeH264DLL()
{

}
#endif//USE_DLL

#if !(defined(RVDS) || defined(_WIN32_WCE))
static unsigned long GetTime(){
#ifdef WIN32
       return voOS_GetSysTime();
#else
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
#endif	
}
#endif



#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
static void ParseConfig2(int argc, char **argv, char* inFile,char* outFile,char* speedFile,int* loopCount,int* outFrameNumber)
{
     int result;
	 int n;
	 nDecodeNextFile = 0;
	 while( (result = getopt(argc, argv, "i:o:s:l:n:c:")) != -1 )
	 {
	 
//	 printf("result=?, optopt=%c, optarg=%s\n", optopt, optarg);

           switch(result)
          {
               case 'i':
			   	   sprintf(inFile, "%s", optarg);
                   break;
              case 'o':
			  	   sprintf(outFile, "%s", optarg);
                   break;
              case 's':
			  	   sprintf(speedFile, "%s", optarg);
                   break;
              case 'l':
			  	    sscanf(optarg, "%d",  loopCount);
                    break;
			  case 'n':
			  	    sscanf(optarg, "%d",  outFrameNumber);
                    break;	
			  case 'c':			  	    
			  	    sscanf(optarg, "%d",  &cpuNumber);
                    break;		
			  case 'w':
				    sscanf(optarg, "%d",	&inputWidth);			  	
                    break;
			  case 'h':			  	
				    sscanf(optarg, "%d",  &inputHeight); 			  
                    break;	
			  case 't':
			  	    sscanf(optarg, "%d",  &test_type); 	
                    break;			
              default:
                   break;
           }
    }	
}
#endif


int main(int argc, char **argv) 
{
	char *inputBuf;
	VO_HANDLE hCodec;
	int		returnCode;
	VO_CODECBUFFER inData;
	VO_VIDEO_BUFFER  outData;
	VO_VIDEO_OUTPUTINFO outFormat;
	VO_VIDEO_DECAPI decApi={0};
	char inFileName[256], outFileName[256], outSpeedName[256];
	double duration;
	long thumbduration;
	int outTotalFrame = INT_MAX;
	int format = VO_H264_ANNEXB;
	int loopCount=1;
	int inSize = 0;
	int inFrame = 0;
	int	endOfFile = 0;
	int frameNum3[3]={0};
	int frameType;
	char* nextFrame,*currFrame;
	int leftSize=INBUFFER_SIZE;//maks no sense, just for consistent with !hasSizeHead case asdf;
	int size;
	long param;
	int deblockRowNum = 0;
	int deblockDualCore = 0;
	int coreNum = 0;
	int optFlag = 0;
	int mulMBs = 0;
	VO_CODEC_INIT_USERDATA userData;
	VO_MEM_OPERATOR memData;

	int i, dataFrame;
#if !(defined(RVDS) || defined(_WIN32_WCE))
       double start = 0.0, end = 0.0;
       long thumbstart = 0, thumbend = 0;
#else
	clock_t start, finish;
#endif
	//return testdef();
	//strcpy(configfile,argv[1]); 
	//testArray();
	
	if(LoadH264DLL())
	{
		return 0;
	}
	if(GetGetH264DecAPI())
	{
		return 0;
	}
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
	strcpy(configfile,"/sdcard/h264_testsuite/cfg/vo264_test.cfg");
#else//LINUX
	//E:\H264MainTestSuite\refactor\refactor_hp.txt
	if (argc>=3&&argv[1]&&atoi(argv[2])==1)
	{
		strcpy(configfile,argv[1]);
	}
	else
	{
		strcpy(configfile,"D:/work/svn/h264_testsuite/cfg/vo264_test.cfg");
	}
	//if(coreNum==0) mulMBs = 1;//this is for compare with JM
		
#endif//LINUX

	pvoGetH264DecAPI(&decApi,0);
	fconfig = fopen(configfile,"r");
	if(fconfig==NULL)
	{
		printf("Test  The config file does not exist\n");
		if(argc < 2)
		   return 0;
	}

	do
	{
		int disableOutput;
		int hasSizeHead = 0;
		memset(outSpeedName, 0, sizeof(outSpeedName));
		memset(outFileName, 0, sizeof(outFileName));
		memset(inFileName, 0, sizeof(inFileName));
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)	  
		if(argc > 2)
			ParseConfig2(argc, argv, inFileName,outFileName,outSpeedName,&loopCount,&outTotalFrame);
		else
#endif				
		     ParseConfig(inFileName,outFileName,outSpeedName,&loopCount,&outTotalFrame);

#ifndef RVDS
	       ParseParams();
#endif

		if (outSpeedName[0] != '\0')
			speedFile = fopen(outSpeedName, "wb");
		else
			speedFile = NULL;

		//hasSizeHead = strstr(inFileName,".vo264");

		if (!speedFile){
			printf("\nWarning: no output speed file!");
		}
#if  defined(VOARMV6) || defined(VOARMV7)		
		disableOutput = 1;
#else
        disableOutput = loopCount>1;
#endif
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
				printf("\ninfile:%s\n",inFileName);
			if (outFileName[0] != '\0'&&!disableOutput)
			{
#ifdef TEST_VERSION

				strcat(outFileName,"2");
#endif			
				outFile = fopen(outFileName, "wb");



			}
			else
				outFile = NULL;

			if (!outFile){
				printf("\nWarning: no output video file!");
			}

#ifdef _WIN32_WCE
			start = GetTickCount();
#else
#if !(defined(RVDS) || defined(_WIN32_WCE)  )
            start = GetTime();
#else
			start = clock();
#endif
#endif
			// allocate input buffer;
			// initialization;
			//test the mem operator
			if (1)
			{
				memData.Alloc = Alloc4;
				memData.Free = Free4;
				userData.memData=&memData;
			}
			returnCode = decApi.Init(&hCodec,VO_VIDEO_CodingH264,0);//&userData);//voH264DecInit(&hCodec);
			if(returnCode)
			{
				printf("\nError %d: Decoder start failed!", returnCode);
				exit(0); 
			}

			frameNumber = 0;
			//fileFormat=param=VO_H264_ANNEXB;////
			//printf("init done\n");
			//decApi.SetParam(hCodec,VO_ID_H264_STREAMFORMAT, &param);
			//decApi.SetParam(hCodec,0x40000007,NULL);
			//param = 0;
			//decApi.SetParam(hCodec,VO_ID_H264_ENABLE_JUMP2I, &param);
			//param = 1;
			//decApi.SetParam(hCodec,VO_ID_H264_ENABLE_PARTICAIL_DECODE, &param);
			//
			//decApi.SetParam(hCodec,VO_ID_H264_ENABLE_FIRST_FRAME_NON_INTRA, &param);
			//param = 100;decApi.SetParam(hCodec,VO_ID_H264_ENABLE_OUTPUTASAP,&param);
			
			//param = 1;decApi.SetParam(hCodec,VO_ID_H264_STREAMINGMODE, &param);
			//param = VHSF_GET_T35_USERDDATA;
			//decApi.SetParam(hCodec,VO_ID_H264_ENABLE_SEI, &param);

#ifndef TEST_VERSION
			//set params
			{
				int num = voH264Params.itemNum;
				for(i=0;i<num;i++)
				{
					int id		= voH264Params.item[i].id;
					int value   = voH264Params.item[i].value;
					decApi.SetParam(hCodec,id, &value);
					printf("SetParam id=%X,val=%d\n",id,value);
				}
			}			
#endif

#if 1//def DISABLE_DEBLOCK
			param=1;
			decApi.SetParam(hCodec,VO_ID_H264_DISABLEDEBLOCK, &param);
			param=OPT_DISABLE_DECODE;
			decApi.SetParam(hCodec,VO_ID_H264_OPT_FLAG, &param);			
#endif//
			inputBuf = (char *)calloc(INBUFFER_SIZE ,1);
			nextFrame = currFrame = inputBuf;
			if(!hasSizeHead)
			{
				leftSize = fread(currFrame,1,INBUFFER_SIZE,inFile);
				currFrame = GetNextFrame(currFrame-3,leftSize);	
			}

			//for test
			{
				int width,height;	
				//GetSizeInfo((unsigned char*)currFrame+4,120,&width,&height);
				//printf("(%d,%d)",width,height);
			}
			//GetFrameType(currFrame,leftSize);
			
			
			
			
			
			dataFrame = 0;
			while(frameNumber < outTotalFrame&&leftSize>4){


				if(hasSizeHead)
				{
					int size1= fread(currFrame,1,4,inFile);
					if(size1!=4)
						break;
					inSize = *(int*)currFrame;
					size1= fread(currFrame,1,inSize,inFile);
					if(size1!=inSize)
						break;
					leftSize = INBUFFER_SIZE;//maks no sense, just for consistent with !hasSizeHead case
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
							if(leftSize>512*1024)
							{
								printf("\nthe next frame is too big to handle,exit\n");
								exit(0);
							}
							if(feof(inFile))
							{
								endOfFile = 1;
								inSize=leftSize;
								break;
							}
							else
							{
								int readSize;
								memmove(inputBuf,currFrame,leftSize);
								currFrame=inputBuf;
								readSize=fread(currFrame+leftSize,1,INBUFFER_SIZE-leftSize,inFile);
								leftSize+=readSize;
							}
						}
					} while(1);
				}
				inFrame++;
				inData.Buffer           = (VO_PBYTE)currFrame;
				inData.Length 			= inSize;
				inData.Time				= inFrame;
				//returnCode =decApi.GetParam(hCodec,VO_PID_VIDEO_FRAMETYPE,&inData);
				//printf("Input: %d(size=%d),type=%C\n",inFrame,inData.Length,frameTypeStr[inData.Time]);

				{
					char* sliceType[]={"P","B","I"};//,"I","I","I"};
					int naluType=currFrame[4]&0x0f;
					//int frameType=GetFrameType(currFrame+4,inSize);
					if (naluType==5||naluType==1)
					{
						
						if(0)//dataFrame++<2200) 
						{
							leftSize-=inSize;
							currFrame=nextFrame;
							continue;
						}
					}
					else
					{
						//printf("Input: %d(size=%d) type=%d \n",inFrame,inSize,naluType);
					}
				}

				leftSize-=inSize;
				currFrame=nextFrame;
#if ERROR_TEST
				//MakeDataDirty(&inData,2);
#endif
				returnCode = decApi.SetInputData(hCodec,&inData);//voH264DecProcess(hCodec, &inData,&outData, &outFormat);
				if(returnCode)
				{
					printf("\nError %d: Decod failed!\n", returnCode);
					//break;
				}

                if(1)
                {
#ifdef _WIN32_WCE
				    thumbstart = GetTickCount();
#else
#if !(defined(RVDS) || defined(_WIN32_WCE)  )
				    thumbstart = GetTime();
#else
				    thumbstart = clock();
#endif
#endif
                }

				//
				//while(((returnCode=decApi.GetOutputData(hCodec,&outData,&outFormat))==0)&&outData.Buffer[0])
				returnCode=decApi.GetOutputData(hCodec,&outData,&outFormat);
				while(outData.Buffer[0])
				{
					if(0)
					{ //test params
						VO_CODECBUFFER buf;
						decApi.GetParam(hCodec,VO_ID_H264_T35_USERDATA,&buf);
						if(buf.Length>0)
						{
							printf("Get one T35 user data,len=%d\n",buf.Length); 
						}
					}
					if(!disableOutput)
					{
						OutputOneFrame(&outData,outFormat.Format.Width,outFormat.Format.Height,outFormat.Format.Type );
						//printf("outFormat.Format.Type %d\n",outFormat.Format.Type);
//						printf("frame %d\n", frameNumber);
					}
					else
					{
						frameNumber++;
	                    if(outFormat.Format.Type == VO_VIDEO_FRAME_I)
                        {

#ifdef _WIN32_WCE
				            thumbend = GetTickCount();
#else
#if !(defined(RVDS) || defined(_WIN32_WCE)  )
				            thumbend = GetTime();
#else
				            thumbend = clock();
#endif
#endif
                            printf("I slice  thumbstart %d, thumbend %d, duration %d\n", thumbstart, thumbend, thumbend - thumbstart);
                        }					
	//					printf("frame %d outFormat.Format.Type %d\n", frameNumber, outFormat.Format.Type);
					}

					if(1)
					{
#ifdef _WIN32_WCE
						thumbstart = GetTickCount();
#else
#if !(defined(RVDS) || defined(_WIN32_WCE)  )
						thumbstart = GetTime();
#else
						thumbstart = clock();
#endif
#endif
					}

					returnCode=decApi.GetOutputData(hCodec,&outData,&outFormat);
				}
				if(returnCode&&returnCode!=VO_ERR_INPUT_BUFFER_SMALL&&outFormat.Format.Type!=VO_VIDEO_FRAME_B)
				{
					int value = 1;
					decApi.SetParam(hCodec,VO_PID_COMMON_FLUSH,&value);
					//printf("flushing\n");
				}

                if(1)
                {

#ifdef _WIN32_WCE
				    thumbend = GetTickCount();
#else
#if !(defined(RVDS) || defined(_WIN32_WCE)  )
				    thumbend = GetTime();
#else
				    thumbend = clock();
#endif
#endif
                    printf("I slice thumbstart %d, thumbend %d, duration %d\n", thumbstart, thumbend, thumbend - thumbstart);
                }
				

			}

			decApi.GetParam(hCodec,VO_ID_H264_FLUSH_PICS, &outFormat);
			returnCode=decApi.GetOutputData(hCodec,&outData,&outFormat);

    if(outData.Buffer[0])
    {
	     frameNumber++;
#if !(defined(RVDS) || defined(ARM))
         printf("frame: %d\n",frameNumber);
#endif
	  

        if(!disableOutput)
	    {
	        OutputOneFrame(&outData,outFormat.Format.Width,outFormat.Format.Height,outFormat.Format.Type );
	    }
	    while(outFormat.Flag)
	    {
            returnCode=decApi.GetOutputData(hCodec,&outData,&outFormat);
	        frameNumber++;
#if !(defined(RVDS) || defined(ARM))
        printf("frame: %d\n",frameNumber);
#endif
		
            if(!disableOutput)
	        {
		      OutputOneFrame(&outData,outFormat.Format.Width,outFormat.Format.Height,outFormat.Format.Type );
	        }
	      }
      }			

			decApi.Uninit(hCodec);//voH264DecUninit(hCodec);
			//decApi.SetParam(hCodec,VO_PID_COMMON_FLUSH,&param);	


 
#ifdef _WIN32_W	CE
			finish = GetTickCount();
			duration = (finish - start) * 0.001;
#else
#if !(defined(RVDS) || defined(_WIN32_WCE) )
            end = GetTime();
#else
			finish = clock();
#endif
#if !(defined(RVDS) || defined(_WIN32_WCE))
            duration = (end - start);
            printf("end %2.3f start %2.3f\n", end, start);
#else
			duration = (double)(finish - start) / CLOCKS_PER_SEC;
#endif
			printf("\nH.264 decoder speed = (%2.3f fps) (%dx%d)", frameNumber * 1000 /duration,outFormat.Format.Width,outFormat.Format.Height);
			printf("\n\ttotal frame = %d, total time = %2.3f  micro seconds(%d,%d)\n", 
				frameNumber, duration , outFormat.Format.Width,outFormat.Format.Height);
#endif //_WIN32_WCE

			if (speedFile){
				fprintf(speedFile, "\nH.264 decoder speed = %2.3f (fps) (%dx%d)", frameNumber/duration,outFormat.Format.Width,outFormat.Format.Height);
				fprintf(speedFile, "\n\ttotal frame = %d, total time = %2.3f seconds\n", 
					frameNumber, duration);

				//fclose (speedFile);
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

    if(fconfig)
	    fclose(fconfig);
	fconfig= NULL;
	FreeH264DLL();
	#if 0//ndef ARM
	_CrtDumpMemoryLeaks();
	#endif//
	//Armv7Copy16x16();
	return 0;
}

