
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "voH264New.h"
//#include  "defines.h"
//#include "global.h"

#define DECOUTPUT_TEST      1
#define INT_MAX1 2147483647
#define ERROR_TEST	0
#define ERROR_SIZE	20  //error rate, X/1000

#define USE_CONFIG	0
#define DEFAULT_LOOP 1
#define USE_MULTI_FRAME 1
#define CPU_NUM		4
#define NAL_TIME 0

#if defined(ARM)
#define PRINT_OUTPUT_POC    0
#define BITSTREAM_FILENAME  "/sdcard/dump.H264"
#define DECRECON_FILENAME   "/sdcard/h264dec/curyuv/freh4.yuv"
#define ENCRECON_FILENAME   "test_rec.yuv"
#define DECOUTPUT_VIEW0_FILENAME  "H264_Decoder_Output_View0.yuv"
#define DECOUTPUT_VIEW1_FILENAME  "H264_Decoder_Output_View1.yuv"
#else
#define PRINT_OUTPUT_POC    0
//#define BITSTREAM_FILENAME  "E:\\TDDOWNLOAD\\Catherine_attchment\\Documents\\vomxVideo.dat"
#define BITSTREAM_FILENAME  "D:\\birds2.264"
//#define BITSTREAM_FILENAME  "D:\\test suite\\h264_testsuite\\mainclip\\CABAST3_Sony_E.jsv"
#define DECRECON_FILENAME   "D:\\decstream\\new.yuv"
#define ENCRECON_FILENAME   "test_rec.yuv"
#define DECOUTPUT_VIEW0_FILENAME  "H264_Decoder_Output_View0.yuv"
#define DECOUTPUT_VIEW1_FILENAME  "H264_Decoder_Output_View1.yuv"	
#endif
#if defined(ARM)
#define YU_TBD 1
#if !(defined(RVDS) || defined(_WIN32_WCE))
#if NAL_TIME
static unsigned long GetTime(){
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000000 + tval.tv_usec;
}
#else
static unsigned long GetTime(){
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
}
#endif
#endif
#else
#define YU_TBD 1

#ifndef MARKTIME

#ifdef WIN32

#include <windows.h>
#include <winbase.h>
#include <time.h>

static LARGE_INTEGER tmp1,tmp2;
#define MARKTIME(t)	\
	(QueryPerformanceCounter(&tmp2),t = tmp2.QuadPart/(double)tmp1.QuadPart)

#define MARKTIME_INIT(t)	\
	(QueryPerformanceFrequency(&tmp1),MARKTIME(t))
#else

#include		<time.h>

static struct timeval tval;
#define MARKTIME(t)	\
	(gettimeofday(&tval, NULL), t=tval.tv_sec + tval.tv_usec/1000000.0)

#define MARKTIME_INIT(t)	MARKTIME(t)

#endif

#endif

static unsigned long GetTime(){
	double t;
	MARKTIME_INIT(t);

	return (unsigned long)(t*1000);
}
#undef MARKTIME
#undef MARKTIME_INIT
#endif

static int	outTotalFrame = INT_MAX1;
static int inputWidth,inputHeight,cpuNumber = CPU_NUM;
static int test_type=4;
static char  configfile[256] = "cfg.txt";
static FILE* fconfig=NULL;
static int nDecodeNextFile=0;

typedef struct file_par
{
  char infile[255];                       //!< H.264 inputfile
  char outfile[255];                      //!< Decoded YUV 4:2:0 output
  char reffile[255];                      //!< Optional YUV 4:2:0 reference file for SNR measurement

} FileParameters;

#if ERROR_TEST
static void  MakeDataDirty(VO_CODECBUFFER *inBuffer,int nErrorSize, FILE *fDump)
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
		VO_U8 vu;

		nErrorPos = rand () % inBuffer->Length;
		vu = inBuffer->Buffer[nErrorPos];
		inBuffer->Buffer[nErrorPos] = rand () % 256;
		if(nErrorPos<0)
			printf(" vu %d; 222 =%d; %d;--;inBuffer->Length=%d\r\n", vu , inBuffer->Buffer[nErrorPos], nErrorPos,inBuffer->Length );

	}

	if (fDump)
	{
		fwrite(inBuffer->Buffer, inBuffer->Length, 1, fDump);
		fflush(fDump);
	}
}
#endif//ERROR_TEST



static void ParseCommand(FileParameters *p_Inp, int ac, char *av[])
{
  int CLcount;

  // Process default config file
  CLcount = 1;

  // Parse the command line
#if 1
  while (CLcount < ac)
  {
    if (0 == strncmp (av[CLcount], "-i", 2) || 0 == strncmp (av[CLcount], "-I", 2))  // A file parameter?
    {
      strncpy(p_Inp->infile, av[CLcount+1], 255);
      CLcount += 2;
    }  
    else if (0 == strncmp (av[CLcount], "-o", 2) || 0 == strncmp (av[CLcount], "-O", 2))  // A file parameter?
    {
      strncpy(p_Inp->outfile, av[CLcount+1], 255);
      CLcount += 2;
    } 
	else if (0 == strncmp (av[CLcount], "-c", 2) || 0 == strncmp (av[CLcount], "-C", 2))  // A file parameter?
	{
		sscanf(av[CLcount+1],"%d", &cpuNumber);
		CLcount += 2;
	} 
    else
    {
      //snprintf (errortext, ET_SIZE, "Error in command line, ac %d, around string '%s', missing -f or -p parameters?", CLcount, av[CLcount]);
      //error (errortext, 300);
		CLcount+=1;
    }
  }

#else
  sprintf(p_Inp->infile, "AUD_MW_E.264");
  sprintf(p_Inp->outfile, "out.yuv");
#endif
  printf ("\n");
}

static void Configure(FileParameters *p_Inp, int ac, char *av[])
{
  //char *config_filename=NULL;
  //char errortext[ET_SIZE];
  memset(p_Inp, 0, sizeof(FileParameters));
  strcpy(p_Inp->infile, BITSTREAM_FILENAME); //! set default bitstream name
  strcpy(p_Inp->outfile, DECRECON_FILENAME); //! set default output file name
  strcpy(p_Inp->reffile, ENCRECON_FILENAME); //! set default reference file name
  cpuNumber = CPU_NUM;

#if YU_TBD
  ParseCommand(p_Inp, ac, av);
#else
//   sprintf(p_Inp->infile, "/sdcard/h264dec/self264main/15032594_AVC_AVC'M50_6000K_1280x720'24f.264");
    sprintf(p_Inp->infile, "/sdcard/h264dec/self264main/14165110_MPEG4_AVC'M31_778K'4m12s'800x600'25f.264");
    //sprintf(p_Inp->infile, "D:\\decstream\\wrongclip\\10191849_AVC_AVC'E51_820x340'23f.264");
//  sprintf(p_Inp->infile, "/sdcard/YUV/DirtyRawData.dta");
//   sprintf(p_Inp->infile, "/sdcard/h264dec/self264main/13131978_MPEG4_AVC'M30_2462K'1m13s'848x352'30f.264");

  //sprintf(p_Inp->infile, "/sdcard/h264dec/main/CANL1_TOSHIBA_G.264");
  sprintf(p_Inp->outfile, "/sdcard/h264dec/out.yuv");
  //printf("%s\n",p_Inp->infile);
#endif
}
int fileFormat=0;//default is ANNEXB
FILE* outFile=NULL;
FILE *inFile = NULL,  *speedFile = NULL;
#define MAX_SINGLE_INPUT  (200*1024)

#define READ_LEN 2*1024
#define INBUFFER_SIZE (READ_LEN*1024)

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
static const VO_U8 VO_UE_VAL[512]={
	31,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9,10,10,10,10,11,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const VO_U8 VO_BIG_LEN_TAB[256]={
	0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

static signed long vo_big_len(unsigned long val)
{
	int len = 0;
	if (val & 0xffff0000) {
		val >>= 16;
		len += 16;
	}
	if (val & 0xff00) {
		val >>= 8;
		len += 8;
	}
	len += VO_BIG_LEN_TAB[val];

	return len;
}

static char* GetNextFrame(char* currPos,int size)
{
	char* endPos = currPos+size-4;
#if USE_MULTI_FRAME	//when using frame_thread mode we must send one complete frame into decode, otherwise we only send one nalu into decode
	char* p = currPos;  
	unsigned char * tmp_ptr;
	unsigned int bFind = 0;
	unsigned int value;
	if(fileFormat)
		return NULL;

	do 
	{
		for (; p < endPos; p++)
		{
			if (XRAW_IS_ANNEXB(p))
			{
				tmp_ptr = (unsigned char *)(p + 3);
				break;
			}
			if (XRAW_IS_ANNEXB2(p))
			{
				tmp_ptr = (unsigned char *)(p + 4);
				break;
			}
		}
		if(p>=endPos || tmp_ptr>=endPos)	//need 5 bytes to parse
			return NULL;
		value = (*tmp_ptr++)&0x1F;
// printf("find nalu, type %d\r\n", value);
		if (value == 1 || value == 5)	//slice or IDR
		{
			value = (((unsigned int)(*tmp_ptr))<<24) | ((unsigned int)(*(tmp_ptr+1))<<16) | ((unsigned int)(*(tmp_ptr+2))<<8) | ((unsigned int)*(tmp_ptr+3));
// printf("value %08X\r\n", value);
			if(value >=(1<<27))
			{
				value >>= 32-9;
				value = VO_UE_VAL[value];
			}
			else
			{
				value >>= 2*vo_big_len(value)-31;
				value--;
			}
// printf("start mb nr %d\r\n", value);
			if (value == 0)	//start_mb_nr = 0, find a new pic
			{
				if (bFind)
				{
					break;
				}
				else
				{
					bFind = 1;
				}
			}
		}
		else if (bFind)
		{
			break;
		}
		p+=3;
	} while (1);
#else
	char* p = currPos+3;  
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

#endif
	return p;
}
#undef XRAW_IS_ANNEXB
#undef XRAW_IS_ANNEXB2


int VO_API OutputOneFrame(VO_VIDEO_BUFFER *par,int width,int height,int frametype)
{
	int i;
	char* buf;
	if (!outFile)
		return 0;

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
// 		fscanf(fconfig,"%*[^\"]");

// 		fscanf(fconfig,"\"%[^\"]",speedFile);               // speed file
// 		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",outFrameNumber);           
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",loopCount);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&inputWidth);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&inputHeight);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&cpuNumber);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&nDecodeNextFile);          
		fscanf(fconfig,"%*[^\n]");
	}
}


/*!
 ***********************************************************************
 * \brief
 *    main function for JM decoder
 ***********************************************************************
 */
#if defined RVDS    
	int main(int argc, char *argv[])
#elif defined USE_GPROF
	int gprof_main(int argc, char *argv[])
#else
    int main(int argc, char *argv[])
#endif

{
  int iRet;
  int iFramesOutput=0, iFramesDecoded=0;
  char outSpeedName[256] = "voH264Dec_speed.txt";
  FileParameters InputParams={0};
  VO_VIDEO_DECAPI decApi={0};
  VO_HANDLE hCodec;
  VO_CODECBUFFER inData;
  VO_VIDEO_BUFFER  outData;
  VO_VIDEO_OUTPUTINFO outFormat;
  unsigned char *inputBuf;
  unsigned char* nextFrame,*currFrame;
  int leftSize=INBUFFER_SIZE;
  int inSize = 0;
  int disableOutput = !DECOUTPUT_TEST;
  int loop = DEFAULT_LOOP;
  int cfg = USE_CONFIG;
  double start, end;
  VO_S64 TimeStamp = 0;
#if NAL_TIME
  double nal_start,nal_end;
#endif
  double totaltime = 0;
  int loopcount = 0;
  fileFormat = 0;
  if(cfg)
  {
	  if (argc > 1)
	  {
		  strcpy(configfile,argv[1]);
	  }
	  fconfig = fopen(configfile,"r");
	  if (!fconfig)
	  {
		  printf("can not open config file :%s\n", configfile);
		  exit(0);
	  }
	  if (outSpeedName[0] != '\0')
	  {
		  speedFile = fopen(outSpeedName, "w");
	  }
	  else
		  speedFile = NULL;
  }
  do{
  //get input parameters;
  loopcount = 0;
  totaltime = 0;
  memset(&InputParams, 0, sizeof(InputParams));
  if(!cfg) 	
    Configure(&InputParams, argc, argv);
  else
  {
  	ParseConfig(InputParams.infile,InputParams.outfile,outSpeedName,&loop,&outTotalFrame);
  }
  if(cfg)
  {
	  if (speedFile){
		  fprintf(speedFile,"%s\r\n",InputParams.infile);
		  fprintf(speedFile,"Thread: %d, loop: %d\r\n", cpuNumber, loop);
	  }
	  else
	  {
		  printf("\nWarning: no output speed file!\n");
	  }
  }
  while(loopcount<loop)
  {
#if ERROR_TEST
	  FILE *fDump = NULL;
	  fDump = fopen("DirtyData.dta", "wb");
#endif
    //open decoder;
    printf("loop:%d\n",loopcount);
    iFramesOutput=0;
	iFramesDecoded=0;
	leftSize=INBUFFER_SIZE;
#if !(defined(RVDS) || defined(_WIN32_WCE))
    start = GetTime();
#endif
    voGetH264DecAPI(&decApi,0);

	inFile= fopen (InputParams.infile, "rb");
    if (!inFile)
    {
	  printf("\nError: cannot open input H.264 file!");
	  exit(0);
    }
	else
	  printf("\ninfile:%s\n",InputParams.infile);

    if (InputParams.outfile[0] != '\0'&&!disableOutput)	
    {
		printf("\noutfile:%s\n", InputParams.outfile);
	  outFile = fopen(InputParams.outfile, "wb");
// 	  if (!outFile)
// 	  {
// 	    printf("\nError: cannot open output file!");
// 	    exit(0);
// 	  }
    }
    else
    {
	  outFile = NULL;
	}

	if (outFile == NULL)
	{
		printf("\nWarning: no output video file!\n");
	}
	

    decApi.Init(&hCodec,VO_VIDEO_CodingH264,0);
	decApi.SetParam(hCodec, VO_PID_COMMON_CPUNUM, &cpuNumber);
printf("set cpu num %d\n", cpuNumber);
	decApi.SetParam(hCodec, VO_ID_H264_STREAMFORMAT, &fileFormat);

	fileFormat = 0;

    //iRet = OpenDecoder();
    //iRet = SetInputParameters(&InputParams);

    //iRet = decApi.SetParam(hCodec,VO_ID_H264_INPUTPARAM, &InputParams);
    //if(iRet != DEC_OPEN_NOERR)
    //{
      //fprintf(stderr, "Open encoder failed: 0x%x!\n", iRet);
    //  return -1; //failed;
    //}

    inputBuf = (char *)calloc(1, INBUFFER_SIZE);   //malloc input buffer
    nextFrame = currFrame = inputBuf;

    leftSize = fread(currFrame,1,INBUFFER_SIZE,inFile);
#if USE_MULTI_FRAME
	nextFrame = currFrame;
#else
	nextFrame = GetNextFrame(currFrame-3,leftSize); //get first nalu head
// printf("find first nalu, offset %d\r\n", nextFrame-currFrame);
#endif
    while(iFramesDecoded < outTotalFrame&&leftSize>4&&nextFrame)
    {
		inData.Buffer = (VO_PBYTE)nextFrame;
      //find next nalu head
      do 
	  {
	    nextFrame = GetNextFrame(nextFrame,leftSize);
	    if(nextFrame)
	    {
		  inSize = nextFrame-inData.Buffer;
// printf("find next nalu, frame %d, size %d\r\n", iFramesDecoded, inSize);

		  break;
	    }
	    else
	    {
		  if(leftSize>2*1024*1024)
		  {
		    printf("\nthe next frame is too big to handle,exit\n");
		    exit(0);
		  }
		  if(feof(inFile))//last nalu
		  {
		    inSize=leftSize;
// printf("find last nalu, frame %d, size %d\r\n", iFramesDecoded, inSize);
		    break;
		  }
		  else  //need to fill inputBuf
		  {
		    int readSize;
		    memmove(inputBuf,inData.Buffer,leftSize);
		    inData.Buffer = nextFrame =inputBuf;
		    readSize=fread(inputBuf+leftSize,1,INBUFFER_SIZE-leftSize,inFile);
		    leftSize+=readSize;
		  }
	    }
	  } while(1);
	  
	  
	  inData.Length = inSize;
	  inData.Time = TimeStamp++;
#if ERROR_TEST
	  MakeDataDirty(&inData, ERROR_SIZE, fDump);
#endif
	  iRet = decApi.SetInputData(hCodec,&inData);//voH264DecProcess(hCodec, &inData,&outData, &outFormat);
	  if(iRet)
	    printf("\nError %d: Decod failed!\n", iRet);
#if NAL_TIME
		nal_start= GetTime();
#endif

	  iRet=decApi.GetOutputData(hCodec,&outData,&outFormat);
#if NAL_TIME
		nal_end= GetTime();
      printf(" NAL time = %d\n", (int)(nal_end - nal_start));
#endif

	  leftSize-=outFormat.InputUsed;
	  if(outData.Buffer[0])
	  {
// printf("Timestamp %lld, frame %d\n", outData.Time, iFramesDecoded);
	    iFramesDecoded++;
		if (iFramesDecoded==82)
		{
			iFramesDecoded = iFramesDecoded;
		}
		//if (iFramesDecoded==22)
		//{
        //  decApi.SetParam(hCodec,VO_PID_COMMON_FLUSH,NULL);
		//}
		//{
		//	VO_IV_ASPECT_RATIO  info;
		//	decApi.GetParam(hCodec,VO_PID_VIDEO_ASPECTRATIO,&info);
		//}
        if(!disableOutput)
	    {
#if !(defined(RVDS)|| defined(ARM) )
			printf("frame: %d\n",iFramesDecoded);

#endif
		  OutputOneFrame(&outData,outFormat.Format.Width,outFormat.Format.Height,outFormat.Format.Type );
	    }
	    while(outFormat.Flag)	//there has next output
	    {
          iRet=decApi.GetOutputData(hCodec,&outData,&outFormat);
		  if(outData.Buffer[0]) 
		  {
			//the next output is ready
// printf("Timestamp %lld, frame %d\n", outData.Time, iFramesDecoded);
			  iFramesDecoded++;
              if (iFramesDecoded==87)
              {
				  iFramesDecoded = iFramesDecoded;
              }
			  if(!disableOutput)
			  {
#if !(defined(RVDS)|| defined(ARM) )
				  printf("frame: %d\n",iFramesDecoded);
#endif
				OutputOneFrame(&outData,outFormat.Format.Width,outFormat.Format.Height,outFormat.Format.Type );
			  }
			  
		  }
		  else
		  {
			  //the next output is not ready, just break
			  break;
		  }
	    }
	  }

    }
    //decApi.SetInputData(hCodec,&inData);
    decApi.SetParam(hCodec,VO_ID_H264_FLUSH_PICS, &outFormat);
    iRet=decApi.GetOutputData(hCodec,&outData,&outFormat);
    
	  while(outData.Buffer[0] || outFormat.Flag) //in the end of file, we must get all next output
	  {
		if (outData.Buffer[0])
		{
// printf("Timestamp %lld, frame %d\n", outData.Time, iFramesDecoded);
			iFramesDecoded++;
		
			if(!disableOutput)
			{
#if !(defined(RVDS) || defined(ARM))
				printf("frame: %d\n",iFramesDecoded);
#endif
			  OutputOneFrame(&outData,outFormat.Format.Width,outFormat.Format.Height,outFormat.Format.Type );
			}
			
		}
		else
		{
			//next output not ready, don't break in the end of file
// 			Sleep(0);
		}
		iRet=decApi.GetOutputData(hCodec,&outData,&outFormat);
	  }
#if !(defined(RVDS) || defined(_WIN32_WCE))
#if (CAL_DEBLOCK)
    fprintf(stdout," Total deblock time : %lld ms\n",((DecoderParams *)hCodec)->pDecGlobal->tot_deblock/1000);
#endif
#if(CAL_PARSER)
    fprintf(stdout," Total parser time : %lld ms\n",((DecoderParams *)hCodec)->pDecGlobal->tot_parser/1000);
#endif
#if(CAL_SLICE)
    fprintf(stdout," Total slice header time : %lld ms\n",((DecoderParams *)hCodec)->pDecGlobal->tot_slice/1000);
#endif

#if(CAL_HEADER)
    fprintf(stdout," Total header time : %lld ms\n",((DecoderParams *)hCodec)->pDecGlobal->tot_header/1000);
#endif

#if(CAL_MC)
    fprintf(stdout," Total mc time : %lld ms\n",((DecoderParams *)hCodec)->pDecGlobal->tot_mc/1000);
#endif  
  
#endif
	if(iFramesDecoded>1)
	{ VO_IV_ASPECT_RATIO info;
	decApi.GetParam(hCodec,VO_PID_VIDEO_ASPECTRATIO,&info);
	iRet = 0;
	}
    decApi.Uninit(hCodec);
#if !(defined(RVDS) || defined(_WIN32_WCE))
    end = GetTime();
#endif
#if ERROR_TEST
	if (fDump)
	{
		fclose(fDump);
	}
#endif
// 	printf("close in file, %p\n", inFile);
    fclose(inFile);
    if(!disableOutput)
    {
// 		printf("close out file, %p\n", outFile);
      if (outFile)
	  {
        fclose(outFile);
	  }
    }
    inFile=outFile=NULL;
// 	printf("in & out file NULL\n");
    printf("\n time = %d (%.3f fps) total frame: %d\n", (int)(end - start),iFramesDecoded*1000.0/(int)(end - start),iFramesDecoded);
    if(cfg && speedFile)
 	  fprintf(speedFile,"time%d = %d(%.3f)  ", loopcount,(int)(end - start),iFramesDecoded*1000.0/(int)(end - start));
	if(loopcount)
	{
	  totaltime+=(end - start);
	}
	if(loop == 1)
	  totaltime = (end - start);
	loopcount++;
	free(inputBuf);
  }
  loop = loop==1?loop:loop-1;
  totaltime /= loop;
  if(cfg && speedFile)
  {
    fprintf(speedFile,"\r\navgtime = %d (%.3f fps) total frame: %d\r\n\r\n", (int)totaltime,iFramesDecoded*1000.0/(int)(totaltime),iFramesDecoded);
	fflush(speedFile);
  }
  printf("next: %d\n",nDecodeNextFile);
  }while(nDecodeNextFile);
  if(cfg)
  {
	  if (speedFile)
	  {
		  fclose(speedFile);
	  }
	  if (fconfig)
	  {
		  fclose(fconfig);
	  }
  }
  return 0;
}


