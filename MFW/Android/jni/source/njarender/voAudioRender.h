
#include <sys/types.h>
#include <utils/RefBase.h>
#include <utils/Errors.h>
#include <media/AudioSystem.h>
#include <media/AudioTrack.h>
#include <binder/MemoryHeapBase.h>

#include <media/IMediaPlayerService.h>
#include <media/MediaPlayerInterface.h>

namespace android {

#define DEFAULT_AUDIOSINK_BUFFERCOUNT 4
#define DEFAULT_AUDIOSINK_BUFFERSIZE 1200
#define DEFAULT_AUDIOSINK_SAMPLERATE 44100

class voAudioRender : public RefBase
{
        typedef void (*AudioCallback)(
                voAudioRender *audioSink, void *buffer, size_t size, void *cookie);
public:
                            voAudioRender();
    virtual                 ~voAudioRender();

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
    static void             CallbackWrapper(int event, void *me, void *info);

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

}; // namespace android
