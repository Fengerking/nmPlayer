
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

//#ifdef EVC_AMRWBPLUS
//#include <windows.h>
//#endif

#include "amrwbplus_d_api.h"


#define AMRWBPLUS_TEST 1
#define AMRWB_TEST 0

//output
#define OUTPUT_RESULT				1
#ifdef  EVC_AMRWBPLUS
//#undef OUTPUT_RESULT
#endif

#ifdef MSVC_AMRWBPLUS
static void parsecmdline(
  long argc,
  char  *argv[],
  char  **input_filename,
  char  **output_filename, 
  char  **fer_filename, 
  DecoderConfig * conf
  );
#endif
short ReadHeader_buffer_main(
short *extension,
short *mode,
short *st_mode,
short *fst,
unsigned char *buffer_in
);

extern AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_GetChannelNum(short *channel_num);
extern AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_GetFsout(long *fs);
extern FILE *Wave_Fopen(
    char *Filename,
    char *Mode,
    short *NumOfChannels,
    long *SamplingRate,
    short *BitsPerSample,
    long *DataSize
);
extern void Wave_Fclose(
    FILE *FilePtr,
    short BitsPerSample
);



//short out_buffer[320];
short out_buffer[320 * 100];
#ifdef MSVC_AMRWBPLUS
int main(int argc, char *argv[])
#endif
#ifdef EVC_AMRWBPLUS
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
#endif
{
	short i;

    FILE *f_input;
    FILE *f_output;
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
	FILE *f_fer;
#endif
#endif

    
// @shanrong modified
#ifdef MSVC_AMRWBPLUS
#if AMRWB_TEST
    //char input_filename[] = "T00_660.cod";
    //char output_filename[] = "T00_660_msvc.pcm";
    //char input_filename[] = "1.awb";
    //char output_filename[] = "1_temp.pcm";
    //char input_filename[] = "./Debug/11.awb";
    //char output_filename[] = "./Debug/11_msvc.wav";	
    char input_filename[] = "1.awb";
    char output_filename[] = "1_msvc.wav";	
#elif AMRWBPLUS_TEST
    char *input_filename = "";
    char *output_filename = "";
#endif
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
    char *fer_filename = "";
#endif
#endif
#endif
#ifdef EVC_AMRWBPLUS
    //char input_filename[] = "/amrwbp/T00_660.cod";//"T00_660.cod";
    //char output_filename[] = "/amrwbp/T00_660_evc.pcm";
    //char input_filename[] = "/amrwbp/11.awb";
    //char output_filename[] = "/amrwbp/11_evc.wav";
//    char input_filename[] = "/amrwbp/concatener.bit";
//    char output_filename[] = "/amrwbp/concatener_0426.wav";
//    char input_filename[] = "/amrwbp/m_po_x_1_org48.bit";
//    char output_filename[] = "/amrwbp/m_po_x_1_org48.wav";
    //char input_filename[] = "/amrwbp/m_po_x_1_org48.bit";
    //char output_filename[] = "/amrwbp/m_po_x_1_org48_0428.wav";

//    char input_filename[] = "/amrwbp/mode_660_200s.awp";
//    char output_filename[] = "/amrwbp/mode_660_200s.wav";
//    char input_filename[] = "/amrwbp/mode_885_200s.awp";
//    char output_filename[] = "/amrwbp/mode_885_200s.wav";
//    char input_filename[] = "/amrwbp/mode_1265_200s.awp";
//    char output_filename[] = "/amrwbp/mode_1265_200s.wav";
//    char input_filename[] = "/amrwbp/mode_1425_200s.awp";
//    char output_filename[] = "/amrwbp/mode_1425_200s.wav";
//    char input_filename[] = "/amrwbp/mode_1585_200s.awp";
//    char output_filename[] = "/amrwbp/mode_1585_200s.wav";
//    char input_filename[] = "/amrwbp/mode_1825_200s.awp";
//    char output_filename[] = "/amrwbp/mode_1825_200s.wav";
//    char input_filename[] = "/amrwbp/mode_1985_200s.awp";
//    char output_filename[] = "/amrwbp/mode_1985_200s.wav";
//    char input_filename[] = "/amrwbp/mode_2305_200s.awp";
//    char output_filename[] = "/amrwbp/mode_2305_200s.wav";
//    char input_filename[] = "/amrwbp/mode_2385_200s.awp";
//    char output_filename[] = "/amrwbp/mode_2385_200s.wav";

//    char input_filename[]  = "/amrwbp/48000_stereo_ft_16.awp";
//    char output_filename[] = "/amrwbp/48000_stereo_ft_16.wav";
//    char input_filename[]  = "/amrwbp/48000_stereo_ft_17.awp";
//    char output_filename[] = "/amrwbp/48000_stereo_ft_17.wav";
//    char input_filename[]  = "/amrwbp/48000_stereo_ft_18.awp";
//    char output_filename[] = "/amrwbp/48000_stereo_ft_18.wav";
//    char input_filename[]  = "/amrwbp/48000_stereo_ft_19.awp";
//    char output_filename[] = "/amrwbp/48000_stereo_ft_19.wav";
//    char input_filename[]  = "/amrwbp/48000_stereo_ft_20.awp";
//    char output_filename[] = "/amrwbp/48000_stereo_ft_20.wav";
//    char input_filename[]  = "/amrwbp/48000_stereo_ft_21.awp";
//    char output_filename[] = "/amrwbp/48000_stereo_ft_21.wav";
//    char input_filename[]  = "/amrwbp/48000_stereo_ft_22.awp";
//    char output_filename[] = "/amrwbp/48000_stereo_ft_22.wav";
    char input_filename[]  = "/amrwbp/48000_stereo_ft_23.awp";
    char output_filename[] = "/amrwbp/48000_stereo_ft_23.wav";
//    char input_filename[]  = "/amrwbp/48000_stereo_ft_24.awp";
//    char output_filename[] = "/amrwbp/48000_stereo_ft_24.wav";
//    char input_filename[]  = "/amrwbp/48000_stereo_ft_25.awp";
//    char output_filename[] = "/amrwbp/48000_stereo_ft_25.wav";
    
    char *fer_filename = "";
#endif
	
    int in_file_len;
    int counter;
    short frame_len;
    
    unsigned char *buffer = NULL;
    
    AMRWBPLUS_DEC_PARAM_T amrwbplus_para;
    AMRWBPLUS_DEC_RESULT_T result;
    
    //time related
    FILE *fp_chen_time = NULL;
    char chen_time_filename[100];
    long chen_pos;
    long time_begin, time_end;
    short channel_number;
    short bitsPerSample;
    long dataSize;
    long fs_out;

	long frame_counter;
	
	
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
    /* Parse Command Line */
    parsecmdline(argc, argv, &input_filename, &output_filename,&fer_filename, &amrwbplus_para.conf);
#endif
#endif

    //get time_calculated file-name
    strcpy(chen_time_filename, output_filename);
    chen_pos = strlen(chen_time_filename);
    chen_time_filename[chen_pos-4] = '\0';
    strcat(chen_time_filename, "_dectime_.dat");
        

	//input file
    if ((f_input = fopen(input_filename, "rb")) == NULL)
    {
        printf("error in open input file!\n");
        exit(1);
    }
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
	//Frame errasure simualtion
    if (1 == amrwbplus_para.conf.fer_sim)
    {
      if ((f_fer = fopen(fer_filename,"rt")) == NULL) 
      {
        printf("error in open fer_sim file!\n");
        fclose(f_input);
        exit(12);
      }
    }
#endif
#endif
	//time_calculated file
    if ((fp_chen_time = fopen(chen_time_filename, "w")) == NULL)
    {
        printf("error in open/create output file!\n");
        fclose(f_input);
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
        if (1 == amrwbplus_para.conf.fer_sim)
		{
			fclose(f_fer);
		}
#endif
#endif
        exit(11);
    }
    
    //read data from input file
    fseek(f_input, 0, SEEK_END);
    in_file_len = ftell(f_input);
    fseek(f_input, 0, SEEK_SET);
    buffer = (char *)malloc(in_file_len);
    if (NULL == buffer)
    {
        printf("malloc memory failure!\n");
        fclose(f_input);
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
        if (1 == amrwbplus_para.conf.fer_sim)
		{
			fclose(f_fer);
		}
#endif
#endif
        fclose(fp_chen_time);
        exit(3);
    }
    fread(buffer, sizeof(char), in_file_len, f_input);


    //initialize
#if AMRWBPLUS_TEST
   if (in_file_len >= 2)
   {
		ReadHeader_buffer_main(
		                &amrwbplus_para.conf.extension,
						&amrwbplus_para.conf.mode,
						&amrwbplus_para.conf.st_mode,
						&amrwbplus_para.conf.fscale,
						buffer);
	}
	else
	{
		amrwbplus_para.conf.extension = 0;  
		amrwbplus_para.conf.mode = 8;
		amrwbplus_para.conf.st_mode = 0;
		amrwbplus_para.conf.fscale = 0;
	}
    amrwbplus_para.input_data_type = WBPLUS;
#ifdef EVC_AMRWBPLUS
    amrwbplus_para.conf.fs = 48000;
    amrwbplus_para.conf.mono_dec_stereo = 0;
    amrwbplus_para.conf.limiter_on = 1;
    amrwbplus_para.conf.FileFormat = 0;
    amrwbplus_para.conf.fer_sim = 0;

//    amrwbplus_para.conf.fs = 16000;
//    amrwbplus_para.conf.mono_dec_stereo = 0;
//    amrwbplus_para.conf.limiter_on = 0;
//    amrwbplus_para.conf.FileFormat = 0;
//    amrwbplus_para.conf.fer_sim = 0;
#endif
#endif
#if AMRWB_TEST
    amrwbplus_para.input_data_type = WB_MIME;
    amrwbplus_para.conf.mode = 0;
    amrwbplus_para.conf.extension = 0;
    amrwbplus_para.conf.st_mode = -1;
    amrwbplus_para.conf.fscale = 0;
    amrwbplus_para.conf.fs = 16000;
    amrwbplus_para.conf.mono_dec_stereo = 0;
    amrwbplus_para.conf.limiter_on = 0;
    amrwbplus_para.conf.FileFormat = 0;
    amrwbplus_para.conf.fer_sim = 0;
#endif

    result = AMRWBPLUS_StartDecode(&amrwbplus_para);
    if (AMRWBPLUS_DEC_OPERATION_DONE != result)
    {
        printf("initialization fail!\n");
        free(buffer);
        fclose(f_input);
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
        if (1 == amrwbplus_para.conf.fer_sim)
		{
			fclose(f_fer);
		}
#endif
#endif
        fclose(fp_chen_time);
        exit(5);
    }
    
	//output file
	AMRWBPLUS_GetChannelNum(&channel_number);
//    channel_number = 1;
	bitsPerSample = 16;
	AMRWBPLUS_GetFsout(&fs_out);
    if ((f_output = Wave_Fopen(output_filename, "wb", &channel_number, &fs_out,
        &bitsPerSample, &dataSize)) == NULL)
    {
        printf("error in create output file!\n");
        free(buffer);
        fclose(f_input);
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
        if (1 == amrwbplus_para.conf.fer_sim)
		{
			fclose(f_fer);
		}
#endif
#endif
        fclose(fp_chen_time);
        exit(13);
    }

    //ready for decode
    counter = 0;
	amrwbplus_para.inputStream = buffer + counter;
	frame_len = AMRWBPLUS_GetFrameLenth(&amrwbplus_para);
    
    time_begin  = GetTickCount();
	frame_counter = 0;
    amrwbplus_para.inputStream = buffer;

    //amrwbplus_para.outputStream = out_buffer;
	while ((counter + frame_len) <= in_file_len)
    {
        //set the bfi for SIMULATION PURPOSE
        //read frame erasures every forth frame for wb+
        //read frame erasures every frame for amr-wb
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
        if(amrwbplus_para.conf.fer_sim)
        {
            if(amrwbplus_para.conf.extension > 0)
            {
                for(i=0;i<4;i++) 
                {
                    fscanf(f_fer, "%d", (amrwbplus_para.conf.bfi + i));
                }
            }
            else
            {
                fscanf(f_fer, "%d", (amrwbplus_para.conf.bfi + 0));
            }
        }
#endif
#endif
        //set frame length
        amrwbplus_para.inputSize = frame_len;
        //decode this frame
        result = AMRWBPLUS_ReceiveData(&amrwbplus_para);
        if (AMRWBPLUS_DEC_DECODE_ONE_FRAME != result) break;

		frame_counter++;
        
#if (OUTPUT_RESULT)
        fwrite(amrwbplus_para.outputStream, sizeof(short), amrwbplus_para.outputSize, f_output);
#endif

        counter += frame_len;
		amrwbplus_para.inputStream = buffer + counter;
		frame_len = AMRWBPLUS_GetFrameLenth(&amrwbplus_para);
    }
    time_end  = GetTickCount();
    fprintf(fp_chen_time, "\n***decoding time is %dms***\n", (time_end - time_begin));

    result = AMRWBPLUS_EndDecode(&amrwbplus_para);
    if (AMRWBPLUS_DEC_OPERATION_DONE != result)
    {
        printf("close fail!\n");
        free(buffer);
        fclose(f_input);
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
        if (1 == amrwbplus_para.conf.fer_sim)
		{
			fclose(f_fer);
		}
#endif
#endif
        Wave_Fclose(f_output, bitsPerSample);
        fclose(fp_chen_time);
        exit(6);
    }

    free(buffer);
    fclose(f_input);
#ifdef MSVC_AMRWBPLUS
#if AMRWBPLUS_TEST
    if (1 == amrwbplus_para.conf.fer_sim)
	{
		fclose(f_fer);
	}
#endif
#endif
    (void)fflush(f_output);
    Wave_Fclose(f_output, bitsPerSample);
    fclose(fp_chen_time);
    
    return 0;
}



#ifdef MSVC_AMRWBPLUS
static void parsecmdline(
  long argc,
  char  *argv[],
  char  **input_filename,
  char  **output_filename, 
  char  **fer_filename, 
  DecoderConfig * conf
  )
{
   conf->fs = 0;
   conf->mono_dec_stereo = 0;
   conf->limiter_on = 0;
   conf->fer_sim = 0;
   conf->FileFormat = 1;//F3GP;

   if (argc < 5)
   {
      printf("the number of input paramerter should be larger than 4\n");
      exit(101);
   }
   argc--;
   argv++;
   
   while (argc > 0)
   {
      if (!strcmp(*argv, "-fs"))
      {
         argv++;
         argc--;
         conf->fs = atoi(*argv);
      }
      else if (!strcmp(*argv, "-ff"))
      {
         argv++;
         argc--;
         if (!strcmp(*argv, "3gp"))
         {
            conf->FileFormat = 1;//F3GP;
         }
         else
         {
            conf->FileFormat = 0;//FRAW;
         }

      }
      else if (!strcmp(*argv, "-mono"))
      {
         conf->mono_dec_stereo = 1;
      }
      else if (!strcmp(*argv, "-limiter"))
      {
         conf->limiter_on = 1;
      }
      else if (!strcmp(*argv, "-if"))
      {
         argv++;
         argc--;
         *input_filename = *argv;
      }
      else if (!strcmp(*argv, "-of"))
      {
         argv++;
         argc--;
         *output_filename = *argv;
      }
      else if (!strcmp(*argv, "-fer"))
      {
         argv++;
         argc--;
         *fer_filename = *argv;
         conf->fer_sim=1;
      }
      else
      {
         printf("Unknown option %s\n", *argv);
         exit(102);
      }
      argv++;
      argc--;

   }
}
#endif

const short miMode_fx_main[2 * 24] = 
{ /* Core mode */ /* Stereo mode */
  0,                0,
  0,                1,
  0,                4,
  1,                1,
  1,                3,
  1,                5,
  2,                2,
  2,                4,
  2,                6,
  3,                3,
  3,                5,
  3,                7,
  4,                4,
  4,                6,
  4,                9,
  5,                5,
  5,                7,
  5,                11,
  6,                8,
  6,                10,
  6,                15,
  7,                9,
  7,                10,
  7,                15
};                    
const short isfIndex_fx_main[14] =      /* ISF possiblities RAW/3GP payload */
{
  0,
  48,
  54,
  60,
  64,
  72,
  80,
  90,
  96,
  108,
  120,
  128,
  135,
  144
};
short ReadHeader_buffer_main(
short *extension,
short *mode,
short *st_mode,
short *fst,
unsigned char *buffer_in
)
{
    short mode_index, fst_index, index, nb_read = 0;
    unsigned char byte;
    short tfi;
    
    unsigned char *buffer_cur = buffer_in;
    
    //  nb_read += fread(&byte, sizeof(unsigned char), 1, f_serial);
    //  if(nb_read ==0)
    //    return nb_read;
    byte = *buffer_cur++;
    nb_read++;
    
    mode_index = (byte & 127);
    
    //If frame ereased : don't change old conf just modify mode
    if( mode_index > 47 || mode_index < 0 ||        /* mode unknown */
    mode_index == 14 || mode_index == 15 ||     /* Frame lost or ereased */
    (mode_index == 9 && *extension == 1))       /* WB SID in WB+ frame  not supported case so declare a NO_DATA*/    
    {
        //nb_read += fread(&byte, sizeof(unsigned char), 1, f_serial); /* read one more byte to ensure empty header */
        byte = *buffer_cur++;
        nb_read++;
        
        tfi = (byte & 0xc0)>>6;   /* tfi extrapolated by RTP packetizer */
        fst_index = (byte & 0x1F);    
        *fst = isfIndex_fx_main[fst_index];
        
        *mode  = mode_index;
        return 0;         /* There is no more data to read */
    }
    
    *st_mode  = -1;
    
    if(mode_index >15)      /* wb+ */
    {
        if(mode_index < 24)    /* Mono mode only */
        {
            *mode =   mode_index - 16;
        }
        else
        {
            index = mode_index - 24;
            *mode = miMode_fx_main[2*index];
            *st_mode = miMode_fx_main[2*index+1];
        }
        *extension = 1;
        //nb_read += fread(&byte, sizeof(unsigned char), 1, f_serial);
        byte = *buffer_cur++;
        nb_read++;
        tfi = (byte & 0xc0)>>6;
        fst_index = (byte & 0x1F);    
        if(fst_index < 1)
        fst_index = 1;        /* prevent isf < 0.5 */
        *fst = isfIndex_fx_main[fst_index];
    }
    else      /* WB and caracterize WB+*/
    {
        if(mode_index == 10)
        {
            *extension = 1;
            *mode = 2;        /* 14m */        
        }
        else if (mode_index == 11)
        {
            *extension = 1;
            *mode = 2;        /* 18s */
            *st_mode = 6;
        }
        else if (mode_index == 12)
        {
            *extension = 1;
            *mode = 7;        /* 24m */
        }
        else if (mode_index == 13)
        {
            *extension = 1;
            *mode = 5;        /* 24s */
            *st_mode = 7;
        }
        else
        {
            *extension = 0;
            *mode =   mode_index;
        }
        //nb_read += fread(&byte, sizeof(unsigned char), 1, f_serial);
        byte = *buffer_cur++;
        nb_read++;
        tfi = (byte & 0xc0)>>6;
        fst_index = (byte & 0x1F);    
        if(fst_index != 0 && fst_index != 8)
        {
            printf("Internal Sampling Frequency not supported with AMW WB and caracterized WB+ modes\n" );
            exit(102);
        }
        *fst = isfIndex_fx_main[fst_index];
    }
    
    return nb_read;
}

