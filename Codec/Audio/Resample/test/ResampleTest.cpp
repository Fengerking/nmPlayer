	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2004				*
	*																		*
	************************************************************************/

#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		"voResample.h"
#include		"cmnMemory.h"

#define OUTWAV 1
#define ErrorMessage()

FILE *faudio;
unsigned long int wav_size;
static int rate = 44100;//11025;//16000;//44100;

VO_AUDIO_FORMAT pcmformat;

#define READ_SIZE	(1024)	
#define OUT_SIZE	(1024*16)
short outBuf[OUT_SIZE];
short inBuf[1024*12];


int ReadFile2Buf(FILE* infile,unsigned char* dest,int readSize)
{
	int readBytes = 0;
	readBytes = fread(dest, 1, readSize, infile);
	return readBytes;
}

int frameCount =0;

#ifdef _WIN32_WCE

#define MAX_PATH          260
#define MAX_CMDLINE (MAX_PATH*2+16)
#define MAX_ARGV      10

//LPCSTR g_parfile = "\\Storage Card\\Resp\\config.txt";
LPCSTR g_parfile = "\\Storage Card\\wma\\output.wav";
FILE* fconfig;

static void ParseConfig(char* inFile,char* outFile,int* bwrite)
{
	if(fconfig)
	{
		//printf("file pos=%d\n",ftell(fconfig));
		fscanf(fconfig,"%*[^\"]");
		fscanf(fconfig,"\"%[^\"]",inFile);
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",outFile);               // RAW (YUV/RGB) output file
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",bwrite);           
		fscanf(fconfig,"%*[^\n]");	
	}
}
#endif

#if OUTWAV
__inline void shutdown_output ( void ) 
{
	printf("Shutting Down Sound System\n");
	if (faudio != NULL)
		fclose(faudio);
}

int
open_wav_output ( const char *file_name ) {

	char wav_hdr[] = { 0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00,
				0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
				0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
				0x44, 0xAC, 0x00, 0x00, 0x10, 0xB1, 0x02, 0x00,
				0x04, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61,
				0x00, 0x00, 0x00, 0x00 };

	if (file_name[0] == '\0')
		return -1;
	//if ((faudio = open(wav_file, (O_RDWR | O_CREAT | O_TRUNC | O_BINARY))) < 0)
	if ((faudio = fopen(file_name,  "wb")) == NULL)
	{
		return -1;
	} else {
		unsigned long int bytes_per_sec, bytes_per_sample;
		
//		printf("open output file success!\n");
		//chaanel count
		wav_hdr[22] = (char)((pcmformat.Channels) & 0xFF);
		//sample rate
		wav_hdr[24] = (char)((pcmformat.SampleRate) & 0xFF);
		wav_hdr[25] = (char)((pcmformat.SampleRate >> 8) & 0xFF);
		wav_hdr[26] = (char)((pcmformat.SampleRate >> 16) & 0xFF);
		wav_hdr[27] = (char)((pcmformat.SampleRate >> 24) & 0xFF);
		//data remited rate 
		bytes_per_sample = (pcmformat.SampleBits * pcmformat.Channels) >> 3;
		bytes_per_sec = pcmformat.SampleRate * bytes_per_sample;
		wav_hdr[28] = (char)((bytes_per_sec) & 0xFF);
		wav_hdr[29] = (char)((bytes_per_sec >> 8) & 0xFF);
		wav_hdr[30] = (char)((bytes_per_sec >> 16) & 0xFF);
		wav_hdr[31] = (char)((bytes_per_sec >> 24) & 0xFF);
		//byte count per sample
		wav_hdr[32] = (char)((bytes_per_sample) & 0xFF);
		//sample bits
		wav_hdr[34] = (char)((pcmformat.SampleBits) & 0xFF);
	}
	wav_size = fwrite(&wav_hdr, 44, 1, faudio);
	if ( wav_size != 1) 
	{
		shutdown_output();
		return -1;
	}

	wav_size = 0;
//	send_output = write_wav_output;
//	close_output = close_wav_output;
	return 0;
}

int
write_wav_output (unsigned char * output_data, int output_size) 
{
	if (fwrite(output_data, output_size, 1, faudio) != 1) 
	{
		shutdown_output();
		return -1;
	}

	wav_size += output_size;
//	if(wav_size >= 0x380000)
//		wav_size = wav_size+2-2;
	return 0;
}

void
close_wav_output ( void ) {
	char wav_count[4];
	if (faudio == NULL)
		return;

	wav_count[0] = (char)((wav_size) & 0xFF);
	wav_count[1] = (char)((wav_size >> 8) & 0xFF);
	wav_count[2] = (char)((wav_size >> 16) & 0xFF);
	wav_count[3] = (char)((wav_size >> 24) & 0xFF);
	fseek(faudio, 40, SEEK_SET);
	fwrite(&wav_count, 4, 1, faudio);

	wav_size += 36;
	wav_count[0] = (char)((wav_size) & 0xFF);
	wav_count[1] = (char)((wav_size >> 8) & 0xFF);
	wav_count[2] = (char)((wav_size >> 16) & 0xFF);
	wav_count[3] = (char)((wav_size >> 24) & 0xFF);
	fseek(faudio, 4, SEEK_SET);
	fwrite(&wav_count, 4, 1, faudio);
//	printf("out wave is done!");
	shutdown_output();
}
#endif //OUTWAV

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
{
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv)
{
#endif//_WIN32_WCE
	
	FILE *infile, *outfile;
	int	hr=0, t1, t2;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outInfo;
    int firstWrite = 1;
	int eofFile = 0;
	int* info=(int*)inBuf;
	int bytesLeft, nRead;
	int sampleRate;
	int decodedFrame = 0;
	int makeError = 0;
	int total = 0;
	int   Isoutput = 1;
	int	returnCode;
	int inRate = 48000;
	int outRate = 48000;
	int inChannel = 1;
	int outChanel = 2;
	int selectCh = VO_CHANNEL_FRONT_LEFT;
	int chamap[8];
	VO_RESAMPLE_LEVEL level = (VO_RESAMPLE_LEVEL)0;
	char temp[44];
	double performance=0.0;
	int		datalength=0, FPs;

#ifdef _WIN32_WCE
	TCHAR msg[256];
	char infileName[MAX_PATH] = "\\Storage Card\\Resp\\input_44100_2.wav";
	char outfileName[MAX_PATH] = "\\Storage Card\\Resp\\out_up_88200_2.wav";

//	if(!(fconfig = fopen(g_parfile, "rb")))
//	{
//		wsprintf(msg, TEXT("open config file error!!"));
//		MessageBox(NULL, msg, TEXT("resample error"), MB_OK);
//		return 0;
//	}
	
//	ParseConfig(infileName,outfileName, &Isoutput);

#else// _WIN32_WCE
    const char *infileName = argv[1];
    const char *outfileName = argv[2];

#endif//_WIN32_WCE 
	/* open input file */
	infile = fopen(infileName, "rb");
	if (!infile) {
#ifdef _WIN32_WCE
		wsprintf(msg, TEXT("open source wav file error!!"));
		MessageBox(NULL, msg, TEXT("Resample error"), MB_OK);
#endif
		return -1;
	}
	if(44 != fread(&temp, 1, 44, infile))
		return -1;

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

	returnCode  = voGetResampleAPI(&AudioAPI);
	if(returnCode)
		return -1;

//#######################################   Init Decoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingUnused, &useData);
	if(returnCode < 0)
	{
		printf("#### VOI_Error2:fail to initialize the decoder###\n");
		return -1;
	}

	//init in samplerate
	inRate = ((int *)(&temp[24]))[0];
//	if(inRate<=48000)outRate = inRate;
	outRate = 44100;

	AudioAPI.SetParam(hCodec, VO_PID_RESAMPLE_INRATE, &inRate);	

	//samplerate process setting
	AudioAPI.SetParam(hCodec, VO_PID_RESAMPLE_OUTRATE, &outRate);
	AudioAPI.SetParam(hCodec, VO_PID_RESAMPLE_LEVEL, &level);

	//init in channel
	inChannel = ((short *)(&temp[22]))[0];
	AudioAPI.SetParam(hCodec, VO_PID_RESAMPLE_INCHAN, &inChannel);
//	chamap[0] = VO_CHANNEL_FRONT_LEFT;
//	chamap[1] = VO_CHANNEL_FRONT_RIGHT;
	//chamap[2] = VO_CHANNEL_CENTER;
	//chamap[3] = VO_CHANNEL_SIDE_LEFT;
	//chamap[4] = VO_CHANNEL_SIDE_RIGHT;
	//chamap[5] = VO_CHANNEL_LFE_BASS;

//	AudioAPI.SetParam(hCodec, VO_PID_RESAMPLE_CHMAPPING, chamap);  // if the multichannel, you could set it or not
																	// the process has defualt mapping.
	//channel process setting
//#define SELCECTCHANE																   
//#ifdef SELCECTCHANE
//	selectCh = VO_CHANNEL_FRONT_LEFT;
//	AudioAPI.SetParam(hCodec, VO_PID_RESAMPLE_SELECTCHS, &selectCh);  //select channel
//#else
	AudioAPI.SetParam(hCodec, VO_PID_RESAMPLE_OUTCHAN, &outChanel);   //downmix channel.
//#endif


	/* open output file */
	pcmformat.Channels = outChanel;
	pcmformat.SampleRate = outRate;
	pcmformat.SampleBits = 16;	//Now the sample bits only support 16bit
	if(Isoutput)
	{
//		outfile = fopen(outfileName, "wb"); 
		hr = open_wav_output(outfileName);
		if (hr<0) {
#ifdef _WIN32_WCE
			wsprintf(msg, TEXT("open wav file error!!"));
			MessageBox(NULL, msg, TEXT("Resample Decode error"), MB_OK);
#endif
			return -1;
		}
	}
	inData.Buffer = (VO_PBYTE)inBuf;
	bytesLeft = ReadFile2Buf(infile,inData.Buffer,READ_SIZE);//fread(inParam.inputStream, 1, READ_SIZE, infile);

//#######################################    Decoding Section   #########################################
	
	do {

		frameCount++;
		inData.Length    = bytesLeft;
		outData.Buffer   = (VO_PBYTE)outBuf;
		outData.Length = OUT_SIZE;

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		t1 = clock();
#endif
		
		returnCode = AudioAPI.SetInputData(hCodec,&inData);

		
		do {
			outData.Buffer   = (VO_PBYTE)outBuf;
			outData.Length = OUT_SIZE;

			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outInfo);

			if(returnCode == 0)
				decodedFrame++;

			if (Isoutput && returnCode == 0)
			{
				datalength += outData.Length;
				write_wav_output(outData.Buffer, outData.Length);
			}
		} while(returnCode != (VO_ERR_INPUT_BUFFER_SMALL));

#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total += t2 - t1;
#else
		t2 = clock();
		total += t2 - t1;
#endif


#define  MAX_REMAINED 2048
		if (!eofFile) {
			nRead = ReadFile2Buf(infile, (VO_PBYTE)inBuf, READ_SIZE);
			bytesLeft = nRead;
			inData.Buffer = (VO_PBYTE)inBuf;
			if (!nRead)
				break;
		}

	} while (returnCode);

#ifdef _WIN32_WCE
	performance = 0.2*total*pcmformat.SampleRate*4/datalength;//MHz = cpu_freq(MHz) * (total/1000) / ((datalength/4)/samplerate)
	FPs = (int)(decodedFrame*1000.0/total);
//	wsprintf(msg, TEXT("Decode Time: %d clocks"), total);
	wsprintf(msg, TEXT("Performance: %f Mhz/s, %d frames, %d F/S"), performance, decodedFrame, FPs);
	MessageBox(NULL, msg, TEXT("Resample Decode Finished"), MB_OK);
#endif	

//################################################  End Decoding Section  #######################################################

	returnCode = AudioAPI.Uninit(hCodec);
	
	fclose(infile);
	close_wav_output();
//	if (outfile)
//    {
//        fclose(outfile);
//    }

	return 0;
}


