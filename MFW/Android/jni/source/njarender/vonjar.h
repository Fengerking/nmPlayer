
#include <sys/types.h>
#include <utils/RefBase.h>
#include <utils/Errors.h>
#include <media/AudioSystem.h>
#include <media/AudioTrack.h>
#if defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
#include <utils/MemoryHeapBase.h>
#elif defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__
#include <binder/MemoryHeapBase.h>
#endif

namespace android {

#define DEFAULT_AUDIOSINK_BUFFERCOUNT 4
#define DEFAULT_AUDIOSINK_BUFFERSIZE 1200
#define DEFAULT_AUDIOSINK_SAMPLERATE 44100

// ----------------------------------------------------------------------------

class VOAudioSink : public RefBase {
public:
	typedef void (*AudioCallback)(VOAudioSink *audioSink, void *buffer, size_t size, void *cookie);

	virtual             ~VOAudioSink() {}
	virtual bool        ready() const = 0; // audio output is open and ready
	virtual bool        realtime() const = 0; // audio output is real-time output
	virtual ssize_t     bufferSize() const = 0;
	virtual ssize_t     frameCount() const = 0;
	virtual ssize_t     channelCount() const = 0;
	virtual ssize_t     frameSize() const = 0;
	virtual uint32_t    latency() const = 0;
	virtual float       msecsPerFrame() const = 0;

	// If no callback is specified, use the "write" API below to submit
	// audio data. Otherwise return a full buffer of audio data on each
	// callback.
	virtual status_t    open(
			uint32_t sampleRate, int channelCount,
			int format=AudioSystem::PCM_16_BIT,
			int bufferCount=DEFAULT_AUDIOSINK_BUFFERCOUNT,
			AudioCallback cb = NULL,
			void *cookie = NULL) = 0;

	virtual void        start() = 0;
	virtual ssize_t     write(const void* buffer, size_t size) = 0;
	virtual void        stop() = 0;
	virtual void        flush() = 0;
	virtual void        pause() = 0;
	virtual void        close() = 0;
};

class VOAudioOutput : public VOAudioSink {
public:
	VOAudioOutput();
	virtual                 ~VOAudioOutput();
	virtual bool            ready() const { return mTrack != NULL; }
	virtual bool            realtime() const { return true; }
	virtual ssize_t         bufferSize() const;
	virtual ssize_t         frameCount() const;
	virtual ssize_t         channelCount() const;
	virtual ssize_t         frameSize() const;
	virtual uint32_t        latency() const;
	virtual float           msecsPerFrame() const;

	virtual status_t        open(
			uint32_t sampleRate, int channelCount,
			int format, int bufferCount,
			AudioCallback cb, void *cookie);

	virtual void            start();
	virtual ssize_t         write(const void* buffer, size_t size);
	virtual void            stop();
	virtual void            flush();
	virtual void            pause();
	virtual void            close();
	void            setAudioStreamType(int streamType) { mStreamType = streamType; }
	void            setVolume(float left, float right);
	virtual status_t        dump(int fd, const Vector<String16>& args) const;

	static bool             isOnEmulator();
	static int              getMinBufferCount();
private:
	static void             setMinBufferCount();
	static void             CallbackWrapper(
			int event, void *me, void *info);

	AudioTrack*             mTrack;
	AudioCallback           mCallback;
	void *                  mCallbackCookie;
	int                     mStreamType;
	float                   mLeftVolume;
	float                   mRightVolume;
	float                   mMsecsPerFrame;
	uint32_t                mLatency;

	// TODO: Find real cause of Audio/Video delay in PV framework and remove this workaround
	static const uint32_t   kAudioVideoDelayMs;
	static bool             mIsOnEmulator;
	static int              mMinBufferCount;  // 12 for emulator; otherwise 4

public: // visualization hack support
	uint32_t                mNumFramesWritten;
};

class VOAudioCache : public VOAudioSink {
public:
		VOAudioCache(const char* name);
		virtual                 ~VOAudioCache() {}

		virtual bool            ready() const { return (mChannelCount > 0) && (mHeap->getHeapID() > 0); }
		virtual bool            realtime() const { return false; }
		virtual ssize_t         bufferSize() const { return frameSize() * mFrameCount; }
		virtual ssize_t         frameCount() const { return mFrameCount; }
		virtual ssize_t         channelCount() const { return (ssize_t)mChannelCount; }
		virtual ssize_t         frameSize() const { return ssize_t(mChannelCount * ((mFormat == AudioSystem::PCM_16_BIT)?sizeof(int16_t):sizeof(u_int8_t))); }
		virtual uint32_t        latency() const;
		virtual float           msecsPerFrame() const;

		virtual status_t        open(
				uint32_t sampleRate, int channelCount, int format,
				int bufferCount = 1,
				AudioCallback cb = NULL, void *cookie = NULL);

		virtual void            start() {}
		virtual ssize_t         write(const void* buffer, size_t size);
		virtual void            stop() {}
		virtual void            flush() {}
		virtual void            pause() {}
		virtual void            close() {}
		void            setAudioStreamType(int streamType) {}
		void            setVolume(float left, float right) {}
		uint32_t        sampleRate() const { return mSampleRate; }
		uint32_t        format() const { return (uint32_t)mFormat; }
		size_t          size() const { return mSize; }
		status_t        wait();

		sp<IMemoryHeap> getHeap() const { return mHeap; }

		static  void            notify(void* cookie, int msg, int ext1, int ext2);
		virtual status_t        dump(int fd, const Vector<String16>& args) const;

	private:
		VOAudioCache();

		Mutex               mLock;
		Condition           mSignal;
		sp<MemoryHeapBase>  mHeap;
		float               mMsecsPerFrame;
		uint16_t            mChannelCount;
		uint16_t            mFormat;
		ssize_t             mFrameCount;
		uint32_t            mSampleRate;
		uint32_t            mSize;
		int                 mError;
		bool                mCommandComplete;
};

}; // namespace android
