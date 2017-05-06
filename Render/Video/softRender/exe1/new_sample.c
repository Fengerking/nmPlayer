#if defined(WIN32_SAMPEL)
#else
//#define WIN_CE
#define PPC_2003
#endif
#include "ccrrrender.h"
//#define PPC_2005
//#define WIN_CE
#if defined(PPC_2003)
#include "stdio.h"
#include "stdlib.h"
#elif defined(WIN_CE)
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#else
#include "stdio.h"
#include "stdlib.h"
//#include <windows.h>
//#include <commctrl.h>
#endif

#include "voccrender.h"

#if defined(PPC_2005) || defined(PPC_2003)

#include <windows.h>
#include <commctrl.h>
int _tmain(int argc, TCHAR **argv) {

#elif defined(PPC_2003)
int WinMain(){
#else
main(int argc, char *argv[]){
#endif

	int x_tab[4*1024], y_tab[4*1024], k, width, height, i, j;
	unsigned char mb_skip[1024*4];

	int in_width, in_height, in_stride, *x_scale_tab, *y_scale_tab, out_width, out_height, out_stride, mb_width, mb_height, out_size;
	unsigned char *in_buf, *out_buf, *dst_start, *dst;
	unsigned char *src_y, *src_u, *src_v;

	VO_IV_RTTYPE rt;
	HVOCCRRRENDER hnd;

	FILE *in_file = NULL;
	FILE *out_file = NULL;
	FILE *stats_file = NULL ;
	VOCODECVIDEOBUFFER  pInput;
	VOCODECVIDEOBUFFER  pOutput;


#ifdef WIN_CE
	double enctime_start, enctime;
	double totalenctime = 0.;

	if(!(stats_file = fopen("SPEED.txt", "wb"))){
		printf("Open statistic file error!");
	}
/*
	in_file=fopen("/yuv_640x480.yuv", "rb");
//	in_file=fopen("/yuv_320x240.yuv", "rb");
//	in_file=fopen("yuv_176x144.yuv", "rb");
	out_file=fopen("/yuv.raw", "w+b");
*/
#endif

#if defined(PPC_2005) || defined(PPC_2003)
	in_file=fopen("\\Storage Card\\480X230.yuv", "rb");
	out_file=fopen("\\Storage Card\\320x240__.raw", "w+b");
#else
/**/
	k = 1;
	if (argc > 1) {
		for ( ; k < argc; k += 2) {
			switch (argv[k][1]) {
				case 'i':
				case 'I':
					if (in_file != NULL)
						fclose(in_file);

					if ((in_file=fopen(argv[k+1], "rb")) == NULL) {
						goto LExit;
					}

					break;

				case 'o':
				case 'O':
					if (out_file != NULL)
						fclose(out_file);
		
					if ((out_file=fopen(argv[k+1], "w+b")) == NULL) {
						goto LExit;
					}

					break;

				default:
					goto LExit;
			} //!< switch(argv[k][1])
		} //!< for(k)
	}else{
//		show_usage();
		return;
	}

#endif	
	///////////////////
	in_width = 640;
	in_height = 272;
	in_stride = in_width;
	out_width = in_width*3/4;
	out_height = in_height*3/4;
	rt = VO_RT_DISABLE;//VO_RT_DISABLE |	VO_RT_90L | VO_RT_90R | VO_RT_180;

	memset(mb_skip, 0, 1024*4);
	mb_skip[0] = mb_skip[1] = 0;

	in_buf = (unsigned char*)malloc(in_stride*in_height*3/2);

	if(rt == VO_RT_DISABLE){
		out_stride = out_width*2;
		out_size = out_height*out_stride;

	}else{
		out_stride = out_height*2;
		out_size = out_width*out_stride;
	}
	out_buf = (unsigned char*)malloc(out_size);

	fread(in_buf, 1, in_stride*in_height*3/2, in_file);

	src_y = in_buf;
	src_u = in_buf + in_stride * in_height;
	src_v = src_u + ((in_stride * in_height) >> 2);


	pInput.data_buf[0] = src_y;
	pInput.data_buf[1] = src_u;
	pInput.data_buf[2] = src_v;
	pInput.stride[0] = in_stride;
	pInput.stride[1] = in_stride/2;
	pInput.stride[2] = in_stride/2;
	pInput.video_type = VOYUV_PLANAR420;

	pOutput.stride[0] = out_stride;
	pOutput.data_buf[0] = out_buf;
	pOutput.video_type = VORGB565_PACKED;

//	VOCCRRRETURNCODE VOAPI voCCRRInit(VO_HANDLE * phCCRR, VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag)
	voCCRRInit(&hnd, NULL, NULL, 0);
//	cc_ini(&hnd, in_width, out_width, in_height, out_height, rt, YUV420PLANAR, RGB565);

//	VOCCRRRETURNCODE VOAPI voCCRRSetVideoSize(VO_HANDLE hCCRR, VO_U32 * nInWidth, VO_U32 * nInHeight, VO_U32 * nOutWidth, VO_U32 * nOutHeight, VO_IV_RTTYPE nRotateType);
	voCCRRSetVideoSize(hnd, &in_width, &in_height, &out_width, &out_height, VO_RT_DISABLE);//*pRotateType = 	CCRR_ROTATION_DISABLE |	CCRR_ROTATION_90L | CCRR_ROTATION_90R | CCRR_ROTATION_180;
//VOCCRRRETURNCODE VOAPI voCCRRSetVideoType(VO_HANDLE hCCRR, VO_IV_COLORTYPE lInType, VO_IV_COLORTYPE lOutType)
	voCCRRSetVideoType(hnd, VOYUV_PLANAR420, VORGB565_PACKED);//VORGB565_PACKED);



#ifdef WIN_CE
	enctime_start = GetTickCount();
#endif

	for(k = 0; k < 150; k ++){
//		cc_process(hnd, src_y, src_u, src_v, pInput.stride, out_buf,out_stride, NULL);
//VOCCRRRETURNCODE VOAPI voCCRRProcess(VO_HANDLE hCCRR, VO_VIDEO_BUFFER * pInput, VO_VIDEO_BUFFER * pOutput, VO_S64 nStart, VO_BOOL bWait)
		voCCRRProcess(hnd, &pInput, &pOutput, NULL, 0);

		fwrite(out_buf, 1, out_size, out_file);
		fread(in_buf, 1, in_stride*in_height*3/2, in_file);
	}
#ifdef WIN_CE
	enctime_start = GetTickCount() - enctime_start;
#endif
//	cc_uini(hnd);
	voCCRRUninit(hnd);
#ifdef WIN_CE
	fprintf(stats_file, "\n cc %d frame, time: %d ms", k, (int)enctime_start);
#endif

	fwrite(out_buf, 1, out_size, out_file);

LExit:
	if (in_file)
		fclose(in_file);
	if (out_file)
		fclose(out_file);
	if(in_buf)
		free(in_buf);
	if(out_buf)
		free(out_buf);
	
}