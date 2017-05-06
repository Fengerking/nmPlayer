#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <time.h>
#include        <limits.h>
#include        "voH265.h"


//#include        "viMem.h" /* Shared frame buffer operator*/


#ifdef _WIN32_WCE
#define INPUT_FILE_NAME        "\\SX313_Video_R2.dat"/*  PHDec.mp4v\\SD Card\\*/
#define OUT_FILE_NAME          "\\out.yuv"
#define OUT_SPEED_FILE_NAME    "\\Speed_f2_new.txt"
#endif

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
double clock_count()
{
    return 0;
}

VO_S32 frame_count;
VO_S32 OutputOneFrame(VO_VIDEO_BUFFER *par, VO_VIDEO_FORMAT* outFormat, FILE* out_yuv_file)
{
    VO_S32 i, width,height;
    VO_U8* out_src;

    printf("Output frame %d type %d\n", frame_count, outFormat->Type);

    if (!out_yuv_file)
        return 0;

    width = outFormat->Width;
    height= outFormat->Height;

    /* output decoded YUV420 frame */
    /* Y */
    out_src = par->Buffer[0];
    for(i = 0;i < height; i++){
        fwrite(out_src, width, 1, out_yuv_file);
        out_src += par->Stride[0];
    }

    /* U */
    out_src = par->Buffer[1];
    for(i = 0; i< height/2; i++){
        fwrite(out_src, width/2, 1, out_yuv_file);
        out_src += par->Stride[1];
    }

    /* V */
    out_src = par->Buffer[2];
    for(i = 0; i < height/2; i++){
        fwrite(out_src, width/2, 1, out_yuv_file);
        out_src += par->Stride[2];
    }

    return 0;
}

#define NEXT32BITS(x) ((x[0]<<3)|(x[1]<<2)|(x[2]<<1)|(x[3]))
#define NEXT24BITS(x) ((x[0]<<2)|(x[1]<<1)|(x[2]))

/* ANNEXB start code is 0x000001 3 bytes */    
#define LOST_DATA    0
VO_S32 get_nalu(VO_U8 *p_input, VO_S32 input_len, VO_S32 *nal_tyle)
{
    /*find 0x000001 */
    VO_U8 *head = p_input, *end = p_input + input_len;

	if (input_len <= 0)
		return -1;

	while ((NEXT24BITS(head) != 0x000001)) {
		head++;
	}

	if (end == head)
		return -1;//no sc

	head += 3;

	*nal_tyle = ((head[0] >> 1) & 0x3f);

	while ((end > head) && (NEXT24BITS(head) !=  0x000001)) {
		head++;
	}
	
	return (VO_S32)(head - p_input);

}

/*
VO_S32 get_header(VO_U8 *p_input, VO_S32 input_len, VO_U8 ** nalu)
{
	VO_U8* p_nalu;
	VO_S32 head_len = 0;

	do{
		VO_S32 nalu_len;

		nalu_len = get_nalu(p_input, input_len, &p_nalu);

		if (nalu_len < 0) {
			return -1;
		}

		if (((p_input[0] >> 1) & 0x3f) < 31) {
			break;
		}
		head_len += nalu_len;

	}while(head_len < input_len);
	
	nalu = p_input;

    return head_len;
}
*/

typedef VO_S32  (VO_API *voGetH265DecAPIFunc) (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);
voGetH265DecAPIFunc pvoGetH265DecAPI=NULL;

#ifdef LINUX//
#define USE_DLL 1
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif//#ifdef LINUX

#if USE_DLL
void* h265Moudle = NULL;
#define H265DLLPATH "/data/local/hevc/libvoH265Dec.so"

int LoadH265DLL()
{
	h265Moudle = dlopen (H265DLLPATH, RTLD_NOW);
	if(h265Moudle==NULL)
	{
		printf("load %s fail,%s\n",H265DLLPATH,strerror(errno));
		return -1;
	}
	return 0;
}
int GetGetH265DecAPI()
{

	pvoGetH265DecAPI=dlsym(h265Moudle, "voGetH265DecAPI");
	if(pvoGetH265DecAPI==NULL)
	{
		printf("Get voGetH265DecAPI fail,%s\n",H265DLLPATH,strerror(errno));
		return -1;
	}
	return 0;
}
void FreeH265DLL()
{
	printf("before free dll\n");
	dlclose (h265Moudle);
	printf("after free dll\n");
}
#else//USE_DLL
int LoadH265DLL()
{
	return 0;
}
int GetGetH265DecAPI()
{
	pvoGetH265DecAPI=voGetH265DecAPI;
	return 0;
}
void FreeH265DLL()
{

}
#endif//USE_DLL

#if !(defined(RVDS) || defined(_WIN32_WCE))
static unsigned long GetTime(){
#ifdef WIN32
       return 0;//voOS_GetSysTime();
#else
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
#endif	
}
#endif




#define INPUT_FRAME_NUM        100000    /* default input frames*/
#define IN_BUFF_LEN 1024*1024*10 
#define VO_VIDEO_CodingH265 265

VO_S32 main(VO_S32 argc, char **argv) 
{

    FILE *raw_data_file = NULL;
    FILE *out_yuv_file  = NULL;
    FILE *log_file      = NULL;
    VO_U8 raw_data_file_name[256];
    VO_U8 out_yuv_file_name[256];
    VO_U8 log_file_name[256];


    VO_S32 left_buf_len,  file_len;// used_buf_len,, nalu_len;
    VO_U8 *p_input_buf, *p_cur_buf_pos;
    VO_S32 out_frame_num = INPUT_FRAME_NUM;
    VO_S32 frame_count;
	VO_U32 flush = 0;

    VO_HANDLE              h265dec;
    VO_U32                 reture_code;
    VO_CODECBUFFER         in_data;
    VO_VIDEO_BUFFER        out_data;
    VO_VIDEO_OUTPUTINFO    out_data_info;
    VO_VIDEO_DECAPI        h265dec_api;

    VO_VIDEO_CODINGTYPE codec_id = VO_VIDEO_CodingH265;//VO_VIDEO_CodingH265;//VO_VIDEO_CodingH263;//VO_VIDEO_CodingMPEG4;//VO_VIDEO_CodingS263;//H263_DEC;//;
    VO_U32 cpu_num;


#ifdef _WIN32_WCE
    VO_U8 *mb_skip;
    DWORD start, finish;
    out_yuv_file_name[0] = log_file_name[0] = '\0';
    sprintf(raw_data_file_name, INPUT_FILE_NAME);
    sprintf(out_yuv_file_name, OUT_FILE_NAME);
    sprintf(log_file_name, OUT_SPEED_FILE_NAME);
    out_frame_num = INPUT_FRAME_NUM;
    start = GetTickCount();
#else
    VO_S32 i;

    double start, finish;
    //out_yuv_file_name[0] = log_file_name[0] = outFileRgb[0] = '\0';

    if(LoadH265DLL())
	  {
		  return 0;
	  }
	  if(GetGetH265DecAPI())
	  {
		  return 0;
	  }

    if (argc > 1){
        for (i = 1; i < argc; i++){
            if (0 == strncmp (argv[i], "-i", 2)){
                strcpy(raw_data_file_name, argv[i+1]);
                printf("\n INPUT: input file: %s", raw_data_file_name);
            }
            else if (0 == strncmp (argv[i], "-o", 2)){
                strcpy(out_yuv_file_name, argv[i+1]);
                printf("\n INPUT: output video YUV420 file: %s", out_yuv_file_name);
            }
            else if (0 == strncmp (argv[i], "-s", 2)){
                strcpy(log_file_name, argv[i+1]);
                printf("\n INPUT: test speed file: %s", log_file_name);
            }
            else if (0 == strncmp (argv[i], "-f", 2)){
                out_frame_num = atoi(argv[i+1]);
                printf("\n INPUT: user specified total decode frames: %d", out_frame_num);
            }
            else {
                printf("Usage: %s [-i inputFileName] {[-f totalFrames] [-o outVideoFileName] [-s outSpeedFileName]} \n",argv[0]);
                exit(0);
            }
            i++;
        }    
    }
    else { 
        strcpy(raw_data_file_name,"sample_832x480_30_20frame.h265");// "sample_832x480_30_I.h265");
        strcpy(out_yuv_file_name, "out_sample_832x480_30_20frame.h265.yuv");
        strcpy(log_file_name,"h265decLog.txt");
        //printf("Usage: %s [-i inputFileName] {[-f totalFrames] [-o outVideoFileName] [-s outSpeedFileName]} \n",argv[0]);
        //exit(0);
    }

    start = clock_count();
#endif

    /* Open raw video file */
    raw_data_file = fopen(raw_data_file_name, "rb");
    if (!raw_data_file){
        printf("\nError: cannot open input h265 raw video file!");
        exit(0);
    }

    /* Open YUV file */
    out_yuv_file = fopen(out_yuv_file_name, "wb");
    if (!out_yuv_file){
        printf("\nWarning: cannot open yuv file!");
    }

    /* Open log file */
    log_file = fopen(log_file_name, "wb");
    if (!log_file){
        printf("\nWarning: cannot open log file!\n");
    }

    /* Get raw video file length */
    fseek(raw_data_file, 0, SEEK_END);
    file_len = ftell(raw_data_file);
    fseek(raw_data_file, 0, SEEK_SET);

    /* Allocate input buffer*/
    p_input_buf = (char *)malloc(IN_BUFF_LEN * sizeof (char));



    /* Get decoder api*/
    reture_code = pvoGetH265DecAPI(&h265dec_api, 0);
    if(reture_code != VO_ERR_NONE){
        goto End;
    }
    printf("---pass--voGetH265DecAPI \n");

    /* Initialize decoder handle*/
    reture_code = h265dec_api.Init(&h265dec, codec_id, NULL);
    if(reture_code != VO_ERR_NONE){
        goto End;
    }
    printf("---pass--Init \n");
    /* Set parameters of decoder */
    reture_code = h265dec_api.SetParam(h265dec, VO_PID_COMMON_CPUNUM, (VO_PTR)&cpu_num);
    if(reture_code != VO_ERR_NONE)
        goto End;
    printf("---pass--SetParam \n");
    ///* Get API for shared frame buffer operator*/
    //GetVideoMemOperatorAPI(&voVideoMemOperator);
    ///* Set shared frame buffer operator*/
    ////reture_code =h265dec_api.SetParam(h265dec, VO_PID_VIDEO_VIDEOMEMOP, &voVideoMemOperator);

    /* Read raw video data*/
    //if(!(left_file_len = fread(p_input_buf, 1, IN_BUFF_LEN, raw_data_file)))
    //    goto End; 

    /* Fill input buffer*/
    //in_data.Length = left_file_len;
    //in_data.Buffer = (VO_PBYTE)p_input_buf;


    /* Set header data */


    /* Main loop for decoding frames*/
    frame_count     = 0;
    left_buf_len    = 0;
	p_cur_buf_pos   = NULL;
	//nalu_len        = -1;/* not ready */


    printf("---begin--while \n");
    while((file_len > 0) && (out_frame_num > 0)){
		//VO_S32 nal_type;

        if (left_buf_len == IN_BUFF_LEN ) {/*left buffer is full*/
            printf("\nError: Please make sure, at least one nalu in buffer!");
            goto End;
        }

        /* Fill more data */

        memmove(p_input_buf, p_cur_buf_pos, left_buf_len);
        left_buf_len += fread(p_input_buf + left_buf_len, 1, (IN_BUFF_LEN - left_buf_len), raw_data_file);
        p_cur_buf_pos = p_input_buf;


        ///* Get nalu*/
		//nalu_len = get_nalu(p_cur_buf_pos, left_buf_len, &nal_type);

        //if (nalu_len < 0) {// didn't find nalu
        //    if (feof(raw_data_file)){
        //        nalu_len = left_buf_len;
        //    } else {
        //        continue;
        //    }
        //}

        /* Fill codec buffer */
        in_data.Buffer = p_cur_buf_pos;
        in_data.Length = left_buf_len;
        
        /* Get the frame type of input frame*/
        //reture_code = h265dec_api.GetParam(h265dec, VO_PID_VIDEO_FRAMETYPE, (VO_PTR*)&in_data);
        //printf("-----frame :[%d]----0.\n",frame_count);
        /* Set raw video data*/
        h265dec_api.SetInputData(h265dec, &in_data);
        //printf("-----frame :[%d]----1.\n",frame_count);
		/* Get one frame decoded data, out_data_info.InputUsed returens the used length of input buffer*/
		do {
			do {
				reture_code = h265dec_api.GetOutputData(h265dec, &out_data, &out_data_info);
	
				if(reture_code == VO_ERR_NONE){/*No error in current frame*/
					if(out_data_info.Format.Type != VO_VIDEO_FRAME_NULL){
						OutputOneFrame(&out_data, &out_data_info.Format, out_yuv_file);
						frame_count++;
					}else{
						frame_count++;
					}
				}else{
					frame_count++;
				}

			out_frame_num--;
			}while(out_data_info.Flag);
		}while( reture_code != VO_ERR_INPUT_BUFFER_SMALL);

        /* Update file_length and used buffer */
		p_cur_buf_pos += out_data_info.InputUsed;
	    left_buf_len  -= out_data_info.InputUsed;
		file_len      -= out_data_info.InputUsed;
		
    }

    /* Flush decoded frame buffer */
	flush = 1;
	reture_code = h265dec_api.SetParam(h265dec, VO_PID_COMMON_FLUSH, (VO_PTR*)&flush);

	/* Get one frame decoded data, out_data_info.InputUsed returens the used length of input buffer*/
	do {
		reture_code = h265dec_api.GetOutputData(h265dec, &out_data, &out_data_info);
	
		if(reture_code == VO_ERR_NONE){/*No error in current frame*/
			if(out_data_info.Format.Type != VO_VIDEO_FRAME_NULL){
				OutputOneFrame(&out_data, &out_data_info.Format, out_yuv_file);
				frame_count++;
			}else{
				frame_count++;
			}
		}else{
			frame_count++;
		}

	}while(out_data_info.Flag);

    /* Free decoder handle*/
    h265dec_api.Uninit(h265dec);

    finish = clock_count();

    if (log_file){
        fclose (log_file);
    }
End:
    free(p_input_buf);
    fclose(raw_data_file);
    if (out_yuv_file) {
        fclose(out_yuv_file);
    }

  

    FreeH265DLL();

    
    printf("Decoder finish! \n");

    return 0;
}

