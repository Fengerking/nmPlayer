#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/system_properties.h>

#define LOG_TAG "AMCWrap"

//#define __DEBUG

#include "AMCWrap.h"
#include "fAudioHeadDataInfo.h"
#include "JCMediaFormat.h"
#include "JCBufferInfo.h"
#include "JCMediaCodec.h"
#include "JCMediaCrypto.h"
#include "JCCryptoInfo.h"
#include "JCUUID.h"
#include "DRMToolUtility.h"
#include "base64.h"
#include "voSource.h"
#include "JCMediaCodecList.h"
#include "JCMediaCodecInfo.h"
#include "voAMediaCodec.h"




#define AVC_MIME               "video/avc"
#define MPEG4_MIME             "video/mp4v-es"
#define VC1_MIME               "video/vc1"

#define AAC_MIME               "audio/mp4a-latm"
#define MP3_MIME               "audio/mpeg"
#define WMA_MIME               "audio/x-ms-wma"  
#define AC3_MIME               "audio/ac3"
#define EAC3_MIME              "audio/eac3"






// refer to JAVA MediaCodec 
const int BUFFER_FLAG_CODEC_CONFIG   = 2;
const int BUFFER_FLAG_END_OF_STREAM  = 4;
const int BUFFER_FLAG_SYNC_FRAME     = 1;



const int CRYPTO_MODE_AES_CTR        = 1;
const int CRYPTO_MODE_UNENCRYPTED    = 0;


const int INFO_OUTPUT_BUFFERS_CHANGED = -3;
const int INFO_OUTPUT_FORMAT_CHANGED  = -2;
const int INFO_TRY_AGAIN_LATER        = -1;

const int VIDEO_SCALING_MODE_SCALE_TO_FIT = 1;
const int VIDEO_SCALING_MODE_SCALE_TO_FIT_WITH_CROPPING = 2;


AMCWrap::AMCWrap(VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData):mType(vType),
																				mAudioType(VO_AUDIO_CodingUnused),
																				mUserData(pUserData),
																			    mWidth(0),
																				mHeight(0),
																				mSampleRate(0),
																				mChannels(0),
																				mSampleBits(0),
																				mHeadData(NULL),
																				mHeadLen (0),
																				mJavaVM(NULL),
																				mSurface(NULL),
																				mLocalSur(NULL),
																				mEnv(NULL),
																				mEnvUtil(NULL),
																				mCodec(NULL),
																				mFormat(NULL),
																				mCrypto(NULL),
																				mInBuffers(NULL),
																				mOutBuffers(NULL),
																				mInNums(0),
																				mOutNums(0),
																				mIsCrypto(false),
																				mSetHeader(false),
																				mBufInfo(NULL),
																				mIsVideo(true),
																				mDumpInput(false),
																				mDumpOutput(false),
																				mInputIndex(0),
																				mDFd(-1),
																				mAPreIndex(-1),
																				mVideoParser(NULL),
																				mAspectRatio(-1),
																				mUUID(NULL),
																				mDrmInitData(NULL),
																				mDrmInitLen(0),
																				mCryptoInfo(NULL),
																				mIsInvertIV(false),
																				mSecureComName(NULL),
																				mNeedCallFlush(false),
																				mAFDGet(-1),
																				mAFDSend(-1),
																				mIsAmazon(false),
																				mSMode(VIDEO_SCALING_MODE_SCALE_TO_FIT),
																				mVFDGet(-1)
																			

{
	onCreate();
	if(access("/sdcard/vodump/amedia_dump_input.txt", F_OK) == 0)
	{
		__D("FLYING\n");
		mDumpInput = true;
	}
	if(access("/sdcard/vodump/amedia_dump_output.txt", F_OK) == 0)
	{
	   mDumpOutput = true;
	   const char* pName = "/sdcard/vodump/amedia/output/raw.yuv" ;
	   if(mDFd == -1)
	   {
		  mDFd = open(pName, O_CREAT|O_RDWR|O_TRUNC, 0644);
	   }
	   
	}
	
	if(access("/sdcard/vodump/amedia_video_dump_whole_get.txt", F_OK) == 0)
	{
		const char* pName = "/sdcard/vodump/amedia/input/video_whole_get.dat";
		if(mAFDGet == -1)
		{
			mVFDGet = open(pName, O_CREAT|O_RDWR|O_TRUNC, 0644);
			if(mVFDGet == -1)
			{
				__D("Can not open the pName = %s the reason is %s\n", pName, strerror(errno));
			}
		}
	}
	memset(&mVideoParserAPI, 0, sizeof(mVideoParserAPI));

	setIsInvertIV();
	mReadIndex = 1;

}

AMCWrap::AMCWrap(VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData):mType(VO_VIDEO_CodingUnused),
																				mAudioType(vType),
																				mUserData(pUserData),
																			    mWidth(0),
																				mHeight(0),
																				mSampleRate(0),
																				mChannels(0),
																				mSampleBits(0),
																				mHeadData(NULL),
																				mHeadLen (0),
																				mJavaVM(NULL),
																				mSurface(NULL),
																			    mLocalSur(NULL),
																				mEnv(NULL),
																				mEnvUtil(NULL),
																				mCodec(NULL),
																				mFormat(NULL),
																				mCrypto(NULL),
																				mInBuffers(NULL),
																				mOutBuffers(NULL),
																				mInNums(0),
																				mOutNums(0),
																				mIsCrypto(false),
																				mSetHeader(false),
																				mBufInfo(NULL),
																				mIsVideo(false),
																				mDumpInput(false),
																				mDumpOutput(false),
																				mInputIndex(0),
																				mDFd(-1),
																				mAPreIndex(-1),
																				mVideoParser(NULL),
																				mAspectRatio(-1),
																				mUUID(NULL),
																				mDrmInitData(NULL),
																				mDrmInitLen(0),
																				mCryptoInfo(NULL),
																				mIsInvertIV(false),
																				mSecureComName(NULL),
																				mNeedCallFlush(false),
																				mAFDGet(-1),
																				mAFDSend(-1),
																				mIsAmazon(false),
																				mSMode(VIDEO_SCALING_MODE_SCALE_TO_FIT),
																				mVFDGet(-1)
{
	onCreate();
	if(access("/sdcard/vodump/amedia_audio_dump_input.txt", F_OK) == 0)
	{
		mDumpInput = true;
	}
	if(access("/sdcard/vodump/amedia_audio_dump_output.txt", F_OK) == 0)
	{
	   mDumpOutput = true;
	   const char* pName = "/sdcard/vodump/amedia/output/raw.pcm" ;
	   if(mDFd == -1)
	   {
		  mDFd = open(pName, O_CREAT|O_RDWR|O_TRUNC, 0644);
	   }	   
	}

	if(access("/sdcard/vodump/amedia_audio_dump_whole_get.txt", F_OK) == 0)
	{
		const char* pName = "/sdcard/vodump/amedia/input/audio_whole_get.dat";
		if(mAFDGet == -1)
		{
			mAFDGet = open(pName, O_CREAT|O_RDWR|O_TRUNC, 0644);
			if(mAFDGet == -1)
			{
				__D("Can not open the pName = %s the reason is %s\n", pName, strerror(errno));
			}
		}
	}
	
	if(access("/sdcard/vodump/amedia_audio_dump_whole_send.txt", F_OK) == 0)
	{
		const char* pName = "/sdcard/vodump/amedia/input/audio_whole_send.dat";
		if(mAFDSend == -1)
		{
			mAFDSend = open(pName, O_CREAT|O_RDWR|O_TRUNC, 0644);
			__D("mAFDSend = %d\n", mAFDSend);
			if(mAFDSend == -1)
			{
				__D("Can not open the pName = %s the reason is %s\n",pName,  strerror(errno));
			}
		}
	}
	memset(&mVideoParserAPI, 0, sizeof(mVideoParserAPI));

	setIsInvertIV();
	mReadIndex = 1;	
}

AMCWrap::~AMCWrap()
{
	__D("Destroy mIsVideo = %d\n", mIsVideo);
	onDestroy();
	if(mDFd != -1)
	{
		close(mDFd );
		mDFd = -1;
	}
	if(mVideoParser && mVideoParserAPI.Uninit != NULL)
	{
		mVideoParserAPI.Uninit(mVideoParser);
		mVideoParser = NULL;
	}
	if(mAFDGet != -1)
	{
		close(mAFDGet);
		mAFDGet = -1;
	}
	if(mAFDSend != -1)
	{
		close(mAFDSend);
		mAFDSend = -1;
	}
	if(mVFDGet != -1)
	{
		close(mVFDGet);
		mVFDGet = -1;
	}
}


VO_U32  AMCWrap::setInputData(VO_CODECBUFFER * pInput, VO_BOOL bEOS)
{
	if(pInput->Length ==0 && bEOS == VO_FALSE)
		return  VO_ERR_NONE;
	
	/*
	// only for test Galaxy Nexus that can not play the Amazon's aiv player 
	if(!mNeedCallFlush && mIsVideo && pInput->Length == 32)
	{
		return VO_ERR_NONE;
	}
	*/
	
	// fix the issue that make the play crash when can not get drm instance 
	if(mIsCrypto && NULL == mCrypto)
		return VO_ERR_WRONG_STATUS;
	
	if(!mCodec)
	{
		return VO_ERR_CODEC_UNSUPPORTED;
	}
	assert(mCodec != NULL);
	
	// added for fixing the issue when pause->suspend->resume  crash issue
	switchJavaEnv();
	
	char cName[256];
	if(mDumpInput && !mIsCrypto)
	{
		if(mIsVideo)
			sprintf(cName, "/sdcard/vodump/amedia/input/video_%04d", mInputIndex);
		else
			sprintf(cName, "/sdcard/vodump/amedia/input/audio_%04d", mInputIndex);
		int nFd = open(cName, O_CREAT|O_RDWR, 0644);
		if(nFd > 0)
		{
			write(nFd, pInput->Buffer, pInput->Length);
			close(nFd);
			mInputIndex ++;
		}
		
	}
	
	__D("before call mCodec->dequeueInputBuffer(-1);\n");
	__D("THREAD  id = %d mCodec = %p\n", pthread_self(), mCodec);
   int	index = mCodec->dequeueInputBuffer(10000);
   //
   SampleEncryption *pEncryptionInfo = NULL;
   
   //__D("after call mCodec->dequeueInputBuffer(-1);\n");
   __D("FLYING index = %d\n", index);
	if( index < 0)
	{
		if(index == -100)
			return VO_ERR_FAILED;

		__D(" Maybe wait time is short index = %d\n mIsVideo = %d", index, mIsVideo);
		if(access(cName, F_OK) == 0)
		{
			remove(cName);
			mInputIndex --;
		}
		return VO_ERR_IOMXDEC_NeedRetry;
		
	}

	__D("WHY_WHY_WHY mSetHeader = %d mHeadData = %p, mHeadLen = %d\n", mSetHeader, mHeadData, mHeadLen);
	if(!mSetHeader && mHeadData && mHeadLen > 0 && index >= 0)
	{
		jobject obj = mEnv->GetObjectArrayElement(mInBuffers, index);
		assert(obj != NULL);
		void* pAddr = mEnv->GetDirectBufferAddress(obj);
		int nLen = mEnv->GetDirectBufferCapacity(obj);
		//__D("SPEAK pAddr = %p, nLen = %d\n", pAddr, nLen);
		
		memcpy(pAddr, mHeadData, mHeadLen);
		//__D("SPEAK mHeadData = %p, mHeadLen = %d\n", mHeadData, mHeadLen);
		// it should not different whether mIsCrypto ???????
		if(!mIsCrypto)
		{
			mCodec->queueInputBuffer(index, 0, mHeadLen, 0, BUFFER_FLAG_CODEC_CONFIG);
		}
		else
		{
			__D("SPEAK mCryptoInfo = %p\n", mCryptoInfo);
			assert(mCryptoInfo != NULL);
			__D("mCryptoInfo = %p\n", mCryptoInfo);
			unsigned char ucKey[16] ={0};
			unsigned char cIV[16] = {0};
			bool bOk = false;
			
			int nClears[1], nEncryteds[1];
			nClears[0] = mHeadLen;
			nEncryteds[0] = 0;
			bOk = mCryptoInfo->createCryptoInfo(cIV, 16, ucKey, 16, nClears, nEncryteds, 1,  CRYPTO_MODE_AES_CTR);
			if(bOk)
			{
				__D("before call mCodec->queueSecureInputBuffer, index = %d mIsVideo = %d\n", index, mIsVideo);			
				mCodec->queueSecureInputBuffer(index, 0, *mCryptoInfo, 0, BUFFER_FLAG_CODEC_CONFIG);
				__D("after call mCodec->queueSecureInputBuffer\n");
			}
		}
		
		if(obj)
		{
			mEnv->DeleteLocalRef(obj);
			obj = NULL;
		}
		mSetHeader = true;
		index = mCodec->dequeueInputBuffer(-1);
		__D("HEADER index = %d\n", index);
	}
	if(index >= 0 && index < mInNums)
	{
		jobject obj = mEnv->GetObjectArrayElement(mInBuffers, index);
		assert(obj != NULL);
		void* pAddr = mEnv->GetDirectBufferAddress(obj);
		int nLen = mEnv->GetDirectBufferCapacity(obj);
		VO_BYTE* pData = NULL;
		VO_U32   nDataLen = 0;
		VO_CODECBUFFER sBuffer;
		/*
		__D("pAddr = %p, nLen = %d\n", pAddr, nLen);
		__D("pInput->Length = %d\n", pInput->Length);
		*/
		if(nLen > pInput->Length)
		{
			long long timeUs = pInput->Time * 1000;
			int nOffset = 0;
			if(!mIsCrypto)
			{
				if(!mIsVideo)
				{
					// here I dump the whole frame that I get from the source module
					if(mAFDGet != -1)
					{
						write(mAFDGet, pInput->Buffer, pInput->Length);
					}
					if(VO_AUDIO_CodingAAC == mAudioType)
						nOffset= voAACRAWDataOffset(pInput);			
					__D("AUDIO_ADTS nOffset = %d\n", nOffset);
				}
				
				if(pInput->Length > 2 && !mIsVideo)
				{
					if(mAFDSend != -1)
					{
						// here I dump the whole frame that I send to the decoder 
						write(mAFDSend, pInput->Buffer+nOffset, pInput->Length-nOffset);
					}
					memcpy(pAddr, pInput->Buffer+nOffset, pInput->Length -nOffset);
				}
				else
				{
					if(mVFDGet != -1)
						write(mVFDGet, pInput->Buffer, pInput->Length);
					
					memcpy(pAddr, pInput->Buffer, pInput->Length);
				}
#if 0			
				if(mDumpInput && !mIsCrypto)
				{
					void* pAddr = mEnv->GetDirectBufferAddress(obj);
					int nLen = mEnv->GetDirectBufferCapacity(obj);
					__D("After pAddr = %p nLen = %d\n", pAddr, pInput->Length); 
					char cName[256];
					int index = mInputIndex-1;
					if(mIsVideo)
						sprintf(cName, "/sdcard/vodump/amedia/input/video_after_%04d", index);
					else
						sprintf(cName, "/sdcard/vodump/amedia/input/audio_after_%04d", index);
					int nFd = open(cName, O_CREAT|O_RDWR, 0644);
					if(nFd > 0)
					{
						write(nFd, (char*)pAddr, pInput->Length);
						close(nFd);
					}
				
				}
#endif				
			}
			else
			{		
				departWithSampleEncryption(pInput->Buffer, pInput->Length, &pData, &nDataLen, &pEncryptionInfo);
				__D("pData = %p, nDataLen = %d\n", pData, nDataLen);

				if(mDumpInput && mIsCrypto)
				{
					__D("FLYING\n");
					if(mIsVideo)
						sprintf(cName, "/sdcard/vodump/amedia/input/video_crypto_%04d", mInputIndex);
					else
						sprintf(cName, "/sdcard/vodump/amedia/input/audio_crypto_%04d", mInputIndex);
					
					int nFd = open(cName, O_CREAT|O_RDWR, 0644);
					if(nFd > 0)
					{
						write(nFd, pData, nDataLen);
						close(nFd);
						mInputIndex ++;
					}
					else
					{
						__D("can not create cName = %s the reason is %s\n", cName, strerror(errno));
					}
		
				}
				sBuffer.Length = nDataLen;
				sBuffer.Buffer = pData;

				
				if(!mIsVideo)
				{
					if(VO_AUDIO_CodingAAC == mAudioType)
						nOffset= voAACRAWDataOffset(&sBuffer);
				}
				//__D("AUDIO_ADTS nOffset = %d\n", nOffset);
				if(sBuffer.Length > 2 && !mIsVideo)
					memcpy(pAddr, sBuffer.Buffer+nOffset, sBuffer.Length -nOffset);
				else
				{
					if (VO_VIDEO_CodingH264 == mType && mIsVideo)
					{
						__D("convertAVC2H264");
						convertAVC2H264(sBuffer.Buffer, sBuffer.Length);
					}
					
					memcpy(pAddr, sBuffer.Buffer, sBuffer.Length);
			
				}
			}
		
			if(pInput->Length == mHeadLen)
			{	
				
				mCodec->queueInputBuffer(index, 0, pInput->Length, timeUs, BUFFER_FLAG_CODEC_CONFIG);
			}
			else if(pInput->Length == 0)
			{
				//if(!mIsCrypto)
				{
					//__D("before call the queueInputBuffer with index %d length = %d\n", index, pInput->Length);
					mCodec->queueInputBuffer(index, 0, pInput->Length, timeUs, BUFFER_FLAG_END_OF_STREAM);
				}
				/*
				else
				{
					
				}
				*/
			}
			else
			{
				if(!mIsCrypto)
				{
					//__D("before call the queueInputBuffer(index, 0, pInput->Length, timeUs, 0)\n");
					if(pInput->Length > 2 && !mIsVideo)
						mCodec->queueInputBuffer(index, 0, pInput->Length -nOffset, timeUs, 0);
					else
						mCodec->queueInputBuffer(index, 0, pInput->Length, timeUs, 0);
					mNeedCallFlush = true;
				}
				else
				{
					
					__D("before call the queueSecureInputBuffer(index, 0, *mCryptoInfo, timeUs, 0)\n");
					
					SampleEncryption* pInfo = (SampleEncryption*)pEncryptionInfo;
					if(pInfo == NULL)
					{
						VOLOGE("The DRM EncryptionInfo is NULL\n");
						return VO_ERR_WRONG_STATUS;
					}
					__D("pInfo->hNumberOfEntries = %d\n", pInfo->hNumberOfEntries);
					int* pClears = NULL;
					int* pEncryteds = NULL;
					int nItems = 0;
					if(pInfo->hNumberOfEntries > 0)
					{
						pClears = new int [pInfo->hNumberOfEntries];
						pEncryteds = new int [pInfo->hNumberOfEntries];
						for(int i = 0; i < pInfo->hNumberOfEntries; i++)
						{
							SampleEncryption::SubSampleEncryptionEntry  entry = pInfo->pSubEntries[i];
							pClears[i] = entry.hBytesOfClearData;
							__D("entry.hBytesOfClearData = %d\n", entry.hBytesOfClearData);
							pEncryteds[i] = entry.uBytesOfEncrytedData;
							__D("entry.uBytesOfEncrytedData = %d\n", entry.uBytesOfEncrytedData);
						}
						nItems = pInfo->hNumberOfEntries;
					}
					else
					{
						
						pClears = new int [1];
						pEncryteds = new int [1];
						pClears[0] = 0;
						pEncryteds[0] = nDataLen;
						nItems = 1;
						__D("FAKE pEncryteds[0] = %d\n", pEncryteds[0]);
					}
					assert(mCryptoInfo != NULL);
					__D("mCryptoInfo = %p\n", mCryptoInfo);
					unsigned char ucKey[16] ={0};
					unsigned char cIV[16] = {0};
					assert(pInfo->uSizeIV <= 16);
					bool bOk = false;
					if(mIsInvertIV)
					{
						for(int i = 0; i < pInfo->s_TrackEncryption.uSizeIV; i++)
							cIV[i] = pInfo->IV[pInfo->s_TrackEncryption.uSizeIV-1 -i];
					}
					else
						memcpy(cIV, pInfo->IV, pInfo->s_TrackEncryption.uSizeIV);

					bOk = mCryptoInfo->createCryptoInfo(cIV, 16, ucKey, 16, pClears, pEncryteds,nItems,  CRYPTO_MODE_AES_CTR);
					
					if(bOk)
					{
						__D("before call mCodec->queueSecureInputBuffer, index = %d mIsVideo = %d\n", index, mIsVideo);
						mCodec->queueSecureInputBuffer(index, 0, *mCryptoInfo, timeUs, 0);
						mNeedCallFlush  = true;
						__D("after call mCodec->queueSecureInputBuffer\n");
					}
					delete [] pClears;
					delete [] pEncryteds;
					if(!bOk)
					{
						__D("it should make the player stop\n");
						if(obj)
						{
							mEnv->DeleteLocalRef(obj);
							obj = NULL;
						}
						return VO_ERR_WRONG_STATUS;
					}
				}
				if(obj)
				{
					mEnv->DeleteLocalRef(obj);
					obj = NULL;
				}
			}
			return VO_ERR_NONE;
		}
		else
		{
			VOLOGE("should never come here\n");
		}	
	}
	return VO_ERR_WRONG_STATUS;
}
VO_U32  AMCWrap::getOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	if(!mCodec)
		return VO_ERR_CODEC_UNSUPPORTED;
	assert(mCodec != NULL);
	
	// fix the issue that make the play crash when can not get drm instance 
	if(mIsCrypto && NULL == mCrypto)
		return VO_ERR_WRONG_STATUS;

	switchJavaEnv();
	//__D("THREAD  id = %d\n", pthread_self());
	int index = mCodec->dequeueOutputBuffer(*mBufInfo, 10000);
	
	// The Java MediaCode has the error
	if(index == -100)
	{
		__D("Java MediaCode has the exception\n");
		return VO_ERR_FAILED;
	}
	//__D("index = %d (%x) flags = %x\n", index, mBufInfo->mFlags);
	if(INFO_OUTPUT_BUFFERS_CHANGED == index)
	{
		
		jobjectArray ars = mCodec->getOutputBuffers();
		if(ars)
		{
			if(mOutBuffers)
				mEnv->DeleteGlobalRef(mOutBuffers);
			mOutBuffers = (jobjectArray)mEnv->NewGlobalRef(ars);
			mEnv->DeleteLocalRef(ars);
			ars = NULL;
			mOutNums = mEnv->GetArrayLength(mOutBuffers);
			__D("mOutNums = %d\n", mOutNums);
		}
		return VO_ERR_IOMXDEC_NeedRetry;
	}
	else if(INFO_OUTPUT_FORMAT_CHANGED == index)
	{
		JCMediaFormat* pFormat = mCodec->getOutputFormat();
		
		JCMediaFormat::MFInfo* pInfo = pFormat->getMediaInfo();
		//__D("pInfo->mWidth = %d\n", pInfo->mWidth);
		//__D("pInfo->mHeight = %d \n", pInfo->mHeight);
		//__D("pInfo->mMaxInputSize = %d\n", pInfo->mMaxInputSize);
		//__D("pInfo->mDuration = %l\n", pInfo->mDuration);
		//__D("pInfo->mColorFormat = %d\n", pInfo->mColorFormat);
		pFormat->dump();
		// what should I do????
		return VO_ERR_IOMXDEC_NeedRetry;
	}
	else if(INFO_TRY_AGAIN_LATER == index )
	{
		__D("VO_ERR_IOMXDEC_NeedRetry\n");
		return VO_ERR_IOMXDEC_NeedRetry;
	}
	else if( index >=0 && index < mOutNums)
	{
		mBufInfo->update();
		
		__D("mBufInfo->mFlags = %d\n", mBufInfo->mFlags);
		__D("mBufInfo->mOffset = %d \n", mBufInfo->mOffset);
		__D("mBufInfo->mTimeUs = %lld\n", mBufInfo->mTimeUs);
		__D("mBufInfo->mSize = %d\n", mBufInfo->mSize);
	   
		if(mDumpOutput)
		{
		
			jobject obj = mEnv->GetObjectArrayElement(mOutBuffers, index);
			assert(obj != NULL);
			unsigned char* pAddr =(unsigned char*)mEnv->GetDirectBufferAddress(obj);
			
			int nLen = mEnv->GetDirectBufferCapacity(obj);
			__D("mDfd = %d, mBufInfo->mSize = %d\n", mDFd, mBufInfo->mSize);
			if(mDFd)
			{
				write(mDFd, pAddr, mBufInfo->mSize);
			}
			if(obj)
			{
				mEnv->DeleteLocalRef(obj);
				obj = NULL;
			}
		}
		
		// just a test , should 
		//mCodec->releaseOutputBuffer(index, true);
		
		if(pOutBuffer)
		{
			__D("RENDER index = %d\n", index);
			pOutBuffer->Buffer[0] = (VO_PBYTE)index;
			pOutBuffer->ColorType = (VO_IV_COLORTYPE)VO_ACODEC_COLOR_TYPE;
			pOutBuffer->Time =  mBufInfo->mTimeUs / 1000;
		}
		if(pOutInfo)
		{
			pOutInfo->Format.Width = mWidth;
			pOutInfo->Format.Height = mHeight;
			pOutInfo->Flag = 0;
		}
	}

		
	return VO_ERR_NONE ;
}

VO_U32  AMCWrap::getOutputData(VO_CODECBUFFER* pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	__D("mCodec = %p\n", mCodec);
	if(!mCodec)
		return VO_ERR_CODEC_UNSUPPORTED;
	assert(mCodec != NULL);

	// fix the issue that make the play crash when can not get drm instance 
	if(mIsCrypto && NULL == mCrypto)
		return VO_ERR_WRONG_STATUS;
	
	__D("mIsVideo = %d\n", mIsVideo);
	
	int index = mCodec->dequeueOutputBuffer(*mBufInfo, 10000);
	// The Java MediaCode has the error
	if(index == -100)
	{
		__D("Java MediaCode has the exception\n");
		return VO_ERR_FAILED;
	}
	__D("index = %d (%x) flags = %x\n", index, mBufInfo->mFlags);
	if(INFO_OUTPUT_BUFFERS_CHANGED == index)
	{
		__D();
		jobjectArray ars = mCodec->getOutputBuffers();
		if(ars)
		{
			if(mOutBuffers)
				mEnv->DeleteGlobalRef(mOutBuffers);
			mOutBuffers = (jobjectArray)mEnv->NewGlobalRef(ars);
			mEnv->DeleteLocalRef(ars);
			ars = NULL;
			mOutNums = mEnv->GetArrayLength(mOutBuffers);
			__D("mOutNums = %d\n", mOutNums);
		}
		return VO_ERR_IOMXDEC_NeedRetry;
	}
	else if(INFO_OUTPUT_FORMAT_CHANGED == index)
	{
		__D();
		JCMediaFormat* pFormat = mCodec->getOutputFormat();
		
		JCMediaFormat::MFInfo* pInfo = pFormat->getMediaInfo(false);
		mSampleRate = pInfo->mSampleRate;
		mChannels = pInfo->mChannels;
		
		__D("pInfo->mChannels = %d\n", pInfo->mChannels);
		__D("pInfo->mSampleRate = %d \n", pInfo->mSampleRate);
		
		pFormat->dump();
		if(mIsVideo)
			return VO_ERR_IOMXDEC_NeedRetry;
		return VO_ERR_INPUT_BUFFER_SMALL;
	}
	else if(INFO_TRY_AGAIN_LATER == index )
	{
		//__D("VO_ERR_IOMXDEC_NeedRetry\n");
		if(mIsVideo)
			return VO_ERR_IOMXDEC_NeedRetry;
		return VO_ERR_INPUT_BUFFER_SMALL;
	}
	else if( index >=0 && index < mOutNums)
	{
		
		__D("mBufInfo->mFlags = %d\n", mBufInfo->mFlags);
		__D("mBufInfo->mOffset = %d \n", mBufInfo->mOffset);
		__D("mBufInfo->mTimeUs = %lld\n", mBufInfo->mTimeUs);
		__D("mBufInfo->mSize = %d\n", mBufInfo->mSize);
		
		jobject obj = mEnv->GetObjectArrayElement(mOutBuffers, index);
		assert(obj != NULL);
		unsigned char* pAddr =(unsigned char*)mEnv->GetDirectBufferAddress(obj);
			
		int nLen = mEnv->GetDirectBufferCapacity(obj);
		if(mDumpOutput)
		{				
			__D("mDfd = %d, mBufInfo->mOffset = %d mBufInfo->mSize = %d\n", mDFd,mBufInfo->mOffset, mBufInfo->mSize);
			if(mDFd)
			{
				write(mDFd, pAddr+mBufInfo->mOffset, mBufInfo->mSize);
			}
		}
		
	
		if(pOutBuffer)
		{
			if(mAPreIndex != -1)
				mCodec->releaseOutputBuffer(mAPreIndex, false);
			
			// Here I make a mistake, DO not USE the shallow copy, it should be the DEEP copy
			//pOutBuffer->Buffer = (VO_PBYTE)pAddr;
			memcpy(pOutBuffer->Buffer, pAddr, mBufInfo->mSize);
			pOutBuffer->Length = mBufInfo->mSize;
#if 0
			// with Jack's suggestion, it is no necessary to set this value
			//pOutBuffer->Time =  mBufInfo->mTimeUs / 1000;
#else
			// Without setting this, it causes A/V sync (both system and DSP clock)
			pOutBuffer->Time =  mBufInfo->mTimeUs / 1000;
#endif
			mAPreIndex = index;
		}
		if(pOutInfo)
		{
			pOutInfo->Format.SampleRate = mSampleRate;
			pOutInfo->Format.Channels = mChannels;
			pOutInfo->Format.SampleBits = mSampleBits;
		}
		if(obj)
		{
			mEnv->DeleteLocalRef(obj);
			obj = NULL;
		}
	}
		
	return VO_ERR_NONE ;
}

VO_U32  AMCWrap::setParam(VO_S32 uParamID, VO_PTR pData)
{
	VO_U32 nRC = VO_ERR_NONE;
	
	//__D("WHY uParamID = %x\n", uParamID);
	
	JNIEnv* pEnv = NULL;
	
	if(mThreadId != pthread_self())
		mJavaVM->AttachCurrentThread(&pEnv, NULL);
	
	
	switch(uParamID)
	{
	case VO_ACODEC_SURFACE:
		//mSurface = (jobject)pData;
		mLocalSur = (jobject)pData;
		__D("mJavaVM = %p, mLocalSur = %p\n", mJavaVM, mLocalSur);
		// try to fix the crash issue
		if(mEnv && mLocalSur)
		{
			mSurface = mEnv->NewGlobalRef(mLocalSur);
		}	
		break;
	case VO_ACODEC_JVM:	
		mJavaVM = (JavaVM*)pData;
		if(!mEnvUtil)
		{
			mEnvUtil = new JCEnvUtil(mJavaVM, !mIsVideo);
			__D();
			mEnv = mEnvUtil->getEnv();
		}	
		break;
	case VO_ACODEC_DRM:
		mIsCrypto = *((bool*)pData);
		__D("mIsCrypto = %d\n", mIsCrypto);
		break;
    case VO_ACODEC_SMODE:
		mSMode = *((int*)pData);
		assert(mSMode == 1 || mSMode == 2);
		__D("mSMode = %d\n", mSMode);
		if(mCodec)
		{
			mCodec->setVideoScalingMode(mSMode);
		}
		break;
	case VO_PID_COMMON_HEADDATA:
	{
		__D("WHY_WHY VO_PID_COMMON_HEADDATA\n");
		onSetHeadData(pData);
	}
	break;
	case VO_PID_AUDIO_FORMAT:
	{
		__D("VO_PID_AUDIO_FORMAT\n");
		// since some aac file has no head data, so I have to use it
		VO_AUDIO_FORMAT * pFormat = (VO_AUDIO_FORMAT *)pData;
		mSampleRate = pFormat->SampleRate;
		mChannels   = pFormat->Channels;
		mSampleBits = pFormat->SampleBits;
		__D("mSampleRate = %d\n", mSampleRate);
		__D("mChannels = %d\n", mChannels);
		__D("mSampleBits = %d\n", mSampleBits);
	}
	break;
	
 	case VO_PID_COMMON_FLUSH:
	{
		__D(" THREAD into Flush state id = %d\n", pthread_self());
		
		if(mCodec)
		{
			// since we destroy it and recreate it. But it is indeed necessary to deal with this case again.			
			if(pEnv && mNeedCallFlush)
			{
				mCodec->setEnv(pEnv);
				mCodec ->flush();
				mCodec->setEnv(mEnv);
			}			
		}
		return VO_ERR_NONE;	
	}
	
	break;

	case VO_PID_COMMON_PAUSE:
		__D("into Pause state \n");
		return VO_ERR_NONE;
		
	case VO_PID_COMMON_STOP:
		
		__D("THREAD into Stop state id = %d\n", pthread_self());
		
		/*
		if(mCodec)
		{
			if(pEnv)
				mCodec->setEnv(pEnv);
			mCodec->stop();
		}
		*/
		return VO_ERR_NONE;	
	case VO_ACODEC_RENDER:
		return onRender(pData);
	case VO_ACODEC_DROP:
		return onRender(pData, false);
	case VO_ACODEC_RESET:
		return onReset(pData);
	case VO_ACODEC_EOS:
	{
		// It seems that I draw a snake toes to add these codes. it can cause Nexus 4 loop into ANR when call stop
		/*
		VO_CODECBUFFER sBuffer;
		memset(&sBuffer, 0, sizeof(sBuffer));
		VO_U32 err = setInputData(&sBuffer, VO_TRUE);
		return err;
		*/
	}
	return VO_ERR_NONE;
	}
	onSetParam();
	return VO_ERR_NONE;
}
VO_U32  AMCWrap::getParam(VO_S32 uParamID, VO_PTR pData)
{
	switch (uParamID)
	{
	case VO_ACODEC_LIVE:
		*((VO_BOOL*)pData) = (mCodec!=NULL) ? VO_TRUE:VO_FALSE;		
		break;
	case VO_PID_VIDEO_ASPECTRATIO:
		{
			__D("mAspectRatio = %d\n", mAspectRatio);
			if(-1 != mAspectRatio)
			{
				*((VO_U32 *)pData) = mAspectRatio;
				__D(" mAspectRatio = %d\n",  mAspectRatio);
			}
		}
		break;
	}
	
	return VO_ERR_NONE;
}

void AMCWrap::onSetParam()
{  
	if((mIsVideo && mSurface && mEnv &&  mMime && mWidth > 0 && mHeight > 0 && mCodec == NULL) || (!mIsVideo && mEnv &&  mMime && mChannels > 0 && mSampleRate > 0 && mCodec == NULL))
		createJCData(mIsVideo);
}

void AMCWrap::onCreate()
{
	mThreadId = pthread_self();
	__D("THREAD id = %d\n", mThreadId);
	if(mIsVideo)
	{
		if(VO_VIDEO_CodingH264 == mType)
			mMime = AVC_MIME;
		else if(VO_VIDEO_CodingMPEG4 == mType)
			mMime = MPEG4_MIME;
		else
			mMime = NULL;
	}
	else
	{
		if(VO_AUDIO_CodingAAC == mAudioType)
			mMime = AAC_MIME;
		else if(VO_AUDIO_CodingWMA == mAudioType)
			mMime = WMA_MIME;
		else if(VO_AUDIO_CodingMP3 == mAudioType)
			mMime = MP3_MIME;
		else if(VO_AUDIO_CodingAC3 == mAudioType)
			mMime = AC3_MIME;
		else if(VO_AUDIO_CodingEAC3 == mAudioType)
			mMime = EAC3_MIME;
		else
			mMime = NULL;
	}

	if(mMime != NULL)
	{
		__D("mMime = %s\n", mMime);
	}
}
void AMCWrap::onDestroy()
{
	switchJavaEnv();
	__D();
	if(mHeadData){
		delete [] mHeadData;
		mHeadData = NULL;
	}
	
	if(mFormat){
		delete mFormat;
		mFormat = NULL;
	}
	if(mCrypto){
		mCrypto->release();
		delete mCrypto;
		mCrypto = NULL;
	}
	
	if(mUUID){
		delete mUUID;
		mUUID = NULL;
	}
	if(mBufInfo){
		delete mBufInfo;
		mBufInfo = NULL;
	}
	__D();

	if(mDrmInitData)
	{
		delete [] mDrmInitData;
		mDrmInitData = NULL;
	}
	if(mSecureComName)
	{
		delete []mSecureComName;
		mSecureComName = NULL;
	}
	if(mInBuffers)
	{
		mEnv->DeleteGlobalRef(mInBuffers);
		mInBuffers = NULL;
	}
	if(mOutBuffers)
	{
		mEnv->DeleteGlobalRef(mOutBuffers);
		mOutBuffers = NULL;
	}
	if(mSurface)
	{
		mEnv->DeleteGlobalRef(mSurface);
		mSurface = NULL;
	}
	if(mCodec){
		mCodec->stop();
		mCodec->release();
		__D();
		delete mCodec;
		__D();
		mCodec = NULL;
		__D();
	}
	if(mEnvUtil){
		delete mEnvUtil;
		mEnvUtil = NULL;
	}
	
}
VO_U32 AMCWrap::onRender(void* pData, bool bRender)
{
	assert(mCodec != NULL);
	VO_VIDEO_BUFFER * pBuff = (VO_VIDEO_BUFFER*)pData;
	int index = (int)pBuff->Buffer[0];
	__D("index = %d pBuff->Time = %lld\n", index, pBuff->Time);
	if(index >= 0 && index < mOutNums)
		mCodec->releaseOutputBuffer(index, bRender);
	return VO_ERR_NONE;
	
}
bool AMCWrap::createJCData(const bool isVideo)
{
	__D("isVideo = %d\n", isVideo);
	bool bOk = false;
	if(!mSecureComName)
	{
		mSecureComName = new char[256];
		memset(mSecureComName, 0, 256);
	}
	// Use it to check  the target platform wheter support the pointed mMime, 
	// Otherwise return it directly
	bOk = getSecureCompName(mSecureComName);
	if(!bOk)
	{
		__D("The Target platfrom can not support the mMime = %s\n", mMime);
		return false;
	}

	// deal with the Nexus S issue 
	if(mIsVideo && isInBlackList())
	{
		__D();
		return false;
	}

	__D("SF_NEW_LIVE mSecureComName = %s\n", mSecureComName);		
	assert(mCodec == NULL);
	mCodec = new JCMediaCodec(mEnv);
	if(mCodec->init())
	{
		if(isVideo && mIsCrypto)
			bOk = mCodec->createByCodecName(mSecureComName);
		else
			bOk = mCodec->createDecoderByType(mMime);
		if(bOk)
		{
			assert(mFormat == NULL);
			__D();
			mFormat = new JCMediaFormat(mEnv);
			__D();
			
			bool bInit = mFormat->init();
			if(!bInit)
			{
				onDestroy();
				return false;
			}
			__D();
			if(isVideo)
				mFormat->createVideoFormat(mMime, mWidth, mHeight);
			else
			{
				__D();
				mFormat->createAudioFormat(mMime, mSampleRate, mChannels);
			
				__D("WHY_WHY mHeadData = %p\n", mHeadData);
				__D("WHY_WHY mHeadLen = %d\n", mHeadLen);
				__D("mSampleRate = %d\n", mSampleRate);
				__D("mChannels = %d\n", mChannels);
				__D("mSampleBits = %d\n", mSampleBits);

				if(VO_AUDIO_CodingAAC == mAudioType)
					setAACHeadData();
			}
			__D();
			// at present I only test without crypto 
			if(mIsCrypto)
			{
				bOk = createJCDRMData();
				if(!bOk)
				{
					delete mCrypto;
					mCrypto = NULL;
					return false;
				}
			}
			__D();
			if(NULL == mFormat->getObj())
			{
				onDestroy();
				return false;
			}
			if(isVideo)
			{
				mCodec->configure(*mFormat, mSurface, mCrypto, 0);		
				mCodec->setVideoScalingMode(mSMode);
			}
			else
				mCodec->configure(*mFormat, NULL, mCrypto, 0);
			__D();
			
	
			__D();
			mCodec->start();
			jobjectArray ars = mCodec->getInputBuffers();
			if(ars)
			{
				mInBuffers=(jobjectArray)mEnv->NewGlobalRef(ars);
				mEnv->DeleteLocalRef(ars);
				ars = NULL;
				mInNums = mEnv->GetArrayLength(mInBuffers);
				__D("mInNums = %d mIsVideo = %d\n", mInNums, mIsVideo);
			}
			ars = mCodec->getOutputBuffers();
			if(ars)
			{
				mOutBuffers = (jobjectArray)mEnv->NewGlobalRef(ars);
				mEnv->DeleteLocalRef(ars);
				ars = NULL;
				mOutNums = mEnv->GetArrayLength(mOutBuffers);
				__D("mOutNums = %d mIsVideo = %d\n", mOutNums, mIsVideo);
			}

			if(!mBufInfo)
			{
				__D();
				mBufInfo = new JCBufferInfo(mEnv);
				mBufInfo->init();
				mBufInfo->createObject();
				__D();
			}
		}
		return true;
	}
	return false;
}

void AMCWrap::makeHeadInfo()
{
	VO_AUDIO_HEADDATAINFO hInfo;
	VO_CODECBUFFER inBuf;
	hInfo.nSampleRate = mSampleRate;
	hInfo.nChannels =   mChannels;
	hInfo.nSampleBits = mSampleBits; 
				
	mHeadLen = 2;
	// hardcode for test only
	/*mHeadData[0] = 0x12;
	  mHeadData[1] = 0x10;
	*/
	inBuf.Buffer = mHeadData;
	inBuf.Length = mHeadLen;					
	voAACRAWHeadPack(&inBuf, &hInfo);
	__D("AUDIO_FAKE mHeadData[0] = %x, mHeadData[1] = %x\n", mHeadData[0], mHeadData[1]);
}

bool AMCWrap::createJCDRMData()
{	
	__D();
	bool bOk = false;
	mUUID = new JCUUID(mEnv);
	bOk = mUUID->init();
	// now I hard code it
	mUUID->createFromString("11EF8BA9-79D6-4ACE-A3C8-27DCD51D2111");

	__D();
				
	mCrypto = new JCMediaCrypto(mEnv);
	__D();
	bOk = mCrypto->init();
	
	bOk &= mCrypto->createMediaCrypto(*mUUID, mDrmInitData, mDrmInitLen);
		
	mCryptoInfo = new JCCryptoInfo(mEnv);
	bOk &= mCryptoInfo->init();
	
	__D("bOk = %d\n", bOk);
	return bOk;
}
bool AMCWrap::getSecureCompName(char*& inOutName, bool isEncoder)
{
	bool bOk = false;
	switchJavaEnv();
	JCMediaCodecList cList(mEnv);
	cList.init();
	int nCount = cList.getCodecCount();
	for(int i = 0; i < nCount && !bOk; i++)
	{
		JCMediaCodecInfo* pInfo = cList.getCodecInfoAt(i);
		jstring strName = pInfo->getName();
		const char* pStr = mEnv->GetStringUTFChars(strName, NULL);
		__D("pStr = %s\n", pStr);
		jobjectArray arr = pInfo->getSupportedTypes();
		int nItems = mEnv->GetArrayLength(arr);
		for(int j = 0; j < nItems; j++)
		{
			jstring jtype = (jstring)mEnv->GetObjectArrayElement(arr, j);
			const char* pType = mEnv->GetStringUTFChars(jtype, NULL);
			__D("pType = %s\n", pType);
			if(strcasecmp(pType, mMime) == 0 && (isEncoder == pInfo->isEncoder()))
			{
				assert(inOutName != NULL);
				strcpy(inOutName, pStr);
				__D("inOutName = %s\n", inOutName);
				strcat(inOutName, ".secure");
				__D("inOutName = %s\n", inOutName);
				mEnv->ReleaseStringUTFChars(jtype, pType);
				mEnv->DeleteLocalRef(jtype);
				bOk = true;
				break;
			}
			mEnv->ReleaseStringUTFChars(jtype, pType);
			mEnv->DeleteLocalRef(jtype);
		}
		mEnv->ReleaseStringUTFChars(strName, pStr);
		mEnv->DeleteLocalRef(strName);
		mEnv->DeleteLocalRef(arr);
		delete pInfo;
	}
	return bOk;
}

bool AMCWrap::setIsInvertIV()
{
	char cPlatform[PROP_VALUE_MAX];
	memset(cPlatform, 0, sizeof(cPlatform));
	__system_property_get ("ro.board.platform", cPlatform);

	// since the qcom use msmxxx to define its the platform
	// TI use ompx
	// Nvidira uses tegrax
	//
	__D("cPlatform = %s\n", cPlatform);
	if(strncasecmp(cPlatform, "msm", 3) == 0 )
		mIsInvertIV = true;
	
	// please use the special name to check whether it is amazon device
	// NOTE: No useful names from engineering box yet. Need to update this 
	//       later
	//LOGI("%s::%d: cPlatform is %s", __FUNCTION__, __LINE__, cPlatform);
	if (__system_property_find("ro.build.lab126.build") != NULL) {
	    if(strncasecmp(cPlatform, "msm8960", 3) == 0) {
	        char cName[PROP_VALUE_MAX] = {0}; 
	        char cDevice[PROP_VALUE_MAX] = {0}; 
	        char cManufacturer[PROP_VALUE_MAX] = {0}; 
	        char cModel[PROP_VALUE_MAX] = {0};
	        __system_property_get("ro.product.name", cName);
	        __system_property_get("ro.product.device", cDevice);
	        __system_property_get("ro.product.manufacturer", cManufacturer);
	        __system_property_get("ro.product.model", cModel);

			//LOGI("%s::%d: cModel is %s", __FUNCTION__, __LINE__, cModel);
			//LOGI("%s::%d: cName is %s", __FUNCTION__, __LINE__, cName);
	        mIsAmazon = ((strncasecmp(cName, "bueller", 7) == 0) && 
	                     (strncasecmp(cDevice, "bueller", 7) == 0) &&
                             (strncasecmp(cManufacturer, "Amazon", 6) == 0));
                if (mIsAmazon == true) { // This looks like an Amazon box, check model name
                    // Amazon only guarantees first 4 bytes of new model name to be contant
                    mIsAmazon &= ((strncasecmp(cModel, "bueller", 7) == 0) || // old model name
                                  (strncasecmp(cModel, "AFTB", 4) == 0));     // new model name
                }
		}
	}
        //LOGI("%s::%d: mIsAmazon is %d", __FUNCTION__, __LINE__, mIsAmazon);
	return true;
}

void AMCWrap::convertAVC2H264(unsigned char* pData, unsigned uSize)
{
	VO_PBYTE pOutData = pData;
	VO_PBYTE pInData = pData;
	VO_U32 nInSize = uSize;

	if (pOutData == NULL)
		return;

	VO_PBYTE pBuffer = pInData;
	VO_U32	 nFrameLen = 0;
	VO_U32 nNalLen = 4;
	VO_U32 nNalWord = 0X01000000;
	VO_U32 nVideoSize = 0;

	int i = 0;
	while (pBuffer - pInData + nNalLen < nInSize)
	{
		nFrameLen = *pBuffer++;
		for (i = 0; i < (int)nNalLen - 1; i++)
		{
			nFrameLen = nFrameLen << 8;
			nFrameLen += *pBuffer++;
		}

		if(nFrameLen > nInSize)
			return;

		if (nNalLen == 3 || nNalLen == 4)
		{
			memcpy ((pBuffer - nNalLen), &nNalWord, nNalLen);
		}
		else
		{
			memcpy (pOutData + nVideoSize, &nNalWord, 4);
			nVideoSize += 4;
			memcpy (pOutData + nVideoSize, pBuffer, nFrameLen);
			nVideoSize += nFrameLen;
		}

		pBuffer += nFrameLen;
	}
}

VO_U32  AMCWrap::onReset(void* pData)
{
	VO_SOURCE_TRACKINFO* pInfo = (VO_SOURCE_TRACKINFO*)(pData);
	if(!mCodec)
		return VO_ERR_WRONG_STATUS;
	if(pInfo)
	{
		JNIEnv* pEnv = NULL;
		if(!mIsAmazon || !mIsVideo)
		{
			if(mThreadId != pthread_self())
				mJavaVM->AttachCurrentThread(&pEnv, NULL);
			if(pEnv)
			{
				mCodec->setEnv(pEnv);
				mEnv = pEnv;
			}
		
			__D("pEnv = %p\n");
			assert(mCodec != NULL);
			__D("RESET flush\n");
			//mCodec->flush();
			__D("RESET ->stop\n");
			mCodec->stop();
			__D("RESET ->release\n");
			if(!mIsCrypto)
				mCodec->release();
			// give some some time to make sure the decoder can be released
			usleep(100);
			delete mCodec;
			mCodec = NULL;
			mCodec = new JCMediaCodec(mEnv); 
			
			if(mCodec->init())
			{
				bool bOk = false;
				if(mIsVideo && mIsCrypto)
					bOk = mCodec->createByCodecName(mSecureComName);
				else
					bOk = mCodec->createDecoderByType(mMime);
			}
		}

		VO_CODECBUFFER voBuf;
		voBuf.Length = pInfo->HeadSize;
		voBuf.Buffer = pInfo->HeadData;
		__D("RESET voBuf.Length = %d\n", voBuf.Length);
		if(mHeadData)
		{
			delete [] mHeadData;
			mHeadData = NULL;
			mHeadLen = 0;
		}
		__D("RESET\n");
		onSetHeadData(&voBuf);
		mSetHeader = false;

		if(!mIsAmazon || !mIsVideo)
		{
			assert(mFormat != NULL);
			//assert(mIsVideo == true);
			
			if(pEnv)
				mFormat->setEnv(pEnv);
			if(mCrypto && pEnv)
				mCrypto->setEnv(pEnv);

			if(mIsVideo)
			{
				__D("RESET mWidth = %d mHeight = %d\n", mWidth, mHeight);
				mFormat->setIntValue("width", mWidth);
				mFormat->setIntValue("height", mHeight);
				__D("RESET\n");
				mCodec->configure(*mFormat,  mSurface, mCrypto, 0);	
				__D("RESET\n");
			}
			else
			{
				mFormat->setIntValue("channel-count", mChannels);
				mFormat->setIntValue("sample-rate",  mSampleRate);
				if(VO_AUDIO_CodingAAC == mAudioType)
				{
					setAACHeadData();
					/*
					if(mIsCrypto)
						mSetHeader = false;
					*/
				}
				
				mCodec->configure(*mFormat,  NULL, mCrypto, 0);	
			}
			mCodec->start();
		
			jobjectArray ars = mCodec->getInputBuffers();
			if(ars)
			{
				if(mInBuffers)
					mEnv->DeleteGlobalRef(mInBuffers);
				mInBuffers=(jobjectArray)mEnv->NewGlobalRef(ars);
				mEnv->DeleteLocalRef(ars);
				ars = NULL;
				mInNums = mEnv->GetArrayLength(mInBuffers);
				__D("RESET mInNums = %d\n", mInNums);
			}
			ars = mCodec->getOutputBuffers();
			if(ars)
			{
				if(mOutBuffers)
					mEnv->DeleteGlobalRef(mOutBuffers);
				mOutBuffers = (jobjectArray)mEnv->NewGlobalRef(ars);
				mEnv->DeleteLocalRef(ars);
				ars = NULL;
				mOutNums = mEnv->GetArrayLength(mOutBuffers);
				__D("RESET mOutNums = %d\n", mOutNums);
			}
			mNeedCallFlush  = false;
		}
	}
	return VO_ERR_NONE;
}

void AMCWrap::onSetHeadData(void* pData)
{
	VO_CODECBUFFER * pBuffer = (VO_CODECBUFFER *)pData;
	bool bDumpInitData = false;
	if(access("/sdcard/vodump/dump_init.txt", F_OK) == 0)
	{
		bDumpInitData = true;
	}
	if(pBuffer->Length > 0 &&  pBuffer->Buffer)
	{
		if(mIsCrypto)
		{
			VO_BYTE* pOrg = NULL;
			VO_U32   nOrgLen = 0;
				
			VO_BYTE* pDrmData = NULL;
			VO_U32   nDrmLen = 0;
			__D("pBuffer->Length = %d, pBuffer->Buffer = %p\n", pBuffer->Length, pBuffer->Buffer);
			depart(pBuffer->Buffer, pBuffer->Length, &pOrg, &nOrgLen, &pDrmData, &nDrmLen);
				
			if(bDumpInitData)
			{
				int nFd = open("/sdcard/vodump/header/init_org.dat", O_CREAT|O_RDWR|O_TRUNC, 0644);
				if(nFd > 0)
				{
					write(nFd, pDrmData, nDrmLen);
					close(nFd);
				}
			}
			__D("nOrgLen = %d, pOrg = %p\n", nOrgLen, pOrg);
			__D("nDrmLen = %d, pDrmData = %p\n", nDrmLen, pDrmData);

			mDrmInitLen = nDrmLen;
			
			if(mIsAmazon)
			{
				mDrmInitData = new unsigned char[nDrmLen];
				memcpy(mDrmInitData, pDrmData, nDrmLen); 
			}
			else
			{
				// If I use Base64Decode2 , I have to set mDrmInitLen, it is different with Base64Decode			
				mDrmInitData = (unsigned char*)Base64Decode2((VO_CHAR*)pDrmData, mDrmInitLen, VO_FALSE);
			}
                   				
			if(bDumpInitData)
			{
				int nFd = open("/sdcard/vodump/header/init_dec64.dat", O_CREAT|O_RDWR|O_TRUNC, 0644);
				if(nFd > 0)
				{
					write(nFd, mDrmInitData, mDrmInitLen);
					close(nFd);
				}
			}
			__D("mInitDrmData = %p, mInitDrmLen = %d\n", mDrmInitData, mDrmInitLen);
			if(nOrgLen != 0)
			{
				mHeadLen = nOrgLen;
				mHeadData = new unsigned char[mHeadLen];
				memcpy(mHeadData, pOrg, mHeadLen);

				if(bDumpInitData)
					dumpOrgHeadData();
			}
		}
		else
		{		
			mHeadLen = pBuffer->Length;
			mHeadData = new unsigned char[mHeadLen];
			memcpy(mHeadData, pBuffer->Buffer, mHeadLen);

			if(bDumpInitData)
				dumpOrgHeadData();
		}
		__D("mHeadLen = %d\n", mHeadLen);	
		if(mIsVideo)
		{
			VO_U32 nRC = VO_ERR_NONE;
			if(NULL == mVideoParser)
			{
				// initialize video parser
				nRC = voGetVideoParserAPI(&mVideoParserAPI, mType);
				if(VO_ERR_NONE == nRC)
					nRC = mVideoParserAPI.Init(&mVideoParser);
			}

			if(VO_ERR_NONE == nRC)
			{
				VO_CODECBUFFER sBuffer;
				sBuffer.Length = mHeadLen;
				sBuffer.Buffer = mHeadData;
				nRC = mVideoParserAPI.Process(mVideoParser, &sBuffer);
				if(VO_ERR_NONE == nRC)
				{
					nRC = mVideoParserAPI.GetParam(mVideoParser, VO_PID_VIDEOPARSER_WIDTH, &mWidth);
					nRC = mVideoParserAPI.GetParam(mVideoParser, VO_PID_VIDEOPARSER_HEIGHT, &mHeight);

					__D("width %d, height %d from video parser", mWidth, mHeight);

					VO_PARSER_ASPECTRATIO sRatio;
					memset(&sRatio, 0, sizeof(VO_PARSER_ASPECTRATIO));
					sRatio.mode = -1;
					nRC = mVideoParserAPI.GetParam(mVideoParser, VO_PID_VIDEOPARSER_RATIO, &sRatio);
					if(VO_ERR_NONE == nRC)
					{
						__D("aspect ratio %d, width %d, height %d", sRatio.mode, sRatio.width, sRatio.height);

						if(sRatio.mode == VO_RATIO_MAX)
						{
							__D("handle undefined aspect ratio, %dx%d", sRatio.width, sRatio.height);

							VO_S32 nWidth = sRatio.width;
							VO_S32 nHeight = sRatio.height;
							if(nWidth > 0x7fff || nHeight > 0x7fff)
							{
								nWidth = (nWidth + 512) >> 10;
								nHeight = (nHeight + 512) >> 10;
							}
							mAspectRatio= (nWidth << 16) | nHeight;
						}
						else
							mAspectRatio = sRatio.mode;
					}
					else
					{
						__D("failed to VO_PID_VIDEOPARSER_RATIO 0x%08X", nRC);
					}
				}
			}
		}
		else
		{
			if(mHeadLen > 0)
			{
				VO_AUDIO_HEADDATAINFO hInfo;
				VO_CODECBUFFER sBuffer;
				sBuffer.Length = mHeadLen;
				sBuffer.Buffer = mHeadData;
				
				if(bDumpInitData)
				{
					int nFd = open("/sdcard/vodump/header/header_audio.dat", O_CREAT|O_RDWR|O_TRUNC, 0644);
					if(nFd > 0)
					{
						write(nFd, mHeadData, mHeadLen);
						close(nFd);
					}
				}
				memset(&hInfo, 0, sizeof(VO_AUDIO_HEADDATAINFO));
				int nRC = VO_ERR_FAILED;
				if(VO_AUDIO_CodingAAC == mAudioType)
					nRC = voGetAACHeadInfo(&sBuffer, &hInfo);
				else if(VO_AUDIO_CodingMP3 == mAudioType)
					nRC = voGetMP3HeadInfo(&sBuffer, &hInfo);
				else if(VO_AUDIO_CodingWMA == mAudioType)
				{
					__D("should add audio format VO_AUDIO_CodingWMA \n");
				}
				else if(VO_AUDIO_CodingAC3 == mAudioType)
				{
				}
				else if(VO_AUDIO_CodingEAC3 == mAudioType)
				{
					
				}
				if(nRC == VO_ERR_NONE)
				{
					mSampleRate = hInfo.nSampleRate;
					mChannels = hInfo.nChannels;
					mSampleBits = hInfo.nSampleBits;
				}
			}
			// Sometime I get the wrong data, So I have to adjust them
			if(mChannels == 0)
				mChannels = 2;
			if(mSampleRate >= 96000)
				mSampleRate = 48000;

			__D("mSampleRate = %d\n", mSampleRate);
			__D("mChannels = %d\n", mChannels);
			__D("mSampleBits = %d\n", mSampleBits);
		}
	}
}

void AMCWrap::setAACHeadData()
{
	// the streaming can give me a head data, but the length sometimes is NOT 2 , so I have to conver it
	unsigned char* pTemp = NULL;
	if(!mHeadData || mHeadLen >=7 )
	{
					
		if(mHeadData)
		{
			pTemp = new unsigned char[mHeadLen];
			memcpy(pTemp, mHeadData, mHeadLen);

			delete[] mHeadData;
			mHeadData = NULL;
		}
		mHeadData = new unsigned char[2];
		memset(mHeadData, 0, 2);
		VO_CODECBUFFER inBuf;
		if(0 == mHeadLen)
		{
			makeHeadInfo();
		}
		else
		{
			inBuf.Buffer = pTemp;
			inBuf.Length = mHeadLen;
						
			VO_CODECBUFFER outBuf;
			outBuf.Buffer = mHeadData;
			outBuf.Length = 2;
			__D("Convert mHeadData = %p mHeadLen = %d\n", mHeadData, mHeadLen);
			__D("Convert pTemp = %p\n", pTemp);
					
			VO_U32 err = voAACHeadData7to2Bytes(&inBuf, &outBuf);
			__D("AUDIO_CONVERT mHeadData[0] = %x, mHeadData[1] = %x\n", mHeadData[0], mHeadData[1]);
			mHeadLen = 2;
			delete [] pTemp;
			pTemp  = NULL;
			if(err != 0)
				makeHeadInfo();
		}
					
	}

	__D("FAKE mHeadLen = %d\n", mHeadLen);
	// Since the mIsCrypto use the queueSecureInputBuffer to set header data
	if(!mIsCrypto)
	{
		mFormat->setByteBuffer("csd-0", mHeadData, mHeadLen);
		__D("AUDIO_CONVERT mHeadData[0] = %x, mHeadData[1] = %x\n", mHeadData[0], mHeadData[1]);
		mSetHeader = true;
	}
}
void AMCWrap::switchJavaEnv()
{
	JNIEnv* pEnv = NULL;
	// deal with the crash issue that create it  and destory it in different thread
	__D();
	if(mThreadId != pthread_self())
	{
		__D();
		mJavaVM->AttachCurrentThread(&pEnv, NULL);
		if(pEnv)
		{
			if(mFormat)
				mFormat->setEnv(pEnv);
			if(mCrypto)
				mCrypto->setEnv(pEnv);
			if(mBufInfo)
				mBufInfo->setEnv(pEnv);
			if(mCodec)
				mCodec->setEnv(pEnv);
			if(mUUID)
				mUUID->setEnv(pEnv);
			if(mCryptoInfo)
				mCryptoInfo->setEnv(pEnv);
			mEnv = pEnv;
			mThreadId = pthread_self();
		}	
	}
}

void AMCWrap::dumpOrgHeadData()
{
	char cName[1024];
	sprintf(cName, "/sdcard/vodump/header/header_%s.dat", mIsVideo ? "video":"audio");
					
	int nFd = open(cName, O_CREAT|O_RDWR|O_TRUNC, 0644);
				
	if(nFd > 0)
	{
		write(nFd, mHeadData, mHeadLen);
		close(nFd);
	}
}
bool AMCWrap::isInBlackList()
{
	char cPlatform[PROP_VALUE_MAX] = {0};
	char cDevice[PROP_VALUE_MAX] = {0};
	char cHardware[PROP_VALUE_MAX] = {0};
	
	__system_property_get ("ro.board.platform", cPlatform);
	__system_property_get ("ro.product.device", cDevice);
	__system_property_get ("ro.hardware", cHardware);
	// these characters are the Nexus S
	//[ro.board.platform]: [s5pc110]
	//[ro.product.device]: [crespo]
	//[ro.hardware]: [herring]

	__D("cPlatform = %s\n", cPlatform);
	__D("cDevice = %s\n", cDevice);
	__D("cHardware = %s\n", cHardware);
	
	if( (strncasecmp(cPlatform, "s5pc110", 7) == 0 && strncasecmp(cDevice, "crespo", 6) == 0 && strncasecmp(cHardware, "herring", 7) == 0))
		return true;
	return false;
}
