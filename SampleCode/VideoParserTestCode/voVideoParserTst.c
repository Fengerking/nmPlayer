#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "voVideoParser.h"

extern int ParserVP6();
extern int ParserWMV();
extern int TestMPEG4Parser();
extern int TestMPEG2Parser();
extern int TestH263Parser();
extern int H264_test();

int main()
{
	VO_VIDEO_CODINGTYPE codec_type;

	codec_type = VO_VIDEO_CodingH264;

	switch(codec_type)
	{
	case VO_VIDEO_CodingVP6:
		ParserVP6();
		break;
	case VO_VIDEO_CodingVP8:
		break;
	case VO_VIDEO_CodingWMV:
	case VO_VIDEO_CodingVC1:
		ParserWMV();
		break;
	case VO_VIDEO_CodingMPEG4:
		TestMPEG4Parser();
		break;
	case VO_VIDEO_CodingMPEG2:
		TestMPEG2Parser();
		break;
	case VO_VIDEO_CodingH263:
		TestH263Parser();
		break;
	case VO_VIDEO_CodingH264:
		H264_test();
		break;

	default:
		return -1;
	}

	
	return 0;	
}
