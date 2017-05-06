/* vomeplayer.cpp
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "vomeplayer"
#include <utils/Log.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <utils/threads.h>

#if defined __VONJ_ECLAIR__
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryBase.h>
#endif

#include <media/AudioTrack.h>
#include "vomeplayer.h"
#include "voOMXOSFun.h"
#include "cmnFile.h"
#include "voLog.h"

#include <jni.h>
#include <JNIHelp.h>
#include <AndroidRuntime.h>

#include "vojnilog.h"

extern int sftest();

namespace android {

class Test 
{
public:
    static const sp<ISurface>& getISurface(const sp<Surface>& s) 
    {
        return s->getISurface();
    }

    static void TestSurface(const sp<Surface>& s)
    {
        LOGW("TestSurface");

        int m_nVideoWidth = 320;
        int m_nVideoHeight = 480;
        int nFormat = PIXEL_FORMAT_RGB_565;

        sp<MemoryHeapBase>  m_pFrameHeap = new MemoryHeapBase(m_nVideoWidth * m_nVideoHeight * 2);
        if (m_pFrameHeap == NULL)
        {
            LOGW("m_pFrameHeap == NULL");
        }
        ISurface::BufferHeap Buffers (m_nVideoWidth, m_nVideoHeight, m_nVideoWidth, m_nVideoHeight,
                                            nFormat, m_pFrameHeap);
        status_t status;
        s->getISurface()->unregisterBuffers();
        LOGW("vomeRGB565VideoRender::createMemHeap unregisterBuffers");
        status = s->getISurface()->registerBuffers(Buffers);
        LOGW("vomeRGB565VideoRender::createMemHeap registerBuffers = %d - %s", status, strerror(status));
        unsigned char* m_pOutBuffer = static_cast<unsigned char*>(m_pFrameHeap->base());

        memset (m_pOutBuffer, 0x55, m_nVideoWidth * m_nVideoHeight * 2);
    
#if 1
        LOGW("postBuffer+++");
        s->getISurface()->postBuffer(0);
        sleep(3);
        LOGW("postBuffer---");
#endif
    }
};

#define VOINFO(format, ...) do { \
	LOGI("%s::%s()->%d: " format "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); } while(0);

// client singleton for binder interface to service
Mutex vomeplayer::sServiceLock;
sp<IMediaPlayerService> vomeplayer::sMediaPlayerService;
sp<vomeplayer::DeathNotifier> vomeplayer::sDeathNotifier;
SortedVector< wp<vomeplayer> > vomeplayer::sObitRecipients;

// establish binder interface to service
const sp<IMediaPlayerService>& vomeplayer::getMediaPlayerService()
{
    return NULL;
}

void vomeplayer::addObitRecipient(const wp<vomeplayer>& recipient)
{
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.add(recipient);
}

void vomeplayer::removeObitRecipient(const wp<vomeplayer>& recipient)
{
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.remove(recipient);
}


int vomeplayer::vomeMsgHandler(void* pUserData, int nID, void* pParam1, void* pParam2)
{
	vomeplayer* pm = (vomeplayer*)pUserData;	
	if (pm == NULL) return 0;

	return pm->msgHandler(nID, pParam1, pParam2);
}

void vomeplayer::vomeSignalProc(void* pparam) {
	JNILOGD("vomeSignalProc");
	vomeplayer* pp = (vomeplayer*) pparam;
	if (pp) 
		pp->vomeMonitor();
}

void vomeplayer::vomeMonitor() {

	do {		

		mSignal2.wait(mNotifyLock); 
		
		int msg = m_event.msg;
		int ext1 = m_event.ext1;
		int ext2 = m_event.ext2;
		mEventLock.unlock();		

		mListener->notify(msg, ext1, ext2);

	} while (mCurrentState != VOME_PLAYER_STOPPED);
}

vomeplayer::vomeplayer()
{

	JNILOGV("vomeplayer constructor 2");


	//LOGV("constructor");
	mListener = NULL;
	mCookie = NULL;
	mDuration = -1;
	mStreamType = AudioSystem::MUSIC;
	mCurrentPosition = -1;
	mSeekPosition = -1;
	mCurrentState = VOME_PLAYER_IDLE;
	mPrepareSync = false;
	mPrepareStatus = NO_ERROR;
	mLoop = false;
	mLeftVolume = mRightVolume = 1.0;
	mVideoWidth = mVideoHeight = 0;
	m_bAudioPaused4Seek = false;
	m_pvoplayer = NULL;
	m_paudiosink = NULL;
	m_pvideosink = NULL;
	memset(m_szworkingpath, 0, 128);
	mLockThreadId = 0;
}

void vomeplayer::onFirstRef()
{
}

vomeplayer::~vomeplayer()
{
	VOINFO("vomeplayer gonna uninit...");
		if (m_pvoplayer) {
			delete m_pvoplayer;
			m_pvoplayer = NULL;
		}

		if (m_paudiosink != NULL) {
			m_paudiosink->stop();
			delete m_paudiosink;
			m_paudiosink = NULL;
		}

		if (m_pvideosink != NULL) {
			delete m_pvideosink;
			m_pvideosink = NULL;
		}
	VOINFO("vomeplayer uninit over...");
    //IPCThreadState::self()->flushCommands();
}

void vomeplayer::disconnect()
{
	VOINFO("disconnect...");
	clear_l();
	if (m_pvoplayer) {
		m_pvoplayer->Uninit();
		delete m_pvoplayer;
		m_pvoplayer = NULL;
	}
	VOINFO("disconnect over...");
}

// always call with lock held
void vomeplayer::clear_l()
{
	//VOLOGW ();

    mDuration = -1;
    mCurrentPosition = -1;
    mSeekPosition = -1;
    mVideoWidth = mVideoHeight = 0;
}

status_t vomeplayer::setListener(const sp<vomeplayerListener>& listener)
{
    JNILOGV("setListener");
    Mutex::Autolock _l(mLock);
    mListener = listener;
    return NO_ERROR;
}


status_t vomeplayer::setDataSource(const sp<IMediaPlayer>& player)
{
	//VOLOGW ();

    status_t err = UNKNOWN_ERROR;
    return err;
}

status_t vomeplayer::setDataSource(const char *url)
{
	//VOLOGW ("Source: %s", url);

	 JNILOGD("vomeplayer setDataSource");
	   
	
	status_t err = BAD_VALUE;
	if (url == NULL) 
		return err;
	
	if (m_pvoplayer != NULL) {
		delete m_pvoplayer;
		m_pvoplayer = NULL;
	}

	{
		char name[128];
		sprintf(name, "vome_thread-%s-%s", __DATE__, __TIME__);
		AndroidRuntime::createJavaThread(name, (void(*)(void*))vomeSignalProc, (void*)this); 
	}


	m_pvoplayer = new CVOMEPlayer();
	m_pvoplayer->SetCallBack(vomeMsgHandler, this);
	m_pvoplayer->Init(m_szworkingpath);
	clear_l();

	err = m_pvoplayer->SetSource(url);
	mCurrentState = VOME_PLAYER_INITIALIZED;

	return err;
}

status_t vomeplayer::setDataSource(int fd, int64_t offset, int64_t length)
{
	//VOLOGW ("Source: %d", fd);

    JNILOGD2("setDataSource(%d, %lld, %lld)", fd, offset, length);
	
    status_t err = UNKNOWN_ERROR;

		if (m_pvoplayer != NULL) {
			delete m_pvoplayer;
			m_pvoplayer = NULL;
		}
		m_pvoplayer = new CVOMEPlayer();
		m_pvoplayer->SetCallBack(vomeMsgHandler, this);
		m_pvoplayer->Init(m_szworkingpath);
		m_pvoplayer->SetSource(fd, offset, length);

    return err;
}

status_t vomeplayer::invoke(const Parcel& request, Parcel *reply)
{
	//VOLOGW ();

    Mutex::Autolock _l(mLock);
    JNILOGE2("invoke failed: wrong state %X", mCurrentState);
    return INVALID_OPERATION;
}

status_t vomeplayer::setMetadataFilter(const Parcel& filter)
{
	//VOLOGW ();

    JNILOGD("setMetadataFilter");
    Mutex::Autolock lock(mLock);
    if (mPlayer == NULL) {
        return NO_INIT;
    }
    //return mPlayer->setMetadataFilter(filter);
    return NO_INIT;
}

status_t vomeplayer::getMetadata(bool update_only, bool apply_filter, Parcel *metadata)
{
	//VOLOGW ();

    JNILOGD("getMetadata");
    Mutex::Autolock lock(mLock);
    if (mPlayer == NULL) {
        return NO_INIT;
    }
    //return mPlayer->getMetadata(update_only, apply_filter, metadata);
    return NO_INIT;
}


status_t vomeplayer::setVideoSurface(const sp<Surface>& surface)
{
    JNILOGD("vomeplayer setVideoSurface");
    
    Mutex::Autolock _l(mLock);
    //LOGW("vomeplayer::setVideoSurface Heaps: %08x, %08x - Flags: %x", surface->heapBase(0), surface->heapBase(1), surface->getFlags());

#if 0
    Test::TestSurface(surface);
#endif
#if 0
    sftest();
#endif

	//JNILOGI2("%s", "gonna create video render...");
		initVideoSink();
#if USESUR
    m_pvideosink->SetVideoSurface(surface);
#else
    m_pvideosink->SetVideoSurface(Test::getISurface(surface));
    m_pvideosink->SetDispSize(480, 320);
#endif

    return NO_ERROR;
}

// must call with lock held
status_t vomeplayer::prepareAsync_l()
{
    if ((m_pvoplayer != 0) && (mCurrentState & (VOME_PLAYER_INITIALIZED | VOME_PLAYER_STOPPED))) {
      mCurrentState = VOME_PLAYER_PREPARING;
      			JNILOGD("Before m_pvoplayer PrepareAsync()");
			status_t ret = m_pvoplayer->PrepareAsync();
			JNILOGD("after m_pvoplayer  PrepareAsync()");
			return ret;
    }

    JNILOGE2("prepareAsync called in state %d", mCurrentState);
    return INVALID_OPERATION;
}

// TODO: In case of error, prepareAsync provides the caller with 2 error codes,
// one defined in the Android framework and one provided by the implementation
// that generated the error. The sync version of prepare returns only 1 error
// code.
status_t vomeplayer::prepare()
{
    JNILOGD("vomeplayer prepare");
    
    Mutex::Autolock _l(mLock);
    mLockThreadId = getThreadId();

      JNILOGD2("after get Thread ID %d", mLockThreadId);
    	
    if (mPrepareSync) {
        mLockThreadId = 0;
        JNILOGE("mPrepareSync");
        return -EALREADY;
    }
    mPrepareSync = true;
  
       
    status_t ret = prepareAsync_l();

       
    if (ret != NO_ERROR) {
        mLockThreadId = 0;
        JNILOGE("prepareAsync_l ERROR!");
        return ret;
    }

    JNILOGD("before wait");
    if (mPrepareSync) {
        mSignal.wait(mLock);  // wait for prepare done
        mPrepareSync = false;
    }
    JNILOGV2("prepare complete - status=%d", mPrepareStatus);
    mLockThreadId = 0;
    return mPrepareStatus;
}

status_t vomeplayer::prepareAsync()
{
 	//VOLOGW ();

    Mutex::Autolock _l(mLock);
    return prepareAsync_l();
}

status_t vomeplayer::start() {
      JNILOGD("vomeplayer start");
      
	Mutex::Autolock _l(mLock);
	if (mCurrentState & VOME_PLAYER_STARTED)
		return NO_ERROR;

	if ((m_pvoplayer!= 0) && (mCurrentState & (VOME_PLAYER_PREPARED|VOME_PLAYER_PLAYBACK_COMPLETE|VOME_PLAYER_PAUSED))) {
		mCurrentState = VOME_PLAYER_STARTED;
		JNILOGI2("current state: %d", mCurrentState);
		//m_pvoplayer->SetLoop(mLoop);
		status_t ret = m_pvoplayer->Run();
		if (ret != NO_ERROR) {
			mCurrentState = VOME_PLAYER_STATE_ERROR;
		} else {
			if (mCurrentState == VOME_PLAYER_PLAYBACK_COMPLETE) {
				JNILOGI2("%s", "playback completed immediately following start()");
			}
		}
		return ret;
	}

	JNILOGE2("start called in state %d", mCurrentState);
	return INVALID_OPERATION;
}

status_t vomeplayer::stop()
{
    JNILOGD("vomeplayer stop"); 
 
    Mutex::Autolock _l(mLock);
    if (mCurrentState & VOME_PLAYER_STOPPED) return NO_ERROR;

		if (m_pvoplayer) {
			m_pvoplayer->Stop();
		}

    mCurrentState = VOME_PLAYER_STOPPED;

    JNILOGE2("stop called in state %d", mCurrentState);
    return NO_ERROR;
}

status_t vomeplayer::pause()
{
     JNILOGD("vomeplayer pause"); 
     
    Mutex::Autolock _l(mLock);
    if (mCurrentState & VOME_PLAYER_PAUSED)
        return NO_ERROR;

		if (m_pvoplayer) {
			status_t ret = m_pvoplayer->Pause();
			if (ret != NO_ERROR) {
				mCurrentState = VOME_PLAYER_STATE_ERROR;
			} else {
				mCurrentState = VOME_PLAYER_PAUSED;
			}
			return ret;
		}

    JNILOGE2("pause called in state %d", mCurrentState);
    return INVALID_OPERATION;
}

bool vomeplayer::isPlaying()
{
	//VOLOGW ();

    Mutex::Autolock _l(mLock);

		bool val;
		if (m_pvoplayer) {
			val = m_pvoplayer->IsPlaying();
		}

    if (mPlayer != 0) {
        bool temp = false;
        mPlayer->isPlaying(&temp);
        JNILOGV2("isPlaying: %d", temp);
        if ((mCurrentState & VOME_PLAYER_STARTED) && ! temp) {
            JNILOGE("internal/external state mismatch corrected");
            mCurrentState = VOME_PLAYER_PAUSED;
        }
        return temp;
    }
    JNILOGV("isPlaying: no active player");
    return val;
}

status_t vomeplayer::getVideoWidth(int *w)
{
	//VOLOGW ();

    JNILOGV("getVideoWidth");
    Mutex::Autolock _l(mLock);
    if (m_pvoplayer == NULL) return INVALID_OPERATION;
		*w = mVideoWidth;
    return NO_ERROR;
}

status_t vomeplayer::getVideoHeight(int *h)
{
	//VOLOGW ();

    JNILOGV("getVideoHeight");
    Mutex::Autolock _l(mLock);
    if (m_pvoplayer == NULL) return INVALID_OPERATION;
			*h = mVideoHeight;

    //if (mPlayer == 0) return INVALID_OPERATION;
    //*h = mVideoHeight;
    return NO_ERROR;
}

status_t vomeplayer::getCurrentPosition(int *msec)
{
	//VOLOGW ();

    JNILOGV("getCurrentPosition");
    Mutex::Autolock _l(mLock);
    //if (mPlayer != 0) 
		if (m_pvoplayer)
		{
        if (mCurrentPosition >= 0) {
            LOGV("Using cached seek position: %d", mCurrentPosition);
            *msec = mCurrentPosition;
            return NO_ERROR;
        }
        //return mPlayer->getCurrentPosition(msec);
        return m_pvoplayer->GetPos(msec);
    }

    return INVALID_OPERATION;
}

status_t vomeplayer::getDuration_l(int *msec)
{
    JNILOGV("getDuration");
    bool isValidState = (mCurrentState & (VOME_PLAYER_PREPARED | VOME_PLAYER_STARTED | VOME_PLAYER_PAUSED | VOME_PLAYER_STOPPED | VOME_PLAYER_PLAYBACK_COMPLETE));
    if (m_pvoplayer != 0 && isValidState) {
        status_t ret = NO_ERROR;
        if (mDuration <= 0)
            ret = m_pvoplayer->GetDuration(&mDuration);
        if (msec)
            *msec = mDuration;
        return ret;
    }
    LOGE("Attempt to call getDuration without a valid mediaplayer");
    return INVALID_OPERATION;
}

status_t vomeplayer::getDuration(int *msec)
{
    Mutex::Autolock _l(mLock);
    return getDuration_l(msec);
}

status_t vomeplayer::seekTo_l(int msec)
{
	JNILOGI2("seekTo %d", msec);
	if ((m_pvoplayer!=0) && (mCurrentState & (VOME_PLAYER_STARTED|VOME_PLAYER_PREPARED|VOME_PLAYER_PAUSED|VOME_PLAYER_PLAYBACK_COMPLETE))) {
		if ( msec < 0 ) {
			JNILOGI2("Attempt to seek to invalid position: %d", msec);
			msec = 0;
		} else if ((mDuration > 0) && (msec > mDuration)) {
			JNILOGI2("Attempt to seek to past end of file: request = %d, EOF = %d", msec, mDuration);
			msec = mDuration;
		}
		// cache duration
		mCurrentPosition = msec;
		if (mSeekPosition < 0) {
			getDuration_l(NULL);
			mSeekPosition = msec;

			if (m_paudiosink) {
				m_paudiosink->pause();
				m_bAudioPaused4Seek = true;
			}

			return m_pvoplayer->SetPos(msec);
		} else {
			JNILOGI2("Seek in progress - queue up seekTo[%d]", msec);
			return NO_ERROR;
		}
	}
	JNILOGI2("Attempt to perform seekTo in wrong state: mPlayer=%p, mCurrentState=%u", mPlayer.get(), mCurrentState);
	return INVALID_OPERATION;
}

status_t vomeplayer::seekTo(int msec)
{
   mLockThreadId = getThreadId();
    Mutex::Autolock _l(mLock);
    status_t result = seekTo_l(msec);
    mLockThreadId = 0;

    return result;
}

status_t vomeplayer::reset()
{
    JNILOGV("reset");
    Mutex::Autolock _l(mLock);
    mLoop = false;
    if (mCurrentState == VOME_PLAYER_IDLE) return NO_ERROR;
    mPrepareSync = false;
    if (m_pvoplayer != 0) {
        status_t ret = m_pvoplayer->Reset();
        if (ret != NO_ERROR) {
            JNILOGE2("reset() failed with return code (%d)", ret);
            mCurrentState = VOME_PLAYER_STATE_ERROR;
        } else {
            mCurrentState = VOME_PLAYER_IDLE;
        }
        return ret;
    }
    clear_l();
    return NO_ERROR;
}

status_t vomeplayer::setAudioStreamType(int type)
{
	//VOLOGW ();

    JNILOGV("vomeplayer::setAudioStreamType");
    Mutex::Autolock _l(mLock);
    if (mStreamType == type) return NO_ERROR;
    if (mCurrentState & ( VOME_PLAYER_PREPARED | VOME_PLAYER_STARTED |
                VOME_PLAYER_PAUSED | VOME_PLAYER_PLAYBACK_COMPLETE ) ) {
        // Can't change the stream type after prepare
        JNILOGE2("setAudioStream called in state %d", mCurrentState);
        return INVALID_OPERATION;
    }
    // cache
    mStreamType = type;
    return OK;
}
int		vomeplayer::				SetParam(int id, void* param)
{
	if (m_pvoplayer != 0) 
		return m_pvoplayer->SetParam(id, param);

	return OK;
}
int		vomeplayer::				GetParam(int id, void* param)
{
	if (m_pvoplayer != 0) 
		return m_pvoplayer->GetParam(id, param);

	return OK;
}
status_t vomeplayer::setLooping(int loop)
{
	//VOLOGW ();

    JNILOGV("vomeplayer::setLooping");
    Mutex::Autolock _l(mLock);
    mLoop = (loop != 0);
    if (mPlayer != 0) {
        mPlayer->setLooping(loop);
    }
    if (m_pvoplayer != 0) 
        return m_pvoplayer->SetLoop(loop);

    return OK;
}

bool vomeplayer::isLooping()
{
	//VOLOGW ();

    JNILOGV("isLooping");
    Mutex::Autolock _l(mLock);
    //if (mPlayer != 0) {
    if (m_pvoplayer != 0) {
        return mLoop;
    }
    JNILOGV("isLooping: no active player");
    return false;
}

status_t vomeplayer::setVolume(float leftVolume, float rightVolume)
{
	//VOLOGW ();

    JNILOGV2("vomeplayer::setVolume(%f, %f)", leftVolume, rightVolume);
    Mutex::Autolock _l(mLock);
    mLeftVolume = leftVolume;
    mRightVolume = rightVolume;
    if (mPlayer != 0) {
        return mPlayer->setVolume(leftVolume, rightVolume);
    }
    return OK;
}

void vomeplayer::notify(int msg, int ext1, int ext2)
{
    
    bool send = true;
    bool locked = false;

   

// for the issue that the msg of SET VIDEO SIZE can't be delived to JAVA
/*
    if (mLockThreadId != getThreadId()) {
        mLock.lock();
        locked = true;
    }
	*/


    if (m_pvoplayer == 0) {
        JNILOGV2("notify(%d, %d, %d) callback on disconnected mediaplayer", msg, ext1, ext2);
        if (locked) mLock.unlock();   // release the lock when done.
        return;
    }

		mEventLock.lock();
		m_event.msg = msg;
		m_event.ext1 = ext1;
		m_event.ext2 = ext2;

    switch (msg) {
    case VOME_NOP: // interface test message
        break;
    case VOME_PREPARED:
        mCurrentState = VOME_PLAYER_PREPARED;
        if (mPrepareSync) {
            mPrepareSync = false;
            mPrepareStatus = NO_ERROR;
            mSignal.signal();
        }
        break;
    case VOME_PLAYBACK_COMPLETE: {
        if (!mLoop) 
            mCurrentState = VOME_PLAYER_PLAYBACK_COMPLETE;
				}
        break;
    case VOME_ERROR:
        // Always log errors.
        // ext1: Media framework error code.
        // ext2: Implementation dependant error code.
        JNILOGE2("error (%d, %d)", ext1, ext2);
        mCurrentState = VOME_PLAYER_STATE_ERROR;
        if (mPrepareSync)
        {
            JNILOGV("signal application thread");
            mPrepareSync = false;
            mPrepareStatus = ext1;
            mSignal.signal();
            send = false;
        }
        break;
    case VOME_INFO:
        // ext1: Media framework error code.
        // ext2: Implementation dependant error code.
        JNILOGW2("info/warning (%d, %d)", ext1, ext2);
        break;
    case VOME_SEEK_COMPLETE:
        if (mSeekPosition != mCurrentPosition) {
            JNILOGI2("%s", "Executing queued seekTo(%d)", mSeekPosition);
            mSeekPosition = -1;
            seekTo_l(mCurrentPosition);
        } else {
            JNILOGI2("%s", "All seeks complete - return to regularly scheduled program");
            mCurrentPosition = mSeekPosition = -1;
        }
        break;
    case VOME_BUFFERING_UPDATE:
        JNILOGV2("buffering %d", ext1);
        break;
    case VOME_SET_VIDEO_SIZE:
        mVideoWidth = ext1;
        mVideoHeight = ext2;
        JNILOGI("SET_VIDEO_SIZE msg");
        break;
    default:
        JNILOGV2("unrecognized message: (%d, %d, %d)", msg, ext1, ext2);
        break;
    }

    //sp<vomeplayerListener> listener = mListener;
    if (locked) mLock.unlock();

    // this prevents re-entrant calls into client code
    /*if ((listener != 0) && send) {
        Mutex::Autolock _l(mNotifyLock);
        listener->notify(msg, ext1, ext2);
    }*/
		mSignal2.signal();
		//mEventLock.unlock();
}

void vomeplayer::DeathNotifier::binderDied(const wp<IBinder>& who)
{

    JNILOGW("vomeplayer server died!");

    // Need to do this with the lock held
    SortedVector< wp<vomeplayer> > list;
    {
        Mutex::Autolock _l(vomeplayer::sServiceLock);
        vomeplayer::sMediaPlayerService.clear();
        list = sObitRecipients;
    }

    // Notify application when media server dies.
    // Don't hold the static lock during callback in case app
    // makes a call that needs the lock.
    size_t count = list.size();
    for (size_t iter = 0; iter < count; ++iter) {
        sp<vomeplayer> player = list[iter].promote();
        if ((player != 0) && (player->mPlayer != 0)) {
            player->notify(VOME_ERROR, VOME_ERROR_SERVER_DIED, 0);
        }
    }
}

status_t vomeplayer::initAudioSink(void* param) 
{
	
	VO_AUDIO_FORMAT *paf = (VO_AUDIO_FORMAT*)param;
	JNILOGI2("samplerate: %d, channels: %d, sampblebits: %d, PCM_16_BIT: %d\n", 
	paf->SampleRate, paf->Channels, paf->SampleBits, AudioSystem::PCM_16_BIT);

	if (m_paudiosink == NULL) {
		m_paudiosink = new VOAudioOutput();
	} else {
		m_paudiosink->stop();
		m_paudiosink->close();
	}

	status_t rc = m_paudiosink->open(paf->SampleRate, paf->Channels, AudioSystem::PCM_16_BIT, DEFAULT_AUDIOSINK_BUFFERCOUNT, NULL, NULL);

	m_paudiosink->start();
	return rc;
}

status_t vomeplayer::initVideoSink() {

	if (m_pvideosink == NULL) {
		m_pvideosink = new voVideoRender();
		m_pvideosink->setWorkingPath(m_szworkingpath);
	}

	return NO_ERROR;
}

status_t vomeplayer::setWorkingPath(const char* path) {

	if (path == NULL || strlen(path) > 128) 
		return INVALID_OPERATION;

	if (strcmp(m_szworkingpath, path) == 0) 
		return ALREADY_EXISTS;

	strcpy(m_szworkingpath, path);
	int len = strlen(m_szworkingpath);
	if (m_szworkingpath[len-1] == '/')
		m_szworkingpath[len-1] = '\0';

	return NO_ERROR;
}

vomeplayer::DeathNotifier::~DeathNotifier()
{
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.clear();
    if (sMediaPlayerService != 0) {
        sMediaPlayerService->asBinder()->unlinkToDeath(this);
    }
}

extern "C" {
#define FLOATING_POINT 1
//#include "fftwrap.h"
}

int vomeplayer::snoop(short* data, int len, int kind) {
    return 0;
}

int vomeplayer::msgHandler(int nID, void * pParam1, void * pParam2)
{
	switch (nID) {
		case VOAP_IDC_setAudioFormat: {
			initAudioSink(pParam1);
		}
		break;

		case VOAP_IDC_setVideoSize: {
			
			mVideoWidth = *(int*)pParam1;
			mVideoHeight = *(int*)pParam2;
			initVideoSink();
			m_pvideosink->SetVideoSize(mVideoWidth, mVideoHeight);
		}
		break;

		case VOAP_IDC_notifyEvent: {
			
			VOA_NOTIFYEVENT* pe = (VOA_NOTIFYEVENT*)pParam1;
			VOINFO("msg: %d, ext1: %d, ext2: %d", pe->msg, pe->ext1, pe->ext2);
			notify(pe->msg, pe->ext1, pe->ext2);
		}
		break;

		case VOAP_IDC_renderVideo: {
			
			if (m_pvideosink) {
				m_pvideosink->Render((VO_ANDROID_VIDEO_BUFFERTYPE*)pParam1);
			}
		}
		break;

		case VOAP_IDC_RenderAudio: {			
			if (m_paudiosink) {

				if (m_bAudioPaused4Seek) {
					m_bAudioPaused4Seek = false;
					m_paudiosink->start();
				}

				m_paudiosink->write((unsigned char*)pParam1, *(int*)pParam2);
			}
		}
		break;

		case VOAP_IDC_setAudioStart: {
			if (m_paudiosink)
				m_paudiosink->start();
		}
		break;
		
		case VOAP_IDC_setAudioStop: {
			m_paudiosink->stop();
		}
		break;
		
		case VOAP_IDC_setAudioPause: {
			m_paudiosink->pause();
		}
		break;

		case VOAP_IDC_setAudioFlush: {
			m_paudiosink->flush();
		}
		break;

		default: break;
	}
	return 0;
}

}; // namespace android
