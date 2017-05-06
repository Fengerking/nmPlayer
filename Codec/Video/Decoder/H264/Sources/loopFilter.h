#ifndef LOOP_FILTER_H
#define LOOP_FILTER_H
//#define USE_HIGHER_THREAD
//#define THREAD_DEBLOCK  

#ifdef WMMX
#include "global.h"
#if 1//def ARM //DISABLE the IPP_DEBLOCK on PC
#define IPP_DEBLOCK
void IPP_InPlaceDeblockMBs(ImageParameters *img,TMBsProcessor *info);
#endif//ARM
#endif//WMMX
#endif	