
/*********************************************************************************************************************************
Microsoft Windows Media
Copyright (C) VisualOn Corporation, All rights reserved.

File: voWMVDecSample.c

Notes: This is a sample application which demonstrates how to properly exercise the WMV API set. 
       This is not considered production quality code.

Usage Notes:
  
  Config file : It contains testing parameters. It should follow following format.
				"D:\Data_Cmp\RCV\xxx.rcv"    ..input file 
				"D:\Data_Cmp\xxx.yuv"     	 ..output file,YUV420 data.if you do not want to output yuv for speed test,set it as an invalid directory
				"D:\Data_Cmp\xxx.txt"  	     ..output file,Performance testing result.
				100	                         ..decodeFrameNumber //the decoded frame number you want,if you want to decode all,set it as a big number,say 100000
				1		                     ..testCount  //for speed test,it is better to set it greater than 3
				0		                     ..1 go on testing the next file,0:exit

  Input file  : RCV file. It is video raw data dumped from WMV file by a special tool.
			 The file syntax is the following format:
			 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    -----------------------------------------------------------
			   Type   | Number of frames | Bytes in ext data | ext data | Frame Height | Frame Width | Bytes in frame 0 | Frame 0 Time Stamp | Frame 0 data | Bytes in frame 1 | Frame 1 Time Stamp | Frame 1 data | ...... | Bytes in frame N | Frame N Time Stamp | Frame N data |
			  (1Byte) |    (3 bytes)     |     (4 bytes)     |          |  (4 bytes)   |  (4 bytes)  |    (4 bytes)     |    (4 bytes)       |              |     (4 bytes)    |    (4 bytes)       |              |        |     4 bytes)     |    (4 bytes)       |              |
			 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    -----------------------------------------------------------
  Output file : 1. Binary file with YUV420 data decoded. 
			    2. Performance testing result file. 

***********************************************************************************************************************************/           
#include "stdio.h"
#include "voVC1.h" 
//#include "cmnMemory.h" 
#include <stdlib.h> 
#include <math.h> 


#ifdef VOANDROID
	#include<pthread.h>
	#include <semaphore.h>
    #include <unistd.h>
    #include<sys/time.h>
    #include<sys/resource.h>

	#define THREAD_FUNCTION  void*
	#define thread_sleep(nms) usleep(nms*1000)
#endif
//#define USER_MEM_OPERATOR_DEMO

//#define VIDEO_MEM_SHARE_DEMO
//#define STABILITY 1
//#define ADD_ERROR_DATA 1

//#define CONF_FILE 1
#ifdef VOANDROID
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
static unsigned long timeGetTime(){
  struct timeval tval;
  gettimeofday(&tval, NULL);
  return tval.tv_sec*1000 + tval.tv_usec/1000;
}
#endif // VOWINCE

#define	SUPPORT_VC1_INPUT

#ifdef _WIN32_WCE 
#define PPC2005				   //Define PPC2003, PPC200
#endif

#ifdef CONF_FILE
#if  defined(_win32_) || defined(RVDS)
#define		CONFIG_FILE_NAME	"D:/svn/Numen/trunk/SampleCode/voWMVDec_Test_cfg.txt"
#else
#define		CONFIG_FILE_NAME	"\\Storage Card\\voWMVDec_Test_cfg.txt"
#endif
#endif

#if defined(PPC2003) || defined(PPC2005)
#include <windows.h>
#endif

#ifdef VIDEO_MEM_SHARE_DEMO
#include "memTest.h" 
#endif

#define IN_BUFF_LEN            (1024 * 1024 * 2)
#define IN_BUFF_LEN2           (IN_BUFF_LEN/2)

int   OutputOneFrame( VO_VIDEO_BUFFER *par, VO_VIDEO_OUTPUTINFO* outFormat ,FILE*outFile);
int   GetHeadParam  ( VO_BYTE *pInputBuff, int lInBuffLen, VO_BITMAPINFOHEADER *pBmpInfo );
void  ParseConfig   ( char* inFile, char* outFile, char* speedFile, int* loopCount, int* outFrameNumber );
unsigned int WMVQueryMem(int iImageWidth, int iImageHeight);

#ifdef SUPPORT_VC1_INPUT
int g_InputType;		//0 is RCV,1 is VC1.
#define ASF_BINDING 1
#endif

FILE  *inFile, *outFile, *speedFile;
int   frameNumber;
int	  iRcvVer = 0;
char  configfile[256];
FILE* fconfig;
int   nDecodeNextFile=0;
int	  iFrameType = 0;
int	  leftLen = 0;

#ifdef RVDS
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


typedef struct {
	VO_U32 bInterlacedSource;//for WVC1
	VO_U32 bSeqInterpolation;//for WMV3
	VO_U32 bNumBFrames;//for WMV3
	VO_U32 bPreProRange;//for WMV3
}
SEQHEADERINFO;
//
//pSample : sequence header data
//nSize : sequence header data length
//pSeqInfo : will be used by GetMWVFrameType

//note: used for WMV3 WMVA,WVC1.(WMV1,WMV2 does not need,because they have no B frames)
VO_U32 GetWMVSeqHeaderInfo(VO_PBYTE pSample, VO_U32 nSize,SEQHEADERINFO *pSeqInfo)
{
	VO_BYTE b=0,b1=0;	
	VO_PBYTE ptr=NULL;

  //
	//if(*pSample != 0x00 || *(pSample+1) != 0x00 || *(pSample+2) != 0x01 || *(pSample+3) != 0x0f)
    if(*(pSample+1) != 0x00 || *(pSample+2) != 0x00 || *(pSample+3) != 0x01 || *(pSample+4) != 0x0f)
	{//WMV3
		if(nSize<44)
			return -1;
		ptr = pSample+40;
		b = *(ptr+2);
		b1 = *(ptr+3);
		pSeqInfo->bPreProRange = (b1&0x80);
		pSeqInfo->bNumBFrames  = (b1&0x70); //25 bit
		pSeqInfo->bSeqInterpolation = (b1&0x02);		
		return 0;
	}
	else
	{//WVC1
		if(nSize < 10 )
			return -1;
		b = *(pSample + 9);				
		pSeqInfo->bInterlacedSource= (b&0x40);
		return 0;
	}	
}

//
//pSample: frame header data
//nSize:   frame header length
//pSeqInfo : info from Sequence header
//note: used for WMV3 WMVA,WVC1.(WMV1,WMV2 does not need,because they have no B frames)
VO_VIDEO_FRAMETYPE GetMWVFrameType(VO_PBYTE pSample, VO_U32 nSize,SEQHEADERINFO pSeqInfo )
{
    VO_BYTE b[6];
    VO_PBYTE ptr;

	if(*pSample == 0x00 && *(pSample+1) == 0x00 && *(pSample+2) == 0x01 && *(pSample+3) == 0x0d) //WVC1
	{
				if(nSize<5)
					return -1;
        ptr = pSample+4;
        b[0] = (*ptr)&0x80;b[1] = (*ptr)&0x40;
        b[2] = (*ptr)&0x20;b[3] = (*ptr)&0x10;
        b[4] = (*ptr)&0x08;b[5] = (*ptr)&0x04;
        if(pSeqInfo.bInterlacedSource)
        {            
            if( b[0] == 0)
            {
                 if(b[1] ==0 )
                    return VO_VIDEO_FRAME_P;
                else if(b[2] ==0 )
                    return VO_VIDEO_FRAME_B;
                else if(b[3] ==0 )
                    return VO_VIDEO_FRAME_I;
                else if(b[4] == 0)//BI actually
                    return VO_VIDEO_FRAME_B;
                else//skip
                    return VO_VIDEO_FRAME_S;
            }
            else 
            {
                if(b[2] ==0 )
                    return VO_VIDEO_FRAME_P;
                else if(b[3] ==0 )
                    return VO_VIDEO_FRAME_B;
                else if(b[4] ==0 )
                    return VO_VIDEO_FRAME_I;
                else if(b[5] == 0)//BI actually
                     return VO_VIDEO_FRAME_B;
                else//skip
                    return VO_VIDEO_FRAME_S;              
            }
        }
        else
        {
            if(b[0] ==0 )
                return VO_VIDEO_FRAME_P;
            else if(b[1] ==0 )
                return VO_VIDEO_FRAME_B;
            else if(b[2] ==0 )
                return VO_VIDEO_FRAME_I;
            else if(b[3] == 0)//BI actually
                return VO_VIDEO_FRAME_B;
            else//skip
                return VO_VIDEO_FRAME_S;
        }
	}
	else//WMV3
	{		
        VO_U32 pos=0;
        if(nSize<1)
            return -1;
        ptr = pSample;        
        b[0] = *ptr & 0x80;b[1] = *ptr & 0x40;
        b[2] = *ptr & 0x20;b[3] = *ptr & 0x10;
        b[4] = *ptr & 0x08;b[5] = *ptr & 0x04;
        if(pSeqInfo.bSeqInterpolation)
            pos += 1;

        pos += 2;
        if(pSeqInfo.bPreProRange)
            pos += 1;

        if(b[pos] )
            return VO_VIDEO_FRAME_P;
        else
        {
            if(pSeqInfo.bNumBFrames ==0)
                 return VO_VIDEO_FRAME_I;
            else
            {
                if(b[pos+1])
                    return VO_VIDEO_FRAME_I;
                else
                    return VO_VIDEO_FRAME_B;
            }
        }
	}
}


////////////////////////////////////////////


#if defined (PPC2005)

int _tmain(int argc, _TCHAR* argv[])

#elif defined (PPC2003)

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPWSTR     lpCmdLine,
                   int        CmdShow)
#else               
int		 main( int argc, char **argv)
#endif
{
	VO_BYTE               *inputBuf;
	VO_HANDLE           hCodec;
	VO_U32				returnCode;
	VO_CODECBUFFER		inData;
	VO_CODECBUFFER		inDataHead;
	VO_VIDEO_BUFFER		outData;
	VO_VIDEO_OUTPUTINFO outFormat;
	VO_VIDEO_BUFFER     prevOutData;
	VO_VIDEO_DECAPI		voVC1Dec;
	char                inFileName[256], outFileName[256], outSpeedName[256];
	double              duration,mfps = 0.0;
	int                 outTotalFrame = 1000000;
	int                 tempLen, fileSize = 0;
	int                 header = 1;
	unsigned char      *pHeaderParam = NULL;
	int                 iFrameLen = 0;
	unsigned char      *pTmp = NULL;
	int                 iLenAdd = 0;
	int					loopCount = 30;
	int					decoderCalledCount = 0;

    int timestap = 0;
	int aspectratio=1;

	unsigned int start = 0, finish = 0;
    unsigned int  CPUNUM = 2,framenum_ext = 6, outputmode = 1;

    unsigned char * error_data = NULL;
    char errorfile[250];

    SEQHEADERINFO SeqHeaderInfo;

#define MAX_HEADER_SIZE	1024

#ifdef VIDEO_MEM_SHARE_DEMO
	VO_MEM_VIDEO_OPERATOR  videoMemOperator;
#endif
	VO_VIDEO_BUFFER pframebuffer[12];


	printf("\n>>>>>>>>>>>>>>>>>>> WMV Decoder Start >>>>>>>>>>>>>>>>>>>\n");

#ifdef CONF_FILE
	sprintf(configfile, CONFIG_FILE_NAME);

	fconfig = fopen(configfile,"r");
	if(fconfig == NULL) {
		printf("Can not open the config file %s\n", configfile);
		return 0;
	}
#endif

	/* Decode one file per loop. */
	do 
	{
		int disableOutput;
#ifdef CONF_FILE
		ParseConfig(inFileName,outFileName,outSpeedName,&loopCount,&outTotalFrame);
        //sprintf(error_file,"%s_error.rcv",inFileName);
#else

		inFileName[0] = outFileName[0] = outSpeedName[0]='\0';
		loopCount =1;
		/*if(argc!= 5)
		{
			printf("\nUsage: voWMVDec.exe intputFile outputFile");
			return;
		}
		else*/
		{
#ifdef VOWINCE
            strcpy(inFileName, "/My Storage/vc1_007_AP_Prog_D0B0M0U0V0Q0P0L1E0e0R0r0_176x144.rcv");
			strcpy(outFileName, "/My Storage/vc1_007_AP_Prog_D0B0M0U0V0Q0P0L1E0e0R0r0_176x144.yuv");
			CPUNUM = 1;//*argv[3] -0x30;
			loopCount =  1;//(*argv[4][0] -0x30)*100 + (*argv[4][1] -0x30)*10 + (*argv[4][2] -0x30);
#else
			strcpy(inFileName, argv[1]);
			strcpy(outFileName, argv[2]);
			CPUNUM = *argv[3] -0x30;
			loopCount = *argv[4] -0x30;
#endif
		}

#endif

		if (outSpeedName[0] != '\0') {
			speedFile = fopen(outSpeedName, "wb");
		}
		else {
			speedFile = NULL;
		}
		if (!speedFile){
			printf("\nWarning: no output speed file!");
		}
		disableOutput = loopCount > 1;
		decoderCalledCount = 0;

		/* Decode one time of the same file per loop. Decode more than one times for performance testing. */
		do 
		{
			frameNumber = 0; 
			start = finish = 0;
			printf("\n--------Time %d, decoding file %s \n" , decoderCalledCount,inFileName);


			printf("inFileName = %s \n",inFileName);
			inFile = fopen ( inFileName, "rb" );
			if (!inFile) {
				printf("\nError: Cannot open input testing file! \n");
				exit(0);
			}

			if (outFileName[0] != '\0' && !disableOutput) {
				outFile = fopen(outFileName, "wb");
			} 
			else {
				outFile = NULL;
			}

			if (!outFile) {
				printf("\nWarning: no output video file! \n");
			}

            sprintf(errorfile,"%s_%derrordata.rcv",inFileName,loopCount); 

			pHeaderParam = (unsigned char *)malloc (sizeof(VO_BITMAPINFOHEADER) + MAX_HEADER_SIZE);
			if ( !pHeaderParam ) {
				goto End2;	
			}

			memset( &inData, 0, sizeof(inData) );
			memset( &inDataHead, 0, sizeof(inDataHead) );
			memset( &outData, 0, sizeof(outData) );
			memset( &outFormat, 0, sizeof(outFormat) );
			memset( &prevOutData, 0, sizeof(prevOutData) );
			memset( pHeaderParam, 0, sizeof(VO_BITMAPINFOHEADER) + MAX_HEADER_SIZE );

			fseek(inFile, 0, SEEK_END);
			fileSize = ftell(inFile);
			fseek(inFile, 0, SEEK_SET);

            //error_data = (VO_BYTE *)malloc(fileSize * sizeof (VO_BYTE));

			/* allocate input buffer. */ 
			inputBuf = (VO_BYTE *)malloc(IN_BUFF_LEN * sizeof (VO_BYTE));
			if ( !inputBuf ) {
				goto End2;	
			}

			if( !(leftLen = fread(inputBuf, 1, IN_BUFF_LEN, inFile)) ) {
				goto End2;
			}

			g_InputType = 0;
			if( IsInputTypeVC1(inputBuf) ){
				int nRC = GetElementaryChunk(inputBuf, &inDataHead, pHeaderParam);
				g_InputType = 1;				
				iFrameLen = inDataHead.Length - ASF_BINDING;
				if ( /*-1 == nRC ||*/ 0 >= iFrameLen ) {
					goto End2;
				}
			}
			else 
			{
				/* Get the header parameter for voWMV9DecSetParameter(). */
				iFrameLen = GetHeadParam ( inputBuf, leftLen, (VO_BITMAPINFOHEADER *)pHeaderParam );
				if ( -1 == iFrameLen ) {
					goto End2;
				}
				inDataHead.Buffer = (VO_PBYTE)pHeaderParam;
				inDataHead.Length = ((VO_BITMAPINFOHEADER *)pHeaderParam)->biSize; 
			}

			/* get decoder api */
			returnCode = voGetVC1DecAPI(&voVC1Dec, 0);
			if(returnCode != VO_ERR_NONE){
				goto End;
			}

			/* Initialize instance. */
#ifdef USER_MEM_OPERATOR_DEMO
			{   //Memory alloc and free outside decoder.
				VO_CODEC_INIT_USERDATA userData;
				cmnMemFillPointer( VO_VIDEO_CodingVC1 );
				userData.memflag  = VO_IMF_USERMEMOPERATOR;
				userData.memData  = (VO_PTR)&g_memOP;

				returnCode = voVC1Dec.Init(&hCodec, VO_VIDEO_CodingVC1, &userData);
				if( returnCode != VO_ERR_NONE ) {
					goto End;
				}
			}
#else       // Default way, Memory alloc and free inside decoder. Set last parameter NULL.
			returnCode = voVC1Dec.Init(&hCodec, VO_VIDEO_CodingVC1, NULL);
			if( returnCode != VO_ERR_NONE ) {
				goto End;
			}
#endif


/******************************************************************************/
/******************************************************************************/
			if(0){
				FILE * fp =fopen("E:/MyResource/Video/clips/VC1/vc1_007_AP_Prog_D0B1M0U0V0Q1P0L1E0e0R0r0_176x144_IP.yuv","wb");
				fclose(fp);
			}

			//if(loopCount%2 == 0)
			//	CPUNUM = 2;
			//else
			//	CPUNUM = 1;
			//printf("%d ---------- \n",CPUNUM);
			//CPUNUM = 2;
			returnCode = voVC1Dec.SetParam (hCodec, VO_PID_COMMON_CPUNUM,&CPUNUM);
			//framenum_ext = 1;
		    //returnCode = voVC1Dec.SetParam (hCodec, VO_PID_COMMON_FRAME_BUF_EX,&framenum_ext);

			outputmode = 0;
			returnCode = voVC1Dec.SetParam (hCodec, VO_PID_VIDEO_OUTPUTMODE,&outputmode);
#if 1
			{
				FILE* outFile2 =fopen(outFileName,"wb");
				fclose(outFile2);
			}
#endif

			/* Set header parameters. */

#ifdef VIDEO_MEM_SHARE_DEMO
			GetVideoMemOperatorAPI(&videoMemOperator);
			returnCode = voVC1Dec.SetParam (hCodec, VO_PID_VIDEO_VIDEOMEMOP, (VO_PTR)&videoMemOperator );
			if( returnCode != VO_ERR_NONE ) {
				goto End;
			}
#endif
           //GetWMVSeqHeaderInfo(inDataHead.Buffer, inDataHead.Length,&SeqHeaderInfo);

#ifdef ADD_ERROR_DATA
            if(1){
                VO_BYTE* errorBuf = inDataHead.Buffer;//(VO_BYTE *)malloc(IN_BUFF_LEN * sizeof (VO_BYTE));
				unsigned long i,errorpos,errordata;		
				unsigned long readsize = inDataHead.Length;

				/*for(i=0;i<3*(readsize/10);i++)
				{
					errorpos = (unsigned long)rand()%readsize;
					errordata = (int)rand()%256;
					errorBuf[errorpos] = errordata;
				}*/

				if(g_InputType)
                {
                    FILE* fp =fopen(errorfile,"wb");
                    fwrite(inDataHead.Buffer+1,inDataHead.Length-1,1,fp);
                    fclose(fp);
                }
				else
				{
					FILE* fp =fopen(errorfile,"wb");
                    fwrite(inDataHead.Buffer,inDataHead.Length,1,fp);
                    fclose(fp);
				}
            }
#endif
           {
               returnCode = voVC1Dec.SetParam (hCodec, VO_PID_COMMON_HEADDATA, (VO_PTR)&inDataHead );
               if( returnCode != VO_ERR_NONE ) {
                   goto End;
               }  
           }


			inData.Buffer = inputBuf + iFrameLen;
			fileSize -= iFrameLen;
			leftLen  -= iFrameLen;

#ifdef CONF_FILE
			/* Decode one frame of a file per loop. */
			while( (fileSize>0) && (frameNumber < outTotalFrame) ) {
#else
			while( (fileSize>0) && (frameNumber < outTotalFrame) ) {
#endif

				/* Initialize input structure for voWMV9DecProcess().*/
#ifdef SUPPORT_VC1_INPUT
				if( g_InputType ){
					int nRC = GetElementaryChunk(inData.Buffer, &inData, NULL);
					iLenAdd = 0;
					iFrameLen = inData.Length;
				}
				else 
#endif
				{
					pTmp = inData.Buffer;
					iFrameLen  = *pTmp++;      
					iFrameLen |= (*pTmp++) << 8;
					iFrameLen |= (*pTmp++) << 16;
					iFrameLen |= (*pTmp++) << 24;
					iLenAdd = 4;
					/* RCV is V2 format. */
					if ( 1 == iRcvVer ) { 
						iFrameLen &= 0x0fffffff;
						/* Jump the dwTimeStamp. */
						iLenAdd += 4;
                        timestap  = *pTmp;                

					    timestap |= (*(pTmp+1)) << 8;
					    timestap |= (*(pTmp+2)) << 16;
					    timestap |= (*(pTmp+3)) << 24;

					}
					inData.Length  = iFrameLen;
					inData.Buffer += iLenAdd;
				}


				returnCode = voVC1Dec.GetParam(hCodec, VO_PID_VIDEO_FRAMETYPE, &inData);
                //iFrameType = GetMWVFrameType(inData.Buffer, inData.Length,SeqHeaderInfo);  
				//returnCode = voVC1Dec.GetParam(hCodec, VO_PID_VIDEO_FRAMETYPE, &inData);
    //            if( returnCode != VO_ERR_NONE ) {
    //                goto End;
    //            } 
				iFrameType = inData.Time;

               //printf("frameNumber = %d  %d***\n",frameNumber,iFrameType);

               //printf(" %d  \n",iFrameType);
	   //     
				//inData.Time = frameNumber;/* Just for timestamp testing.*/

				/* for debug */
				if( frameNumber == 63)
				{
					frameNumber += 0;
				}

#		if defined(PPC2003) || defined(PPC2005)
				start = GetTickCount();	    
#elif  VOANDROID  
				start = timeGetTime();
#else
				start = clock();	    
#endif

				/* Decode one frame and get output parameters. */
#ifdef ADD_ERROR_DATA
            srand((int)time(0));
            //if(frameNumber == 1)
            {
                VO_BYTE* errorBuf = inData.Buffer;//(VO_BYTE *)malloc(IN_BUFF_LEN * sizeof (VO_BYTE));
				unsigned long i,errorpos,errordata;		
				unsigned long readsize = inData.Length;
                FILE*fp=NULL;   
               // memcpy(error_data,inData.Buffer,inData.Length);

				for(i=0;i<1*(readsize/50);i++)
				{
					errorpos = (unsigned long)rand()%readsize;
					errordata = (int)rand()%256;
					errorBuf[errorpos] = errordata;
                    //printf(" %d  %d",errordata,errorpos);
				}
                //printf("\n");

                {
                    FILE* fp =fopen(errorfile,"ab");
                    fwrite(inData.Buffer,inData.Length,1,fp);
                    fclose(fp);
                }            
            }
#endif
           {
			   if(frameNumber == 42)
				   frameNumber =frameNumber;
			   if(frameNumber == 199)
				   frameNumber =frameNumber;

				returnCode = voVC1Dec.SetInputData(hCodec, &inData);
            }          
		   
		   returnCode = voVC1Dec.GetOutputData(hCodec, &outData, &outFormat);   

			//if(pOutPutInfo->Flag)
			//returnCode = voVC1Dec.GetParam (hCodec, VO_PID_VIDEO_ASPECTRATIO,&aspectratio);	
				
				if( returnCode == VO_ERR_NONE && outData.Buffer[0])
                {
					if(!disableOutput) 
						OutputOneFrame(&outData, &outFormat,outFile);
				}
#if 1
				if(CPUNUM >1)
				{
					while(outFormat.Flag)
					{
						returnCode = voVC1Dec.GetOutputData(hCodec, &outData, &outFormat);
						if( returnCode == VO_ERR_NONE && outData.Buffer[0])
						{
							if(!disableOutput)
								OutputOneFrame(&outData, &outFormat,outFile);
						}
						//thread_sleep(1);
						//printf("#----");
					}
				}
#endif
#if defined(PPC2003) || defined(PPC2005)
				finish += (GetTickCount() - start);	    
#elif  VOANDROID 
				finish += (timeGetTime() - start);
#else
				finish += (clock() - start);	    
#endif	
				frameNumber ++;

				/* Backup the previous frame's output parameters. */ 
				prevOutData.Buffer[0] = outData.Buffer[0];
				prevOutData.Buffer[1] = outData.Buffer[1];
				prevOutData.Buffer[2] = outData.Buffer[2];
				prevOutData.Stride[0] = outData.Stride[0];
				prevOutData.Stride[1] = outData.Stride[1];
				prevOutData.Stride[2] = outData.Stride[2];
				prevOutData.ColorType = outData.ColorType ;

				iLenAdd += iFrameLen;
				fileSize -= iLenAdd;
				leftLen  -= iLenAdd;

				/* Update input structure. */
				if( (fileSize > leftLen) && (leftLen > 0) && (leftLen < IN_BUFF_LEN2) ) {
					memmove(inputBuf, inputBuf + (IN_BUFF_LEN - leftLen),  leftLen);
					tempLen       = fread(inputBuf + leftLen, 1, (IN_BUFF_LEN - leftLen), inFile);
					leftLen       = tempLen + leftLen;
					inData.Buffer = inputBuf;
				} 
				else {
					inData.Buffer += iFrameLen;
				}

                /*{
                    FILE* fp= fopen("E:/MyResource/Video/clips/WMV/frames_types.txt","a");
                    fprintf(fp," frameNumber= %d  timestamp= %d   frametype : %d \n",frameNumber,timestap,iFrameType);
                    fclose(fp);
                }*/
				//printf("======= %d frameNumber= %d  Type : %d  %d %d\n",returnCode,frameNumber,
				//	outFormat.Format.Type,outFormat.Format.Width,outFormat.Format.Height);
                //if(frameNumber == 1)
                //    goto End;
                //printf("frameNumber = %d  %d\n",frameNumber,iFrameType);
				
			}

			if(0){
				int kk =0;
				for(kk=0;kk<framenum_ext;kk++)
				{
					FILE* outFile2;
					char filename[256];
					
					VO_U8* Y = pframebuffer[kk].Buffer[0];
					VO_U8* U = pframebuffer[kk].Buffer[1];
					VO_U8* V = pframebuffer[kk].Buffer[2];
					VO_U32 width = outFormat.Format.Width;
					VO_U32 height = outFormat.Format.Height;
					VO_U32 stridey = outData.Stride[0];
					VO_U32 strideuv = outData.Stride[1];
					//sprintf(filename,"E:/MyResource/Video/clips/VC1/176x144_sharebuffer.yuv",(frameNumber/6)*6+kk);
					outFile2 =fopen(outFileName,"ab");

					if(Y||U||V)
						OutputOneFrame2( Y, U, V, width,height, stridey, strideuv,outFile2);

					pframebuffer[kk].Buffer[0] = NULL;
					pframebuffer[kk].Buffer[1] = NULL;
					pframebuffer[kk].Buffer[2] = NULL;
					fclose(outFile2);

				}
			}


			//printf("##### Get last frame \n");
			/* Get last frame.*/	
			if(CPUNUM >1)  //out put the frame if available in thread
			{			
				VO_U32 islastframe = 1;
				returnCode = voVC1Dec.SetParam (hCodec, VO_PID_DEC_VC1_SET_LASTVIDEOFLAG,&islastframe);
				returnCode = voVC1Dec.GetOutputData(hCodec, &outData, &outFormat);
				if( returnCode == VO_ERR_NONE && outData.Buffer[0])
				{
					if(!disableOutput)
						OutputOneFrame(&outData, &outFormat,outFile);
				}

				//printf("###### outFormat.Flag =%d \n",outFormat.Flag);

				while(outFormat.Flag)
				{
					returnCode = voVC1Dec.GetOutputData(hCodec, &outData, &outFormat);
					if( returnCode == VO_ERR_NONE && outData.Buffer[0])
					{
						if(!disableOutput)
							OutputOneFrame(&outData, &outFormat,outFile);
					}
				}
			}
#if 1
			returnCode = voVC1Dec.GetParam(hCodec, VO_PID_DEC_VC1_GET_LASTVIDEOBUFFER, &outData);      
			returnCode = voVC1Dec.GetParam(hCodec, VO_PID_DEC_VC1_GET_LASTVIDEOINFO, &outFormat); 
			if(returnCode ==0)
			{
				if(!disableOutput)
					OutputOneFrame(&outData, &outFormat,outFile);
			}
#endif
			
End:
			if (outFile) {
				fclose(outFile);
			}

           // printf(" Uninit \n" );			

			duration = (double)finish * 0.001;

            printf("\nframeNumber =%d,speed = %2.3f (fps)",frameNumber, frameNumber/duration);

            mfps += frameNumber/duration;

            /* Uninitialize instance. */
			voVC1Dec.Uninit(hCodec);

			if (speedFile) {
				fprintf(speedFile, "\nspeed = %2.3f (fps)", frameNumber/duration);
				fprintf(speedFile, " total frame = %d, total time = %2.3f seconds. (%dx%d)\n", frameNumber, duration,outFormat.Format.Width, outFormat.Format.Height);
			}

End2: 
#ifdef VOWINXP  
            if(errorfile){
                    FILE* fp =fopen(errorfile,"wb");
                    fclose(fp);
                    remove(errorfile);
            }
#endif
			if( inputBuf ) 
				free(inputBuf);

			if (inFile) 
				fclose(inFile);

			if( pHeaderParam ) 
				free( pHeaderParam );   

			printf(" \n ----------------------------------- \n ");

			decoderCalledCount ++;
		} while( --loopCount > 0 );

        printf(" \n\n decode %d times, Average speed : %2.3f fps  \n ",decoderCalledCount, mfps/decoderCalledCount);

		if (speedFile) {
			fclose (speedFile);
		}

	} while (nDecodeNextFile);

#ifdef CONF_FILE
	fclose(fconfig);
	fconfig= NULL;
#endif

	printf("\n\n>>>>>>>>>>>>>>>> WMV Decoder End >>>>>>>>>>>>>>>>>\n");
	return 0;
}


int GetHeadParam ( VO_BYTE *pInputBuff, int lInBuffLen, VO_BITMAPINFOHEADER *pBmpInfo )
{
#define MAKEFOURCC_WMV(ch0, ch1, ch2, ch3) \
        ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) |   \
        ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))

#define mmioFOURCC_WMV(ch0, ch1, ch2, ch3)  MAKEFOURCC_WMV(ch0, ch1, ch2, ch3)

#define FOURCC_WMV1_WMV     mmioFOURCC_WMV('W','M','V','1') /* Window Media Video v7 */
#define FOURCC_wmv1_WMV     mmioFOURCC_WMV('w','m','v','1')
#define FOURCC_WMV2_WMV     mmioFOURCC_WMV('W','M','V','2') /* Window Media Video v8 */
#define FOURCC_wmv2_WMV     mmioFOURCC_WMV('w','m','v','2')
#define FOURCC_WMV3_WMV     mmioFOURCC_WMV('W','M','V','3') /* Window Media Video v9 */
#define FOURCC_wmv3_WMV     mmioFOURCC_WMV('w','m','v','3')
#define FOURCC_WMVA_WMV     mmioFOURCC_WMV('W','M','V','A') /* Window Media Video v9 Advance Profile */
#define FOURCC_wmva_WMV     mmioFOURCC_WMV('w','m','v','a')
#define FOURCC_WMVP_WMV     mmioFOURCC_WMV('W','M','V','P') /* Window Media Video v9 Image */
#define FOURCC_wmvp_WMV     mmioFOURCC_WMV('w','m','v','p')
#define FOURCC_WVP2_WMV     mmioFOURCC_WMV('W','V','P','2') /* Window Media Video v9.1 Image v2 */
#define FOURCC_wvp2_WMV     mmioFOURCC_WMV('w','v','p','2')
#define FOURCC_MP4S_WMV     mmioFOURCC_WMV('M','P','4','S') /* Microsoft ISO MPEG4 Video Codec v1 */
#define FOURCC_mp4s_WMV     mmioFOURCC_WMV('m','p','4','s')
#define FOURCC_MP42_WMV     mmioFOURCC_WMV('M','P','4','2') /* Microsoft MPEG4 Video Codec v2 */
#define FOURCC_mp42_WMV     mmioFOURCC_WMV('m','p','4','2')
#define FOURCC_MP43_WMV     mmioFOURCC_WMV('M','P','4','3') /* Microsoft MPEG4 Video Codec v3 */
#define FOURCC_mp43_WMV     mmioFOURCC_WMV('m','p','4','3')
#define FOURCC_WVC1_WMV     mmioFOURCC_WMV('W','V','C','1') /* VC-1 */
#define FOURCC_wvc1_WMV     mmioFOURCC_WMV('w','v','c','1')

	int	   i;
	int    iCodecVer = 0;
    int    hdrext = 0;
	unsigned int    ext_size = 0;
	unsigned char  *pHdrExt = NULL;
	unsigned char  *pInBuff = pInputBuff;
	unsigned int	dwRcvNumFrames = 0;
	unsigned int	uiPicHorizSize = 0, uiPicVertSize = 0;

	dwRcvNumFrames  = (unsigned char)*pInBuff++;        
	dwRcvNumFrames |= ((unsigned char)(*pInBuff++) << 8);
	dwRcvNumFrames |= ((unsigned char)(*pInBuff++) << 16);
	dwRcvNumFrames |= ((unsigned char)(*pInBuff++) << 24);

    iRcvVer         = (dwRcvNumFrames>>30) & 0x1; /* RCV is V2 format or not. */
    iCodecVer       = dwRcvNumFrames>>24;
    
    hdrext=(iCodecVer>>7)&0x1;
    if (iRcvVer == 0) {
        iCodecVer&=0x7f;
    } else {
        iCodecVer&=0x3f;
    }

    if (iCodecVer == 0) {
        iCodecVer = FOURCC_WMV1_WMV;
    } else if (iCodecVer == 1) {
        iCodecVer = FOURCC_MP43_WMV;
    } else if (iCodecVer == 2) {
        iCodecVer = FOURCC_WMV2_WMV;
    } else if (iCodecVer== 3) {
        iCodecVer = FOURCC_MP42_WMV;
    } else if (iCodecVer == 4) {
        iCodecVer = FOURCC_MP4S_WMV;
    } else if (iCodecVer == 5) {
        iCodecVer = FOURCC_WMV3_WMV;
    } else if (iCodecVer == 6) {
        iCodecVer = FOURCC_WMVA_WMV;
    } else if (iCodecVer == 7) {
        iCodecVer = FOURCC_WVP2_WMV;
    } else if (iCodecVer == 8) {
        iCodecVer = FOURCC_WVC1_WMV;
    } else { 
		return -1;
	}


    if( hdrext != 0 ) {
		ext_size  = (unsigned char)*pInBuff++;
		ext_size |= (unsigned char)(*pInBuff++) << 8;
		ext_size |= (unsigned char)(*pInBuff++) << 16;
		ext_size |= (unsigned char)(*pInBuff++) << 24;

#ifdef STABILITY
		if(ext_size > 1024)
			return -1;
#endif

		pHdrExt = (unsigned char *)pBmpInfo + sizeof(VO_BITMAPINFOHEADER);
		for ( i = 0; i < ext_size; i ++ ) {
			*pHdrExt++ = *pInBuff++;
		}

		pBmpInfo->biSize = sizeof(VO_BITMAPINFOHEADER) + ext_size;
    }
	pBmpInfo->biCompression = iCodecVer;

	uiPicVertSize  = (unsigned char)(*pInBuff++);
	uiPicVertSize |= (unsigned char)(*pInBuff++) << 8;
	uiPicVertSize |= (unsigned char)(*pInBuff++) << 16;
	uiPicVertSize |= (unsigned char)(*pInBuff++) << 24;
	uiPicHorizSize  = (unsigned char)(*pInBuff++);
	uiPicHorizSize |= (unsigned char)(*pInBuff++) << 8;
	uiPicHorizSize |= (unsigned char)(*pInBuff++) << 16;
	uiPicHorizSize |= (unsigned char)(*pInBuff++) << 24;

	pBmpInfo->biWidth = uiPicHorizSize;
	pBmpInfo->biHeight = uiPicVertSize;

    if ( iRcvVer == 1 ) {   
		/* Jump the rcv_additional_header_size,hdr_buffer, bitrate, framerate. */ 
		pInBuff   += 16; 
    }

	return (int)( (unsigned int)pInBuff - (unsigned int)pInputBuff );
}

int OutputOneFrame2( VO_U8* Y,VO_U8* U,VO_U8* V, VO_U32 width,VO_U32 height,VO_U32 stridey,VO_U32 strideuv,FILE*outFile)
{
	int i;
	unsigned char* out_src;
	if (!outFile) {
		return 0;
	}

	if (!Y || !U || !V) {
		return 0;
	}

	/* output decoded YUV420 frame */
	/* Y */
	out_src = Y;
	for( i = 0;i < height; i++ ) {
		fwrite(out_src, width, 1, outFile);
		out_src += stridey;
	}

	/* U */
	out_src = U;
	for(i = 0; i< height/2; i++ ) {
		fwrite(out_src, width/2, 1, outFile);
		out_src += strideuv;
	}

	/* V */
	out_src = V;
	for( i = 0; i < height/2; i++ ) {
		fwrite(out_src, width/2, 1, outFile);
		out_src += strideuv;
	}

	return 0;
}


int OutputOneFrame( VO_VIDEO_BUFFER *par, VO_VIDEO_OUTPUTINFO* outFormat,FILE*outFile)
{
	int i, width,height;
	unsigned char* out_src;
	if (!outFile) {
		return 0;
	}

	if (!par->Buffer[0] || !par->Buffer[1] || !par->Buffer[2]) {
		return 0;
	}

	//printf("\n@@@@@@@@@@@@Frame %d finished, InFrameType:%d, OutFrameType:%d \n", frameNumber, iFrameType, outFormat->Format.Type);

	width  = outFormat->Format.Width;
	height = outFormat->Format.Height;

	/* output decoded YUV420 frame */
	/* Y */
	out_src = par->Buffer[0];
	for( i = 0;i < height; i++ ) {
		fwrite(out_src, width, 1, outFile);
		out_src += par->Stride[0];
	}

	/* U */
	out_src = par->Buffer[1];
	for(i = 0; i< height/2; i++ ) {
		fwrite(out_src, width/2, 1, outFile);
		out_src += par->Stride[1];
	}

	/* V */
	out_src = par->Buffer[2];
	for( i = 0; i < height/2; i++ ) {
		fwrite(out_src, width/2, 1, outFile);
		out_src += par->Stride[2];
	}

	return 0;
}

void ParseConfig(char* inFile,char* outFile,char* speedFile,int* loopCount,int* outFrameNumber)
{

	if(fconfig)
	{
		fscanf(fconfig,"%*[^\"]");
		fscanf(fconfig,"\"%[^\"]",inFile);
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		/* RAW (YUV/RGB) output file */ 
		fscanf(fconfig,"\"%[^\"]",outFile);               
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		/* speed file */ 
		fscanf(fconfig,"\"%[^\"]",speedFile);               
		fscanf(fconfig,"%*[^\n]");


		fscanf(fconfig,"%d",outFrameNumber);           
		fscanf(fconfig,"%*[^\n]");
		
		fscanf(fconfig,"%d",loopCount);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&nDecodeNextFile);          
		fscanf(fconfig,"%*[^\n]");

		
	}
}

#ifdef SUPPORT_VC1_INPUT

int	IsInputTypeVC1(unsigned char *pInputBuf)
{
	if( pInputBuf[0] == 0x00 && pInputBuf[1] == 0x00 && 
		pInputBuf[2] == 0x01 && pInputBuf[3] == 0x0F ) {
		return 1;
	}
	return 0;
}

int GetElementaryChunk(unsigned char *pInputBuf, VO_CODECBUFFER *pInData, unsigned char *pHeaderParam)
{
#define START_CODE_LEN	4

    unsigned char *pBufStart, *pBufEnd;
	unsigned char  chunkType,firstChunkType;
	int            nIsSeqHeader = 0;

    /* Skip over any zero-stuffing bytes */
    for (pBufStart = pInputBuf;
         pBufStart - pInputBuf < leftLen && *pBufStart == 0;
         pBufStart ++);

    /* Check for a start code */
    if (pBufStart - pInputBuf     < 2		||
        pBufStart - pInputBuf + 2 > leftLen	||
        *pBufStart != 0x01)
    {
        /* No start code found */
	printf("\nThis is the last frame.");
        return -1;
    }

	firstChunkType = pBufStart[1];
    pBufEnd = pBufStart + 2;
    pBufStart += 2;          /* Point at the first byte in the encapsulated data */

    /* Scan for the start code of next chunk or end of buffer */

	if( firstChunkType == 0x0F ) { //Start code of sequence header.
		while (pBufEnd - pInputBuf < leftLen)  {
			if ( pBufEnd[0] == 0x00 && pBufEnd[1] == 0x00 && pBufEnd[2] == 0x01 ) {
				chunkType = pBufEnd[3];
				if( chunkType == 0x0D /*|| chunkType == 0x0F*/ )
					break;
			}
			pBufEnd++;
		}
	}
	else {
		while (pBufEnd - pInputBuf < leftLen)  {
			if ( pBufEnd[0] == 0x00 && pBufEnd[1] == 0x00 && pBufEnd[2] == 0x01 ) {
				chunkType = pBufEnd[3];
				if( chunkType == 0x0D )
					break;
			}
			pBufEnd ++; 
		}
	}


    if (pBufEnd-pInputBuf <= leftLen) {  //zou 330 <   ----> <=
        /* pBufEnd points at the last byte we want to include in this chunk */
        pBufEnd --;  /* scan back over zero bytes */
    }
    else{
        /* End of buffer, No start code found */
        return -1;
    }

	/* Get data and length of next chunk */
	if( firstChunkType == 0x0F ){ //Sequence header.
		pInData->Buffer = pHeaderParam;
		pInData->Length = (pBufEnd - pInputBuf + 1) + ASF_BINDING;
		if(pHeaderParam){
			*pHeaderParam++ = 0x25;	 //Insert ASF binding byte.
			memcpy(pHeaderParam, pInputBuf, pBufEnd - pInputBuf + 1);
		}
	}
	else {
		pInData->Buffer = pInputBuf;
		pInData->Length = pBufEnd - pInputBuf + 1;
	}

	return 0;

}

#endif