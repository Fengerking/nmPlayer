	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2004				*
	*																		*
	************************************************************************/


#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		"voSink.h"
#include		"vofile.h"
#include		"voAudio.h"
#include		"voVideo.h"

#define READ_SIZE	102400
unsigned char inBuf[READ_SIZE];

#define INPUTVIDEO "inputvideo.h264"
#define INPUTAUDIO "inputaudio.aac"
#define OUTPUTFILE	"output.mp4"


int ReadFile2Buf(FILE* infile,unsigned char* dest)
{
	int readBytes = 0;
	int nReadSize = 0;

	readBytes = fread(&nReadSize , 1 , 4 , infile);
	if(readBytes == 4)
	{
		readBytes = fread(dest, 1, nReadSize , infile);
		return readBytes;
	}
		
	return 0;
}

int frameCount =0;

// for gcc compiler;
int main(int argc, char **argv)
{

	FILE *invfile, *inafile ;
	char inVFileName[256], inAFileName[256],outFileName[256];

	VO_SINK_WRITEAPI sinkAPI;
	 

	inVFileName[0] = inAFileName[0] = outFileName[0] = '\0';
	sprintf(inVFileName, INPUTVIDEO);
	sprintf(inAFileName, INPUTAUDIO);
	sprintf(outFileName, OUTPUTFILE);

	/* open input file */
	invfile = fopen(inVFileName, "rb");
	if (!invfile) {
		return -1;
	}

	inafile = fopen(inAFileName, "rb");
	if (!invfile) {
		return -1;
	}

	voGetMP4WriterAPI(&sinkAPI , 0);

	VO_FILE_SOURCE fileSource;
	memset(&fileSource , 0 , sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nMode = VO_FILE_READ_WRITE;
	fileSource.pSource = outFileName;

	VO_SINK_OPENPARAM sinkOpen;
	memset(&sinkOpen , 0 , sizeof(VO_SINK_OPENPARAM));
	sinkOpen.nAudioCoding = VO_AUDIO_CodingAAC;
	sinkOpen.nVideoCoding = VO_VIDEO_CodingH264;

	VO_PTR hFileHandle = NULL;
	sinkAPI.Open(&hFileHandle , &fileSource , &sinkOpen);
	if(hFileHandle == NULL)
		goto CLOSE;

	VO_AUDIO_FORMAT audioFmt;
	audioFmt.Channels = 2;
	audioFmt.SampleBits = 16;
	audioFmt.SampleRate = 44100;
	sinkAPI.SetParam(hFileHandle , VO_PID_AUDIO_FORMAT , &audioFmt);

	VO_VIDEO_FORMAT vidFmt;
	vidFmt.Height = 240;
	vidFmt.Width  = 320;
	vidFmt.Type = VO_VIDEO_FRAME_NULL;

	sinkAPI.SetParam(hFileHandle , VO_PID_VIDEO_FORMAT , &vidFmt);

	bool bVideoEOS = false;
	bool bAudioEOS = false;

	int nReadSize = 0;
	long long llVideoTime = 0;
	long long llAudioTime = 0;
	VO_SINK_SAMPLE sinkSample;
	memset(&sinkSample , 0 , sizeof(VO_SINK_SAMPLE));
	while (!bVideoEOS || !bAudioEOS )
	{
		nReadSize = ReadFile2Buf(invfile , inBuf);
		if(nReadSize > 0)
		{
			sinkSample.Buffer = inBuf;
			sinkSample.Size = nReadSize;
			sinkSample.nAV = 1;
			sinkSample.Time = llVideoTime;
			llVideoTime += 40;

			sinkAPI.AddSample(hFileHandle , &sinkSample);
		}
		else
		{
			bVideoEOS = true;
		}

		nReadSize = ReadFile2Buf(inafile , inBuf);
		if(nReadSize > 0)
		{
			sinkSample.Buffer = inBuf;
			sinkSample.Size = nReadSize;
			sinkSample.nAV = 0;
			sinkSample.Time = llAudioTime;
			llAudioTime += 23;

			sinkAPI.AddSample(hFileHandle , &sinkSample);
		}
		else
		{
			bAudioEOS = true;
		}
	}

	sinkAPI.Close(hFileHandle);

CLOSE:
	fclose(invfile);
	fclose(inafile);
	return 0;
}


