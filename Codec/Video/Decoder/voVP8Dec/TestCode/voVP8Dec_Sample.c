#ifdef VOWINCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#elif VOWINXP
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#elif RVDS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else VOARDROID
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
static unsigned long timeGetTime(){
  struct timeval tval;
  gettimeofday(&tval, NULL);
  return tval.tv_sec*1000 + tval.tv_usec/1000;
}
#endif // VOWINCE


#include "voVP8.h"

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

//#define ERROR_VP8 1
#ifdef ERROR_VP8
#include "stdio.h"
#define MEM_VALUE_T int
#define MAU_T unsigned char
void mem_put_le16(void *vmem, MEM_VALUE_T val)
{
    MAU_T *mem = (MAU_T *)vmem;

    mem[0] = (val >>  0) & 0xff;
    mem[1] = (val >>  8) & 0xff;
}
 void mem_put_le32(void *vmem, MEM_VALUE_T val)
{
    MAU_T *mem = (MAU_T *)vmem;

    mem[0] = (val >>  0) & 0xff;
    mem[1] = (val >>  8) & 0xff;
    mem[2] = (val >> 16) & 0xff;
    mem[3] = (val >> 24) & 0xff;
}

void write_ivf_file_header(FILE *outfile,
                                  unsigned int fourcc,
                                  int frame_cnt)
{
    char header[32];

    header[0] = 'D';
    header[1] = 'K';
    header[2] = 'I';
    header[3] = 'F';
    mem_put_le16(header + 4,  0);                 /* version */
    mem_put_le16(header + 6,  32);                /* headersize */
    mem_put_le32(header + 8,  fourcc);            /* headersize */
    mem_put_le16(header + 12, 640);          /* width */
    mem_put_le16(header + 14, 480);          /* height */
    mem_put_le32(header + 16, 0); /* rate */
    mem_put_le32(header + 20, 0); /* scale */
    mem_put_le32(header + 24, 0);         /* length */
    mem_put_le32(header + 28, 0);                 /* unused */

    if(fwrite(header, 1, 32, outfile));
}

static void write_ivf_frame_header(FILE *outfile,
                                   long frame_sz)
{
    char             header[12];

    mem_put_le32(header, frame_sz);
    mem_put_le32(header + 4,0);
    mem_put_le32(header + 8,0);

    if(fwrite(header, 1, 12, outfile));
}

void generate_errordtat_ivf(char* infilename,char*errordatafilename)
{
	VO_CODECBUFFER	indataTP;
	int buf_alloc_sz=0,is_ivf,fourcc;
	FILE* errorfile,*infile;

	memset(&indataTP,0,sizeof(VO_CODECBUFFER));	
	infile   = fopen(infilename, "rb");
	errorfile = fopen(errordatafilename, "wb");
	is_ivf = file_is_ivf(infile, &fourcc);  // ivf file format
	write_ivf_file_header(errorfile,0x30385056,0);
	infilename = errordatafilename;

	while (!read_one_frame_data(infile, &(indataTP.Buffer), &(indataTP.Length), &buf_alloc_sz,is_ivf)) // parse data
	{
		unsigned long i,errorpos,errordata;		
		unsigned long readsize = indataTP.Length;
		for(i=0;i<20*(readsize/512);i++)
		{
			errorpos = (unsigned long)rand()%readsize;
			errordata = (int)rand()%256;
			//printf("[%4d %4d] ",errorpos,errordata);
			indataTP.Buffer[errorpos] = errordata;
		}	
		//printf("\n");
		write_ivf_frame_header(errorfile, readsize);
		fwrite(indataTP.Buffer,readsize,1,errorfile);
	}
	buf_alloc_sz =0;
	free(indataTP.Buffer);
	fclose(infile);
	fclose(errorfile);
}
#endif


#define IVF_FRAME_HDR_SZ (sizeof(unsigned long ) + 8) //sizeof(unsigned __int64)
#define RAW_FRAME_HDR_SZ (sizeof(unsigned long ))
static unsigned int mem_get_le16(const void *vmem)
{
    unsigned int  val;
    const unsigned char *mem = (const unsigned char *)vmem;

    val = mem[1] << 8;
    val |= mem[0];
    return val;
}
static unsigned int mem_get_le32(const void *vmem)
{
    unsigned int  val;
    const unsigned char *mem = (const unsigned char *)vmem;

    val = mem[3] << 24;
    val |= mem[2] << 16;
    val |= mem[1] << 8;
    val |= mem[0];
    return val;
}

unsigned int file_is_ivf(FILE *infile, unsigned int *fourcc)
{
    char raw_hdr[32];
    int is_ivf = 0;

    if (fread(raw_hdr, 1, 32, infile) == 32)
    {
        if (raw_hdr[0] == 'D' && raw_hdr[1] == 'K'
            && raw_hdr[2] == 'I' && raw_hdr[3] == 'F')
        {
            is_ivf = 1;

            if (mem_get_le16(raw_hdr + 4) != 0)
				is_ivf =0;
                //fprintf(stderr, "Error: Unrecognized IVF version! This file may not"
                //        " decode properly.");

            *fourcc = mem_get_le32(raw_hdr + 8);
        }
    }

    if (!is_ivf)
		fclose(infile);
    return is_ivf;
}
static int read_one_frame_data(FILE *infile, unsigned char **buf, unsigned long *buf_sz,unsigned long *buf_alloc_sz,int  is_ivf)
{
    char     raw_hdr[IVF_FRAME_HDR_SZ];
    unsigned long new_buf_sz;
    /* For both the raw and ivf formats, the frame size is the first 4 bytes
     * of the frame header. We just need to special case on the header
     * size.
     */
	if(is_ivf)
	{
		if (fread(raw_hdr, is_ivf ? IVF_FRAME_HDR_SZ : RAW_FRAME_HDR_SZ, 1,
				  infile) != 1)
		{
		   // if (!feof(infile))
			 //   fprintf(stderr, "Failed to read frame size\n");
			new_buf_sz = 0;
		}
		else
		{
			new_buf_sz = mem_get_le32(raw_hdr);
			if (new_buf_sz > 256 * 1024 * 1024)
			{
			   // fprintf(stderr, "Error: Read invalid frame size (%u)\n",
				  //      new_buf_sz);
				new_buf_sz = 0;
			}
			if (!is_ivf && new_buf_sz > 256 * 1024)
				return -1;
				//fprintf(stderr, "Warning: Read invalid frame size (%u)"
				 //       " - not a raw file?\n", new_buf_sz);
			if (new_buf_sz > *buf_alloc_sz)
			{
				unsigned char *new_buf = realloc(*buf, 2 * new_buf_sz);
				if (new_buf)
				{
					*buf = new_buf;
					*buf_alloc_sz = 2 * new_buf_sz;
				}
				else
				{
					//fprintf(stderr, "Failed to allocate compressed data buffer\n");
					new_buf_sz = 0;
				}
			}
		}
		*buf_sz = new_buf_sz;
		if (*buf_sz)
		{
			if (fread(*buf, 1, *buf_sz, infile) != *buf_sz)
			{
				//fprintf(stderr, "Failed to read full frame\n");
				return 1;
			}
			return 0;
		}
	}
	else
	{
	}
    return 1;
}

void output_one_frame_data(FILE* outfile,VO_VIDEO_BUFFER outdata,VO_VIDEO_OUTPUTINFO outInfo,int flipuv)
{
	int i;
	unsigned char* buf_y = outdata.Buffer[0];
	unsigned char* buf_u = outdata.Buffer[1];
	unsigned char* buf_v = outdata.Buffer[2];
	unsigned int     stride_y =  outdata.Stride[0];
	unsigned int     stride_u =  outdata.Stride[1];
	unsigned int     stride_v =  outdata.Stride[2];
	unsigned int     width = outInfo.Format.Width;
	unsigned int     height = outInfo.Format.Height;

	if(outInfo.Format.Type ==	VO_VIDEO_FRAME_NULL)
		return;

	for(i=0;i<height;i++)
	{
		fwrite(buf_y, 1, width, outfile);
		buf_y += stride_y;
	}

	if(!flipuv)
	{
		for(i=0;i<(height+1)/2;i++)
		{
			fwrite(buf_u, 1, (1+width)/2, outfile);
			buf_u += stride_u;
		}

		for(i=0;i<(height+1)/2;i++)
		{
			fwrite(buf_v, 1, (1+width)/2, outfile);
			buf_v += stride_v;
		}	
	}
	else
	{
		for(i=0;i<(height+1)/2;i++)
		{
			fwrite(buf_v, 1, (1+width)/2, outfile);
			buf_v += stride_v;
		}

		for(i=0;i<(height+1)/2;i++)
		{
			fwrite(buf_u, 1, (1+width)/2, outfile);
			buf_u += stride_u;
		}
	}
}

void output_one_frameMB_YUV(FILE* fp,VO_VIDEO_BUFFER outdata,VO_VIDEO_OUTPUTINFO outInfo)
{
	int i,j;
	unsigned char* buf_y = outdata.Buffer[0];
	unsigned char* buf_u = outdata.Buffer[1];
	unsigned char* buf_v = outdata.Buffer[2];
	unsigned int     stride_y =  outdata.Stride[0];
	unsigned int     stride_u =  outdata.Stride[1];
	unsigned int     stride_v =  outdata.Stride[2];
	unsigned int     width = outInfo.Format.Width;
	unsigned int     height = outInfo.Format.Height;

	int mb_row =0,mb_col=0;

	if(outInfo.Format.Type ==	VO_VIDEO_FRAME_NULL)
		return;

	for(mb_row = 0; mb_row < (height>>4); mb_row++)
	{
		for(mb_col = 0; mb_col < (stride_y>>4); mb_col++)
		{
			unsigned char* mbbuffy = buf_y + mb_row*16*stride_y + mb_col*16;
			unsigned char* mbbuffu = buf_u + mb_row*8*stride_u + mb_col*8;
			unsigned char* mbbuffv = buf_v + mb_row*8*stride_v + mb_col*8;
			fprintf(fp,"***********row %d**col %d**************\n",mb_row,mb_col);
			for(i=0;i<16;i++)
			{
				for(j=0;j<16;j++)
				{
					fprintf(fp,"0x%x ",mbbuffy[i*stride_y+j]);
				}
				fprintf(fp,"\n");
			}
			fprintf(fp,"\n");

			for(i=0;i<8;i++)
			{
				for(j=0;j<8;j++)
				{
					fprintf(fp,"0x%x ",mbbuffu[i*stride_u+j]);
				}
				fprintf(fp,"\n");
			}
			fprintf(fp,"\n");

			for(i=0;i<8;i++)
			{
				for(j=0;j<8;j++)
				{
					fprintf(fp,"0x%x ",mbbuffv[i*stride_v+j]);
				}
				fprintf(fp,"\n");
			}
			fprintf(fp,"\n");

		}
	}
}

float Decode_VP8(char *infile ,char *oufile,int times,int nthreads,int disabledeblock);
#ifdef VOWINCE
int _tmain(int argc, TCHAR **argv) 
#else 
int main(int argc, char **argv)
#endif
{
	unsigned long i=0,j=0;
	float fps=0;
#ifdef VOWINCE
	char *infile  = "/My Storage/vp8/vp80-00-comprehensive-015.ivf";//argv[0];
	char *oufile = "/My Storage/vp8/sdk3.0_ref_v7.yv12";
	char *speedfile = "Program Files/voVP8DecTst/performance.txt";
#elif VOWINXP
	char *infile  = "E:/MyResource/Video/clips/VP8/QuickTest/vp80-00-comprehensive-014.ivf";//argv[0];
	char *oufile = "E:/MyResource/Video/clips/VP8/QuickTest/vp80-00-comprehensive-014.yuv";
	char *speedfile = "E:/MyResource/Video/clips/VP8/QuickTest/speed_vp8.txt";
#elif RVDS
	char *infile  = "D:/123.vp8";//argv[0];
	char *oufile = "D:/sdk3.0_ref.yv12";
	char *speedfile = "D:/speed_vp8.txt";
#else VOANDROID
	char *infile  = "1.ivf";
	char *oufile = "sdk3.0_ref_android.yv12";
	char *speedfile = "speed_vp8.txt";
#endif

	char mfilename[255];
	char mfilenameout[255];
	int TIMES = 1,nthreads =1,disabledeblock=0;
	mfilename[0] ='\0';
	mfilenameout[0] = '\0';

	if(argc>2)
	{
		strcpy(mfilename, argv[1]);
		strcpy(mfilenameout, argv[2]);
		nthreads = *argv[3] -0x30;
		TIMES = *argv[4] -0x30;
		disabledeblock = *argv[5] -0x30;
	}
	else
	{
		sprintf(mfilename,"E:/MyResource/Video/clips/VP8/QuickTest/VP8_1011k_352x288_24f_profile-1_token-parts_4.ivf");
		sprintf(mfilenameout,"E:/MyResource/Video/clips/VP8/QuickTest/VP8_1011k_352x288_24f_profile-1_token-parts_4.yuv");
		//sprintf(mfilename,"/My Storage/vp8/13114182_VP8_647K_320x132_25f_profile-3_threads-1_sharpness-0_token-parts-2.ivf");
		//sprintf(mfilenameout, "/My Storage/vp8/13114182_VP8_647K_320x132_25f_profile-3_threads-1_sharpness-0_token-parts-2.yv12");
	}
#ifdef VOWINXP
	srand((int)time(0));
#endif
#define Enable_OutPut 1
	for(j=0;j<1;j++)
	{
		fps = 0;
		for(i=0;i< TIMES;i++) //TIMES
		{
    		fps +=Decode_VP8(mfilename,mfilenameout,i,nthreads,disabledeblock);

			printf("**********%d********* \n",i);
		}
	#ifdef VOWINCE
		{
			FILE* fp =fopen(speedfile,"a");
			fprintf(fp,"sdk3.0 %f fps \n",fps /i);
			fclose(fp);
		}
	#else
		printf("  vp8dec : %f fps\n",fps /i);
	#endif
	}
}

float Decode_VP8(char *infilename ,char *outfilename,int times,int nthreads,int disabledeblock)
{	
	VO_HANDLE phCodec;
	VO_CODECBUFFER	indata;
	VO_VIDEO_BUFFER	outdata;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI	voVP8Dec;

	int is_ivf,fourcc;
	FILE   *infile,*outfile,*errorfile;
	unsigned long buf_alloc_sz =0;
	int decode_frames = 0;

	long start =0,finish=0;
	unsigned long  dx_time=0; 
	char errordatafilename[255];

	VO_S32 ret =0;

#ifdef ERROR_VP8
	if(0)
	{
		sprintf(errordatafilename,"E:/MyResource/Video/clips/VP8/QuickTest/errordata.ivf");
		generate_errordtat_ivf(infilename,errordatafilename);
		infilename = errordatafilename;
	}
#endif

	memset(&indata,0,sizeof(VO_CODECBUFFER));
	memset(&outdata,0,sizeof(VO_VIDEO_BUFFER));
	memset(&outInfo,0,sizeof(VO_VIDEO_OUTPUTINFO));

	 infile   = fopen(infilename, "rb");
	 outfile = fopen(outfilename, "wb");

	 is_ivf = file_is_ivf(infile, &fourcc);  // ivf file format

	ret = voGetVP8DecAPI(&voVP8Dec,0);

	ret =voVP8Dec.Init(&phCodec, VO_VIDEO_CodingVP8, NULL);

	nthreads = 2;
	ret =voVP8Dec.SetParam(phCodec,VO_PID_COMMON_CPUNUM,&nthreads);

	//disable_deblock = 0;
	ret =voVP8Dec.SetParam(phCodec,VO_PID_DEC_VP8_DISABLEDEBLOCK,&disabledeblock);

	while (!read_one_frame_data(infile, &(indata.Buffer), &(indata.Length), &buf_alloc_sz,is_ivf)) // parse data
    {
		//continue;
#ifdef VOWINCE
	start = GetTickCount();
#elif RVDS
#elif VOANDROID
		start = timeGetTime();
#else
		start = clock();
#endif
		ret =voVP8Dec.SetInputData(phCodec, &indata);

		if(decode_frames ==17) //zou97
			decode_frames =decode_frames;

		ret =voVP8Dec.GetOutputData(phCodec, &outdata, &outInfo);

#if 0
		if(ret != 0)
			printf(" -----%d  %d %d   type -->%d error!!\n",decode_frames,outInfo.Format.Width,outInfo.Format.Height,outInfo.Format.Type);
		else
			printf(" -----%d  %d %d   type -->%d  correct! \n",decode_frames,outInfo.Format.Width,outInfo.Format.Height,outInfo.Format.Type);
#endif
		

#ifdef Enable_OutPut
		output_one_frame_data(outfile,outdata,outInfo,0);

		if(0)
		{
			char yuvname[255];
			FILE* fp ;
			sprintf(yuvname,"E:/MyResource/Video/clips/VP8/QuickTest/tmp/new/%d.yuv",decode_frames);
			fp =fopen(yuvname,"wb");
			output_one_frameMB_YUV(fp,outdata,outInfo);
			fclose(fp);
		}
#endif		
		decode_frames++;
		//if(decode_frames== 3)
		//	goto STOP;
#ifdef VOWINCE
		finish = GetTickCount();
		dx_time += finish-start;
#elif RVDS
#elif VOANDROID
	finish =  timeGetTime();
	dx_time += finish-start;
#else
	finish = clock();
	dx_time += finish-start;
#endif

	}

STOP:
	voVP8Dec.Uninit(phCodec);

	free(indata.Buffer);
	fclose(infile);
	fclose(outfile);
    return ((float)decode_frames) / (float)((dx_time)/1000.0);
}
