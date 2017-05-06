
#include "contributors.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if defined WIN32
  #include <conio.h>
#endif

#ifdef _WIN32_WINNT
#include <sys/timeb.h>
#endif //_WIN32_WINNT

#if !BUILD_WITHOUT_C_LIB
#include <assert.h>
#endif

#include "global.h"
#include "block.h"

//#include "H264_C_Type.h"
#include "avd_neighbor.h"
#include "rtp.h"
#include "memalloc.h"
#include "mbuffer.h"
//#include "leaky_bucket.h"
#include "fmo.h"
#include "annexb.h"
#include "output.h"
#include "cabac.h"
#include "parsetcommon.h"
#include "h264VdDump.h"

#include "erc_api.h"

#if (AVD_PLATFORM & AVD_PENTIUM) | defined(TEST_C55x_ON_PC)
#if			DUMP_VERSION
#include <direct.h>
#include <errno.h>
#endif	//DUMP_VERSION
#endif //!(AVD_PLATFORM & AVD_TI_C55x) 
#ifdef TRACE_MEM
static void TestMem(ImageParameters *img,int id,int flag);
#endif
#if (DUMP_VERSION & DUMP_BITS)
char 				_dmpBitsDir[256];
FILE				*_dmpBitsFp = NULL;
//FILE				*_dmpAllBitsFp = NULL;
int					_dmpTotBits,
					_dmpSliceBits,
					_dmpTotalMBBits,		//!< includes bits not explicitly tracked, such as CBP, deltaQP, etc.
					_dmpMBTypeBits,			//!< MB, subMB types
					_dmpMBModeBits,			//!< intra chroma coding mode, Intra_4x4 mode bits
					_dmpMBIndexBits,		//!< reference indices
					_dmpMBMVDBits,
					_dmpMBResidualBits[3];
int					_dmpSliceMBTypeBits,	
					_dmpSliceMBModeBits,	
					_dmpSliceMBIndexBits,	
					_dmpSliceMBMVDBits,
					_dmpSliceMBResidualBits[3];
#endif //(DUMP_VERSION & DUMP_BITS)

#if (DUMP_VERSION & DUMP_DEBLOCK)
char 				_dmpDblkDir[256];
#endif //(DUMP_VERSION & DUMP_DEBLOCK)

#if (DUMP_VERSION & DUMP_SLICE)
char 				_dmpSlcDir[256];

int					_dumpIP4 [10];
int					_dumpIP8 [5];
int					_dumpIP16[5];
int					_dumpMbMode[12];
int					_dumpIP4Tot [10];
int					_dumpIP8Tot [5];
int					_dumpIP16Tot[5];
int					_dumpMbModeTot[12];
char				IPredName4x4  [10][32] = {"V","H","DC","DiaDL","DiaDR","VR","HD","VL","HU","NONE"};
char				IPredName16x16[5][32]  = {"V16","H16","DC16","PLN16","NONE16"};
char				IPredName8x8  [5][32]  = {"V8","H8","DC8","PLN8","NONE8"};
char				MBModeName    [12][32]  = {"PCopy_BDir","MC16x16","MC16x8","MC8x16",
								"MC8x8","MC8x4","MC4x8","MC4x4","I8SubI4",
								"I16MB","I4MB","SI4MB"};
char				MCDirName     [3][32]  = {"F","B","Bi"};
// for itrans type
int					_dumpNumItrans = 6;
int					_dumpItransTp		[6];
int					_dumpItransTpTot[6];
char				ItransTpName    [6][32]  = {"Zeros","DC","1stCol","LeftHalf","UpHalf","Normal"};
// for intra and zero coeff;
int					_dumpIntra4Y0Cof;
int					_dumpIntra4C0Cof;
int					_dumpIntra4YNon0Cof;
int					_dumpIntra4CNon0Cof;
int					_dumpIntra4Y0CofTot;
int					_dumpIntra4C0CofTot;
int					_dumpIntra4YNon0CofTot;
int					_dumpIntra4CNon0CofTot;

#endif //(DUMP_VERSION & DUMP_SLICE)

#if			(DUMP_VERSION & DUMP_BMP)
char 				_dmpBmpDir[256];
#endif	//(DUMP_VERSION & DUMP_BMP)

#if			(DUMP_VERSION & DUMP_SEQUENCE)
FILE				*_dmpSeqFp=NULL;
#endif	//(DUMP_VERSION & DUMP_SEQUENCE)
#if (DUMP_VERSION & DUMP_TIMESTAMP)
FILE				*_dmpTsFp;
#endif
#if (DUMP_VERSION & DUMP_MEMORY)
FILE				*_dmpMemory;
int					_totalMemory;
#endif //(DUMP_VERSION & DUMP_MEMORY)

#if (DUMP_VERSION )
FILE				*_dmpDblkFp[2]		= {NULL};
FILE				*_dmpSlcFp[2]		= {NULL};
FILE				*_dmpSlcFpData[2]	= {NULL};
void AvdLog(int logId,int rowIDX, char *format, ...)
{
	FILE *fp;
	va_list arg;
	//if (logId != DUMP_SEQUENCE && logId != DUMP_SLICE && _dumpSkipped)
	//return;

	switch (logId){

	case DUMP_SEQUENCE:
		fp = _dmpSeqFp;
		break;
	case DUMP_SLICE:
		fp = _dmpSlcFp[rowIDX];
		break;
	case DUMP_SLICEDATA:
		fp = _dmpSlcFpData[rowIDX];
		break;
	case DUMP_DEBLOCK:
		fp = _dmpDblkFp[rowIDX];
		break;
	default: 
		return;
	}
	if (!fp)
		return;

	va_start(arg, format);
	vfprintf(fp, format, arg);
	va_end(arg);
	fflush(fp);
}

#endif //(DUMP_VERSION & DUMP_RUN_TIME)

#define JM          "7"
#define VERSION     "7.6"

#define LOGFILE     "log.dec"
#define DATADECFILE "dataDec.txt"
#define TRACEFILE   "trace_dec.txt"
FILE* gLogFile2=NULL;
extern objectBuffer_t *erc_object_list;
extern ercVariables_t *erc_errorVar;
//extern ColocatedParams *Co_located;


// global variable from global.h
#ifdef SHOW_SNR
avdUInt8 **imgY_ref;                                //!< reference frame find snr
avdUInt8 ***imgUV_ref;
struct snr_par    *snr;         //!< statistics
#endif

avdNativeInt  (*nal_startcode_follows) (ImageParameters *img,TMBsProcessor* info,avdNativeInt eos_bit);

// I have started to move the inp and img structures into global variables.
// They are declared in the following lines.  Since inp is defined in conio.h
// and cannot be overridden globally, it is defined here as input
//
// Everywhere, input-> and img-> can now be used either globally or with
// the local override through the formal parameter mechanism

//struct img_par    *img;         //!< image parameters

void InitClipArrs();
static void DeInitClipArrs();

#if DUMP_VERSION
#ifndef TEST_VERSION
#define DUMP_DIR "c:/avdDump"
#else//TEST_VERSION
#define DUMP_DIR "c:/avdDumpTest"
#endif//TEST_VERSION
void InitDump()
{
#if !BUILD_WITHOUT_C_LIB
	char dmpPath[256];
	char fName[256];
#ifdef _WIN32_WCE
	sprintf(dmpPath, "/My Documents"); //hard coded for now;
	//sprintf(dmpPath, "/baseline/avdDump"); //hard coded for now;
	//sprintf(_dmpDblkDir,"%s/deblock",dmpPath);	
	//sprintf(_dmpSlcDir,"%s/slice",dmpPath);
#else//_WIN32_WCE
#ifdef MAIN_PROFILE
	sprintf(dmpPath, "c:/avdDump_main"); //hard coded for now;
#else//MAIN_PROFILE
	sprintf(dmpPath, DUMP_DIR); //hard coded for now;
#endif//MAIN_PROFILE
#endif//_WIN32_WCE
	
	// "errno" link error in Filter: turn off error checking for Filter debugging;


#if (DUMP_VERSION & DUMP_BITS)
	sprintf(_dmpBitsDir,"%s/bits",dmpPath);

	sprintf(fName,"%s/allBits.txt",_dmpBitsDir);
	//_dmpBitsFp = fopen(fName, "w");
	_dmpTotBits = 0;
#endif //(DUMP_VERSION & DUMP_BITS)

#if (DUMP_VERSION & DUMP_TIMESTAMP)
	_dmpTsFp=fopen("d:/h264TS.txt","w");
#endif//(DUMP_VERSION & DUMP_TIMESTAMP)

#if (DUMP_VERSION & DUMP_SLICE)
	sprintf(_dmpSlcDir,"%s/slice",dmpPath);

#endif //(DUMP_VERSION & DUMP_SLICE)
#if (DUMP_VERSION & DUMP_DEBLOCK)
	sprintf(_dmpDblkDir,"%s/deblock",dmpPath);

#endif //(DUMP_VERSION & DUMP_DEBLOCK)

#if (DUMP_VERSION & DUMP_BMP)
	sprintf(_dmpBmpDir,"%s/bitmap",dmpPath);

#endif //(DUMP_VERSION & DUMP_BMP)

#if (DUMP_VERSION & DUMP_SEQUENCE)
	sprintf(fName,"%s/sequence.txt",dmpPath);
	_dmpSeqFp = fopen(fName, "w");
#endif //(DUMP_VERSION & DUMP_SEQUENCE)

#if (DUMP_VERSION & DUMP_MEMORY)
	sprintf(fName,"%s/memory.txt",dmpPath);
	_dmpMemory = fopen(fName, "w");
	_totalMemory = 0;
#endif //(DUMP_VERSION & DUMP_MEMORY)

#if (DUMP_VERSION & DUMP_RUN_TIME)
	sprintf(_dmpTimeFileName,"%s/runTime.txt",dmpPath);
	memset(_dmpFunctionTotalTime, 0, DT_TOTAL_FUNCTIONS * sizeof (int));
#endif //(DUMP_VERSION & DUMP_MEMORY)

#endif //!BUILD_WITHOUT_C_LIB

#if (DUMP_VERSION & DUMP_SLICE)
	memset(_dumpIP4Tot,  0, 10*sizeof(int));
	memset(_dumpIP8Tot,  0, 5*sizeof(int));
	memset(_dumpIP16Tot, 0, 5*sizeof(int));
	memset(_dumpMbModeTot, 0, 12*sizeof(int));
	memset(_dumpItransTpTot, 0, _dumpNumItrans*sizeof(int));
	_dumpIntra4Y0CofTot = _dumpIntra4C0CofTot = 
		_dumpIntra4YNon0CofTot = _dumpIntra4CNon0CofTot = 0;
#endif //(DUMP_VERSION & DUMP_SLICE)
}

void DeInitDump()
{
#if !BUILD_WITHOUT_C_LIB
#if (DUMP_VERSION & DUMP_SEQUENCE)
	if(_dmpSeqFp)
		fclose(_dmpSeqFp);
#endif
#if (DUMP_VERSION & DUMP_TIMESTAMP)
	if(_dmpTsFp)
		fclose(_dmpTsFp);
#endif//(DUMP_VERSION & DUMP_TIMESTAMP)
#if (DUMP_VERSION & DUMP_MEMORY)
	fclose(_dmpMemory);
#endif //(DUMP_VERSION & DUMP_MEMORY)
#if (DUMP_VERSION & DUMP_BITS)
//	fclose(_dmpAllBitsFp);
#endif //(DUMP_VERSION & DUMP_BITS)
#endif //!BUILD_WITHOUT_C_LIB
}
#endif //DUMP_VERSION


#define CLIP_QP_OFFSET 16 
VoiH264VdReturnCode deInitDec(ImageParameters *img)
{
	avdNativeInt i;
	H264VdLibParam* params = img->vdLibPar;
	TSpecitialInfo	*specialInfo = img->specialInfo;
	TIOInfo	*ioInfo = img->ioInfo;
	TCLIPInfo	*clipInfo = img->clipInfo;
	
	//trace_memIssues(img);
	
	free_global_buffers(img);
	//DeInitSliceList(img);
	free_slice(img,img->currentSlice);

	
	uninit_out_buffer(img);

	if (clipInfo->clip255)
		DeInitClipArrs(img);
	SafevoH264AlignedFree(img,ioInfo->in_buffer);
	ioInfo->in_buffer =	NULL;

	for (i = 0; i < MAXPPS; i++){
		if (img->PicParSet[i]){
			if (img->PicParSet[i]->slice_group_id){
				SafevoH264AlignedFree(img,img->PicParSet[i]->slice_group_id);
			}
			SafevoH264AlignedFree(img,img->PicParSet[i]);
		}
	}

	for (i = 0; i < MAXSPS; i++){
		if (img->SeqParSet[i]){

			if (img->SeqParSet[i]->vui_seq_parameters)
				SafevoH264AlignedFree(img,img->SeqParSet[i]->vui_seq_parameters);
			if(img->SeqParSet[i]->offset_for_ref_frame)
				SafevoH264AlignedFree(img,img->SeqParSet[i]->offset_for_ref_frame);
			SafevoH264AlignedFree(img,img->SeqParSet[i]);
		}
	}


	if(params->deblockFlag==EDF_IMX31_HD)
		ARM11_MX31DeblockUnInit(img);
	img->dec_picture = NULL;
	SafevoH264AlignedFree(img,img->dequant_coef_shift);
#if FEATURE_T8x8
	SafevoH264AlignedFree(img,img->dequant_coef_shift8);
#endif//FEATURE_T8x8
	SafevoH264AlignedFree(img,img->pocInfo);
	SafevoH264AlignedFree(img,img->funcInfo);
	
	SafevoH264AlignedFree(img,img->dpb); // beginning of multiple arrays;
	SafevoH264AlignedFree(img,img->specialInfo->seiBuf.Buffer);
	SafevoH264AlignedFree(img,img->specialInfo);
	SafevoH264AlignedFree(img,img->ioInfo);
	SafevoH264AlignedFree(img,img->sizeInfo);
	SafevoH264AlignedFree(img,img->cabacInfo);
	SafevoH264AlignedFree(img,img->clipInfo);
	SafevoH264AlignedFree(img,img->licenseCheck);
#if MUL_ROW_DEBLOCK
	UnInitMulRowDB(img);
#endif//MUL_ROW_DEBLOCK
	if(params->sharedMem&&params->sharedMem->Uninit)
	{
		params->sharedMem->Uninit(0);
	}
#if DUMP_VERSION
	DeInitDump();
#endif //DUMP_VERSION
#ifdef TRACE_MEM
	trace_memIssues(img);
#endif//TRACE_MEM
	return VOIH264VD_SUCCEEDED;
}
//reset the decoder,free all buffer and then reallocate it

#define MAXNUM_NALU_OFFSET			128
VoiH264VdReturnCode initDec(ImageParameters *img)
{
	avdNativeInt i, j, size;
	avdUInt8 *inputStreamBuf;
	avdUInt16	*tmp16;
	int newMemId = 2011;
	TCLIPInfo	*clipInfo;

#if (DUMP_VERSION & DUMP_MEMORY)
	int memUsed = _totalMemory;
#endif
#if DUMP_VERSION
	InitDump();
#endif //DUMP_VERSION
	i = 0;

	
	

	img->initDone = 1;
	
	img->ioInfo = (TIOInfo *)voH264AlignedMalloc(img,newMemId++,sizeof(TIOInfo));
	init_out_buffer(img);
	img->pocInfo = (TPOCInfo *)voH264AlignedMalloc(img,newMemId++,sizeof(TPOCInfo));
	
	img->funcInfo = (TFuncInfo *)voH264AlignedMalloc(img,newMemId++,sizeof(TFuncInfo));
	img->specialInfo = (TSpecitialInfo*)voH264AlignedMalloc(img,newMemId++,sizeof(TSpecitialInfo));
	
	
	img->sizeInfo = (TSizeInfo *)voH264AlignedMalloc(img,newMemId++,sizeof(TSizeInfo));

	img->cabacInfo = (TCABACInfo *)voH264AlignedMalloc(img,newMemId++,sizeof(TCABACInfo));
	
	clipInfo = img->clipInfo = (TCLIPInfo *)voH264AlignedMalloc(img,newMemId++,sizeof(TCLIPInfo));

	img->licenseCheck = (TLicenseCheck2*)voH264AlignedMalloc(img,newMemId++,sizeof(TLicenseCheck2));
	size = //24 * 16 * sizeof(avdInt16) + // for mbBits->cof[24][4][4];
		sizeof(DecodedPictureBuffer) // img->dpb
		+ 52 + (CLIP_QP_OFFSET<<1) * sizeof (avdUInt8) // clipInfo->clipQP51
		+ MAXSPS * sizeof (seq_parameter_set_rbsp_t *) // img->SeqParSet
		+ MAXPPS * sizeof (pic_parameter_set_rbsp_t *) // img->PicParSet

		;

	// Notice that in WMMX, mbBits->cof needs to be 8 aligned, so put it as the first one;
	img->dpb = (DecodedPictureBuffer *)voH264AlignedMalloc(img,4, size);CHECK_ERR_RET_INT
	clipInfo->clipQP51 = (avdUInt8 *)((avdUInt8 *)img->dpb + sizeof(DecodedPictureBuffer));

	img->SeqParSet = (seq_parameter_set_rbsp_t **)(clipInfo->clipQP51 + 52 + (CLIP_QP_OFFSET<<1) 
		* sizeof (avdUInt8));

	img->PicParSet = (pic_parameter_set_rbsp_t **)((avdUInt8 *)img->SeqParSet + 
		MAXSPS * sizeof (seq_parameter_set_rbsp_t *));
	


	img->dequant_coef_shift = tmp16 = (avdUInt16 *)voH264AlignedMalloc(img,501, 96*9*sizeof(avdUInt16));CHECK_ERR_RET_INT
	i = 0;
	do {
		j = 0;
		do {
			*tmp16++ = (dequant_coef[j]<<i);
		} while (++j < 96);
	} while (++i < 9);
#if FEATURE_T8x8
	img->dequant_coef_shift8 = tmp16 = (avdUInt16 *)voH264AlignedMalloc(img,502, 384*9*sizeof(avdUInt16));CHECK_ERR_RET_INT
		i = 0;
	do {
		j = 0;
		do {
			*tmp16++ = (dequant_coef8[j]<<i);
		} while (++j < 384);
	} while (++i < 9);
#endif//FEATURE_T8x8
	memset(img->dpb, 0, sizeof(DecodedPictureBuffer));
	memset(img->SeqParSet, 0, MAXSPS * sizeof (seq_parameter_set_rbsp_t *));
	memset(img->PicParSet, 0, MAXPPS * sizeof (pic_parameter_set_rbsp_t *));
	img->dec_picture = NULL;
	init_out_buffer(img);

	// Allocate Slice data struct
	

	//InitSliceList(img);
	malloc_slice(img);
	//img->SliceList[0] = img->currentSlice;

	CHECK_ERR_RET_INT
	size = 51 + 1 + (CLIP_QP_OFFSET<<1);
	for (i = 0; i < size; i++){
		clipInfo->clipQP51[i] = (i < CLIP_QP_OFFSET) ? 0 : (i > 51 + CLIP_QP_OFFSET) ?
			51 : (i - CLIP_QP_OFFSET);
	}
	clipInfo->clipQP51 += CLIP_QP_OFFSET;
	clipInfo->clip255 = NULL;
	clipInfo->clipHeight = clipInfo->clipHeightCr = NULL;
	clipInfo->clipWidth = clipInfo->clipWidthCr = NULL;
	clipInfo->clipArrHeight= 0;

	img->type = I_SLICE;

#ifdef TRACE_MEM1

	TestMem(img,10086,VOMR_LEAK);
	TestMem(img,10087,VOMR_MULTIPLE_FREE);
	TestMem(img,10088,VOMR_OVERWRITE);
#endif //TRACE_MEM
	AvdLog2(LL_INFO,"initDec done\n");
	return VOIH264VD_SUCCEEDED;
}

#define CLIP255_OFFSET 102400
#define CLIP_H_W_OFFSET (8 * MB_BLOCK_SIZE)
#ifdef WMMX
static const avdInt16 quotiety[6][4]=
{
	{2,2,2,1},
	{2,1,-2,-2},
	{2,-1,-2,2},
	{2,-2,2,-1},
	{2, 2,2,2},
	{2,1,-1,-2},
};
#endif//WMMX
void InitClipArrs(ImageParameters *img)
{
	avdNativeInt i, twoHWOffset, tmp, size;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TCLIPInfo	*clipInfo = img->clipInfo;
#if (DUMP_VERSION & DUMP_MEMORY)
	int memUsed = _totalMemory;
#endif //(DUMP_VERSION & DUMP_MEMORY)
	if (clipInfo->clip255)
		DeInitClipArrs(img);

	twoHWOffset  = (CLIP_H_W_OFFSET<<1);
	size = CLIP255_OFFSET*2 * sizeof(avdUInt8) //clp255
		+ (sizeInfo->height + twoHWOffset // clipHeight
		+ sizeInfo->height_cr + twoHWOffset //height_cr
		+ sizeInfo->width + twoHWOffset //width
		+ sizeInfo->width_cr + twoHWOffset) * sizeof(avdInt16); //width_cr
#if FEATURE_INTERLACE
	size += ((sizeInfo->height_cr>>1) + twoHWOffset) * sizeof(avdInt16); //clipHalfHeightCr
#endif //VOI_H264D_NON_BASELINE

	clipInfo->clipArrHeight= sizeInfo->height;
	clipInfo->clip255      = (avdUInt8 *)voH264AlignedMalloc(img,8, size);//CHECK_ERR_RET_INT
	clipInfo->clipHeight   = (avdInt16 *)(clipInfo->clip255 + CLIP255_OFFSET*2 * sizeof(avdUInt8)); 
	//voH264AlignedMalloc(img,9, (sizeInfo->height + twoHWOffset) * sizeof(avdNativeInt));
	clipInfo->clipHeightCr = clipInfo->clipHeight + sizeInfo->height + twoHWOffset;
	//voH264AlignedMalloc(img,10, (sizeInfo->height_cr + twoHWOffset) * sizeof(avdNativeInt));
	clipInfo->clipWidth    = clipInfo->clipHeightCr + sizeInfo->height_cr + twoHWOffset;
		//voH264AlignedMalloc(img,11, (sizeInfo->width + twoHWOffset) * sizeof(avdNativeInt));
	clipInfo->clipWidthCr  = clipInfo->clipWidth + sizeInfo->width + twoHWOffset;
		//voH264AlignedMalloc(img,12, (sizeInfo->width_cr + twoHWOffset) * sizeof(avdNativeInt));

#if FEATURE_INTERLACE
	clipInfo->clipHalfHeightCr = clipInfo->clipWidthCr + sizeInfo->width_cr + twoHWOffset;
		//voH264AlignedMalloc(img,13, ((sizeInfo->height_cr>>1) + twoHWOffset) * sizeof(avdNativeInt));
	clipInfo->clipHalfHeightCr += CLIP_H_W_OFFSET;
	tmp = (sizeInfo->height_cr>>1)-1;
	for (i = -CLIP_H_W_OFFSET; i < (sizeInfo->height_cr>>1) + CLIP_H_W_OFFSET; i++)
		clipInfo->clipHalfHeightCr[i] = (i<0) ? 0 : ((i>tmp) ? tmp : i);
#endif // VOI_H264D_NON_BASELINE

	// change pt location;
	clipInfo->clip255 += CLIP255_OFFSET;
	for (i = -CLIP255_OFFSET; i < CLIP255_OFFSET; i++)
		clipInfo->clip255[i] = (i<0) ? 0 : ((i>255) ? 255 : i);

	clipInfo->clipHeight += CLIP_H_W_OFFSET;
	tmp = sizeInfo->height-1;
	for (i = -CLIP_H_W_OFFSET; i < sizeInfo->height + CLIP_H_W_OFFSET; i++)
		clipInfo->clipHeight[i] = (i<0) ? 0 : ((i>tmp) ? tmp : i);

	clipInfo->clipHeightCr += CLIP_H_W_OFFSET;
	tmp = sizeInfo->height_cr-1;
	for (i = -CLIP_H_W_OFFSET; i < sizeInfo->height_cr + CLIP_H_W_OFFSET; i++)
		clipInfo->clipHeightCr[i] = (i<0) ? 0 : ((i>tmp) ? tmp : i);

	clipInfo->clipWidth += CLIP_H_W_OFFSET;
	tmp = sizeInfo->width-1;
	for (i = -CLIP_H_W_OFFSET; i < sizeInfo->width + CLIP_H_W_OFFSET; i++)
		clipInfo->clipWidth[i] = (i<0) ? 0 : ((i>tmp) ? tmp : i);

	clipInfo->clipWidthCr += CLIP_H_W_OFFSET;
	tmp = sizeInfo->width_cr-1;
	for (i = -CLIP_H_W_OFFSET; i < sizeInfo->width_cr + CLIP_H_W_OFFSET; i++)
		clipInfo->clipWidthCr[i] = (i<0) ? 0 : ((i>tmp) ? tmp : i);
#ifdef WMMX
	img->itransTemCoeff = (avdInt16 *)voH264AlignedMalloc(img,120,sizeof(quotiety));
	memcpy(img->itransTemCoeff,quotiety,sizeof(quotiety));
#endif//WMMX
#if (DUMP_VERSION & DUMP_MEMORY)
	AvdLog(DUMP_MEMORY, "\n clipArray = %d, total = %d", 
		_totalMemory - memUsed, _totalMemory);
#endif //(DUMP_VERSION & DUMP_MEMORY)
}

void DeInitClipArrs(ImageParameters *img)
{
	TCLIPInfo	*clipInfo = img->clipInfo;
	voH264AlignedFree(img,clipInfo->clip255      - CLIP255_OFFSET);
	clipInfo->clip255 = NULL;
	clipInfo->clipHeight = clipInfo->clipHeightCr = NULL;
	clipInfo->clipWidth = clipInfo->clipWidthCr = NULL;
	clipInfo->clipArrHeight= 0;
#ifdef WMMX
	SafevoH264AlignedFree(img,img->itransTemCoeff);
#endif//WMMX

}

/*!
 ************************************************************************
 * \brief
 *    Reports the gathered information to appropriate outputs
 *
 * \par Input:
 *    struct inp_par *inp,
 *    struct img_par *img,
 *    struct snr_par *stat
 *
 * \par Output:
 *    None
 ************************************************************************
 */


/*!
 ************************************************************************
 * \brief
 *    Allocates a stand-alone partition structure.  Structure should
 *    be freed by FreePartition();
 *    data structures
 *
 * \par Input:
 *    n: number of partitions in the array
 * \par return
 *    pointer to DataPartition Structure, zero-initialized
 ************************************************************************
 */

DataPartition *AllocPartition(ImageParameters *img,int n)
{
	DataPartition *partArr, *dataPart;
	avdUInt8 *currPt;
	avdNativeInt i, sizeTotal, sizePart;
#if (DUMP_VERSION & DUMP_MEMORY)
	int memUsed = _totalMemory;
#endif //(DUMP_VERSION & DUMP_MEMORY)

	sizePart = n * sizeof(DataPartition);
	sizeTotal = sizePart + n * sizeof(Bitstream);
	partArr = (DataPartition *) voH264AlignedMalloc(img,18, sizeTotal);CHECK_ERR_RET_NULL
	if (partArr == NULL)
	{
		//char errortext[ET_SIZE];
		//snprintf(errortext, ET_SIZE, "AllocPartition: Memory allocation for Data Partition failed");
		AVD_ERROR_CHECK2(img,"AllocPartition: Memory allocation for Data Partition failed", 100);
	}

	currPt = (avdUInt8 *)partArr + sizePart;
	for (i=0; i<n; i++) // loop over all data partitions
	{
		dataPart = &(partArr[i]);
		dataPart->bitstream = (Bitstream *)currPt;
		currPt += sizeof(Bitstream);
	}
#if (DUMP_VERSION & DUMP_MEMORY)
	AvdLog(DUMP_MEMORY, "\n AllocPartition = %d, total = %d", 
		_totalMemory - memUsed, _totalMemory);
#endif //(DUMP_VERSION & DUMP_MEMORY)
	return partArr;
}




/*!
 ************************************************************************
 * \brief
 *    Frees a partition structure (array).  
 *
 * \par Input:
 *    Partition to be freed, size of partition Array (Number of Partitions)
 *
 * \par return
 *    None
 *
 * \note
 *    n must be the same as for the corresponding call of AllocPartition
 ************************************************************************
 */


void FreePartition (ImageParameters *img,DataPartition *dp, avdNativeInt n)
{


	if(dp!=NULL&&dp->bitstream != NULL);
		SafevoH264AlignedFree(img,dp);
}


/*!
 ************************************************************************
 * \brief
 *    Allocates the slice structure along with its dependent
 *    data structures
 *
 * \par Input:
 *    Input Parameters struct inp_par *inp,  struct img_par *img
 ************************************************************************
 */
void malloc_slice(ImageParameters *img)
{
	Slice *currSlice;
	avdNativeInt n;
#if (DUMP_VERSION & DUMP_MEMORY)
	int memUsed = _totalMemory;
#endif //(DUMP_VERSION & DUMP_MEMORY)

	img->currentSlice = (Slice *) voH264AlignedMalloc(img,20, sizeof(Slice) + sizeof (NALU_t));CHECK_ERR_RET_VOID
#ifdef USE_JOBPOOL
       for(n =0 ; n < SLICELIST_NUM; n++){
	   	img->SliceList[n] = (Slice *) voH264AlignedMalloc(img,20, sizeof(Slice) + sizeof (NALU_t));CHECK_ERR_RET_VOID
       }
#endif
	if ( (currSlice = img->currentSlice) == NULL)
	{
		char errortext[ET_SIZE];
		snprintf(errortext, ET_SIZE, "Memory allocation for Slice datastruct in NAL-mode");
		AVD_ERROR_CHECK2(img,errortext,100);
	}

	currSlice->nalu = (NALU_t *)((avdUInt8 *)img->currentSlice + sizeof(Slice));

#if FEATURE_CABAC
	//  img->currentSlice->rmpni_buffer=NULL;
	//! you don't know whether we do CABAC hre, hence initialize CABAC anyway
	// if (inp->symbol_mode == CABAC)
	if (1)
	{
		// create all context models
		currSlice->mot_ctx = create_contexts_MotionInfo(img);
		currSlice->tex_ctx = create_contexts_TextureInfo(img);
	}
#endif//VOI_H264D_NON_BASELINE
#if FEATURE_BFRAME

	n = 3;
#else//FEATURE_BFRAME
	n = 1;
#endif//FEATURE_BFRAME



	//! assume data partitioning (worst case) for the following mallocs()
	if ((currSlice->partArr = AllocPartition(img,n)) == NULL)
		no_mem_exit ("AllocPartition");
   CHECK_ERR_RET_VOID
#if (DUMP_VERSION & DUMP_MEMORY)
	AvdLog(DUMP_MEMORY, "\n malloc_slice = %d, total = %d", 
		_totalMemory - memUsed, _totalMemory);
#endif //(DUMP_VERSION & DUMP_MEMORY)
}


/*!
 ************************************************************************
 * \brief
 *    Memory frees of the Slice structure and of its dependent
 *    data structures
 *
 * \par Input:
 *    Input Parameters struct inp_par *inp,  struct img_par *img
 ************************************************************************
 */
void free_slice(ImageParameters *img,Slice * currSlice)
{
	avdNativeInt n=0;
	//Slice *currSlice = img->currentSlice;
#if FEATURE_CABAC
	if (1)
	{
		// delete all context models
		delete_contexts_MotionInfo(img,currSlice->mot_ctx);
		delete_contexts_TextureInfo(img,currSlice->tex_ctx);
		currSlice->mot_ctx = NULL;
		currSlice->tex_ctx = NULL;
	}
#endif//VOI_H264D_NON_BASELINE
#ifdef USE_JOBPOOL
       for(n =0 ; n < SLICELIST_NUM; n++)
       {
	   	voH264AlignedFree(img,img->SliceList[n]);
       }
#endif		
	FreePartition(img,currSlice->partArr, n);
	currSlice->partArr = NULL;
	voH264AlignedFree(img,currSlice);

}

/*!
 ************************************************************************
 * \brief
 *    Dynamic memory allocation of frame size related global buffers
 *    buffers are defined in global.h, allocated memory must be freed in
 *    void free_global_buffers()
 *
 *  \par Input:
 *    Input Parameters struct inp_par *inp, Image Parameters struct img_par *img
 *
 *  \par Output:
 *     Number of allocated bytes
 ***********************************************************************
 */
avdInt32 init_global_buffers(ImageParameters *img)
{
	avdNativeInt mbH, mbW, sizeTmp,i, yOff, uvOff, yJump;
	avdInt32 memory_size=0;
	TMBBitStream	*mbBits;
	TSizeInfo	*sizeInfo = img->sizeInfo;


	mbW = (sizeInfo->width>>4);
	mbH = (sizeInfo->height>>4);
#if FEATURE_INTERLACE
	if(!img->active_sps->frame_mbs_only_flag)
	{
		int size = mbH*mbW;
		img->fieldBitFlags = voH264AlignedMalloc(img,3005,size);
	}
#endif//FEATURE_INTERLACE

#ifdef USE_JOBPOOL
          InitJobPool(img);
#endif
	
	Alloc_Neighbor_Info(img);

	return (memory_size);
}
#ifdef TRACE_MEM

void TestMem(ImageParameters *img,int id,int flag)
{
#define TEST_SIZE 16
	char* tmp = malloc(TEST_SIZE+PADDING_LEN);
	trace_new(img,id,tmp,TEST_SIZE+PADDING_LEN,0);
	switch (flag)
	{
	case VOMR_LEAK:
		break;
	case VOMR_MULTIPLE_FREE:
		free(tmp);
		trace_delete(img,tmp);
		free(tmp);
		trace_delete(img,tmp);
		break;
	case VOMR_OVERWRITE:
		tmp[TEST_SIZE]=1;
		free(tmp);
		trace_delete(img,tmp);
		break;
	}
	
}
void trace_new(ImageParameters *img,int id,int address,int size,int offset)
{
	VOMemTableItem* item;
	int i,ItemSize = img->ItemSize;
	avdUInt8* pad=((avdUInt8*)address+size-PADDING_LEN);
	pad[0]=pad[1]=pad[2]=pad[3]=PADDING_VALUE;
	if(ItemSize>MAX_MEM_ITEM)
	{
		AvdLog2(LL_INFO,"error:trace_new, too many itemsize=%d\n",img->ItemSize);
		return;
	}
	//it is possible that the freed buffer is re-allocate to the new buf,
	//so check if there is duplicated address
	
	for (i=0;i<ItemSize;i++)
	{
		VOMemTableItem* item = &(img->voMemTable[i]);
		if(item->address==address)
		{
			AvdLog2(LL_INFO,"!!!there is duplicated address %d:id=%d,address=Ox%X,free=%d,size=%d,offset=%d,overflow=%d\n",
				i,item->id,item->address,item->isAlloc,item->size,item->offset,item->isOverFlow);
			item->address+=1;
			break;
		}
	}
	item = &(img->voMemTable[img->ItemSize]);
	item->id	  = id;
	item->address = address;
	item->isAlloc = 1;
	item->size = size;
	item->offset = offset;
	item->isOverFlow = 0;
	img->ItemSize++;
}
static void trace_overflow_item(ImageParameters *img,VOMemTableItem* item)
{
	int i = item->id;
	avdUInt8* pad=((avdUInt8*)item->address+item->size-PADDING_LEN);
	if(item->isAlloc==1)
	{
		item->isOverFlow = (pad[0]!=PADDING_VALUE)||(pad[1]!=PADDING_VALUE)||(pad[2]!=PADDING_VALUE)||(pad[3]!=PADDING_VALUE);
		if(item->isOverFlow)
		AvdLog2(LL_INFO,"!!!overflow %d:id=%d,address=Ox%X,free=%d,size=%d,offset=%d,overflow=%d\n",
			i,item->id,item->address,item->isAlloc,item->size,item->offset,item->isOverFlow);
	}
}
void trace_overflow(ImageParameters *img)
{
	int i;
	int ItemSize=img->ItemSize;
	for (i=0;i<ItemSize;i++)
	{
		trace_overflow_item(img,&(img->voMemTable[i]));
	}
}
void trace_delete(ImageParameters *img,int address)
{
	int i;
	int ItemSize=img->ItemSize;
	for (i=0;i<ItemSize;i++)
	{
		VOMemTableItem* item = &(img->voMemTable[i]);
		if(item->address==address)
		{
			trace_overflow_item(img,item);
			item->isAlloc--;
			break;
		}
	}
}

void trace_memIssues(ImageParameters *img)
{
	char fileName[128];
	FILE* file=getMemFile();
	if(file)
	{
		int i;
		int ItemSize=img->ItemSize;
		printf("item size=%d\n",ItemSize);
		for (i=0;i<ItemSize;i++)
		{
			VOMemTableItem* item = &(img->voMemTable[i]);
			if(1)//item->isAlloc)
			{
				sprintf(fileName,"%d:id=%d,address=Ox%X,free=%d,size=%d,offset=%d,overflow=%d\n",
					i,item->id,item->address,item->isAlloc,item->size,item->offset,item->isOverFlow);
				fprintf(file,fileName);
				printf(fileName);
				if(item->isAlloc||item->isOverFlow)
				{
					AvdLog2(LL_INFO,"!!!there is memory issue:\n%s\n",fileName);
				}
			}
		}
		
	}
	else
	{
		printf("error:memfile is null\n");
	}
}
#endif//TRACE_MEM
/*!
 ************************************************************************
 * \brief
 *    Free allocated memory of frame size related global buffers
 *    buffers are defined in global.h, allocated memory is allocated in
 *    int init_global_buffers()
 *
 * \par Input:
 *    Input Parameters struct inp_par *inp, Image Parameters struct img_par *img
 *
 * \par Output:
 *    none
 *
 ************************************************************************
 */
void free_global_buffers(ImageParameters *img)
{
	avdNativeUInt yJump;


	
#if FEATURE_WEIGHTEDPRED  
	{
		TWeightInfo* weight = img->weightInfo;
		if (weight)
		{	
			SafevoH264AlignedFree(img,weight);
		}
		
	}
	
#endif //VOI_H264D_NON_BASELINE   
#if FEATURE_INTERLACE
	if(img->fieldBitFlags)
	{
		SafevoH264AlignedFree(img,img->fieldBitFlags);
	}
#endif//FEATURE_INTERLACE	
	Free_Neighbor_Info(img);

#ifdef USE_JOBPOOL
       ExitJobPool(img);
#endif

	free_dpb(img);

}



