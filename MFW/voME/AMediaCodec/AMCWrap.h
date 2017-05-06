#ifndef __AMC_WRAP__H
#define __AMC_WRAP__H

#include "jni.h"
#include "voAudio.h"
#include "voVideo.h"
#include "JCEnvUtil.h"
#include "voVideoParser.h"

class JCMediaCodec;
class JCMediaCrypto;
class JCMediaFormat;
class JCMediaCrypto;
class JCBufferInfo;
class JCUUID;
class JCCryptoInfo;

class AMCWrap
{
public:
	AMCWrap(VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);
	AMCWrap(VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);
	virtual ~AMCWrap();
public:
	VO_U32  setInputData(VO_CODECBUFFER * pInput, VO_BOOL bEOS=VO_FALSE);
	VO_U32  getOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);
	VO_U32  getOutputData(VO_CODECBUFFER* pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo);
	VO_U32  setParam(VO_S32 uParamID, VO_PTR pData);
	VO_U32  getParam(VO_S32 uParamID, VO_PTR pData);

private:
	void    onSetParam();
	void    onCreate();
	void    onDestroy();
	bool    createJCData(const bool isVideo=true);
	bool    createJCDRMData();
	VO_U32  onRender(void* pData, bool bRender = true);
	void    makeHeadInfo();
	bool    getSecureCompName(char*& inOutName, bool isEncoder = false);
	// because the different vendor has it's own requirement, I check it according to the platfrom characteristics
	bool    setIsInvertIV();
	VO_U32  onReset(void* pData);
	void	convertAVC2H264(unsigned char* pData, unsigned uSize);
	void    onSetHeadData(void* pData);
	void    setAACHeadData();
	void    switchJavaEnv();

	// at present, MediaCodec can not work with Nexus S, so I take a temp work around that make our engine to use software decoder
	bool    isInBlackList();
	// only for debug using
	void    dumpOrgHeadData();
private:
	VO_VIDEO_CODINGTYPE        mType;
	VO_AUDIO_CODINGTYPE        mAudioType;
	VO_CODEC_INIT_USERDATA*    mUserData;
	int                        mWidth;
	int                        mHeight;
	int                        mSampleRate;  /*!< Sample rate */
	int	                       mChannels;    /*!< Channel count */
	int	                       mSampleBits;  /*!< Bits per sample */
	const char*                mMime; 
	unsigned char*             mHeadData;
	int                        mHeadLen;
	JavaVM*                    mJavaVM;
	jobject                    mSurface;
	jobject                    mLocalSur;  // because the engine JNI level can delete it, so I have to keep it in my module
	JNIEnv*                    mEnv;
	JCEnvUtil*                 mEnvUtil;
	JCMediaCodec*              mCodec;
	JCMediaFormat*             mFormat;
	JCMediaCrypto*             mCrypto;
	JCUUID*                    mUUID;
	JCCryptoInfo*              mCryptoInfo;
	jobjectArray               mInBuffers;
	jobjectArray               mOutBuffers;
	int                        mInNums;
	int                        mOutNums;
	bool                       mIsCrypto;
	bool                       mSetHeader;
	JCBufferInfo*              mBufInfo;
	long                       mThreadId;
	bool                       mIsVideo;
	int                        mAPreIndex;   // The previous audio buffer index
	
	unsigned char*             mDrmInitData; // deal with the drm init data
	VO_U32                     mDrmInitLen;  
	
	VO_VIDEO_PARSERAPI		   mVideoParserAPI;
	VO_HANDLE				   mVideoParser;
	VO_U32					   mAspectRatio;
	bool                       mIsInvertIV; // QCOM ask us to invert IV, maybe other vendor do not have this request.
	char*                      mSecureComName; // Since find out the component name take some time. so I restore it.
	bool                       mNeedCallFlush;
	bool                       mIsAmazon;      // here I use it to optimize BA and seek
	int                        mSMode;         // Scaling mode 1, SCALE_TO_FIT mode 2 means SCALE_TO_FIT_WITH_CROPPING
	
	// only for debugging usage
	bool                      mDumpInput;
	int                       mInputIndex;
	bool                      mDumpOutput;
	int                       mDFd;
	int                       mAFDGet;  // dump the whole input audio sample that I get from the source
	int                       mAFDSend; // dump the whole input audio data that I send to the audio decoder;
	int                       mVFDGet;  // dump the whole input video sample that I get from the source
	// only for debugging read the frame data, so I keep it
	int                       mReadIndex; 
};
#endif // __OMXDEC_WRAP__H
