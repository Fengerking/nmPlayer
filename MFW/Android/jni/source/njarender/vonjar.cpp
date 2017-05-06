

#undef LOG_TAG
#define LOG_TAG "VOAudioSink"

#if defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__
#include <binder/MemoryBase.h>
#elif defined __VONJ_DONUT__
#include <utils/MemoryBase.h>
#endif
#include <cutils/properties.h>
#include <utils/Timers.h>
#include "vomeplayer.h"

#include "vojnilog.h"

namespace android {

#if 1
#define VOINFO(format, ...)
#else
#define VOINFO(format, ...) { \
	LOGI("[david] %s()->%d: " format, __FUNCTION__, __LINE__, __VA_ARGS__); }
#endif

static const int NUMVIZBUF = 32;
static const int VIZBUFFRAMES = 1024;
static const int TOTALBUFTIMEMSEC = NUMVIZBUF * VIZBUFFRAMES * 1000 / 44100;
static size_t kDefaultHeapSize = 1024 * 1024; // 1MB

static bool gotMem = false;
static sp<MemoryBase> mem[NUMVIZBUF];
static uint64_t timeStamp[NUMVIZBUF];
static uint64_t lastReadTime;
static uint64_t lastWriteTime;
static int writeIdx = 0;

static int64_t vouptimeMillis() {
	int64_t when = systemTime(SYSTEM_TIME_MONOTONIC);
	return (int64_t) nanoseconds_to_milliseconds(when);
} 

static void allocVizBufs() {
	if (!gotMem) {
		for (int i=0;i<NUMVIZBUF;i++) {
			sp<MemoryHeapBase> heap = new MemoryHeapBase(VIZBUFFRAMES*2, 0, "snooper");
			mem[i] = new MemoryBase(heap, 0, heap->getSize());
			timeStamp[i] = 0;
		}
		gotMem = true;
	}
}

static sp<MemoryBase> getVizBuffer() {

	allocVizBufs();

	lastReadTime = vouptimeMillis() + 100; // account for renderer delay (we shouldn't be doing this here)

	// if there is no recent buffer (yet), just return empty handed
	if (lastWriteTime + TOTALBUFTIMEMSEC < lastReadTime) {
		return NULL;
	}
	char buf[200];

	int closestIdx = -1;
	uint32_t closestTime = 0x7ffffff;
	for (int i = 0; i < NUMVIZBUF; i++) {
		uint64_t tsi = timeStamp[i];
		uint64_t diff = tsi > lastReadTime ? tsi - lastReadTime : lastReadTime - tsi;
		if (diff < closestTime) {
			closestIdx = i;
			closestTime = diff;
		}
	}

	if (closestIdx >= 0) {
		return mem[closestIdx];
	}

	// we won't get here, since we either bailed out early, or got a buffer
	JNILOGD("Didn't expect to be here");
	return NULL;
}

static void storeVizBuf(const void *data, int len, uint64_t time) {
	short *viz = (short*)mem[writeIdx]->pointer();
	short *src = (short*)data;
	for (int i = 0; i < VIZBUFFRAMES; i++) {
		*viz++ = ((int(src[0]) + int(src[1])) >> 1) & ~0x7;
		src += 2;
	}
	timeStamp[writeIdx++] = time;
	if (writeIdx >= NUMVIZBUF) {
		writeIdx = 0;
	}
}

static void makeVizBuffers(const char *data, int len, uint64_t time) {

	allocVizBufs();

	uint64_t startTime = time;
	const int frameSize = 4; // 16 bit stereo sample is 4 bytes
	while (len >= VIZBUFFRAMES * frameSize) {
		storeVizBuf(data, len, time);
		data += VIZBUFFRAMES * frameSize;
		len -= VIZBUFFRAMES * frameSize;
		time += 1000 * VIZBUFFRAMES / 44100;
	}
}

#undef LOG_TAG
#define LOG_TAG "VOAudioOutput"
/* static */ const uint32_t VOAudioOutput::kAudioVideoDelayMs = 0;
/* static */ int VOAudioOutput::mMinBufferCount = 4;
/* static */ bool VOAudioOutput::mIsOnEmulator = false;

VOAudioOutput::VOAudioOutput()
    : mCallback(NULL),
      mCallbackCookie(NULL) {
    mTrack = 0;
    mStreamType = AudioSystem::MUSIC;
    mLeftVolume = 1.0;
    mRightVolume = 1.0;
    mLatency = 0;
    mMsecsPerFrame = 0;
    mNumFramesWritten = 0;
    setMinBufferCount();
}

VOAudioOutput::~VOAudioOutput()
{
    close();
}

void VOAudioOutput::setMinBufferCount()
{
    char value[PROPERTY_VALUE_MAX];
    if (property_get("ro.kernel.qemu", value, 0)) {
        mIsOnEmulator = true;
        mMinBufferCount = 12;  // to prevent systematic buffer underrun for emulator
    }
}

bool VOAudioOutput::isOnEmulator()
{
    setMinBufferCount();
    return mIsOnEmulator;
}

int VOAudioOutput::getMinBufferCount()
{
    setMinBufferCount();
    return mMinBufferCount;
}

ssize_t VOAudioOutput::bufferSize() const
{
    if (mTrack == 0) return NO_INIT;
    return mTrack->frameCount() * frameSize();
}

ssize_t VOAudioOutput::frameCount() const
{
    if (mTrack == 0) return NO_INIT;
    return mTrack->frameCount();
}

ssize_t VOAudioOutput::channelCount() const
{
    if (mTrack == 0) return NO_INIT;
    return mTrack->channelCount();
}

ssize_t VOAudioOutput::frameSize() const
{
    if (mTrack == 0) return NO_INIT;
    return mTrack->frameSize();
}

uint32_t VOAudioOutput::latency () const
{
    return mLatency;
}

float VOAudioOutput::msecsPerFrame() const
{
    return mMsecsPerFrame;
}

status_t VOAudioOutput::open(
        uint32_t sampleRate, int channelCount, int format, int bufferCount,
        AudioCallback cb, void *cookie)
{
    mCallback = cb;
    mCallbackCookie = cookie;

    // Check argument "bufferCount" against the mininum buffer count
    if (bufferCount < mMinBufferCount) {
        JNILOGD2("bufferCount (%d) is too small and increased to %d", bufferCount, mMinBufferCount);
        bufferCount = mMinBufferCount;
    }
    JNILOGV2("open(%u, %d, %d, %d)", sampleRate, channelCount, format, bufferCount);
    if (mTrack) close();
    int afSampleRate;
    int afFrameCount;
    int frameCount;

		status_t stat = UNKNOWN_ERROR;

		stat = AudioSystem::getOutputFrameCount(&afFrameCount, mStreamType);
    if (stat != NO_ERROR) {
        return NO_INIT;
    }

    if (AudioSystem::getOutputSamplingRate(&afSampleRate, mStreamType) != NO_ERROR) {
        return NO_INIT;
    }

    frameCount = (sampleRate*afFrameCount*bufferCount)/afSampleRate;

    AudioTrack *t;
#if defined __VONJ_ECLAIR__  || defined __VONJ_FROYO__
    if (mCallback != NULL) {
        t = new AudioTrack(
                mStreamType,
                sampleRate,
                format,
                (channelCount == 2) ? AudioSystem::CHANNEL_OUT_STEREO : AudioSystem::CHANNEL_OUT_MONO,
                frameCount,
                0 /* flags */,
                CallbackWrapper,
                this);
    } else {
        t = new AudioTrack(
								mStreamType,
                sampleRate,
                format,
                (channelCount == 2) ? AudioSystem::CHANNEL_OUT_STEREO : AudioSystem::CHANNEL_OUT_MONO,
                frameCount);
    }
#elif defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
		frameCount = (sampleRate*afFrameCount*bufferCount)/afSampleRate;
		t = new AudioTrack(mStreamType, sampleRate, format, channelCount, frameCount);
		if ((t == 0) || (t->initCheck() != NO_ERROR)) {
			JNILOGE("Unable to create audio track");
			delete t;
			return NO_INIT;
		}
#endif

		stat = t->initCheck();
    if ((t == 0) || (stat != NO_ERROR)) {
        JNILOGE("Unable to create audio track");
        delete t;
        return NO_INIT;
    }

    JNILOGV("setVolume");
    t->setVolume(mLeftVolume, mRightVolume);
    mMsecsPerFrame = 1.e3 / (float) sampleRate;
    mLatency = t->latency() + kAudioVideoDelayMs;
    mTrack = t;
    return NO_ERROR;
}

void VOAudioOutput::start()
{
    JNILOGV("start");
    if (mTrack) {
        mTrack->setVolume(mLeftVolume, mRightVolume);
        mTrack->start();
        mTrack->getPosition(&mNumFramesWritten);
    }
}


ssize_t VOAudioOutput::write(const void* buffer, size_t size)
{
    LOG_FATAL_IF(mCallback != NULL, "Don't call write if supplying a callback.");

	//JNILOGV("Write");

    //LOGV("write(%p, %u)", buffer, size);
    if (mTrack) {
        // Only make visualization buffers if anyone recently requested visualization data
        uint64_t now = vouptimeMillis();
        if (lastReadTime + TOTALBUFTIMEMSEC >= now) {
            // Based on the current play counter, the number of frames written and
            // the current real time we can calculate the approximate real start
            // time of the buffer we're about to write.
            uint32_t pos;
            mTrack->getPosition(&pos);

            // we're writing ahead by this many frames:
            int ahead = mNumFramesWritten - pos;
            // which is this many milliseconds, assuming 44100 Hz:
            ahead /= 44;

            makeVizBuffers((const char*)buffer, size, now + ahead + mTrack->latency());
            lastWriteTime = now;
        }
        ssize_t ret = mTrack->write(buffer, size);
        mNumFramesWritten += ret / 4; // assume 16 bit stereo
        return ret;
    }
    return NO_INIT;
}

void VOAudioOutput::stop()
{
    //VOINFO("%s\n", "stop...");
    if (mTrack) mTrack->stop();
    lastWriteTime = 0;
}

void VOAudioOutput::flush()
{
    JNILOGV("flush");
    if (mTrack) mTrack->flush();
}

void VOAudioOutput::pause()
{
    JNILOGV("pause");
    if (mTrack) mTrack->pause();
    lastWriteTime = 0;
}

void VOAudioOutput::close()
{
    JNILOGV("close");
    delete mTrack;
    mTrack = 0;
}

void VOAudioOutput::setVolume(float left, float right)
{
    JNILOGV2("setVolume(%f, %f)", left, right);
    mLeftVolume = left;
    mRightVolume = right;
    if (mTrack) {
        mTrack->setVolume(left, right);
    }
}

// static
void VOAudioOutput::CallbackWrapper(
        int event, void *cookie, void *info) {
    if (event != AudioTrack::EVENT_MORE_DATA) {
        return;
    }

    VOAudioOutput *me = (VOAudioOutput *)cookie;
    AudioTrack::Buffer *buffer = (AudioTrack::Buffer *)info;

    (*me->mCallback)(
            me, buffer->raw, buffer->size, me->mCallbackCookie);
}

#undef LOG_TAG
#define LOG_TAG "VOAudioCache"
VOAudioCache::VOAudioCache(const char* name) :
    mChannelCount(0), mFrameCount(1024), mSampleRate(0), mSize(0),
    mError(NO_ERROR), mCommandComplete(false)
{
    // create ashmem heap
    mHeap = new MemoryHeapBase(kDefaultHeapSize, 0, name);
}

uint32_t VOAudioCache::latency () const
{
    return 0;
}

float VOAudioCache::msecsPerFrame() const
{
    return mMsecsPerFrame;
}

status_t VOAudioCache::open(
        uint32_t sampleRate, int channelCount, int format, int bufferCount,
        AudioCallback cb, void *cookie)
{
    if (cb != NULL) {
        return UNKNOWN_ERROR;  // TODO: implement this.
    }

    JNILOGV2("open(%u, %d, %d, %d)", sampleRate, channelCount, format, bufferCount);
    if (mHeap->getHeapID() < 0) return NO_INIT;
    mSampleRate = sampleRate;
    mChannelCount = (uint16_t)channelCount;
    mFormat = (uint16_t)format;
    mMsecsPerFrame = 1.e3 / (float) sampleRate;
    return NO_ERROR;
}

ssize_t VOAudioCache::write(const void* buffer, size_t size)
{
    JNILOGV2("write(%p, %u)", buffer, size);
    if ((buffer == 0) || (size == 0)) return size;

    uint8_t* p = static_cast<uint8_t*>(mHeap->getBase());
    if (p == NULL) return NO_INIT;
    p += mSize;
    if (mSize + size > mHeap->getSize()) {
        JNILOGE2("Heap size overflow! req size: %d, max size: %d", (mSize + size), mHeap->getSize());
        size = mHeap->getSize() - mSize;
    }
    memcpy(p, buffer, size);
    mSize += size;
    return size;
}

// call with lock held
status_t VOAudioCache::wait()
{
    Mutex::Autolock lock(mLock);
    if (!mCommandComplete) {
        mSignal.wait(mLock);
    }
    mCommandComplete = false;

    if (mError == NO_ERROR) {
        JNILOGV("wait - success");
    } else {
        JNILOGV("wait - error");
    }
    return mError;
}

void VOAudioCache::notify(void* cookie, int msg, int ext1, int ext2)
{
    JNILOGV2("notify(%p, %d, %d, %d)", cookie, msg, ext1, ext2);
    VOAudioCache* p = static_cast<VOAudioCache*>(cookie);

    // ignore buffering messages
    if (msg == MEDIA_BUFFERING_UPDATE) return;

    // set error condition
    if (msg == MEDIA_ERROR) {
        JNILOGE2("Error %d, %d occurred", ext1, ext2);
        p->mError = ext1;
    }

    // wake up thread
    JNILOGV("wakeup thread");
    p->mCommandComplete = true;
    p->mSignal.signal();
}

status_t VOAudioCache::dump(int fd, const Vector<String16>& args) const
{
	const size_t SIZE = 256;
	char buffer[SIZE];
#if defined __VONJ_ECLAIR__  || defined __VONJ_FROYO__
	String8 result;

	result.append(" VOAudioCache\n");
	if (mHeap != 0) {
		snprintf(buffer, 255, "  heap base(%p), size(%d), flags(%d), device(%s)\n",
				mHeap->getBase(), mHeap->getSize(), mHeap->getFlags(), mHeap->getDevice());
		result.append(buffer);
	}
	snprintf(buffer, 255, "  msec per frame(%f), channel count(%d), format(%d), frame count(%ld)\n",
			mMsecsPerFrame, mChannelCount, mFormat, mFrameCount);
	result.append(buffer);
	snprintf(buffer, 255, "  sample rate(%d), size(%d), error(%d), command complete(%s)\n",
			mSampleRate, mSize, mError, mCommandComplete?"true":"false");
	result.append(buffer);
	::write(fd, result.string(), result.size());
#endif
	return NO_ERROR;
}

status_t VOAudioOutput::dump(int fd, const Vector<String16>& args) const
{
	const size_t SIZE = 256;
	char buffer[SIZE];
#if defined __VONJ_ECLAIR__  || defined __VONJ_FROYO__
	String8 result;

	result.append(" VOAudioOutput\n");
	snprintf(buffer, 255, "  stream type(%d), left - right volume(%f, %f)\n",
			mStreamType, mLeftVolume, mRightVolume);
	result.append(buffer);
	snprintf(buffer, 255, "  msec per frame(%f), latency (%d)\n",
			mMsecsPerFrame, mLatency);
	result.append(buffer);
	::write(fd, result.string(), result.size());
	if (mTrack != 0) {
		mTrack->dump(fd, args);
	}
#endif
	return NO_ERROR;
}
} // namespace android

