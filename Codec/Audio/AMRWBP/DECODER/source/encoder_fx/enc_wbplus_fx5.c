 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "amr_plus_fx.h"
#include "enc_if_fx.h"
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "wbplus3gplib.h"

void Copy_coder_state(
  Coder_State_Plus_fx *wbP,/* AMR-WB+ state struct */
  void   *st,               /* AMR-WB state struct  */
  Word16 sw,                /* sw=0 -> switch from WB to WB+, sw=1 -> switch from WB+ to WB */
  Word16 use_case_mode
);
void copyright(void) 
{
   AMRWBPLUS_FPRINTF(stderr, "\n");
   AMRWBPLUS_FPRINTF(stderr, "\n");
}
static void usage(char *argv)
{
   AMRWBPLUS_FPRINTF(stderr,
           "Usage: %s -rate <Bit rate> [-mono] | -mi <mode index> [-isf <factor>] [-lc] [-dtx] -ff <3gp/raw> -if <infile.wav> -of <outfile.wb+>\n",
           argv);
   AMRWBPLUS_FPRINTF(stderr, "\n");
   AMRWBPLUS_FPRINTF(stderr, "-rate   Bit rate between 6-36 kbps mono or 7-48 kbps stereo \n");
   AMRWBPLUS_FPRINTF(stderr, "-mono   Force mono encoding \n");
   AMRWBPLUS_FPRINTF(stderr, "\n");
   AMRWBPLUS_FPRINTF(stderr, "\n");
   AMRWBPLUS_FPRINTF(stderr, "-mi     Mode Index (0..15  -> AMR WB\n                  16..47 -> AMR WB+) (see ts 26.304 Table 14) \n");
   AMRWBPLUS_FPRINTF(stderr,
                   "-isf    Internal Sampling Frequency (0.5... 1.5, default is 1.0).\n");
   AMRWBPLUS_FPRINTF(stderr, "\n");
   AMRWBPLUS_FPRINTF(stderr, "\n");

   AMRWBPLUS_FPRINTF(stderr, "-lc     low complexity (for AMR-WB+ modes).\n");
   AMRWBPLUS_FPRINTF(stderr,
                   "-dtx    enables VAD/DTX functionality (for AMR-WB modes).\n");
   AMRWBPLUS_FPRINTF(stderr, "\n");
   AMRWBPLUS_FPRINTF(stderr, "-ff     3gp File Format / raw format\n");
   AMRWBPLUS_FPRINTF(stderr, "-if     input audio WAV file.\n");
   AMRWBPLUS_FPRINTF(stderr, "-of     output AMRWB+ 3gp file.\n");
   AMRWBPLUS_FPRINTF(stderr, "-cf     configuration file\n");
   AMRWBPLUS_FPRINTF(stderr, "\n");

}

static Word32 get_bitrate(EncoderConfig * conf)
{
  Word32 Ltmp;
  Word16 tmp16;
  test();
  if(conf->fscale!=0) 
  {
    tmp16 = mult_r(get_nb_bits(conf->extension, conf->mode, conf->st_mode), 26214); /*Q0*Q21 -> Q6*/
    Ltmp = L_mult(conf->fscale, mult_r(tmp16,21845));   /* Q6*Q6 ->Q13 */
    return Ltmp;
  }
  else 
  {
    return L_shr(L_mult(get_nb_bits(conf->extension, conf->mode, conf->st_mode),26214),9); /*Q22 - 9 ->Q13*/
  }

}
void get_raw_3gp_mode(Word16 *mode, Word16 *st_mode, Word16 raw_3gp_mode ,Word16 extension )
{ 
    Word16 index;
    /* Mono mode only */
    test();test();test();test();test();test();test();test();
    if( sub(raw_3gp_mode,8) <= 0 )
    {
        test();
        if (extension != 0)
        {
            AMRWBPLUS_FPRINTF(stderr, "-isf is not supported by amr_wb\n");
            exit(EXIT_FAILURE);
        }

        *mode = raw_3gp_mode;       move16();move16();
        *st_mode = -1;
    }
    else if(sub(raw_3gp_mode, 10)== 0)    /* 14m */
    {
        *mode = 2;      move16();move16();
        *st_mode = -1;
    }
    else if (sub(raw_3gp_mode,11)== 0) /* 18s */
    {
        *mode = 2;      move16();move16();
        *st_mode = 6;
    }
    else if (sub(raw_3gp_mode,12)== 0)   /* 24m*/
    {
        *mode = 7;          move16();move16();
        *st_mode = -1;
    }
    else if (sub(raw_3gp_mode,13) == 0)    /*24s*/
    {
        *mode = 5;          move16();move16();
        *st_mode = 7;
    }  
    else if( (sub(raw_3gp_mode,16) >= 0) && (sub(raw_3gp_mode,24) < 0) )
    {
        *mode =   raw_3gp_mode - 16;  move16();move16();
        *st_mode = -1;
    }
    else if( (sub(raw_3gp_mode,24) >= 0) && (sub(raw_3gp_mode,47) <= 0) )
    {
        index = raw_3gp_mode - 24;  move16();move16();move16();
        *mode = miMode_fx[2*index];
        *st_mode = miMode_fx[2*index+1];
    }
    else
    {
        AMRWBPLUS_PRINT("Invalid Mode Index\n");
        exit(EXIT_FAILURE);
    }

}
static Word16 get_isf_index(Word16 *fscale)
{ 
  Word16 index, i ;
  Word16 dist, tmp16;
  index = 0;    move16();
  dist = 512;     move16();
  /* Mono mode only */
  for (i = 0;i < 14; i++)
  {
    tmp16 = abs_s(sub(*fscale, isfIndex_fx[i]));
    test();
    if(sub(tmp16,dist) < 0)
    {
      dist = tmp16;   move16();
      index = i;      move16();
    }

  }
  *fscale = isfIndex_fx[index];      move16();
  return index;
}
static void parsecmdline(
  int argc,
  char  *argv[],
  char  **input_filename,
  char  **output_filename, 
  char **config_filename, 
  EncoderConfig * conf,
  Word16 *rate 
  )
{
   Word16 simple_mode,amr_wb, amr_wbp_carac, mi_mode;
   Word32 srate;
   Word32 mrate;
    
   if (argc == 1)
   {
      usage(argv[0]);
      exit(EXIT_FAILURE);
   }
   conf->extension    = 0;               move16();
   conf->allow_dtx    = 0;               move16();
   conf->use_case_mode= USE_CASE_A;      move16();
   conf->fscale       = 0;               move16();
   conf->mode         = -1;              move16();
   conf->st_mode      = -1;              move16();
   conf->FileFormat   = F3GP;            move16();
   conf->mode_index   = -1;              move16();
   conf->fscale_index = 0;               move16();
   conf->bc           = 0;               move16();
   
   
   simple_mode = 0;                      move16();
   amr_wb = 0;                     move16();
   amr_wbp_carac = 0;                     move16();
   mi_mode = 0;                          move16();
   mrate = -1;                           move16();
   srate = -1;                           move16();
   *rate = -1;                           move16();
 
   argc--;
   argv++;

  
      test();
      while (argc > 0)
      { test();
        if (!strcmp(*argv, "-mi"))
        {  test();test();
           if (simple_mode)
           {
              AMRWBPLUS_FPRINTF(stderr, "Can't use -rate with -mi\n");
              exit(EXIT_FAILURE);
           }
           mi_mode = 1;
           argv++;
           argc--;
         
           conf->mode_index = atoi(*argv);   move16();
           test();test();test();test();test();
           if (conf->mode_index < 0 || sub(conf->mode_index,47) > 0)
           {
              AMRWBPLUS_FPRINTF(stderr, "Unknown Mode Index (see TS 26.290)\n");
              exit(EXIT_FAILURE);
           }
           else if (sub(conf->mode_index,9) == 0 || sub(conf->mode_index,14) == 0 || sub(conf->mode_index,15) == 0)
           {
              AMRWBPLUS_FPRINTF(stderr, "Mode Index %d is reserved (see TS 26.290)\n", conf->mode_index);
              exit(EXIT_FAILURE);  
           }
           else
           {
             test();test();test();test();
             if ( (conf->mode_index >= 0) &&  (sub(conf->mode_index,8) <= 0) ) /* amr_wb modes */
             {
                 get_raw_3gp_mode(&(conf->mode), &(conf->st_mode),(Word16) atoi(*argv), conf->extension);
                 amr_wb = 1;
             }
             else if ( (sub(conf->mode_index,10) >= 0) && (sub(conf->mode_index,13) <= 0) )  /* WB+ tested modes */    
             {  
                 get_raw_3gp_mode(&(conf->mode), &(conf->st_mode),(short) atoi(*argv), conf->extension);
                 conf->extension = 1;
                 amr_wbp_carac = 1;
             }
             else
             {
                conf->extension = 1;      move16();
                get_raw_3gp_mode(&(conf->mode), &(conf->st_mode),conf->mode_index, conf->extension );
                test();
                if(conf->fscale == 0)
                {
                  conf->fscale = FSCALE_DENOM;   move16();
                  conf->fscale_index = 8;   
                }

             }
         
           }
        }
        else if (!strcmp(*argv, "-isf"))
        {
           if (simple_mode)
           {
              AMRWBPLUS_FPRINTF(stderr, "No simple mode when using -isf\n");
              exit(EXIT_FAILURE);
           }
           if (amr_wb)
           {
              AMRWBPLUS_FPRINTF(stderr, "-isf is not supported by amr_wb\n");
              exit(EXIT_FAILURE);
           }
           argv++;
           argc--;
           mi_mode = 1;               /* -isf is only allow with -mi */
           conf->extension = 1;   move16();
           if ((atof(*argv) >= 0.5) && (atof(*argv) <= 1.5))
           {
              conf->fscale = (Word16) ((atof(*argv) * FSCALE_DENOM) + 0.5f);   move16();
              /* force scale to be an even number */
              conf->fscale = (conf->fscale >> 1) << 1;   move16();
              /* limit the scale factor */
              test();
              if (sub(conf->fscale,FAC_FSCALE_MAX) > 0)
              {
                 conf->fscale = FAC_FSCALE_MAX;   move16();
              }
              test();
              if (sub(conf->fscale,FAC_FSCALE_MIN) < 0)
              {
                 conf->fscale = FAC_FSCALE_MIN;   move16();
              }
              conf->fscale_index = get_isf_index(&(conf->fscale));        move16(); /* Use "fscale from index" */
           }
           else
           {
              AMRWBPLUS_FPRINTF(stderr, "Unknown Inernal Sampling Frequency factor\n");
              exit(EXIT_FAILURE);
           }
        }
        else if (!strcmp(*argv, "-rate"))
        {
          test();
          if(mi_mode > 0)
          {
            AMRWBPLUS_FPRINTF(stderr, "Can't use -rate with -mi or -isf \n");
            exit(EXIT_FAILURE);
          }
          argv++;
          argc--;
          simple_mode = 1;      move16();
          *rate = (Word16)(atof(*argv)*64+0.5);     move16(); /*Q6*/
          conf->extension = 1;                      move16();
          test();test();
          if(sub(*rate,384) < 0 || sub(*rate,3072) > 0) 
          {
            AMRWBPLUS_FPRINTF(stderr, "Minimum rate is 6.0kbps and maximum rate is 48.0 kbps\n");
            exit(EXIT_FAILURE);
          }
        }
        else if (!strcmp(*argv, "-mono"))
        {
            conf->st_mode = -2; move16(); /* indicate mono is forced */
            test();
            if(sub(*rate,2304) > 0) 
            {
               AMRWBPLUS_FPRINTF(stderr, "Maximum mono rate is 36.0 kbps\n");
               exit(EXIT_FAILURE);
            }
        }
        else if (!strcmp(*argv, "-lc"))
        {
           conf->use_case_mode = USE_CASE_B;   move16();
        }
        else if (!strcmp(*argv, "-dtx"))
        {
           conf->allow_dtx = 1;   move16();
        }
        else if (!strcmp(*argv, "-bc"))
        {
           conf->bc = 1;
        }
        else if (!strcmp(*argv, "-if"))
        {
           argv++;
           argc--;
           *input_filename = *argv;   move16();
        }
        else if (!strcmp(*argv, "-of"))
        {
           argv++;
           argc--;
           *output_filename = *argv;   move16();
        }
        else if (!strcmp(*argv,"-cf")) 
        {
            argv++;
            argc--;
            *config_filename = *argv;   move16();
        }
        else if (!strcmp(*argv, "-ff"))
        {
           argv++;
           argc--;
           if(!strcmp(*argv, "raw"))
           {
              conf->FileFormat = FRAW;
           }
           else
           {
              conf->FileFormat = F3GP;
           }
        }
        else
        {
           AMRWBPLUS_FPRINTF(stderr, "Unknown RTP/3GP File Format option %s\n", *argv);
           exit(EXIT_FAILURE);
        }
        argv++;
        argc--;
     }
     if (amr_wbp_carac && conf->fscale != 0)
     {
        AMRWBPLUS_FPRINTF(stderr, "-isf is not supported with AMR WB caracterized modes\n");
        exit(EXIT_FAILURE);
     }
     if (conf->st_mode == -2 && simple_mode != 1)
     {
        AMRWBPLUS_FPRINTF(stderr, "Choose right Mode Index to encode mono File\n-mono is only supported with -rate\n");
        exit(EXIT_FAILURE);
     }
   
}


static void set_frame_length(
  Word32  samplingRate, 
  Word16  fscale, 
  Word16  *L_frame,
  Word16  *L_next,
  Word16  *L_next_st,
  Word16 *invsamplingRate)
{

  switch (samplingRate) 
  {
    test();test();test();test();test();
    case 8000:
      test();
      if(fscale==0)
      {
        *L_frame = L_FRAME8k;       move16();
        *L_next = L_NEXT8k;         move16();
        *L_next_st = L_NEXT_ST8k;   move16();
        *invsamplingRate = 16777;   move16(); /*Q15+12*/
      }
      else 
      {
        AMRWBPLUS_FPRINTF(stderr, "8kHz files are allowed only without fsratio(isf) -mi <= 14  \n");
        exit(EXIT_FAILURE);
      }
    break;

    case 16000:
      test();
      if(fscale==0)
      {
        *L_frame = L_FRAME16kPLUS;          move16();
        *L_next = L_NEXT16k;                move16();
        *L_next_st = L_NEXT_ST16k;          move16();
        *invsamplingRate = 8389;   move16(); /*Q15+12*/
      }
      else 
      {
        AMRWBPLUS_FPRINTF(stderr, "16kHz files are allowed only without fsratio(isf) -mi <= 14  \n");
        exit(EXIT_FAILURE);
      }
    break;

    case 24000:
      test();
      if(fscale==0)
      {
        *L_frame = L_FRAME24k;               move16();
        *L_next = L_NEXT24k;               move16();
        *L_next_st = L_NEXT_ST24k;         move16();
        *invsamplingRate = 5592;   move16(); /*Q15+12*/
      }
      else 
      {
        AMRWBPLUS_FPRINTF(stderr, "24kHz files are allowed only without fsratio(isf) -mi <= 14  \n");
        exit(EXIT_FAILURE);
      }
    break;

    case 32000:
      test();
      if (sub(fscale,FSCALE_DENOM) == 0) 
      {
        *L_frame = L_FRAME32k;    move16();
        *invsamplingRate = 4194;   move16(); /*Q15+12*/
      }
      else 
      {
        AMRWBPLUS_PRINT( "32kHz WAV files are allowed only at fsratio=1.\n" );
        exit(EXIT_FAILURE);
      }
    break;

    case 48000:
      *L_frame = 2*L_FRAME48k;      move16();
      *invsamplingRate = 2796;   move16(); /*Q15+12*/
    break;

    default:
      AMRWBPLUS_FPRINTF(stderr,"error in sampling freq: use 16, 24, 32 or 48 kHz \n");
      exit(EXIT_FAILURE);
    break;
  }
}
static void deinterleave(
  Word16 *buf,
  Word16 *left,
  Word16 *right,
  Word16 length
)
{
  Word16 i;
  for (i=0; i<length; i++) 
  {
    left[i] = buf[i*2];           move16();
    right[i] = buf[(i*2)+1];      move16();
  }

}
static void GetRate(EncoderConfig *conf, Word16 rate, const Word16 *TableRate, Word16 lenght)
{
  Word16 index, i ;
  Word16 dist, ftmp;
  dist = MAX_16;     move16();
  /* Mono mode only */
  for (i = 0;i < lenght; i+=3)
  {
    ftmp = abs_s(sub(rate, TableRate[i]));
    test();
    if(sub(ftmp,dist) < 0)
    {
      dist = ftmp;    move16();
      index = i;      move16();
    }
  }
  conf->mode_index = TableRate[index+1];    move16();
  conf->fscale_index = TableRate[index+2];    move16();
  get_raw_3gp_mode(&conf->mode, &conf->st_mode, conf->mode_index, conf->extension);
  conf->fscale = isfIndex_fx[conf->fscale_index];    move16();
}

long ext_frame = 0;

#ifdef _DEBUG
#include <crtdbg.h>

void DetectMemLeaks() 
{ 
     _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF|_CRTDBG_CHECK_ALWAYS_DF); 
     _CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE); 
     _CrtSetReportFile(_CRT_ASSERT,_CRTDBG_FILE_STDERR); 
} 

#endif
void Close_wbp(Coder_State_Plus_fx *st, Word16 UseCaseB)
{

  if(st->_stClass != NULL && UseCaseB > 0 )
  {
    free(st->_stClass);
    st->_stClass = NULL;
  }
  if(st->_vadSt != NULL && UseCaseB > 0 )
    Wb_vad_exit(&st->_vadSt);
 

  if(st != NULL)
  {
    free(st);
    st = NULL;
  }

}
/* Don't have to be instrumented */
Word16 get_config(FILE *fp, Word32 t[])
{
  Word16 OK = 0;
  
  if (!fp || feof(fp))
    return 0;
  
  /*
    read from config file the following items
    time  extension  mode_index fscale 
    time (in seconds) must always be above 0.0, 0.000001 is OK
  */
  
  while (!OK && !feof(fp)) 
  {
    Word8 s[100], *sp;
    Word32 ix  = 0;
    t[0] = 0;

    fgets(s,99,fp);
    sp = strtok(s," \t");
    
    t[0] = (Word32)(atof(sp)*8192+0.5);    /*Q13*/
    sp = strtok(0," \t");
  
    if (sp)
    {
      t[1] = (Word32)(atoi(sp));    /*extension Q0*/
      sp = strtok(0," \t");
  
      if (sp)
      {
        t[2] = (Word32)(atoi(sp));    /*mi Q0*/
        sp = strtok(0," \t");
        if (sp)
        {
          t[3] = (Word16)((atof(sp) * FSCALE_DENOM) + 0.5f);       /*fscale Q0*/
         /* force scale to be an even number */
          //t[3] = ((t[3]>> 1)<< 1); 
          sp = strtok(0," \t");
        }
      }
    }
    
    if (t[0] != 0)
    {
      return 1;
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{

  FILE *f_speech;         /* File of speech data			 */	
  FILE *f_serial;         /* File of serial bits for transmission  */

  Word16  speech16[L_FRAME_FSMAX*2];
  Word16  serial[NBITS_MAX];			/* serial parameters.			 */
  unsigned char serialAmrwb[NBITS_MAX];		/* serial parameters.			 */

  Word16 channel_right[4*L_FRAME_FSMAX]; // Kan halveras om ett sampel i taget läses in
  Word16 channel_left[2*L_FRAME_FSMAX];

  Coder_State_Plus_fx *st;
  Word16  numOfChannels, bitsPerSample; 	
  void *stAmrwbEnc=NULL;

  Word32 frame;
  Word16 i, lg, L_frame, nb_bits=0;
  Word32 samplingRate, dataSize;
  Word16 L_next, L_next_st;
  Word16 nb_samp, invsamplingRate, nb_hold = 0;
  Word16 mode, extension;
  Word32 Ltmp;
  Word16 serial_size=0;


  char *input_filename;
  char *output_filename;
  Word16 st_mode, old_st_mode;
  EncoderConfig conf;
  Word16  fst;
  Word16 rate;
  char FileFormatType[25];

  FILE *f_config = NULL;
  Word32 rec_time = 0;
  Word32 config_file_time = 0;
  Word32 t[4]; /* time,conf.extension,conf.mode_index,conf.fscale */
  Word32 old_bitrate;
  Word8 *config_filename = NULL;
  Word16 Chan2mult;



  /* Initializations */
  input_filename = NULL;
  output_filename = NULL;
  move16();move16();move16();move16();move16();move16();move16();


#ifdef _DEBUG
DetectMemLeaks() ;
#endif

  /* Display Copyright Banner */
  copyright();

  /* Parse Command Line  */
  parsecmdline(argc, argv, &input_filename, &output_filename,&config_filename, &conf  ,&rate
        );
  
  /* Open input wave file */

  if ((f_speech = Wave_Fopen(input_filename, "rb", &numOfChannels, &samplingRate, 
        &bitsPerSample, &dataSize)) == NULL)
  {
    AMRWBPLUS_FPRINTF(stderr, "Error opening the input file %s.\n", input_filename);
    exit(EXIT_FAILURE);
  }
   /* Simple interface */
  test();
  if (sub(rate,-1) != 0)
  {
    test();test();
    if(sub(conf.st_mode,-2) == 0 || sub(numOfChannels,1) == 0)
    {
        GetRate(&conf, rate, MonoRate, 3*18);
    }
    else
    {
        GetRate(&conf, rate, StereoRate, 3*27);
    }

  }

  /* test if it is stereo input */
  test();test();
  if((conf.st_mode >= 0) && (sub(numOfChannels,2) != 0) ) 
  {
    AMRWBPLUS_FPRINTF(stderr, "Input file %s must be stereo\n", input_filename);
    exit(EXIT_FAILURE);
  }

  /* test if it is 16 bits PCM */
  test();
  if(sub(bitsPerSample,16) != 0) 
  {
    AMRWBPLUS_FPRINTF(stderr, "Input file %s must be 16 bits encoded\n", input_filename);
    exit(EXIT_FAILURE);
  }
  test();
  if (sub(conf.FileFormat,F3GP) == 0)
  {
      strcpy(FileFormatType,"3gp File Format");
      test();
      if(sub(conf.bc,1) == 0) 
      {
        /* create backward compatible file */
        Create3GPAMRWB();
      } 
      else 
      {
        Create3GPAMRWBPlus();
      }
  }
  else
  {
      strcpy(FileFormatType,"Raw File Format");
      /* Open the output bitstream file */
      if ((f_serial = fopen(output_filename, "wb")) == NULL) 
      {
	      AMRWBPLUS_FPRINTF(stderr, "Error opening output bitstream file %s.\n",output_filename);
	      exit(EXIT_FAILURE);
      }
  }
  if ((conf.extension == 0) && (samplingRate != 16000))
  {
    AMRWBPLUS_FPRINTF(stderr, "AMR-WB work only at 16kHz\n");
    exit(EXIT_FAILURE);
  }

  /* set fscale=1.0 when fs = 32000 or 48000 */
  test();test();test();
  if (((L_sub(samplingRate,32000) == 0) || (L_sub(samplingRate,48000) == 0)) 
        && (conf.fscale == 0))
  {
    conf.fscale = FSCALE_DENOM;         move16();
    conf.fscale_index = 8;          move16();
  }


  /* Set default buffer lengths */
  set_frame_length(samplingRate,conf.fscale,&L_frame,&L_next,&L_next_st, &invsamplingRate);

  /* reserve memory for encoder and decoder states */
  st = malloc(sizeof(Coder_State_Plus_fx));
  
  /*
   Display Information
  */
  old_bitrate = get_bitrate(&conf);
  AMRWBPLUS_FPRINTF(stderr,"%s\nEncoding @ %6.2fkbps",FileFormatType ,(float)old_bitrate/8192.0);

  /*
   Initialise the Complexity Counters
  */
  Init_WMOPS_counter ();


  /* AMRWB+ extensions*/
  Init_coder_amrwb_plus(st, numOfChannels, conf.fscale, conf.use_case_mode,1);

  stAmrwbEnc = E_IF_init_fx();
  /*
   Fill-up lookahead buffers 
  */
  lg = Read_data(f_speech, channel_right, (numOfChannels*L_frame));
  test();
  if (L_sub(lg, L_shr(L_mult(numOfChannels,L_frame),1)) != (0)) 
  {  
    AMRWBPLUS_PRINT("Error: file too Word16 !\n");
    exit(EXIT_FAILURE);
  }
  test();  
  if (sub(numOfChannels,2) == 0) 
  {
    /* remove interleave of left and right samples */				
    deinterleave(channel_right,channel_left,channel_right,L_frame);
  }
  test();  
  if (conf.extension > 0)
  {
    test();
    nb_samp = Coder_amrwb_plus_first(channel_right, channel_left, numOfChannels,  L_frame,
                                    (numOfChannels == 1) ? L_next : L_next_st, conf.fscale, st);
  }
  else
  {
    E_IF_encode_first_fx(stAmrwbEnc, speech16);
    nb_samp = 320;      move16();
  }

  AMRWBPLUS_FPRINTF(stderr,"\n --- Running ---\n");

  /*---------------------------------------------------------------------------*
  * Loop for every analysis/transmission frame.                               *
  *   -New L_FRAME_PLUS data are read. (L_FRAME_PLUS = number of speech data per frame) *
  *   -Conversion of the speech data from 16 bit integer to real              *
  *   -Call coder_wb to encode the speech.                                    *
  *   -The compressed serial output stream is written to a file.              *
  *   -The synthesis speech is written to a file                              *
  *--------------------------------------------------------------------------*/

  /* set current config */
  mode = conf.mode;                         move16();
  extension = conf.extension;               move16();    
  old_st_mode = st_mode = conf.st_mode;     move16();move16();
  frame=0;                                  move32();

  fst = conf.fscale;                        move16();
  test();
  if (config_filename != 0) 
  {
    /* Don'T complexity of config file */
    f_config = fopen(config_filename,"r");
    if (!f_config) 
    {
      AMRWBPLUS_FPRINTF(stderr, "Error opening config file %s.\n",config_filename);
      exit(EXIT_FAILURE);
    }
    
    while (!get_config(f_config,t) && !feof(f_config)) 
    {
       AMRWBPLUS_PRINT("%2.3f \n",(float)t[0]/8192.0);
    }
    config_file_time = t[0];   
    rec_time = 0;              
  }



  Chan2mult = sub(numOfChannels,1);
  dataSize = L_shl(dataSize,Chan2mult);
  dataSize = L_sub(dataSize, lg);

  test(); test();
  while (lg != 0 || frame == 0)
  {
      test();test();
      Reset_WMOPS_counter();
      
      AMRWBPLUS_FPRINTF(stderr," Frames processed: %ld    \r", frame);
      /* Update frame counter */
      frame =L_add(frame,1);

      test();
      if (f_config) 
      {  
         Word16 tmp_modeIndex;

         /*Don'T count complexity of reading mode config but count complexity if handling it */
         test(); test();
         if (L_sub(rec_time,config_file_time) >= 0 && L_sub(t[0],-8192) != 0 && !feof(f_config)) {
            extension = extract_l(t[1]); 
            tmp_modeIndex = extract_l(t[2]);  

            test();
            if (extension == 0) 
            {
               mode  = tmp_modeIndex;   move16();
               st_mode = -1;              move16();
               conf.mode = tmp_modeIndex;   move16();
               conf.st_mode = -1;  move16();
               conf.mode_index = tmp_modeIndex; move16();
            } 
            else
            {
               conf.mode_index = tmp_modeIndex;        move16();
               get_raw_3gp_mode(&(conf.mode), &(conf.st_mode),conf.mode_index, extension );
            }
            fst = extract_l(t[3]);  
            get_isf_index(&fst);      /* Use "fst from index" */
            
            while (!get_config(f_config,t)&& !feof(f_config)) 
            {
            }
            config_file_time = t[0];    move32();
         }
      }

      test();
      if (sub(fst,conf.fscale) != 0) 
      {
          conf.fscale = fst;    move16();
          Init_coder_amrwb_plus(st, numOfChannels, conf.fscale,
                              conf.use_case_mode, 0);

          set_frame_length(samplingRate,conf.fscale,&L_frame,&L_next,&L_next_st, &invsamplingRate);
          conf.fscale_index = get_isf_index(&(conf.fscale));      /* Use "fscale from index" */
      }

      nb_hold = sub(L_frame, nb_samp);

      Copy(channel_right+nb_samp, channel_right, nb_hold);
      Copy(channel_left+nb_samp, channel_left, nb_hold);
      lg = Read_data(f_speech, channel_right+nb_hold, (numOfChannels*(L_frame-nb_hold)));

      dataSize = L_sub(dataSize, lg);
      test();
      if(dataSize<0) /* prevent to read end file wav header */
      {
        break;
      }
      /* Remove interleaving in case of stereo input */
      test();
      if (sub(numOfChannels,2) == 0) 
      {
          deinterleave(channel_right+nb_hold,channel_left+nb_hold,channel_right+nb_hold,L_frame-nb_hold);
      }
      
      /* Test if the mode changed and update the config accordingly */
      /* Should only be used for mode switching experiments.... */
      
      test();test();test();test();
      if(((extension==0) && (sub(conf.extension,1) == 0 ))
          || ((sub(extension,1) == 0) && (conf.extension == 0)))
      {
      /* Copy encoder data in case of switch between WB and WB+ modes */
        test();test();test();test();test();
        if ( ((mode >= 0 && sub(mode,9) <= 0) || sub(mode,15) == 0) && (conf.extension>0) )
        {
            /* Switch from WB+ to WB */
            Copy_coder_state(st, stAmrwbEnc, 1, conf.use_case_mode); 
        }
        else if ( (mode >= 0 && sub(mode,8) <= 0) && (conf.extension==0) )
        {
            /* Switch from WB to WB+ */
            Copy_coder_state(st, stAmrwbEnc, 0, conf.use_case_mode);
        }
        conf.extension = extension;        move16();
      }
      test();  
      if (conf.extension > 0) 
      {
        /*     AMRWB+ Extensions */
        /* update needed if mode changes */
         nb_bits = get_nb_bits(conf.extension, conf.mode, conf.st_mode);
         test();
         if (sub(numOfChannels,2) == 0)
         {
              nb_samp = Coder_amrwb_plus_stereo(channel_right, channel_left, conf.mode, 
                                  L_frame, serial, st, conf.use_case_mode, conf.fscale, conf.st_mode);
          }
          else
          {   
              nb_samp = Coder_amrwb_plus_mono(channel_right,  conf.mode, 
                                      L_frame, serial, st, conf.use_case_mode, conf.fscale);
          }
          old_st_mode = conf.st_mode;     move16();
          
         /* write into the bitstream */
          test(); 
          if(sub(conf.FileFormat,F3GP) == 0)
          {
            WriteSamplesAMRWBPlus( conf,serial, nb_bits);
          }
          else
          {
              for (i=0; i<4; i++)
              {
                  WriteHeader(conf, nb_bits, i, f_serial);  
                  WriteBitstreamPlus( nb_bits, i, serial, f_serial);    
              }
          }
      }
      else 
      {
          
          /*        AMRWB                                        */
          
          for (i=0;i<4;i++) 
          {
              serial_size = (Word16 ) E_IF_encode_fx(stAmrwbEnc, (Word16)conf.mode, &channel_right[i*320], serialAmrwb, conf.allow_dtx);
              if(conf.FileFormat == F3GP)
              {
                WriteSamplesAMRWBPlus(conf,serialAmrwb, serial_size);
              }
              else
              {
                WriteHeader(conf, (Word16)serial_size, i, f_serial);  
                WriteBitstream( (Word16)serial_size, serialAmrwb, f_serial);    
              }
          }
          nb_samp = 4 * L_FRAME16k;  move16();

      }
      test();
      if (L_sub(L_abs(L_sub(old_bitrate,get_bitrate(&conf))),1) > 0) 
      {
         old_bitrate = get_bitrate(&conf);
         AMRWBPLUS_FPRINTF(stderr, "Rectime: %2.3f Encoding @ %6.2fkbps\n", (float)rec_time/8193.0f, (float)old_bitrate/8192.0f);
      }
      Ltmp = mult(shl(nb_samp,1), invsamplingRate);
      rec_time = L_add(rec_time, Ltmp);
      
  }
    
  if(conf.FileFormat == F3GP)
  {
      Close3GP(output_filename);
  }
  else
  {
      fclose(f_serial);
  }

 
  if(stAmrwbEnc != NULL)
    E_IF_exit_fx(stAmrwbEnc);
  
  Close_wbp(st, conf.use_case_mode);  

  if (fst == 0) fst = FSCALE_DENOM;
  WMOPS_output(1, (float)fst / FSCALE_DENOM );
  
  Wave_Fclose(f_speech, bitsPerSample);
  
  return 0;
}
