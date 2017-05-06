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

void vo_B_RD_PRED(unsigned char* pp,unsigned char*predictor)
{
	predictor[3 * 16 + 0] = (pp[0] + pp[1] * 2 + pp[2] + 2) >> 2;
    predictor[3 * 16 + 1] =
        predictor[2 * 16 + 0] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
    predictor[3 * 16 + 2] =
        predictor[2 * 16 + 1] =
            predictor[1 * 16 + 0] = (pp[2] + pp[3] * 2 + pp[4] + 2) >> 2;
    predictor[3 * 16 + 3] =
        predictor[2 * 16 + 2] =
            predictor[1 * 16 + 1] =
                predictor[0 * 16 + 0] = (pp[3] + pp[4] * 2 + pp[5] + 2) >> 2;
    predictor[2 * 16 + 3] =
        predictor[1 * 16 + 2] =
            predictor[0 * 16 + 1] = (pp[4] + pp[5] * 2 + pp[6] + 2) >> 2;
    predictor[1 * 16 + 3] =
        predictor[0 * 16 + 2] = (pp[5] + pp[6] * 2 + pp[7] + 2) >> 2;
    predictor[0 * 16 + 3] = (pp[6] + pp[7] * 2 + pp[8] + 2) >> 2;
}
float Decode_VP8(char *infile ,char *oufile);
#ifdef VOWINCE
int _tmain(int argc, TCHAR **argv) 
#else 
int main(int argc, char **argv)
#endif
{
	unsigned long i=0,j=0;
	float fps=0;
#ifdef VOWINCE
	char *infile  = "/Storage Card/vp8/vp80-00-comprehensive-015.ivf";//argv[0];
	char *oufile = "/Storage Card/vp8/sdk3.0_ref_v7.yv12";
	char *speedfile = "Program Files/voVP8DecTst/performance.txt";
#elif VOWINXP
	char *infile  = "D:/svn/Numen/engineer/zou_zhihuang/Codec/Video/Vp8/errorclips/mkv_vp8_raw.ivf";//argv[0];
	char *oufile = "D:/svn/Numen/engineer/zou_zhihuang/Codec/Video/Vp8/errorclips/sdk3.0_vo.yv12";
	char *speedfile = "D:/svn/Numen/engineer/zou_zhihuang/Codec/Video/Vp8/errorclips/speed_vp8.txt";
#elif RVDS
	char *infile  = "D:/123.vp8";//argv[0];
	char *oufile = "D:/sdk3.0_ref.yv12";
	char *speedfile = "D:/speed_vp8.txt";
#else VOANDROID
	char *infile  = "240x320-1401f_366.vp8";
	char *oufile = "sdk3.0_ref.yv12";
	char *speedfile = "speed_vp8.txt";
#endif
	/*unsigned char above[64*64];	
	unsigned char left[64*64];	
	unsigned char tmp[64*64];
	
	for(i=0;i<16*16;i++)
	{
		above[i] = 1+i;
		left[i]  = 16+i;
		tmp[i] = 0;
	}
	
	vo_B_RD_PRED_neon(above,tmp);
	
	for(i=0;i<16*16;i++)
	{
		above[i] = 1+i;
		left[i]  = 16+i;
		tmp[i] = 0;
	}
	vo_B_RD_PRED(above,tmp);*/

#define Enable_OutPut 1
	for(j=0;j<1;j++)
	{
		fps = 0;
		for(i=0;i<1;i++)
		{
    		fps +=Decode_VP8(infile,oufile);
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
	//printf("????");
}

float Decode_VP8(char *infilename ,char *outfilename)
{	
	VO_HANDLE phCodec;
	VO_CODECBUFFER	indata;
	VO_VIDEO_BUFFER	outdata;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI	voVP8Dec;

	int is_ivf,fourcc;
	FILE   *infile,*outfile;
	unsigned long buf_alloc_sz =0;
	int decode_frames = 0;

	long start =0,finish=0;
	unsigned long  dx_time=0;
	unsigned int nthreads = 4; 

	memset(&indata,0,sizeof(VO_CODECBUFFER));
	memset(&outdata,0,sizeof(VO_VIDEO_BUFFER));
	memset(&outInfo,0,sizeof(VO_VIDEO_OUTPUTINFO));

	 infile   = fopen(infilename, "rb");
	 outfile = fopen(outfilename, "wb");

	 is_ivf = file_is_ivf(infile, &fourcc);  // ivf file format


	voGetVP8DecAPI(&voVP8Dec,0);

	voVP8Dec.Init(&phCodec, VO_VIDEO_CodingVP8, NULL);

	voVP8Dec.SetParam(phCodec,VO_PID_DEC_VP8_THREADS,&nthreads);

	while (!read_one_frame_data(infile, &(indata.Buffer), &(indata.Length), &buf_alloc_sz,is_ivf)) // parse data
    {
#ifdef VOWINCE
	start = GetTickCount();
#elif RVDS
#elif VOANDROID
		start = timeGetTime();
#else
		start = clock();
#endif
		voVP8Dec.SetInputData(phCodec, &indata);
		voVP8Dec.GetOutputData(phCodec, &outdata, &outInfo);

#ifdef Enable_OutPut
		output_one_frame_data(outfile,outdata,outInfo,0);
#endif		
		decode_frames++;
		//printf("%d  \n",decode_frames);
		//if(decode_frames==100)
			//goto STOP;
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
