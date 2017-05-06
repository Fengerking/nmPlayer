

#undef LOG_TAG
#define LOG_TAG "voAudioRender"

#include <binder/MemoryBase.h>
#include <cutils/properties.h>
#include <utils/Timers.h>
#include <utils/SystemClock.h>

#include "voAudioRender.h"

#include "vojnilog.h"

namespace android {

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

/* static */ const uint32_t voAudioRender::kAudioVideoDelayMs = 0;
/* static */ int voAudioRender::mMinBufferCount = 4;
/* static */ bool voAudioRender::mIsOnEmulator = false;

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
		//LOGI("@@@@    no audio data to look at yet");
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
		//LOGI("@@@ return buffer %d, %d/%d", closestIdx, uint32_t(lastReadTime), uint32_t(timeStamp[closestIdx]));
		return mem[closestIdx];
	}

	// we won't get here, since we either bailed out early, or got a buffer
	LOGD("Didn't expect to be here");
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
	//LOGI("@@@ stored buffers from %d to %d", uint32_t(startTime), uint32_t(time));
}

voAudioRender::voAudioRender()
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

    JNILOGI("voAudio Render construct");
}

voAudioRender::~voAudioRender()
{
    close();
}

void voAudioRender::setMinBufferCount()
{
    char value[PROPERTY_VALUE_MAX];
    if (property_get("ro.kernel.qemu", value, 0)) {
        mIsOnEmulator = true;
        mMinBufferCount = 12;  // to prevent systematic buffer underrun for emulator
    }
}

bool voAudioRender::isOnEmulator()
{
    setMinBufferCount();
    return mIsOnEmulator;
}

int voAudioRender::getMinBufferCount()
{
    setMinBufferCount();
    return mMinBufferCount;
}

ssize_t voAudioRender::bufferSize() const
{
    if (mTrack == 0) return NO_INIT;
    return mTrack->frameCount() * frameSize();
}

ssize_t voAudioRender::frameCount() const
{
    if (mTrack == 0) return NO_INIT;
    return mTrack->frameCount();
}

ssize_t voAudioRender::channelCount() const
{
    if (mTrack == 0) return NO_INIT;
    return mTrack->channelCount();
}

ssize_t voAudioRender::frameSize() const
{
    if (mTrack == 0) return NO_INIT;
    return mTrack->frameSize();
}

uint32_t voAudioRender::latency () const
{
    return mLatency;
}

float voAudioRender::msecsPerFrame() const
{
    return mMsecsPerFrame;
}

status_t voAudioRender::open(
        uint32_t sampleRate, int channelCount, int format, int bufferCount,
        AudioCallback cb, void *cookie)
{
    mCallback = cb;
    mCallbackCookie = cookie;

    // Check argument "bufferCount" against the mininum buffer count
    if (bufferCount < mMinBufferCount) {
        LOGD("bufferCount (%d) is too small and increased to %d", bufferCount, mMinBufferCount);
        bufferCount = mMinBufferCount;

    }
    LOGV("open(%u, %d, %d, %d)", sampleRate, channelCount, format, bufferCount);
    if (mTrack) close();
    int afSampleRate;
    int afFrameCount;
    int frameCount;

    if (AudioSystem::getOutputFrameCount(&afFrameCount, mStreamType) != NO_ERROR) {
        return NO_INIT;
    }
    if (AudioSystem::getOutputSamplingRate(&afSampleRate, mStreamType) != NO_ERROR) {
        return NO_INIT;
    }

    frameCount = (sampleRate*afFrameCount*bufferCount)/afSampleRate;

    AudioTrack *t;
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

    if ((t == 0) || (t->initCheck() != NO_ERROR)) {
        LOGE("Unable to create audio track");
        delete t;
        return NO_INIT;
    }

    LOGV("setVolume");
    t->setVolume(mLeftVolume, mRightVolume);
    mMsecsPerFrame = 1.e3 / (float) sampleRate;
    mLatency = t->latency() + kAudioVideoDelayMs;
    mTrack = t;
    return NO_ERROR;
}

void voAudioRender::start()
{
    LOGV("start");
    if (mTrack) {
        mTrack->setVolume(mLeftVolume, mRightVolume);
        mTrack->start();
        mTrack->getPosition(&mNumFramesWritten);
    }
}

ssize_t voAudioRender::write(const void* buffer, size_t size)
{
    LOG_FATAL_IF(mCallback != NULL, "Don't call write if supplying a callback.");

    //LOGV("write(%p, %u)", buffer, size);
    if (mTrack) {
        // Only make visualization buffers if anyone recently requested visualization data
        uint64_t now = uptimeMillis();
        if (lastReadTime + TOTALBUFTIMEMSEC >= now) {
            // Based on the current play counter, the number of frames written and
            // the current real time we can calculate the approximate real start
            // time of the buffer we're about to write.
            uint32_t pos;
            mTrack->getPosition(&pos);

            // we're writing ahead by this many frames:
            int ahead = mNumFramesWritten - pos;
            //LOGI("@@@ written: %d, playpos: %d, latency: %d", mNumFramesWritten, pos, mTrack->latency());
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

void voAudioRender::stop()
{
    LOGV("stop");
    if (mTrack) mTrack->stop();
    lastWriteTime = 0;
}

void voAudioRender::flush()
{
    LOGV("flush");
    if (mTrack) mTrack->flush();
}

void voAudioRender::pause()
{
    LOGV("pause");
    if (mTrack) mTrack->pause();
    lastWriteTime = 0;
}

void voAudioRender::close()
{
    LOGV("close");
    delete mTrack;
    mTrack = 0;
}

void voAudioRender::setVolume(float left, float right)
{
    LOGV("setVolume(%f, %f)", left, right);
    mLeftVolume = left;
    mRightVolume = right;
    if (mTrack) {
        mTrack->setVolume(left, right);
    }
}

// static
void voAudioRender::CallbackWrapper(
        int event, void *cookie, void *info) {
    if (event != AudioTrack::EVENT_MORE_DATA) {
        return;
    }

    voAudioRender *me = (voAudioRender *)cookie;
    AudioTrack::Buffer *buffer = (AudioTrack::Buffer *)info;

    (*me->mCallback)(
            me, buffer->raw, buffer->size, me->mCallbackCookie);
}

status_t voAudioRender::dump(int fd, const Vector<String16>& args) const
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;

    result.append(" AudioOutput\n");
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
    return NO_ERROR;
}

} // namespace android

