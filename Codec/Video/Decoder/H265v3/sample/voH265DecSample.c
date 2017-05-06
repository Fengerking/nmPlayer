#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <time.h>
#include        <limits.h>
#include        "voH265.h"

#define LOOP_COUNT 1
//#include        "viMem.h" /* Shared frame buffer operator*/
#define DECOUTPUT_TEST      1
#define INT_MAX1 2147483647
#define ERROR_TEST	0
#define ERROR_SIZE	3  //error rate, X/1000
#define CPU_NUM 4

#define USE_NEW_API 1

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
long clock_count()
{
  
#ifdef ANDROID
    return clock()/1000;
#else
    return clock();
#endif

    
}

#define MAX_DECODE_TIME 256
int cur_decode_time = 0;
int frame_width = 0, frame_height = 0;
int hevc_file_len = 0;
VO_S32 frame_count=0;

VO_S32 OutputOneFrame(VO_VIDEO_BUFFER *par, VO_VIDEO_FORMAT* outFormat, FILE* out_yuv_file)
{
    VO_S32 i, width,height;
    VO_U8* out_src;
#if DECOUTPUT_TEST
    printf("Output frame %d type %d, timestamp %lld\n", frame_count, outFormat->Type, par->Time);
    frame_count++;

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
	fflush(out_yuv_file);
#endif
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
#define H265DLLPATH "/data/local/tmp/libvoH265Dec.so"

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

#if ERROR_TEST
static void  MakeDataDirty(VO_CODECBUFFER *inBuffer,int nErrorSize, FILE *fDump)
{
	int i ;
	int nErrorPos = 0;
	nErrorSize = inBuffer->Length * nErrorSize / 1000;
#ifdef _WIN32
	srand(clock());
#else
	srand(time(NULL));
#endif
	for (i = 0; i < nErrorSize; i++)
	{
		VO_U8 vu;

		nErrorPos = rand () % inBuffer->Length;
		vu = inBuffer->Buffer[nErrorPos];
		inBuffer->Buffer[nErrorPos] = rand () % 256;
		if(nErrorPos<0)
			printf(" vu %d; 222 =%d; %d;--;inBuffer->Length=%d\r\n", vu , inBuffer->Buffer[nErrorPos], nErrorPos,inBuffer->Length );

	}

	if (fDump)
	{
		fwrite(inBuffer->Buffer, inBuffer->Length, 1, fDump);
		fflush(fDump);
	}
}
#endif//ERROR_TEST



VO_S32 runOneItem(VO_S32 argc, char **argv) ;

VO_S32 main(VO_S32 argc, char **argv) {
#ifdef ANDROID
  FILE *performance_config_file = fopen("/data/local/tmp/h265_performance_cfg.cfg", "rb");
#else
  FILE *performance_config_file = fopen("h265_performance_cfg.cfg", "rb");
#endif  
  if(performance_config_file==NULL){
		VO_S32 i;
		for(i = 0;i<LOOP_COUNT;i++)
            runOneItem(argc,argv);
  }else{
    long decode_time_array[MAX_DECODE_TIME]={0};
    int all_decode_time = 0;
    int cur_decode_index = 0;
    int i = 0;
    VO_BOOL skip_minMax;
    int min = 0, min_pos=-1;
    int max = 0x7fffffff, max_pos=-1;
    long all_time=0;
    long avg_decode_time = 0;
    double frame_per_sec = 0;
    FILE *p_performance_file = NULL;
    fscanf(performance_config_file, "Test Time(MilliSec)          : %d\n", &all_decode_time );
    fscanf(performance_config_file, "Skip Min Max                 : %d\n", &skip_minMax );
    for( i=0; i<all_decode_time; i++){
            //runOneItem(argc, argv);
            //while(1){
            runOneItem(argc,argv);
            //}
      decode_time_array[cur_decode_index] = cur_decode_time;
      cur_decode_index++;
    }

    for( i=0; i<all_decode_time; i++){ 
      printf("index = %d\t decode time[Millisec] = %d\n",i,decode_time_array[i]);     
    }
    

    if(skip_minMax){
      min_pos = max_pos = 0;
      max = min = decode_time_array[0];
      for(i=1; i<all_decode_time; i++){
        if(decode_time_array[i]<min){
          min = decode_time_array[i];
          min_pos = i;
        }
        if(decode_time_array[i]>max){
          max = decode_time_array[i];
          max_pos = i;
        }
      }
      decode_time_array[min_pos] = 0;
      decode_time_array[max_pos] = 0;
      all_decode_time-=2;
    }    

    for( i=0; i<all_decode_time+2; i++){  //+2, remove min and max; if not skip, just +0, result is same
      all_time+=decode_time_array[i];      
    }

    avg_decode_time = all_time/all_decode_time;
    frame_per_sec = 1000*(frame_count-1)/(double)avg_decode_time;   


#ifdef ANDROID
    p_performance_file = fopen("/data/local/tmp/h265dec_log.txt", "ab+");  
#else
    p_performance_file = fopen("h265dec_log.txt", "ab+"); 
#endif   
    fprintf(p_performance_file, "frame_per_sec                : %f\n", frame_per_sec );
    fprintf(p_performance_file, "frame_count                  : %d\n", frame_count-1 );
    fprintf(p_performance_file, "HEVC file size               : %d\n", hevc_file_len );    
    fprintf(p_performance_file, "Internal Format              : %dx%d\n", frame_width, frame_height );
    fclose(p_performance_file);

    printf("frame_per_sec: %f\t frame_cnt=%d\tresolution=%dx%d\n", frame_per_sec, frame_count-1, frame_width,frame_height );
  }
}

#define USE_THUMBNAIL_MODE 0
//#define  CHECK_MEMORY_LEAK
int nCountMem = 0;
int nCountFree = 0;
#ifdef CHECK_MEMORY_LEAK
#define MAXCALLER 800
unsigned int arrMemInfoStartAddr[MAXCALLER]={0};
int arrMemInfoSize[MAXCALLER]={0};
unsigned int errStartAddr[MAXCALLER] = {0};
#endif
VO_U32 (VO_API outside_Alloc) (VO_S32 uID, VO_MEM_INFO * pMemInfo)
{
    pMemInfo->VBuffer = (VO_PTR)malloc(pMemInfo->Size*sizeof(VO_PTR));
    if (pMemInfo->VBuffer == NULL) {
        return VO_ERR_OUTOF_MEMORY;
    }
#ifdef CHECK_MEMORY_LEAK
    if (nCountMem < MAXCALLER) {
        arrMemInfoStartAddr[nCountMem] = (unsigned int)pMemInfo->VBuffer;
        arrMemInfoSize[nCountMem] = pMemInfo->Size;
        nCountMem++;
    } else {
        printf("not enough buffer for save address, please enlarge the buffer [arrMemInfoStartAddr] and [arrMemInfoSize].\n");
    }
#else
    nCountMem++;
#endif
    return VO_ERR_NONE;
}


VO_U32 (VO_API outside_Free) (VO_S32 uID, VO_PTR pBuff)
{
    if (pBuff != NULL) {
#ifdef CHECK_MEMORY_LEAK
        int i = 0;
        unsigned int nTargetAddr = (unsigned int) pBuff;
        if(nCountFree < MAXCALLER){
            for(i = 0; i < nCountMem; i++){
                if (nTargetAddr == arrMemInfoStartAddr[i]) {
                    arrMemInfoStartAddr[i] = 0;
                    arrMemInfoSize[i] = 0;
                    break;
                }
            }
            if (i == nCountMem) {
                errStartAddr[nCountFree] = nTargetAddr;
                printf("caller [%d] : free error pointer addr--[0x %o].\n",nCountFree, nTargetAddr);
            }
            nCountFree++;
        } else {
            printf("not enough buffer for save address, please enlarge the buffer [errStartAddr].\n");
        }
#else
        nCountFree++;
#endif
        free(pBuff);
        pBuff = NULL;
    }

    return VO_ERR_NONE;
}
VO_U32 (VO_API outside_Set) (VO_S32 uID, VO_PTR pBuff, VO_U8 uValue, VO_U32 uSize)
{
    memset(pBuff,uValue,uSize);
    return VO_ERR_NONE;
}
VO_U32 (VO_API outside_Copy) (VO_S32 uID, VO_PTR pDest, VO_CPTR pSource, VO_U32 uSize)
{
    memcpy(pDest,pSource,uSize);
    return VO_ERR_NONE;
}
VO_U32 (VO_API outside_Check) (VO_S32 uID, VO_PTR pBuffer, VO_U32 uSize);
VO_S32 (VO_API outside_Compare) (VO_S32 uID, VO_CPTR pBuffer1, VO_CPTR pBuffer2, VO_U32 uSize);
VO_U32 (VO_API outside_Move) (VO_S32 uID, VO_PTR pDest, VO_CPTR pSource, VO_U32 uSize)
{
    memmove(pDest,pSource,uSize);
    return VO_ERR_NONE;
}
VO_S32 runOneItem(VO_S32 argc, char **argv) 
{
    FILE *raw_data_file = NULL;
    FILE *out_yuv_file  = NULL;
    FILE *log_file      = NULL;
    VO_U8 raw_data_file_name[256]="";
    VO_U8 out_yuv_file_name[256]="";
    VO_U8 log_file_name[256]="";


    VO_S32 left_buf_len,  file_len;// used_buf_len,, nalu_len;
    VO_U8 *p_input_buf, *p_cur_buf_pos;
    VO_S32 out_frame_num = INPUT_FRAME_NUM;  
	  VO_U32 flush = 0;

    VO_HANDLE              h265dec;
    VO_U32                 reture_code;
    VO_CODECBUFFER         in_data;
    VO_VIDEO_BUFFER        out_data;
    VO_VIDEO_OUTPUTINFO    out_data_info;
    VO_VIDEO_DECAPI        h265dec_api;

    VO_VIDEO_CODINGTYPE codec_id = VO_VIDEO_CodingH265;//VO_VIDEO_CodingH265;//VO_VIDEO_CodingH263;//VO_VIDEO_CodingMPEG4;//VO_VIDEO_CodingS263;//H263_DEC;//;
    VO_U32 cpu_num = CPU_NUM;


#ifdef _WIN32_WCE
    VO_U8 *mb_skip;
    DWORD start, finish;
#if ERROR_TEST
	  FILE *fDump = NULL;
	  fDump = fopen("DirtyData.dta", "wb");
#endif
    out_yuv_file_name[0] = log_file_name[0] = '\0';
    sprintf(raw_data_file_name, INPUT_FILE_NAME);
    sprintf(out_yuv_file_name, OUT_FILE_NAME);
    sprintf(log_file_name, OUT_SPEED_FILE_NAME);
    out_frame_num = INPUT_FRAME_NUM;
    start = GetTickCount();
#else
    VO_S32 i;
    VO_CODEC_INIT_USERDATA user_op;
    VO_CODEC_INIT_USERDATA * p_user_op;
    VO_MEM_OPERATOR mem_op;

    long start, finish;
#if ERROR_TEST
	FILE *fDump = NULL;
	fDump = fopen("DirtyData.dta", "wb");
#endif
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
            else if (0 == strncmp (argv[i], "-c", 2)){
              cpu_num = atoi(argv[i+1]);
              printf("\n INPUT: CPU number: %d", cpu_num);
            }
            else {
                printf("Usage: %s [-i inputFileName] {[-f totalFrames] [-o outVideoFileName] [-s outSpeedFileName]} \n",argv[0]);
                exit(0);
            }
            i++;
        }    
    }
    else { 
#if ERROR_TEST
        strcpy(raw_data_file_name,"D:\\fdumptest.bin");// "sample_832x480_30_I.h265");
#else
		strcpy(raw_data_file_name,"D:\\decstream\\wrongclip\\H265\\imain_RaceHorses_416x240_30_qp32.265");// "sample_832x480_30_I.h265");
		strcpy(raw_data_file_name,"D:\\dump_hevc_dash_1.h265");// "sample_832x480_30_I.h265");
		//strcpy(raw_data_file_name,"D:\\test suite\\h265_testsuite\\autoTest\\input\\832x480_ld_main_tile_1.bin");
#endif
		strcpy(out_yuv_file_name, "E:\\decstream\\new.yuv");
        strcpy(log_file_name,"h265decLog.txt");
        //printf("Usage: %s [-i inputFileName] {[-f totalFrames] [-o outVideoFileName] [-s outSpeedFileName]} \n",argv[0]);
        //exit(0);
    }
#ifdef CHECK_MEMORY_LEAK
    {
        for (i = 0 ; i < nCountMem; i++)
        {
            arrMemInfoStartAddr[i] = 0;
            arrMemInfoSize[i] = 0;
        }
        nCountMem = 0;
        for (i = 0; i < nCountFree; i++)
        {
            errStartAddr[i] = 0;
        }
        nCountFree = 0;
    }
#endif
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
    hevc_file_len = file_len;
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
#ifdef CHECK_MEMORY_LEAK
    user_op.memflag = VO_IMF_USERMEMOPERATOR;
    user_op.memData = (VO_PTR)&mem_op;
    mem_op.Alloc = outside_Alloc;
    mem_op.Free = outside_Free;
    mem_op.Copy = outside_Copy;
    mem_op.Move = outside_Move;
    mem_op.Set = outside_Set;
    p_user_op = &user_op;
    reture_code = h265dec_api.Init(&h265dec, codec_id, p_user_op);
#else
    reture_code = h265dec_api.Init(&h265dec, codec_id, NULL);
#endif
    if(reture_code != VO_ERR_NONE){
        goto End;
    }
    printf("---pass--Init \n");
    /* Set parameters of decoder */
    reture_code = h265dec_api.SetParam(h265dec, VO_PID_COMMON_CPUNUM, (VO_PTR)&cpu_num);
    if(reture_code != VO_ERR_NONE)
        goto End;
#if USE_THUMBNAIL_MODE
	{
		VO_S32 flag = 1; //flag == 1, when thumbnail == 1
		reture_code = h265dec_api.SetParam(h265dec, VO_PID_VIDEO_THUMBNAIL_MODE, (VO_PTR)&flag);
		if(reture_code != VO_ERR_NONE)
			goto End;
	}
#endif
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
#if !USE_NEW_API
        if (left_buf_len == IN_BUFF_LEN ) {/*left buffer is full*/
            printf("\nError: Please make sure, at least one nalu in buffer!");
            goto End;
        }
#endif
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

// 		{
// 			//test set head data
// 			in_data.Buffer = p_cur_buf_pos;
// 			in_data.Length = 90;
// 			h265dec_api.SetParam(h265dec, VO_PID_COMMON_HEADDATA, &in_data);
// // 			p_cur_buf_pos += 90;
// // 			left_buf_len -= 90;
// 		}

        /* Fill codec buffer */
        in_data.Buffer = p_cur_buf_pos;
        in_data.Length = left_buf_len;
		in_data.Time = INPUT_FRAME_NUM - out_frame_num;
        /* Get the frame type of input frame*/
        //reture_code = h265dec_api.GetParam(h265dec, VO_PID_VIDEO_FRAMETYPE, (VO_PTR*)&in_data);
        //printf("-----frame :[%d]----0.\n",frame_count);
        /* Set raw video data*/
#if ERROR_TEST
	  MakeDataDirty(&in_data, ERROR_SIZE, fDump);
#endif
#if USE_NEW_API
		reture_code = h265dec_api.SetInputData(h265dec, &in_data);

		if (reture_code == VO_ERR_NONE || reture_code == VO_ERR_INPUT_BUFFER_SMALL || reture_code == VO_ERR_RETRY)
		{
			/* Update file_length and used buffer */
			p_cur_buf_pos += in_data.Length;
			left_buf_len  -= in_data.Length;
			file_len      -= in_data.Length;

			if (reture_code == VO_ERR_INPUT_BUFFER_SMALL)
			{
				continue;
			}
		}
		else
		{
			//handle error

			goto End;
		}
#else
        h265dec_api.SetInputData(h265dec, &in_data);
#endif
        //printf("-----frame :[%d]----1.\n",frame_count);
		/* Get one frame decoded data, out_data_info.InputUsed returens the used length of input buffer*/
// #if !USE_NEW_API
		do {
			do {
// #endif
				reture_code = h265dec_api.GetOutputData(h265dec, &out_data, &out_data_info);
	
				if(reture_code == VO_ERR_NONE){/*No error in current frame*/
					if(out_data_info.Format.Type != VO_VIDEO_FRAME_NULL){

// 						if (out_frame_num == INPUT_FRAME_NUM - 200) //seek flush test
// 						{
// 							h265dec_api.SetParam(h265dec, VO_PID_COMMON_FLUSH, NULL);
// 							in_data.Buffer = p_cur_buf_pos;
// 							in_data.Length = left_buf_len;
// 							reture_code = h265dec_api.SetInputData(h265dec, &in_data);
// 							if (reture_code == VO_ERR_NONE || reture_code == VO_ERR_INPUT_BUFFER_SMALL || reture_code == VO_ERR_RETRY)
// 							{
// 								/* Update file_length and used buffer */
// 								p_cur_buf_pos += in_data.Length;
// 								left_buf_len  -= in_data.Length;
// 								file_len      -= in_data.Length;
// 							}
// 						}
						OutputOneFrame(&out_data, &out_data_info.Format, out_yuv_file);	
#if USE_NEW_API
						h265dec_api.SetParam(h265dec, VO_PID_COMMON_FRAME_BUF_BACK, &out_data);
#endif
						out_frame_num--;
					}
				}
// #if !USE_NEW_API
			}while(out_data_info.Flag);
		}while( reture_code != VO_ERR_INPUT_BUFFER_SMALL);
// #endif

#if !USE_NEW_API
        /* Update file_length and used buffer */
		p_cur_buf_pos += out_data_info.InputUsed;
	    left_buf_len  -= out_data_info.InputUsed;
		file_len      -= out_data_info.InputUsed;
#endif
		
    }

    /* Flush decoded frame buffer */
	flush = 1;
	reture_code = h265dec_api.SetParam(h265dec, VO_PID_DEC_H265_FLUSH_PICS, (VO_PTR*)&flush);

	/* Get one frame decoded data, out_data_info.InputUsed returens the used length of input buffer*/
	do {
		reture_code = h265dec_api.GetOutputData(h265dec, &out_data, &out_data_info);
	
		if(reture_code == VO_ERR_NONE){/*No error in current frame*/
			if(out_data_info.Format.Type != VO_VIDEO_FRAME_NULL){
				OutputOneFrame(&out_data, &out_data_info.Format, out_yuv_file);				
#if USE_NEW_API
				h265dec_api.SetParam(h265dec, VO_PID_COMMON_FRAME_BUF_BACK, &out_data);
#endif
			}
		}

	}while(out_data_info.Flag);

 
  frame_width = out_data_info.Format.Width;
  frame_height = out_data_info.Format.Height;
  finish = clock_count();

  cur_decode_time = finish - start; 

  /* Free decoder handle*/
  h265dec_api.Uninit(h265dec);

  
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
#ifdef CHECK_MEMORY_LEAK    
	printf("******************************begin printf memory test***************************** \n");

	printf("width * height = [%d x %d].\n",frame_width,frame_height);
	printf("nCountMem malloc = [%d], free =[%d].\n",nCountMem,nCountFree);
    
    {
        int i = 0;
#ifdef ANDROID
		FILE * pFlog = fopen("/data/local/tmp/voH265DecSample.log","w");
#else
        FILE * pFlog = fopen("voH265DecSample.log","w");
#endif
        if (pFlog == NULL) {
            printf("can't open voH265DecSample.log.\n");
            return 0;
        }
         for (i = 0; i<nCountMem; i++) {
            if (arrMemInfoStartAddr[i] != 0) {
                printf("malloc [%d], not free memory addr:[0x %o], size = [%d].\n",i,arrMemInfoStartAddr[i],arrMemInfoSize[i]);
                fprintf(pFlog,"malloc [%d], not free memory addr:[0x %o], size = [%d].\n",i,arrMemInfoStartAddr[i],arrMemInfoSize[i]);
            }            
        }
        for (i = 0; i < nCountFree; i++) {
            if (errStartAddr[i] != 0) {
                printf("free call [%d], error free memory addr:[0x %o].\n",i, errStartAddr[i]);
                fprintf(pFlog,"free call [%d], error free memory addr:[0x %o].\n",i, errStartAddr[i]);
            }
        }
        fclose(pFlog);
    }
#endif
#if ERROR_TEST
	if (fDump)
	{
		fclose(fDump);
	}
#endif
    printf("Decoder finish! \n");

    return 0;
}

