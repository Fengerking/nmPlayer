#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "voVideoParser.h"

#define IN_BUFF_LEN            (1024 * 1024 * 2)
#define IN_BUFF_LEN2           (IN_BUFF_LEN/2)
int ParserVP6()
{
	VO_CHAR *infilename ="E:/MyResource/Video/clips/VP6/VP6_MP3.vp6";
	VO_U8  *readBuf = NULL;
	VO_U8  *inputBuf = NULL;
	VO_U32 outTotalFrame =1000;
	VO_S32 tempLen, leftLen = 0, fileSize = 0;
	VO_S32 frameLen = 0;

	VO_U32 nVersion=0,nProfile =0;
	VO_U32 nFrameType = 0;
	VO_U32 nWidth=0,nHeight =0;
	VO_U32 isInterlace =0;
	VO_U32 IsRefFrame =0;
	VO_U32 isVC1 =0;

	VO_HANDLE hCodec;
	VO_CODECBUFFER pInData;
	VO_S32 ret = 0;
	VO_VIDEO_PARSERAPI voVideoParser;

	FILE *inFile= fopen (infilename, "rb");

	if (!inFile){
		printf("\nError: cannot open input VP6 file!");
		exit(0);
	}

	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	/* allocate input buffer*/
	readBuf = (VO_U8 *)malloc(IN_BUFF_LEN * sizeof (VO_S8));

	ret = voGetVideoParserAPI(&voVideoParser,VO_VIDEO_CodingVP6);
	if(ret != VO_ERR_NONE)
		goto END;

	ret = voVideoParser.Init(&hCodec);
	if(ret != VO_ERR_NONE)
		goto UNINIT;

	if(!(leftLen = fread(readBuf, 1, IN_BUFF_LEN, inFile)))
		goto END;
	inputBuf = readBuf;

	while((fileSize>0) && (outTotalFrame>0))
	{
		if( (leftLen >3) && ((leftLen - 4) >= (inputBuf[3]<<24|inputBuf[2]<<16|inputBuf[1]<<8|inputBuf[0])))
		{
			frameLen = inputBuf[3]<<24|inputBuf[2]<<16|inputBuf[1]<<8|inputBuf[0];
			/* Initialize input buffer for one frame*/
			pInData.Length = frameLen;
			pInData.Buffer = (VO_PBYTE)(inputBuf + 4);
		}
		else
		{
			/* if the leftdata is less than this frameLen, read more data to input buffer*/
			memmove(readBuf, inputBuf,  leftLen);
			tempLen = fread(readBuf + leftLen, 1, (IN_BUFF_LEN - leftLen), inFile);
			if(tempLen >0)
			{
				leftLen = leftLen + tempLen;
				inputBuf = readBuf;
				continue;
			}
			else
			{
				/* end of the inFile*/
				break;
			}
		}
		ret = voVideoParser.Process(hCodec,&pInData);
		//if(ret == VO_RETURN_SQHEADER || ret == VO_RETURN_FMHEADER || ret == VO_RETURN_SQFMHEADER  )
		{
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_VERSION, &nVersion);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_PROFILE, &nProfile);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_FRAMETYPE, &nFrameType);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_WIDTH, &nWidth);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_HEIGHT, &nHeight);

			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_INTERLANCE, &isInterlace);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_REFFRAME, &IsRefFrame);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_ISVC1, &isVC1);

			{
				FILE* fp =fopen("E:/MyResource/Video/clips/VP6/videoparser.txt","a");
				fprintf(fp,"nVersion = %d nProfile= %d  \n",nVersion, nProfile);
				fprintf(fp,"nFrameType = %d nWidth= %d  nHeight=%d \n",nFrameType, nWidth,nHeight);
				fprintf(fp,"isInterlace = %d IsRefFrame= %d  isVC1=%d \n\n",isInterlace, IsRefFrame,isVC1);
				fclose(fp);
			}
	
		}

		inputBuf += (frameLen + 4);
		fileSize -= (frameLen + 4);
		leftLen  -= (frameLen + 4);
	}
UNINIT:
	ret = voVideoParser.Uninit(hCodec);
END:
	return 0;
}