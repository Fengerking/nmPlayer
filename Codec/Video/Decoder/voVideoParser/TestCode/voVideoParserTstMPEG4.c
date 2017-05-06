#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "voVideoParser.h"

#define IN_BUFF_LEN 1024*1024

#define IN_BUFF_LEN 1024*1024

static void PrintParserInfo(VO_VIDEO_PARSERAPI hApi, VO_HANDLE hCodec)
{
	VO_U32 value;
	VO_S3D_Params S3D; //VO_PID_VIDEOPARSER_S3D
	VO_USERDATA_Params userData; //VO_PID_VIDEOPARSER_USERDATA
	VO_CODECBUFFER headData; //VO_PID_VIDEOPARSER_HEADERDATA
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_VERSION, &value))
	{
		//switch(value) ...
		printf("version: %d\n", value);
	}
	else
	{
		printf("version: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_PROFILE, &value))
	{
		//switch(value) ...
		printf("profile: %d\n", value);
	}
	else
	{
		printf("profile: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_LEVEL, &value))
	{
		//switch(value) ...
		printf("level: %d\n", value);
	}
	else
	{
		printf("level: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_FRAMETYPE, &value))
	{
		//switch(value) ...
		printf("frame type: %d\n", value);
	}
	else
	{
		printf("frame type: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_WIDTH, &value))
	{
		//switch(value) ...
		printf("width: %d\n", value);
	}
	else
	{
		printf("width: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_HEIGHT, &value))
	{
		//switch(value) ...
		printf("height: %d\n", value);
	}
	else
	{
		printf("height: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_INTERLANCE, &value))
	{
		//switch(value) ...
		printf("is interlace: %d\n", value);
	}
	else
	{
		printf("is interlace: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_REFFRAME, &value))
	{
		//switch(value) ...
		printf("is ref-frame: %d\n", value);
	}
	else
	{
		printf("is ref-frame: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_ISVC1, &value))
	{
		//switch(value) ...
		printf("is VC1: %d\n", value);
	}
	else
	{
		printf("is VC1: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_ISIDRFRAME, &value))
	{
		//switch(value) ...
		printf("is IDR frame: %d\n", value);
	}
	else
	{
		printf("is IDR frame: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_S3D, &S3D))
	{
		printf("S3D active: %d\n", S3D.active);
		printf("S3D mode: %d\n", S3D.mode);
		printf("S3D fmt: %d\n", S3D.fmt);
		printf("S3D order: %d\n", S3D.order);
		printf("S3D subsampling: %d\n", S3D.subsampling);
	}
	else
	{
		printf("S3D: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_USERDATA, &userData))
	{
		VO_U32 i, j;
		const VO_U8 *pData = userData.buffer;
		printf("user data num: %d\n", userData.count);
		for (i = 0; i < userData.count; ++i)
		{
			for (j = userData.size[i]; j != 0; --j)
			{
				printf("%02X(%c) ", *pData, *(pData++));
			}
			printf("\n");
		}
	}
	else
	{
		printf("user data: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_DPB_SIZE, &value))
	{
		printf("DPB size: %d\n", value);
	}
	else
	{
		printf("DPB size: no info\n");
	}
	if (VO_ERR_NONE == hApi.GetParam(hCodec, VO_PID_VIDEOPARSER_HEADERDATA, &headData))
	{
		VO_U32 i;
		printf("Get header data:");
		for (i = 0; i < headData.Length; ++i)
		{
			if ((i&15) == 0)
			{
				printf("\n");
			}
			printf("%02X ", headData.Buffer[i]);
		}
		printf("\n");
	}
	else
	{
		printf("Header data: no info\n");
	}

}

int TestMPEG4Parser()
{
	VO_CHAR *infilename ="D:/Numen/mpeg4/trunk/Codec/Video/Decoder/mpeg4/example/vs2005/voMPEG4Dec_Sample/LG_7858_720p.mpeg4";
	VO_U8  *readBuf = NULL;
	VO_S32 tempLen, leftLen = 0, fileSize = 0;
	VO_S32 frameLen = 0;
	VO_U32 ret;
	VO_VIDEO_PARSERAPI parserAPI;
	VO_HANDLE hCodec;
	VO_CODECBUFFER parserBuf;
	VO_U32 value;
	FILE *inFile= fopen (infilename, "rb");
	if (!inFile){
		printf("\nError: cannot open input file!\n");
		return -1;
	}
	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	/* allocate input buffer*/
	readBuf = (VO_U8 *)malloc(IN_BUFF_LEN * sizeof (VO_S8));
	parserBuf.Buffer = readBuf;
	leftLen = fread(readBuf, 1, IN_BUFF_LEN, inFile);
	parserBuf.Length = leftLen;
	voGetVideoParserAPI(&parserAPI, VO_VIDEO_CodingMPEG4);
	parserAPI.Init(&hCodec);

	//test file header, sequence header, first frame header
	ret = parserAPI.Process(hCodec, &parserBuf);
	PrintParserInfo(parserAPI, hCodec);

	//test random input buffer
	parserBuf.Buffer += 300;
	parserBuf.Length = 100000;
	ret = parserAPI.Process(hCodec, &parserBuf);
	PrintParserInfo(parserAPI, hCodec);

	if (0)
	{
		//test header generate
		VO_BYTE Mpeg4SeqHeader[]={0x00,0x00 ,0x01 ,0xb0 ,0x01 ,0x00 ,0x00 ,0x01 ,0xb5 ,0x89 ,0x13 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00
			,0x00 ,0x01 ,0x20 ,0x00 ,0xc4 ,0x8d ,0x88 ,0x00 ,0xc5 ,0x00 ,0x04 ,0x00 ,0x14 ,0x63 ,0x00 ,0x00
			,0x01 ,0xb2 ,0x4c ,0x61 ,0x76 ,0x63 ,0x35 ,0x32 ,0x2e ,0x32 ,0x37 ,0x2e ,0x30};
		VO_CODECBUFFER testBuf = {Mpeg4SeqHeader, sizeof(Mpeg4SeqHeader), 0, 0};
		VO_U32 width = 512, height = 384;
		VO_U32 nFPS = 60;
		VO_U32 w_h = (width << 19) | (height << 5);//27 bits
		Mpeg4SeqHeader[22] |= ((VO_U16)nFPS) >> 13;//3 bits
		Mpeg4SeqHeader[23] |= (VO_U8)(nFPS >> 5); //8 bits
		Mpeg4SeqHeader[24] |= (VO_U8)(nFPS << 3); //5 bits
		Mpeg4SeqHeader[25] |= (VO_U8)(w_h>>24);//8bits
		Mpeg4SeqHeader[26] |= (VO_U8)(w_h>>16);//8btis
		Mpeg4SeqHeader[27] |= (VO_U8)(w_h>>8);//8btis
		Mpeg4SeqHeader[28] |= (VO_U8)(w_h);//3btis
		ret = parserAPI.Process(hCodec, &testBuf);
	}
	parserAPI.Uninit(hCodec);
	if (readBuf)
	{
		free(readBuf);
	}
	if (inFile)
	{
		fclose(inFile);
	}
	return 0;
}

int TestMPEG2Parser()
{
	VO_CHAR *infilename ="D:/Numen/mpeg4/trunk/Codec/Video/Decoder/VoMPEG2Decoder/mpeg2_d_sample/3_MPEGPS_MPEGV2'MM_4737K'9s640ms'720x576'25f.mpeg2";
	VO_U8  *readBuf = NULL;
	VO_S32 tempLen, leftLen = 0, fileSize = 0;
	VO_S32 frameLen = 0;
	VO_U32 ret;
	VO_VIDEO_PARSERAPI parserAPI;
	VO_HANDLE hCodec;
	VO_CODECBUFFER parserBuf;
	VO_U32 value;
	FILE *inFile= fopen (infilename, "rb");
	if (!inFile){
		printf("\nError: cannot open input file!\n");
		return -1;
	}
	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	/* allocate input buffer*/
	readBuf = (VO_U8 *)malloc(IN_BUFF_LEN * sizeof (VO_S8));
	parserBuf.Buffer = readBuf;
	leftLen = fread(readBuf, 1, IN_BUFF_LEN, inFile);
	parserBuf.Length = leftLen;
	voGetVideoParserAPI(&parserAPI, VO_VIDEO_CodingMPEG2);
	parserAPI.Init(&hCodec);

	//test file header, sequence header, first frame header
	ret = parserAPI.Process(hCodec, &parserBuf);
	PrintParserInfo(parserAPI, hCodec);

	//test random input buffer
	parserBuf.Buffer += 500;
	parserBuf.Length = 10000;
	ret = parserAPI.Process(hCodec, &parserBuf);
	PrintParserInfo(parserAPI, hCodec);

	parserAPI.Uninit(hCodec);
	if (readBuf)
	{
		free(readBuf);
	}
	if (inFile)
	{
		fclose(inFile);
	}
	return 0;
}

int TestH263Parser()
{
	VO_CHAR *infilename ="D:/Numen/mpeg4/trunk/Codec/Video/Decoder/mpeg4/example/vs2005/voMPEG4Dec_Sample/H263_1408x1152_7M_30f.mpeg4";
	VO_U8  *readBuf = NULL;
	VO_S32 tempLen, leftLen = 0, fileSize = 0;
	VO_S32 frameLen = 0;
	VO_U32 ret;
	VO_VIDEO_PARSERAPI parserAPI;
	VO_HANDLE hCodec;
	VO_CODECBUFFER parserBuf;
	VO_U32 value;
	FILE *inFile= fopen (infilename, "rb");
	if (!inFile){
		printf("\nError: cannot open input file!\n");
		return -1;
	}
	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	/* allocate input buffer*/
	readBuf = (VO_U8 *)malloc(IN_BUFF_LEN * sizeof (VO_S8));
	parserBuf.Buffer = readBuf;
	leftLen = fread(readBuf, 1, IN_BUFF_LEN, inFile);
	parserBuf.Length = leftLen;
	voGetVideoParserAPI(&parserAPI, VO_VIDEO_CodingH263);
	parserAPI.Init(&hCodec);

	//test first frame header
	ret = parserAPI.Process(hCodec, &parserBuf);
	PrintParserInfo(parserAPI, hCodec);

	//test random input buffer
	parserBuf.Buffer += 500;
	parserBuf.Length -= 500;
	ret = parserAPI.Process(hCodec, &parserBuf);
	PrintParserInfo(parserAPI, hCodec);

	parserAPI.Uninit(hCodec);
	if (readBuf)
	{
		free(readBuf);
	}
	if (inFile)
	{
		fclose(inFile);
	}
	return 0;
}