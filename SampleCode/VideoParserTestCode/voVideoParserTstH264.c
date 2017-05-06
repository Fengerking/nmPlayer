#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "voVideoParser.h"

#define IN_BUFF_LEN 1024*1024

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
static char* GetNextFrame_test(char* currPos,int size)
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

int H264_test()
{
	FILE *inFile;
	VO_U8 *inputBuf;
	VO_U8 *nextFrame,*currFrame;
	VO_S32 leftSize=IN_BUFF_LEN;
	VO_S32 inSize = 0;
	VO_S32 ret = 0;
	VO_HANDLE hCodec;
	VO_VIDEO_PARSERAPI voVideoParser;
	VO_S32 testcount = 0;
	VO_CODECBUFFER pInData;
	char infilename[255] = "D:\\videodump.h264";

	VO_U32 nVersion=0,nProfile =0;
	VO_U32 nFrameType = 0;
	VO_U32 nWidth=0,nHeight =0;
	VO_U32 isInterlace =0;
	VO_U32 IsRefFrame =0;
	VO_U32 IsIDRFrame = 0;
	VO_U32 DPB_size = 0;
	VO_S3D_Params S3D_Param;
	VO_H264_USERDATA_Params User_data;


	inFile= fopen (infilename, "rb");
	if (!inFile)
	{
		printf("\nError: cannot open input H.264 file!");
		exit(0);
	}
	else
		printf("\ninfile:%s\n",infilename);

	ret = voGetVideoParserAPI(&voVideoParser,VO_VIDEO_CodingH264);
	if(ret != VO_ERR_NONE)
		goto H264END;

	ret = voVideoParser.Init(&hCodec);
	if(ret != VO_ERR_NONE)
		goto H264UNINIT;

	inputBuf = (char *)calloc(IN_BUFF_LEN ,1);   //malloc input buffer
	nextFrame = currFrame = inputBuf;

	leftSize = fread(currFrame,1,IN_BUFF_LEN,inFile);
	currFrame = GetNextFrame_test(currFrame-3,leftSize); //get first nalu head
	while(testcount < 6&&leftSize>4)
	{
		//find next nalu head
		do 
		{
			nextFrame = GetNextFrame_test(currFrame,leftSize);
			if(nextFrame)
			{
				inSize = nextFrame-currFrame;
				break;
			}
			else
			{
				if(leftSize>1024*1024)
				{
					printf("\nthe next frame is too big to handle,exit\n");
					exit(0);
				}
				if(feof(inFile))//last nalu
				{
					inSize=leftSize;
					break;
				}
				else  //need to fill inputBuf
				{
					int readSize;
					memmove(inputBuf,currFrame,leftSize);
					currFrame=inputBuf;
					readSize=fread(currFrame+leftSize,1,IN_BUFF_LEN-leftSize,inFile);
					leftSize+=readSize;
				}
			}
		} while(1);
		pInData.Buffer = (VO_PBYTE)currFrame;
		pInData.Length = inSize;
		ret = voVideoParser.Process(hCodec,&pInData);
		leftSize-=inSize;
		currFrame=nextFrame;
		testcount++;
	}
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_PROFILE, &nProfile);
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_FRAMETYPE, &nFrameType);
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_WIDTH, &nWidth);
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_HEIGHT, &nHeight);
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_ISIDRFRAME, &IsIDRFrame);
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_INTERLANCE, &isInterlace);
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_REFFRAME, &IsRefFrame);
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_S3D, &S3D_Param);
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_USERDATA, &User_data);
	ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_DPB_SIZE, &DPB_size);

H264UNINIT:
	ret = voVideoParser.Uninit(hCodec);
H264END:
    return 0;	
}
