#ifdef __cplusplus
extern "C" {
#endif

#include "voCheck.h"
#include "voAMRWBP.h"
#include "typedef.h"
#include "basic_op.h"
#include "count.h"

#include "amr_plus_fx.h"
#include "dec_if_fx.h"
#include "enc_if_fx.h"
#include "voChHdle.h"

#include "amrwbplus_d_api.h"



extern const UWord8 voAMRWBPDecBlock_size[];
#if  AMR_DUMP
int amr_debug = 0; 
FILE* fLog = NULL;

#endif//AMR_DUMP

#if defined (_WIN32_WCE)  || defined(WIN32) || defined (LCHECK) 
VO_PTR	g_hAMRWBPDecInst = NULL;
#endif

#define AMR_MEM_ALIGN 32
#define AMR_MEM_ALIGN_MASK 31 
 void *voAMRWBPDecAlignedMalloc(unsigned int size)
{
	Word8 advance;
	Word8 *realPt, *alignedPt;
	size += AMR_MEM_ALIGN;

	if ((realPt = calloc(1,size)) == NULL)
    	return NULL;
	advance = (Word8)(AMR_MEM_ALIGN - ((Word32)realPt & AMR_MEM_ALIGN_MASK));
	//assert(advance > 0);
	alignedPt = realPt + advance; // to aligned location;
	*(alignedPt - 1) = advance; // save real AlignedMalloc pt at alignedPt[-1] location for AlignedFree;

	return alignedPt;
	//return voAMRWBPDecAlignedMalloc(size);
}

void voAMRWBPDecAlignedFree(void *alignedPt)
{
	if (alignedPt)
		free((Word8 *)((Word8 *)alignedPt - *((Word8 *)alignedPt - 1))); // AlignedFree real AlignedMalloc location;
}

static Word32 get_bitrate(DecoderConfig * conf)
{
  Word32 Ltmp;
  Word16 tmp16;
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

static int select_fs(DecoderConfig* conf) 
{
  /* default sampling rate if undefined */
  if (conf->fs == 0)
  {
    if (conf->extension == 0)
    {
      conf->fs=16000;		/*16khz for AMRWB*/
    }
    else if (conf->fscale == 0)
    {
      conf->fs=24000;      /*24khz*/
    }
    else
    {
      conf->fs = 48000;		/*48kHz*/
    }
  } 

  /* user specified sampling rate */
  else 
  {
    if(conf->extension == 0) 
    {
      if(L_sub(conf->fs,16000) != 0) 
      {
        //AMRWBPLUS_FPRINTF( stderr, "Sampling rate not supported" );
        return -1;
      }
    } 
    else
    {
      if (conf->fscale == 0) 
      {
        if ( (L_sub(conf->fs,16000) != 0) && (L_sub(conf->fs,24000) != 0) && (L_sub(conf->fs,8000) != 0)) 
        {
          //AMRWBPLUS_FPRINTF( stderr, "Sampling rate not supported" );
          return -1;
        }
      }
      else
      {
        if ((L_sub(conf->fs,44100)!=0) && (L_sub(conf->fs,48000) != 0) ) 
        {
          //AMRWBPLUS_FPRINTF( stderr, "Sampling rate not supported" );
          return -1;
        }
      }
    }
  }

  return 0;
}
static int set_frame_length(
  DecoderConfig* conf,
  Word16 *L_frame
)
{
  
  switch (conf->fs)
  {
      
    case 8000:
      *L_frame = L_FRAME8k;
      break;
      
    case 16000:
      *L_frame = L_FRAME16kPLUS;      
      break;      
    case 24000:
      *L_frame = L_FRAME24k;
      break;
    case 32000:
      *L_frame = L_FRAME32k;
      break;
#ifdef FILTER_44kHz
    case 44100:
      *L_frame = L_FRAME44k;
      break;
#endif
#ifdef FILTER_48kHz
    case 48000:
      *L_frame = L_FRAME48k;      
      break;
#endif
      
    default:
      return -1;
  }

  return 0;
}

static void Interleave(
  Word16 *right,
  Word16 *left,

  Word16 *out,
  Word16 length
)
{
  Word16 i;

  for (i=sub(length,1); i>=0; i--) 
  {
    out[(i*2)+1] = right[i];
    out[i*2] = left[i];
  }

}


static void Simple_frame_limiter(    
  Word16 x[],    /* i/o: synthesis                        Qx */
  Word16 Qx,     /* i:   scale factor of x                  */
  Word16 mem[],  /* i/o: mem[2] should be init to [0,0]     */
  Word16 n       /* i:   vector size                        */
)
{
#if (FUNC_SIMPLE_FRAME_LIMITER_OPT)
  Word16 i, fac, prev, tmp, frame_fac, max;
  Word32 L_tmp;

  /*------------------------------------------------------------------------------------------*
  * calculate frame AGC factor to avoid saturation 
  *------------------------------------------------------------------------------------------*/

  max = 0;
  for (i=0; i<n; i++)
  {
    tmp = abs_s(x[i]);
    max = (max > tmp) ? max : tmp;
  }

  tmp = shl(30000, Qx);      /* saturation can occurs here */

  frame_fac = 0;
  if (max > tmp)
  {
    //frame_fac = sub(16384, div_s(shr(tmp, 1), max));   /* frame fac in Q15 */
    frame_fac = sub(16384, div_s((tmp >> 1), max));   /* frame fac in Q15 */
    //frame_fac = 16384 - div_s((tmp << 1), max);
  }

  fac = mem[0];   /* fac in Q15 */
  prev = mem[1];   /* prev in Q-1 */

  /*------------------------------------------------------------------------------------------*
   * AGC
   *------------------------------------------------------------------------------------------*/

  for (i=0; i<n; i++)
  {
    /* update AGC factor (slowly) */
    //fac = round16(L_mac(L_mult(32440, fac), 328, frame_fac));
    fac = (32440 * fac + 328 * frame_fac + 0x4000) >> 15;

    /* convert float to integer with AGC */
    //L_tmp = L_deposit_h(x[i]);
    L_tmp = ((Word32)x[i]) << 16;
    //L_tmp = L_msu(L_tmp, fac, x[i]);
    //L_tmp = L_msu(L_tmp, fac, prev);
    L_tmp -= (fac * x[i] + fac * prev) << 1;
    L_tmp = L_shr(L_tmp, Qx);    /* saturation can occurs here */
    
    prev = x[i];
    x[i] = round16(L_tmp);  //can't be modified-shanrong
  }

  mem[0] = fac;
  mem[1] = prev;

  return;

#else

  Word16 i, fac, prev, tmp, frame_fac, max;
  Word32 L_tmp;

  /*------------------------------------------------------------------------------------------*
  * calculate frame AGC factor to avoid saturation 
  *------------------------------------------------------------------------------------------*/

  max = 0;
  for (i=0; i<n; i++)
  {
    tmp = abs_s(x[i]);

    if (sub(tmp, max) > 0)
    {
      max = tmp;
    }
  }

  tmp = shl(30000, Qx);      /* saturation can occurs here */

  frame_fac = 0;
  if (sub(max, tmp) > 0)
  {
    frame_fac = sub(16384, div_s(shr(tmp, 1), max));   /* frame fac in Q15 */
  }

  fac = mem[0]; 			/* fac in Q15 */
  prev = mem[1];			/* prev in Q-1 */

  /*------------------------------------------------------------------------------------------*
   * AGC
   *------------------------------------------------------------------------------------------*/

  for (i=0; i<n; i++)
  {
    /* update AGC factor (slowly) */
    fac = round16(L_mac(L_mult(32440, fac), 328, frame_fac));

    /* convert float to integer with AGC */
    L_tmp = L_deposit_h(x[i]);
    L_tmp = L_msu(L_tmp, fac, x[i]);
    L_tmp = L_msu(L_tmp, fac, prev);
    L_tmp = L_shr(L_tmp, Qx);    /* saturation can occurs here */

    prev = x[i];
    x[i] = round16(L_tmp);


  }

  mem[0] = fac;
  mem[1] = prev;

  return;
#endif
}

AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_StartDecode(AMRWBGlobalData *global)
{
    AMRWBPLUS_DEC_PARAM_T *amrwbplus_para;
	Word32 *conf_fs_temp_p; 
	amrwbplus_para = &(global->params);
	conf_fs_temp_p = &(amrwbplus_para->conf.fs);

#if   AMR_DUMP
#ifdef ARM//PPC
	fLog = fopen("/SDMMC/PPCamrwbpdumplog.txt","wb");
#else//ARM
	fLog = fopen("e:/amrwbpdumplog.txt","wb");
#endif//ARM
#endif//AMR_DUMP
	amrwbplus_para->conf.limiter_on = 1;
    //first allocate memory
    global->stAmrwbDec = D_IF_init_fx();
    if (NULL == global->stAmrwbDec)
    {
        return AMRWBPLUS_DEC_ALLOCATE_MEMORY_FAIL;
    }
	
    //if wb IF2 data to be processed, return directly
    if (WB_MIME == amrwbplus_para->input_data_type)
    {
        global->num_channels = 1;
        amrwbplus_para->outputStream = global->channel_right;
        Set_zero(global->mem_fr_lim_right,2);
        Set_zero(global->mem_fr_lim_left,2);
        return AMRWBPLUS_DEC_OPERATION_DONE;
    }
    //if not WBPLUS, return error
    else if (WBPLUS != amrwbplus_para->input_data_type)
    {
        D_IF_exit_fx(global->stAmrwbDec);       //???
        return AMRWBPLUS_DEC_INPUT_PARA_INVALID;
    }
    
    //AlignedMalloc for st_d
    global->st_d = (Decoder_State_Plus_fx *)voAMRWBPDecAlignedMalloc(sizeof(Decoder_State_Plus_fx));
    if(NULL == global->st_d)
    {
        D_IF_exit_fx(global->stAmrwbDec);       //???
        return AMRWBPLUS_DEC_ALLOCATE_MEMORY_FAIL;
    }
    memset(global->st_d, 0, sizeof(Decoder_State_Plus_fx));

    //In case of bad frame; hard initialization
    if((L_sub(*conf_fs_temp_p ,16000) == 0 || L_sub(*conf_fs_temp_p, 8000) == 0) && sub(amrwbplus_para->conf.mode,14) == 0)
    {   
        //Force to wb
        amrwbplus_para->conf.fscale = 0;
        amrwbplus_para->conf.extension = 0;
    }
    else if ( sub(amrwbplus_para->conf.mode,14) == 0)
    {
        //Force to wb+ if sampling rate desire is different than 16 or 8kHz
        amrwbplus_para->conf.mode = 2;
        amrwbplus_para->conf.fscale = 96;
        amrwbplus_para->conf.extension = 1;
    }
	
    //default sampling rate if undefined
    if (*conf_fs_temp_p == 0)
    {
        if (amrwbplus_para->conf.extension == 0)
        {
            *conf_fs_temp_p = 16000;
        }
        else if (amrwbplus_para->conf.fscale == 0)
        {
            *conf_fs_temp_p = 24000;
        }
        else
        {
#ifdef FILTER_44kHz
            *conf_fs_temp_p = 44100;
#endif
#ifdef FILTER_48kHz
            *conf_fs_temp_p = 48000;
#endif
        }
    }
    
    global->fs_output = *conf_fs_temp_p;
    amrwbplus_para->sampleRate = *conf_fs_temp_p;
    global->fac_up = global->fac_down = 12;			//no oversampling by default
    
    if (amrwbplus_para->conf.fscale != 0)
    {
        switch (*conf_fs_temp_p) 
        {
            case 8000:
            global->fac_up = 2;
            *conf_fs_temp_p = 48000;
            break;
            case 16000:
            global->fac_up = 4;
            *conf_fs_temp_p = 48000;
            break;
            case 24000:
            global->fac_up = 6;
            *conf_fs_temp_p = 48000;
            break;
            case 32000:
            global->fac_up = 8;
            *conf_fs_temp_p = 48000;
            break;
            case 11025:
            global->fac_up = 3;
            *conf_fs_temp_p = 44100;
            break;
            case 22050:
            global->fac_up = 6;
            *conf_fs_temp_p = 44100;
            break;
        }
        Set_zero(global->mem_down_right, 2*L_FILT_DECIM_FS);
        Set_zero(global->mem_down_left, 2*L_FILT_DECIM_FS);
    }
    global->frac_down_right = 0;
    global->frac_down_left = 0;
    global->last_scaling = 0;

    //select sampling rate
    if(select_fs(&(amrwbplus_para->conf)) < 0)
		return -1;
    
    //Set frame lengths and check for sampling frequency
    if(set_frame_length(&(amrwbplus_para->conf),&global->L_frame) < 0)
		return -1;
    
    global->num_channels = 2;
    if(((amrwbplus_para->conf.fscale == 0) && (amrwbplus_para->conf.st_mode < 0))
        || (amrwbplus_para->conf.extension == 0) 
        || (amrwbplus_para->conf.mono_dec_stereo))
    {
        global->num_channels = 1;
    }
    
    Init_decoder_amrwb_plus(global->st_d, (Word16)global->num_channels, amrwbplus_para->conf.fscale, 1);
    
    Set_zero(global->mem_fr_lim_right,2);
    Set_zero(global->mem_fr_lim_left,2);
    
    amrwbplus_para->outputStream = global->channel_right;
    amrwbplus_para->channels	 = global->num_channels;

    return AMRWBPLUS_DEC_OPERATION_DONE;
}

AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_ReceiveData(AMRWBGlobalData *global)
{
    Word16 j;
    Word16 mode;
    Word16 extension;
    Word16 st_mode;
    Word16 Frame_Q;
    Word16 fst;
    Word16 fst_switched;
    Word16 tfi;
	Word16 num_channels;
	Word16 fac_up;
	Word16 *channel_right;
	Word16 *channel_left;
	Word16 *mem_down_right;
	Word16 *mem_down_left;
	Word16 *mem_fr_lim_right;
	Word16 *mem_fr_lim_left;

    Word16 nb_samp;
    Word16 nb_samp_fs;
    Word16 counter;
	AMRWBPLUS_DEC_PARAM_T *amrwbplus_para;
	//Word16 *channel_right = amrwbplus_para->outputStream; //[4*L_FRAME48k];
	//Word16 *channel_left  = [2*L_FRAME48k];
	
	amrwbplus_para = &(global->params);
	num_channels = global->num_channels;
	channel_right = global->channel_right;
	channel_left = global->channel_left;
	mem_down_right = global->mem_down_right;
	mem_down_left = global->mem_down_left;
	mem_fr_lim_right = global->mem_fr_lim_right;
	mem_fr_lim_left = global->mem_fr_lim_left;
	fac_up = global->fac_up;
    //check input parameters
    if (NULL == amrwbplus_para)
    {
        return AMRWBPLUS_DEC_INPUT_POINTER_INVALID;
    }

	/*
     process AWB IF2 data
    */
    if (WB_MIME == amrwbplus_para->input_data_type)
    {
        D_IF_decode_fx(global->stAmrwbDec,
                       (UWord8*)amrwbplus_para->inputStream,
                       amrwbplus_para->outputStream,//speech16,
                       0                                        //0--good frame, 1--bad frame
                      );        
        amrwbplus_para->outputSize = L_FRAME16k;
        
        return AMRWBPLUS_DEC_DECODE_ONE_FRAME;
    }
    //if not WBPLUS, return error
    else if (WBPLUS != amrwbplus_para->input_data_type)
    {
        return AMRWBPLUS_DEC_INPUT_DARA_TYPE_INVALID;
    }
    
    /*
     process AWB+ data
    */
    extension = amrwbplus_para->conf.extension;
    mode      = amrwbplus_para->conf.mode;
    st_mode   = amrwbplus_para->conf.st_mode;
    fst       = amrwbplus_para->conf.fscale;
    
    //begin to decode a frame    
    fst_switched = 0;

    if (amrwbplus_para->conf.fer_sim)
	{
        global->tmpbfi[0] = amrwbplus_para->conf.bfi[0];
        global->tmpbfi[1] = amrwbplus_para->conf.bfi[1];
        global->tmpbfi[2] = amrwbplus_para->conf.bfi[2];
        global->tmpbfi[3] = amrwbplus_para->conf.bfi[3];
	}
    counter = Read_buffer(&tfi, amrwbplus_para->conf.bfi, &(amrwbplus_para->conf), &extension, &mode, &st_mode, &fst, amrwbplus_para->inputStream, (void*) global->serial);
    if (counter > (amrwbplus_para->inputSize))
	{
		//AMRWBPLUS_PRINT("fatal error!");
		return AMRWBPLUS_DEC_NEED_MORE_DATA;
	}
    //bfi and fer_sim       ???
    if (amrwbplus_para->conf.fer_sim)
    {
		if(amrwbplus_para->conf.extension > 0)
		{
			amrwbplus_para->conf.bfi[0] = global->tmpbfi[0];
			amrwbplus_para->conf.bfi[1] = global->tmpbfi[1];
			amrwbplus_para->conf.bfi[2] = global->tmpbfi[2];
			amrwbplus_para->conf.bfi[3] = global->tmpbfi[3];
		}
		else
		{
			amrwbplus_para->conf.bfi[tfi] = global->tmpbfi[0];
		}
    }
    else
    {
        if (amrwbplus_para->conf.extension == 0)
        {
            amrwbplus_para->conf.bfi[tfi] = amrwbplus_para->conf.bfi[0];
        }
    }

    if ((fst != amrwbplus_para->conf.fscale) && !fst_switched) 
    {
        if(select_fs(&(amrwbplus_para->conf)) < 0)
			return -1;
        if(set_frame_length(&(amrwbplus_para->conf), &global->L_frame) < 0)
			return -1;
        fst_switched = 1;
        Init_decoder_amrwb_plus(global->st_d, num_channels, fst, 0);
    }

    //SET CONFIG !!!!!!!
    if(((extension == 0) && (amrwbplus_para->conf.extension == 1))
    || ((extension == 1) && (amrwbplus_para->conf.extension == 0)))
    {
        //Copy decoder data in case of switch between WB and WB+ modes
        if ((((mode >= 0) && (mode <= 9)) || (mode == 15)) && (amrwbplus_para->conf.extension > 0) )
        {
            //Switch from WB+ to WB
            Copy_decoder_state(global->st_d, global->stAmrwbDec, 1);
        }
        else if ( (((mode >= 0) && (mode <= 8)) || (mode == 15)) && (amrwbplus_para->conf.extension == 0) )
        {
            //Switch from WB to WB+
            Copy_decoder_state(global->st_d, global->stAmrwbDec, 0);
        }        
        amrwbplus_para->conf.mode = mode;
        amrwbplus_para->conf.extension = extension;
    }
    
    //decode
    if(amrwbplus_para->conf.extension > 0) 
    {
        nb_samp = Decoder_amrwb_plus(amrwbplus_para->conf.mode, (Word16*)global->serial, amrwbplus_para->conf.bfi, global->L_frame,
                    num_channels, channel_right, channel_left, global->st_d,
                    fst, amrwbplus_para->conf.st_mode, amrwbplus_para->conf.mono_dec_stereo, amrwbplus_para->conf.fscale);

        Frame_Q = global->st_d->Old_Q_syn;
    }
    else 
    {
        nb_samp = L_FRAME16k;
        D_IF_decode_fx(global->stAmrwbDec, (UWord8*)global->serial, global->speech16, (amrwbplus_para->conf.bfi)[tfi]? _bad_frame: _good_frame);
        Frame_Q = 0;
        
        for (j = 0; j < 320; j++)
        {
            channel_right[ j] = global->speech16[j];
        }
    }
    
    if (num_channels == 2)
    {
        //Smooth the output so to avoid harch clipping
        if(amrwbplus_para->conf.limiter_on>0)
        {
            Simple_frame_limiter(channel_right,Frame_Q, global->mem_fr_lim_right, nb_samp);
            Simple_frame_limiter(channel_left,Frame_Q, global->mem_fr_lim_left, nb_samp);
            Frame_Q = 0;
        }
        voAMRWBPDecScale_sig(mem_down_right,2*L_FILT_DECIM_FS, sub(Frame_Q,global->last_scaling));  
        voAMRWBPDecScale_sig(mem_down_left,2*L_FILT_DECIM_FS, sub(Frame_Q,global->last_scaling));  
        
        global->last_scaling = Frame_Q;
        
        nb_samp_fs = Decim_fs(channel_right, nb_samp, channel_right, fac_up, mem_down_right, &global->frac_down_right);
        nb_samp_fs = Decim_fs(channel_left, nb_samp, channel_left, fac_up, mem_down_left, &global->frac_down_left);
        
        Interleave(channel_right, channel_left,channel_right,nb_samp_fs);
        voAMRWBPDecScale_sig(channel_right, (nb_samp_fs << 1), (-Frame_Q));
        amrwbplus_para->outputSize = nb_samp_fs << 1;
    }
    else
    {
        if(amrwbplus_para->conf.limiter_on>0) 
        {
            Simple_frame_limiter(channel_right,Frame_Q, global->mem_fr_lim_right, nb_samp);
            Frame_Q = 0;
        }
        voAMRWBPDecScale_sig(mem_down_right,2*L_FILT_DECIM_FS, sub(Frame_Q, global->last_scaling));  
        global->last_scaling = Frame_Q;
        
        nb_samp_fs = Decim_fs(channel_right, nb_samp, channel_right, fac_up, mem_down_right, &global->frac_down_right);

        //Writ_data(channel_right, nb_samp_fs, f_syn, Frame_Q);
        voAMRWBPDecScale_sig(channel_right, nb_samp_fs, (-Frame_Q));
        //memcpy(amrwbplus_para->outputStream, channel_right, nb_samp_fs * sizeof(Word16));
        amrwbplus_para->outputSize = nb_samp_fs;
    }

    amrwbplus_para->conf.fscale = fst;
    if (fst_switched>0)
    {
        if(set_frame_length(&(amrwbplus_para->conf),&global->L_frame) < 0)
			return -1;
    }

	//memcpy(amrwbplus_para->outputStream,channel_right,amrwbplus_para->outputSize*2);
    amrwbplus_para->bitrate = get_bitrate(&(amrwbplus_para->conf));

	return AMRWBPLUS_DEC_DECODE_ONE_FRAME;
}

AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_EndDecode(AMRWBGlobalData *global)
{
    AMRWBPLUS_DEC_PARAM_T *amrwbplus_para;

	amrwbplus_para = &(global->params);
	if (NULL == amrwbplus_para)
    {
        return AMRWBPLUS_DEC_INPUT_POINTER_INVALID;
    }
    
    if(global->stAmrwbDec)
		D_IF_exit_fx(global->stAmrwbDec);
    if(global->st_d)
		voAMRWBPDecAlignedFree((void*)global->st_d);
#if  AMR_DUMP
   fclose(fLog);
   fLog =NULL;
#endif// AMR_DUMP
    return AMRWBPLUS_DEC_OPERATION_DONE;
}


//??? wb+
extern Word16 get_nb_bits(Word16 extension, Word16 mode, Word16 st_mode);
short AMRWBDAPI AMRWBPLUS_GetFrameLenth(AMRWBPLUS_DEC_PARAM_T *amrwbplus_para, unsigned char*serial)
//amrwbplus_para->inputStream has been evaluated before entering this routine.
{
    Word16 mode;
    Word16 num;
    
    //for wb+ only
    Word16 mode_index;
    Word16 extension;
    Word16 st_mode;
    Word16 index;
    
    
	if (WB_MIME == amrwbplus_para->input_data_type)
	{
		mode = ((*amrwbplus_para->inputStream) >> 3) & 0x0f;
        if ((0 > mode) || (15 < mode))
        {
            return -1;
        }
		else
		{
			return voAMRWBPDecBlock_size[mode];
		}
	}
	else if (WBPLUS == amrwbplus_para->input_data_type)
	{
		mode_index = (*amrwbplus_para->inputStream) & 127;
		
        if ((amrwbplus_para->conf.extension == 0) && (mode_index == 15))
        {
            //DTX FRAME (NO DATA)
            serial[0] = 0x7C; //need in AMR WB
            return 2;
        }
        if ((amrwbplus_para->conf.extension == 0) && (mode_index == 14))
        {
            //Frame lost
            serial[0] = 0x74; //need in AMR WB
            return 2;
        }
        
        if( mode_index > 47 || mode_index < 0 ||        //mode unknown
            //mode_index == 14 || mode_index == 15 ||     //Frame lost or ereased
            (mode_index == 9 && (amrwbplus_para->conf.extension) == 1))       //WB SID in WB+ frame  not supported case so declare a NO_DATA ???
        {
            return 2;         //There is no more data to read
        }
        

        st_mode  = -1;        
        
        if(mode_index >15)      //wb+
        {
            if(mode_index < 24)    //Mono mode only
            {
                mode =   mode_index - 16;
            }
            else
            {
                index = mode_index - 24;
                mode = miMode_fx[2*index];
                st_mode = miMode_fx[2*index+1];
            }
            extension = 1;
        }
        else      //WB and caracterize WB+
        {
            if(mode_index == 10)
            {
                extension = 1;
                mode = 2;        //14m
            
            }
            else if (mode_index == 11)
            {
                extension = 1;
                mode = 2;        //18s
                st_mode = 6;
            }
            else if (mode_index == 12)
            {
                extension = 1;
                mode = 7;        //24m
            }
            else if (mode_index == 13)
            {
                extension = 1;
                mode = 5;        //24s
                st_mode = 7;
            }
            
            else
            {
                extension = 0;
                mode =   mode_index;
            }
        }
        
        if (extension >0)
        {
            num = get_nb_bits(extension, mode, st_mode);//bits
            num = (num>>3) + 2;    //hbfTODO:2 bytes header for each 4 super-frame
        }
		else
	    {
	        num = voAMRWBPDecBlock_size[mode] + 1;
	    }
	    
		return num;
	}
	else
    {
        return -1;
    }
}

/**
* Initialize a AMRWBPlus decoder instance use default settings.
* \param phCodec [out] Return the AMRWBPlus Decoder handle.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_OUT_OF_MEMORY Out of memory.
*/
//#define RAW_DUMP
#ifdef RAW_DUMP    //dump AMR-WB+ raw data
FILE *fdump = NULL;
#endif

VO_U32 VO_API voAMRWBPInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	VO_U32 nRet = 0;
#ifdef LCHECK
    VO_PTR pTemp;
#endif
	AMRWBGlobalData* global;
	AMRWBPLUS_DEC_PARAM_T *params;
	//VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
  //int interMem = 0;
	if(pUserData == NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
    return VO_ERR_INVALID_ARG;
	/*	voMemoprator.Alloc = cmnMemAlloc;
		voMemoprator.Copy = cmnMemCopy;
		voMemoprator.Free = cmnMemFree;
		voMemoprator.Set = cmnMemSet;
		voMemoprator.Check = cmnMemCheck;
		interMem = 1;
		pMemOP = &voMemoprator;*/
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	} 

#ifdef LCHECK
	if((pUserData->memflag & 0xF0) == 0x10  && pUserData->libOperator != NULL)
	{
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_AMRWBP, pUserData->memflag|1, g_hAMRWBPDecInst, pUserData->libOperator);
	}
	else
	{
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_AMRWBP, pUserData->memflag|1, g_hAMRWBPDecInst, NULL);
	}
	if (nRet != VO_ERR_NONE)
	{
		if (pTemp)
		{
			voCheckLibUninit(pTemp);
			pTemp = NULL;
		}
		return nRet;
	}
#endif
	global= voAMRWBPDecAlignedMalloc(sizeof(AMRWBGlobalData));
	if(global==NULL)
		return VO_ERR_OUTOF_MEMORY;
	params = &(global->params);
	params->input_data_type = WBPLUS;
	params->conf.fs = 0;
	params->conf.st_mode = -1;
	params->sampleRate = 0;
	params->first = 1;

	/*if(interMem)
	{
		global->voMemoprator.Alloc = cmnMemAlloc;
		global->voMemoprator.Copy = cmnMemCopy;
		global->voMemoprator.Free = cmnMemFree;
		global->voMemoprator.Set = cmnMemSet;
		global->voMemoprator.Check = cmnMemCheck;
		pMemOP = &global->voMemoprator;
	}*/

	global->pvoMemop = pMemOP;

#ifdef LCHECK
    global->hCheck = pTemp;
#endif

	*phCodec = (VO_HANDLE)global;

#ifdef RAW_DUMP
	fdump = fopen("swap_raw.dat", "wb+");
#endif
	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWBPSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	AMRWBGlobalData* global;
	AMRWBPLUS_DEC_PARAM_T *params;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	global = (AMRWBGlobalData *)hCodec;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	params = &(global->params);
	params->inputStream = pInput->Buffer;
	params->inputSize	= pInput->Length;
	global->uselength = 0;
	global->inbuf = pInput->Buffer;
	global->inlen = pInput->Length;

#ifdef RAW_DUMP
	fwrite(pInput->Buffer, 1, pInput->Length, fdump);
	fflush(fdump);
#endif

	return VO_ERR_NONE;
}


VO_U32 VO_API voAMRWBPGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	AMRWBPLUS_DEC_PARAM_T *params;
	AMRWBPLUS_DEC_RESULT_T result;
	AMRWBGlobalData* global;
	int frame_len;
	if(hCodec==NULL||pOutput==NULL)
		return VO_ERR_INVALID_ARG;
	global = (AMRWBGlobalData*)hCodec;
	params = &(global->params);

	if(params->inputSize <= 0)
		return VO_ERR_INPUT_BUFFER_SMALL;

	if(params->first)//first time,init it
	{
		Word16 tfi;
		
		ReadHeader_buffer(
			&tfi, 
			params->conf.bfi,
			&params->conf.extension,
			&params->conf.mode,
			&params->conf.st_mode,
			&params->conf.fscale,
			params->inputStream);

		result = AMRWBPLUS_StartDecode(global);
		if (AMRWBPLUS_DEC_OPERATION_DONE != result)
			return VO_ERR_FAILED;

		params->first = 0;
	}

	frame_len = AMRWBPLUS_GetFrameLenth(params, (unsigned char*)global->serial);

	if(frame_len>params->inputSize||frame_len<=0)
		return VO_ERR_FAILED;

	result = AMRWBPLUS_ReceiveData(global);

	if (AMRWBPLUS_DEC_DECODE_ONE_FRAME != result)
		return VO_ERR_FAILED;

	if(pOutput->Length < params->outputSize*2)
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	global->uselength  += frame_len;

	params->inputStream += frame_len;
	params->inputSize	-= frame_len;
	global->inbuf += frame_len;
	global->inlen -= frame_len;
    pAudioFormat->InputUsed = frame_len;

	pOutput->Length = params->outputSize*2;//params->outputSize is size in short
	memcpy(pOutput->Buffer, params->outputStream, pOutput->Length);

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = params->channels;
		pAudioFormat->Format.SampleRate = params->conf.fs;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->InputUsed = global->uselength;
	}

#ifdef LCHECK
	voCheckLibCheckAudio(global->hCheck, pOutput, &(pAudioFormat->Format));
#endif
	return VO_ERR_NONE;
}


VO_U32 VO_API voAMRWBPUninit(VO_HANDLE hCodec)
{
	AMRWBPLUS_DEC_RESULT_T result;
	AMRWBGlobalData* global;
	if(hCodec!=NULL)
	{
		global = (AMRWBGlobalData*)hCodec;
		result = AMRWBPLUS_EndDecode(global);

#ifdef LCHECK
		voCheckLibUninit(global->hCheck);
#endif
		voAMRWBPDecAlignedFree((void*)hCodec);
		hCodec = NULL;
	}

#ifdef RAW_DUMP
	fclose(fdump);
#endif
	return VO_ERR_NONE;
}



VO_U32 VO_API voAMRWBPSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	Word16 tfi;
	AMRWBGlobalData* gData = (AMRWBGlobalData*)hCodec;
	AMRWBPLUS_DEC_PARAM_T *inParam;
	VO_CODECBUFFER *pInPtr; 
	unsigned char *in_buffer;
	if (gData==NULL) 
		return VO_ERR_INVALID_ARG;
	inParam = &(gData->params);
	switch(uParamID)
	{
	
	case VO_PID_AMRWBP_MONO_OUT:
		inParam->conf.mono_dec_stereo = *((int *)pData)?1:0;
		break;
	case VO_PID_AMRWBP_HEADER_METE:
		pInPtr = (VO_CODECBUFFER *)pData;
		in_buffer = (unsigned char *)pInPtr->Buffer;

		inParam->inputStream = (unsigned char *)pInPtr->Buffer;
		ReadHeader_buffer(
			&tfi, 
			inParam->conf.bfi,
			&inParam->conf.extension,
			&inParam->conf.mode,
			&inParam->conf.st_mode,
			&inParam->conf.fscale,
			in_buffer);
        break;
	case VO_PID_AUDIO_SAMPLEREATE:
		inParam->conf.fs = *((int *)pData);
		inParam->sampleRate = *((int *)pData);
		break;
	case VO_PID_COMMON_HEADDATA:
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}


VO_U32 VO_API voAMRWBPGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	AMRWBGlobalData* gData = (AMRWBGlobalData*)hCodec;
	AMRWBPLUS_DEC_PARAM_T *inParam;
	if (gData==NULL) 
		return VO_ERR_INVALID_ARG;
	inParam = &(gData->params);
	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;

			fmt->Channels = inParam->channels;
			fmt->SampleRate = inParam->conf.fs;
			fmt->SampleBits = 16;
			break;
		}
	case VO_PID_AUDIO_CHANNELS:
		*((int *)pData) = inParam->channels;
		break;
	case VO_PID_AUDIO_SAMPLEREATE:
		*((int *)pData) = inParam->conf.fs;
		break;
	case VO_PID_AMRWBP_MONO_OUT:
		*((int *)pData) = inParam->conf.mono_dec_stereo;
		break;
	case VO_PID_AMRWBP_FRAME_LENS:
		*((int *)pData) = AMRWBPLUS_GetFrameLenth(inParam, (unsigned char*)gData->serial);
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}


VO_S32 VO_API voGetAMRWBPDecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voAMRWBPInit;
	pDecHandle->SetInputData = voAMRWBPSetInputData;
	pDecHandle->GetOutputData = voAMRWBPGetOutputData;
	pDecHandle->SetParam = voAMRWBPSetParam;
	pDecHandle->GetParam = voAMRWBPGetParam;
	pDecHandle->Uninit = voAMRWBPUninit;

	return VO_ERR_NONE;
}

#ifdef __cplusplus
}
#endif
