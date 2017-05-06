
#define LOG_TAG "vomsmrender"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utils/Errors.h>
#include <utils/Log.h>
#include "voOSFunc.h"
#include "voIndex.h"

extern "C" {
#include <linux/msm_audio.h>
}

#include "vomsmrender.h"

#define VOINFO(format, ...) { \
	LOGI("[david] %s()->%d: " format, __FUNCTION__, __LINE__, __VA_ARGS__); }

vomsmrender::vomsmrender()
	: m_fd(-1)
	, m_nminbufsize(2048)
	, m_nminbuftime(0)
	, m_nstartcount(AUDIO_HW_NUM_OUT_BUF)
	, m_bstarted(false)
	, m_pvoaparam(NULL)
	, m_pBuffer (NULL)
	, m_nBuffSize (0)
	, m_nRenderNum (0)
{
}

vomsmrender::~vomsmrender() {

	Stop();
	close();

	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
}

bool vomsmrender::open()
{
	if (m_fd > 0)
		close();

	VOINFO("system(su): %d\n", system("su"));
	uid_t uid = getuid();
	uid_t euid = geteuid();
	gid_t gid = getgid();
	gid_t egid = getegid();
	VOINFO("uid: %d, gid: %d, euid: %d, egid: %d\n", uid, gid, euid, egid);
	if (setuid(1000) != 0) 
		VOINFO("set uid 1000 failed: %s\n", strerror(errno));

	if (setgid(1005) != 0)
		VOINFO("set gid 1005 failed: %s\n", strerror(errno));

	m_fd = ::open("/dev/msm_pcm_out", O_RDWR);
	if (m_fd < 0) {
		VOINFO("open audio device failed due to %s", strerror(errno));
		return false;
	}

	struct msm_audio_config config;
	int status = 0;
	status = ::ioctl(m_fd, AUDIO_GET_CONFIG, &config);
	if (status < 0) {
		VOINFO("%s\n", "Cannot read config");
		return false;	
	}

	config.channel_count = m_pvoaparam->wchannels;
	config.sample_rate = m_pvoaparam->dwsamplerate;
	config.buffer_size = m_nminbufsize; //m_pvoaparam->dwAvgBytesPerSec;
	config.buffer_count = AUDIO_HW_NUM_OUT_BUF;
	//config.codec_type = CODEC_TYPE_PCM;
	config.type = CODEC_TYPE_PCM;
	status = ::ioctl(m_fd, AUDIO_SET_CONFIG, &config);
	if (status < 0) {
		VOINFO("%s\n", "Cannot set config");
		return false;	
	}

	m_nminbuftime = m_nminbufsize * 2 * 1000 / m_pvoaparam->dwAvgBytesPerSec;
	m_bstarted = false;
  m_nstartcount = AUDIO_HW_NUM_OUT_BUF;
	return true;
}

void vomsmrender::close() {

	if (m_fd > 0) {
		::ioctl(m_fd, AUDIO_FLUSH, 0);
		::close(m_fd);
		m_fd = -1;
	}

	m_bstarted = false;
  m_nstartcount = AUDIO_HW_NUM_OUT_BUF;
}


VO_U32 vomsmrender::SetFormat(VO_AUDIO_FORMAT * pFormat) {
	
	if (m_pvoaparam != NULL) {
		delete m_pvoaparam;
		m_pvoaparam = NULL;
	}

	m_pvoaparam = new VOAUDIOPARAM;

	if (NULL == m_pvoaparam) {
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;
	}

	m_pvoaparam->wchannels = pFormat->Channels;
	m_pvoaparam->wbitspersample = pFormat->SampleBits;
	m_pvoaparam->dwsamplerate = pFormat->SampleRate;
	m_pvoaparam->wBlockAlign = pFormat->Channels * pFormat->SampleBits / 8;
	m_pvoaparam->dwAvgBytesPerSec = m_pvoaparam->wBlockAlign * pFormat->SampleRate;

	unsigned long dwSampleRate = m_pvoaparam->dwsamplerate;
	if (dwSampleRate <= 8000)
		m_pvoaparam->dwsamplerate = 8000;
	else if (dwSampleRate <= 11025)
		m_pvoaparam->dwsamplerate = 11025;
	else if (dwSampleRate <= 12000)
		m_pvoaparam->dwsamplerate = 12000;
	else if (dwSampleRate <= 16000)
		m_pvoaparam->dwsamplerate = 16000;
	else if (dwSampleRate <= 22050)
		m_pvoaparam->dwsamplerate = 22050;
	else if (dwSampleRate <= 24000)
		m_pvoaparam->dwsamplerate = 24000;
	else if (dwSampleRate <= 32000)
		m_pvoaparam->dwsamplerate = 32000;
	else if (dwSampleRate <= 44100)
		m_pvoaparam->dwsamplerate = 44100;
	else if (dwSampleRate <= 48000)
		m_pvoaparam->dwsamplerate = 48000;

	if (m_pvoaparam->wchannels > 2)
		m_pvoaparam->wchannels = 2;
	else if (m_pvoaparam->wchannels <= 0)
		m_pvoaparam->wchannels = 1;

	if (m_status == VOARSTS_RUNNING)
		close();

	if (!open())
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOARSTS_RUNNING)
		Start();

	return VO_ERR_NONE;
}

VO_U32 vomsmrender::Start (void) {

	if (m_fd < 0)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOARSTS_RUNNING)
		return VO_ERR_NONE;

	m_nstartplay = m_nwallclock = 0;
	m_status = VOARSTS_RUNNING;
	return VO_ERR_NONE;
}

VO_U32 vomsmrender::Pause (void) {
	if (m_fd < 0)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOARSTS_PAUSED)
		return VO_ERR_NONE;

	if (m_status != VOARSTS_RUNNING)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	m_status = VOARSTS_PAUSED;
	return VO_ERR_NONE;
}

VO_U32 vomsmrender::Flush (void) {

	voCAutoLock lockTime (&m_csTime);
	m_status = VOARSTS_FLUSHED;

	::ioctl(m_fd, AUDIO_FLUSH, 0);

	m_bstarted = false;
  m_nstartcount = AUDIO_HW_NUM_OUT_BUF;

	m_nstartplay = m_nwallclock = 0;
	return VO_ERR_NONE;
}

VO_U32 vomsmrender::Stop (void) {
	voCAutoLock lockTime (&m_csTime);
	if (m_fd < 0)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOARSTS_STOPPED)
		return VO_ERR_NONE;

	::ioctl(m_fd, AUDIO_STOP, 0);
	m_status = VOARSTS_STOPPED;
	m_nstartplay = m_nwallclock = 0;
	return VO_ERR_NONE;
}

VO_U32 vomsmrender::Render(VO_PBYTE buffer, VO_U32 size, VO_U64 nStart) {

	if (m_status != VOARSTS_RUNNING)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_nRenderNum == 0 && m_nBuffSize + size < m_nminbufsize * 2)
	{
		if (m_pBuffer == NULL)
		{
			m_pBuffer = new VO_BYTE[m_nminbufsize * 2];
		}

		memcpy(m_pBuffer + m_nBuffSize, buffer, size);
		m_nBuffSize = m_nBuffSize + size;
		return VO_ERR_NONE;
	}

	if (m_nBuffSize > 0 && m_nBuffSize < m_nminbufsize * 2)
	{
		VO_U32 nRestSize = m_nminbufsize * 2 - m_nBuffSize;

		memcpy(m_pBuffer + m_nBuffSize, buffer, nRestSize);
		m_nBuffSize = m_nBuffSize + nRestSize;

		buffer = buffer + nRestSize;
		size = size - nRestSize;
	}

	if (m_nBuffSize > 0)
	{
		RenderData (m_pBuffer, m_nBuffSize, nStart); 
		m_nBuffSize = 0;
	}

	if (size > 0)
		return RenderData (buffer, size, nStart);
	else
		return VO_ERR_NONE;
}

VO_U32 vomsmrender::RenderData (VO_PBYTE buffer, VO_U32 size, VO_U64 nStart)
{
	voCAutoLock lockTime (&m_csTime);

	VO_U32 count = size / m_nminbufsize;
	VO_U32 left = size % m_nminbufsize;
	VO_PBYTE p = buffer;

	int written = 0;
	m_nstartplay = nStart;
	m_nwallclock = voOS_GetSysTime();
	// fill 2 buffers before AUDIO_START
	while (count > 0)
	{
		written = ::write(m_fd, p, m_nminbufsize);
		if (written >= 0)
		{
			count -= 1;//written;
			p += written;
		}
		else
		{
			if (errno != EAGAIN)
			{
				return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;
			}
			voOS_Sleep(5);
			VOINFO("%s\n", "EAGAIN - retry");
		}

		// start audio after we fill 2 buffers
		if (m_nstartcount && (--m_nstartcount == 0))
		{
			int s = ::ioctl(m_fd, AUDIO_START, 0);
			if (s == 0)
				m_bstarted = true;
		} 
	} // end while

	if (left > 0)
		written = ::write(m_fd, p, left);

	m_nRenderNum++;

	return VO_ERR_NONE;
}

VO_U32 vomsmrender::GetPlayingTime (VO_S64* pPlayingTime) {

	if (m_bstarted) {
		*pPlayingTime = m_nstartplay + voOS_GetSysTime() - m_nwallclock;
	} else {
		*pPlayingTime = m_nstartplay;
	}
	return VO_ERR_NONE;
}

VO_U32 vomsmrender::GetBufferTime (VO_S32	* pBufferTime) {

	*pBufferTime = 0;//m_nminbuftime;
	return VO_ERR_NONE;
}

VO_U32 vomsmrender::SetParam (VO_U32 nID, VO_PTR pValue) {
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_AUDIO;
}

VO_U32 vomsmrender::GetParam (VO_U32 nID, VO_PTR pValue) {
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_AUDIO;
}

