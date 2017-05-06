
#include "stdio.h"


void *DEBUG_MALLOC(int size,char *Filename,int LineNumber)
{       
        #undef _DEBUG_MALLOC_
        #ifndef _DEBUG_MALLOC_

        void *tmp;
	static int MaNo=0;
	FILE *Mfile;
	Mfile=fopen("E:\Mf.txt","a+");

        MaNo+=1;

	fprintf(Mfile,"\tFile Name = %s",Filename);
	fprintf(Mfile,"\tLine Number = %d",LineNumber);
	fflush(Mfile);
	tmp=malloc(size);
	fprintf(Mfile,"\tAllocated ptr = 0x%x",tmp);
	fprintf(Mfile,"\tSize=%d",size);
        fprintf(Mfile,"\tMaNo=%d\n",MaNo);
	fflush(Mfile);
        fclose(Mfile);
	return tmp;
        #endif

}

void DEBUG_FREE(void *ptr,char *Filename,int LineNumber)
{	
        #undef _DEBUG_FREE_
        #ifndef _DEBUG_FREE_

        FILE *Mfile;
	static int FrNo=0; 
        Mfile=fopen("E:\Mf.txt","a+");
        FrNo+=1;
	fprintf(Mfile,"\tFile Name = %s",Filename);
	fprintf(Mfile,"\tLine Number = %d",LineNumber);
	fprintf(Mfile,"\tFreed ptr = 0x%x",ptr);
	fprintf(Mfile,"\tFrNo=%d\n",FrNo);
        fflush(Mfile);
	free(ptr);
	fclose(Mfile);

        #endif
}
