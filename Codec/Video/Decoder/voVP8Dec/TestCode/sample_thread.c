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
#include "pthread.h"
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

typedef struct Data_struct
{
	unsigned char* src;
	unsigned char* dst;
	unsigned long height;	
	unsigned long src_srtide;
	unsigned long dst_srtide;
}MY_DATA;

int g_sem =0;

int vp8_thread_1(void *p_data)
{
	MY_DATA *my_dat = (MY_DATA *)p_data;
	int i=0,times=0;
	
while(g_sem==0)
{
	printf("-----thread 1 start\n");
	for(times=0;times<600;times++)
	{
		sleep(0);
		for(i=0;i<my_dat->height;i++)
			memcpy(my_dat->dst+i*my_dat->dst_srtide,my_dat->src+i*my_dat->src_srtide,my_dat->dst_srtide);
	}
	g_sem = 1;
	printf("----thread 1 finished \n");
}
	
		
}

int vp8_thread_2(void *p_data)
{
	MY_DATA *my_dat = (MY_DATA *)p_data;
	int i=0;
	for(i=0;i<my_dat->height;i++)
		memcpy(my_dat->dst+i*my_dat->dst_srtide,my_dat->src+i*my_dat->src_srtide,my_dat->dst_srtide);
	
}

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
	char *infile  = "D:/svn/Numen/engineer/zou_zhihuang/Codec/Video/Vp8/vp8-test-vectors-r1/vp80-00-comprehensive-015.ivf";//argv[0];
	char *oufile = "D:/svn/Numen/engineer/zou_zhihuang/Codec/Video/Vp8/vp8-test-vectors-r1/sdk3.0_ref.yv12";
	char *speedfile = "D:/svn/Numen/engineer/zou_zhihuang/Codec/Video/Vp8/vp8-test-vectors-r1/speed_vp8.txt";
#elif VOANDROID
	char *infile  = "vp80-00-comprehensive-015.ivf";
	char *oufile = "sdk3.0_ref.yv12";
	char *speedfile = "speed_vp8.txt";
#endif
	//short tmp[400];	
	
	MY_DATA mydata;
	int times=0;
	unsigned long start =0,finish=0;
	pthread_t           h_thread_lpf;
	
	mydata.src = (unsigned char*)malloc(sizeof(unsigned char)*999*1024);
	mydata.dst = (unsigned char*)malloc(sizeof(unsigned char)*999*1024);
	mydata.height = 1024;
	mydata.src_srtide = 999;
	mydata.dst_srtide = 999;
	
	memset(mydata.src,253,999*1024);
	memset(mydata.dst,253,999*1024);
	
	start = timeGetTime();
	
	pthread_create(&h_thread_lpf, 0, vp8_thread_1, &mydata);
	
	printf("*****thread main start\n");
	
	for(times=0;times<600;times++)
	{
		sleep(0);
		for(i=0;i<mydata.height;i++)
			memcpy(mydata.dst+i*mydata.dst_srtide,mydata.src+i*mydata.src_srtide,mydata.dst_srtide);
  }
  printf("****thread main finished\n");
  
  /*for(times=0;times<600;times++)
	{
		for(i=0;i<mydata.height;i++)
			memcpy(mydata.dst+i*mydata.dst_srtide,mydata.src+i*mydata.src_srtide,mydata.dst_srtide);
  }*/
  
  while(g_sem!=1)
  sleep(0);
		
	finish = timeGetTime();

	printf("%d  vp8dec : %d ms\n",g_sem, finish-start);
	
	free(mydata.src);
	free(mydata.dst);
}
