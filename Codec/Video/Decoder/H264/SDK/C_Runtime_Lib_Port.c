#ifdef __cplusplus
extern "C" {
#endif

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		<limits.h>
#include 		<stdarg.h>
	
#define vprintf(...)
void voH264Memset(void * s, int d, size_t size)
{
	memset(s,d,size);
}
void voH264Printf(const char * format,...)
{
	va_list arg;
	va_start(arg, format);
	vprintf(format, arg);
	va_end(arg);
}
void voPrintf(const char * format,...)
{
	va_list arg;
	va_start(arg, format);
	vprintf(format, arg);
	va_end(arg);
}
void voH264Fprintf(const char * format,...)
{
	va_list arg;
	va_start(arg, format);
	vprintf(format, arg);
	va_end(arg);
}
void voH264Memcpy(void * dst, const void * src, size_t size)
{
	memcpy(dst,src,size);
}
void voH264Memmove(void * dst, const void * src, size_t size)
{
	memmove(dst,src,size);
}
void* voH264Calloc(size_t s, size_t s2)
{
	return calloc(s, s2);
}
void voH264Free(void* d)
{
	free(d);
}
void voH264Qsort(void * a, size_t b, size_t c,
           int (*d)(const void *, const void *))
{
	qsort(a,b,c,d);
}

int voH264IntDiv(int a,int b) 
{
	return a/b;
}
int voH264IntMod(int a,int b) 
{
	return a%b;
}
void AvdLog(int logId, char *format, ...)
{
	va_list arg;
	va_start(arg, format);
#ifdef G1
	//voLog_android_264_debug
#else
	vprintf(format, arg);
#endif
	va_end(arg);
}
#ifdef __cplusplus
}
#endif

