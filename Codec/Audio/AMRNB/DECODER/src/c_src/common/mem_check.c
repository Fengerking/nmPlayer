
#include "stdio.h"

#ifdef ARM
//#include <utils/Log.h>
#endif

void *DEBUG_MALLOC(int size,char *Filename,int LineNumber)
{       
#undef _DEBUG_MALLOC1_
#ifndef _DEBUG_MALLOC1_

#ifdef ARM
	//LOGD("->%d; %s(); DEBUG_MALLOC(LHP)\n:", __LINE__, __FUNCTION__);
#endif

	void *tmp;
	static int MaNo=0;
	FILE *Log_file;
	Log_file=fopen("E://log_f.txt","a+");

	MaNo+=1;

	fprintf(Log_file,"\tFile Name = %s",Filename);
	fprintf(Log_file,"\tLine Number = %d",LineNumber);
	fflush(Log_file);
	tmp=malloc(size);
	fprintf(Log_file,"\tAllocated ptr = 0x%x",tmp);
	fprintf(Log_file,"\tSize=%d",size);
	fprintf(Log_file,"\tMaNo=%d\n",MaNo);
	fflush(Log_file);
	fclose(Log_file);
	return tmp;
#endif

}

void DEBUG_FREE(void *ptr,char *Filename,int LineNumber)
{	
#undef _DEBUG_FREE1_
#ifndef _DEBUG_FREE1_

	FILE *Log_file;
	static int FrNo=0; 
	Log_file=fopen("E://log_f.txt","a+");
	FrNo+=1;
	fprintf(Log_file,"\tFile Name = %s",Filename);
	fprintf(Log_file,"\tLine Number = %d",LineNumber);
	fprintf(Log_file,"\tFreed ptr = 0x%x",ptr);
	fprintf(Log_file,"\tFrNo=%d\n",FrNo);
	fflush(Log_file);
	free(ptr);
	fclose(Log_file);

#endif
}
