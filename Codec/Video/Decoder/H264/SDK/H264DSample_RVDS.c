 #ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		<limits.h>
#include		"voH264.h"
#include		"vimem.h"
#include "cmnMemory.h"
int	outTotalFrame = INT_MAX;
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
FILE* outFile=NULL;
int frameNumber=0;
#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))

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
int VO_API OutputOneFrame(VO_VIDEO_BUFFER *par,int width,int height)
{
	int i;
	char* buf;
	frameNumber++;
	if (!outFile)
		return 0;
	
	printf("output frame %d(%dx%d)\n",frameNumber,width,height);

	// write Y, U, V;
	buf  = par->Buffer[0];
	for(i=0;i<height;i++,buf+=par->Stride[0])
	{
		fwrite(buf, 1, width, outFile);
	}	
	width	/=2;
	height	/=2;
	buf	=  par->Buffer[1];
	for(i=0;i<height;i++,buf+=par->Stride[1])
	{
		fwrite(buf, 1, width, outFile); 
	}	
	
	buf	=  par->Buffer[2];
	for(i=0;i<height;i++,buf+=par->Stride[2])
	{
		fwrite(buf, 1, width, outFile); 
	}	
	fflush(outFile);
	return 0;
}

#ifndef WIN32
__asm void init_cpu() {
// Set up CPU state
	MRC p15,0,r4,c1,c0,0
	ORR r4,r4,#0x00400000 // enable unaligned mode (U=1)
	BIC r4,r4,#0x00000002 // disable alignment faults (A=0) // MMU not enabled: no page tables
	MCR p15,0,r4,c1,c0,0
#ifdef __BIG_ENDIAN
SETEND BE
#endif
	MRC p15,0,r4,c1,c0,2 // Enable VFP access in the CAR -
	ORR r4,r4,#0x00f00000 // must be done before any VFP instructions
	MCR p15,0,r4,c1,c0,2
	MOV r4,#0x40000000 // Set EN bit in FPEXC
	MSR FPEXC,r4

	IMPORT __main
	B __main
}
#endif
#define USER_MEM_OPERATOR_DEMO

#define SYMPLE_INPUT 0
#define MAX_SINGLE_INPUT  (200*1024)
#if SYMPLE_INPUT
#define READ_LEN 100*1024	//10K
#define INBUFFER_SIZE  READ_LEN
#else//SYMPLE_INPUT
#define READ_LEN 512
#define INBUFFER_SIZE (READ_LEN*1000)
#endif
int count_num_i = 1;
FILE *inFile, *outFile, *speedFile;
#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv) 
#endif // _WIN32_WCE
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
	int outTotalFrame = INT_MAX;
	int format = VO_H264_ANNEXB;
	int loopCount=1;
	int inSize = 0;
	int inFrame = 0;
	int	endOfFile = 0;
	int frameNum3[3]={0};
	int frameType;
	char* nextFrame,*currFrame;
	int leftSize=0;
	int size;
	long param;
	VO_CODEC_INIT_USERDATA userData;
	VO_MEM_OPERATOR memData;
	while(count_num_i){

#ifdef _WIN32_WCE
	DWORD start, finish;
	sprintf(configfile,"h264test_cfg_Hprofile.txt");
#else//_WIN32_WCE
	int i;
	clock_t start, finish;
	//return TestDeblcok();
	//strcpy(configfile,argv[1]); 
	strcpy(configfile,"E:\\vector\\H264\\h264test_cfg_Base_RVDS.txt");
#endif//_WIN32_WCE
	voGetH264DecAPI(&decApi,0);
	fconfig = fopen(configfile,"r");
	if(fconfig==NULL)
	{
		printf("The config file does not exist\n");
		return 0;
	}

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
			printf("\ninfile:%s\n",inFileName);
		if (outFileName[0] != '\0'&&!disableOutput)
			outFile = fopen(outFileName, "wb");
		else
			outFile = NULL;
		
		if (!outFile){
			printf("\nWarning: no output video file!");
		}

#ifdef _WIN32_WCE
		start = GetTickCount();
#else
		start = clock();
#endif
		// allocate input buffer;
		// initialization;
		//test the mem operator
		/* Initialize instance. */
#ifdef USER_MEM_OPERATOR_DEMO
		{   //Memory alloc and free outside decoder.
			VO_CODEC_INIT_USERDATA userData;
			cmnMemFillPointer( VO_VIDEO_CodingH264 );
			userData.memflag  = VO_IMF_USERMEMOPERATOR;
			userData.memData  = (VO_PTR)&g_memOP;
			returnCode = decApi.Init(&hCodec, VO_VIDEO_CodingH264, &userData);
		}
#else       // Default way, Memory alloc and free inside decoder. Set last parameter NULL.
		returnCode = decApi.Init(&hCodec, VO_VIDEO_CodingH264, NULL);
#endif

		if(returnCode != VO_ERR_NONE )
		{
			printf("\nError %d: Decoder start failed!", returnCode);
			goto End;
		}
		frameNumber = 0;
		param=VO_H264_ANNEXB;
		decApi.SetParam(hCodec,VO_ID_H264_STREAMFORMAT, &param);
		inputBuf = (char *)calloc(INBUFFER_SIZE ,1);
		currFrame = inputBuf;
		leftSize = fread(currFrame,1,INBUFFER_SIZE,inFile);
	

	while(frameNumber < outTotalFrame&&leftSize>4){
	
#if !SYMPLE_INPUT		
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
						int readSize;
						memmove(inputBuf,currFrame,leftSize);
						currFrame=inputBuf;
						readSize=fread(currFrame+leftSize,1,INBUFFER_SIZE-leftSize,inFile);
						leftSize+=readSize;
					}
				}
			} while(1);
#else//SYMPLE_INPUT
			
			inSize=leftSize;
			nextFrame=currFrame;
#endif
			
	
		inFrame++;
		
		inData.Buffer           = currFrame;
		inData.Length 			= inSize;
		inData.Time				= inFrame;
		leftSize-=inSize;
		currFrame=nextFrame;

		returnCode = decApi.SetInputData(hCodec,&inData);//voH264DecProcess(hCodec, &inData,&outData, &outFormat);
		if(returnCode)
		{
			printf("\nError %d: Decod failed!\n", returnCode);
			//break;
		}
		do 
		{
			if(decApi.GetOutputData(hCodec,&outData,&outFormat)==0&&outData.Buffer[0])
			{
				if(!disableOutput)
				{
					OutputOneFrame(&outData,outFormat.Format.Width,outFormat.Format.Height);
					printf("outFormat.Format.Type %d\n",outFormat.Format.Type);

				}
				//else
				//	frameNumber++;
			}
		} while (outFormat.Flag);

#if SYMPLE_INPUT
		leftSize=fread(currFrame,1,INBUFFER_SIZE,inFile);
#endif
	}

	decApi.Uninit(hCodec);//voH264DecUninit(hCodec);

#ifdef _WIN32_WCE
	finish = GetTickCount();
	duration = (finish - start) * 0.001;
#else
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//printf("\nH.264 decoder speed = %2.3f (fps)", frameNumber/duration);
	//printf("\n\ttotal frame = %d, total time = %2.3f seconds\n", 
		//frameNumber, duration);
#endif //_WIN32_WCE

	if (speedFile){
		fprintf(speedFile, "\nH.264 decoder speed = %2.3f (fps) (%dx%d)", frameNumber/duration,outFormat.Format.Width,outFormat.Format.Height);
		fprintf(speedFile, "\n\ttotal frame = %d, total time = %2.3f seconds\n", 
			frameNumber, duration);
		printf("\nH.264 decoder speed = %2.3f (fps) (%dx%d)", frameNumber/duration,outFormat.Format.Width,outFormat.Format.Height);
		printf("\n\ttotal frame = %d, total time = %2.3f seconds\n", 
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
	fclose(fconfig);
	fconfig= NULL;
 End:
	count_num_i--;
	}

	return 0;
}

