	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CALSARender.cpp

	Contains:	CALSARender class file

	Written by:	David

	Created: 2009-05-06
*******************************************************************************/

#include "CALSARender.h"

#define VOALSA_HWERR_RETURN\
	if (rc < 0) { \
		if (m_phwparams) { \
			snd_pcm_hw_params_free(m_phwparams); \
			m_phwparams = NULL; \
		} \
		return -1; \
	}

#define VOALSA_SWERR_RETURN\
	if (rc < 0) { \
		if (m_pswparams != NULL) { \
			snd_pcm_sw_params_free(m_pswparams); \
			m_pswparams = NULL; \
		} \
		return -1; \
	}


CALSARender::CALSARender(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
				: CBaseAudioRender (hInst, pMemOP)
				, m_hdev(NULL)
				, m_hmixer(NULL)
				, m_pmixerelem(NULL)
				, m_buffersize(0)
				, m_periodsize(0)
				, m_phwparams(NULL)
				, m_pswparams(NULL)
				, m_pmonobuf(NULL)
				, m_bstereo4mono(false)
				, m_status(VOARSTS_STOPPED)
				, m_pvoaparam(NULL)
				, m_nPlayingTime(0)
				, m_wsamplesize(0)
{

	m_szswdev = new char[8];
	memset(m_szswdev, 0, 8);
	strcpy(m_szswdev, "default");

	m_szswctl = new char[8];
	memset(m_szswctl, 0, 8);
	strcpy(m_szswctl, "Master");
}

CALSARender::~CALSARender () {

	Stop();
	CloseDevice();

	if (NULL != m_szswdev) {
		delete m_szswdev;
		m_szswdev = NULL;
	}

	if (NULL != m_szswctl) {
		delete m_szswctl;
		m_szswctl = NULL;
	}
	if(m_pvoaparam)
	{
		if(m_pMemOP)
		{
			m_pMemOP->Free(VO_INDEX_SNK_AUDIO,m_pvoaparam);
			m_pvoaparam = NULL;
		}
	}
}

VO_U32 CALSARender::SetFormat (VO_AUDIO_FORMAT * pFormat) {
	
	if (m_pvoaparam != NULL) {
		delete m_pvoaparam;
		m_pvoaparam = NULL;
	}

	//m_pvoaparam = new VOAUDIOPARAM();
	VO_MEM_INFO	mi;
	mi.Size = sizeof(VOAUDIOPARAM);
	m_pMemOP->Alloc(VO_INDEX_SNK_AUDIO, &mi);
	m_pvoaparam = (PVOAUDIOPARAM)mi.VBuffer;

	if (NULL == m_pvoaparam) {
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;
	}

	m_pvoaparam->wchannels = pFormat->Channels;
	m_pvoaparam->wbitspersample = pFormat->SampleBits;
	m_pvoaparam->dwsamplerate = pFormat->SampleRate;
	m_pvoaparam->wBlockAlign = pFormat->Channels * pFormat->SampleBits / 8;
	m_pvoaparam->dwAvgBytesPerSec = m_pvoaparam->wBlockAlign * pFormat->SampleRate;
	m_pvoaparam->eendiantype = VO_EndianLittle;
	m_pvoaparam->edatasigned =  VO_NumericalDataSigned;
	m_pvoaparam->binterleaved = true;
	m_pvoaparam->epcmmode = VO_AUDIO_PCMModeLinear;
	m_wsamplesize = (m_pvoaparam->wbitspersample * m_pvoaparam->wchannels) >> 3;

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
		CloseDevice();

	if (!InitDevice())
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOARSTS_RUNNING)
		Start();

	return VO_ERR_NONE;
}

VO_U32 CALSARender::Start (void) {

	if (m_hdev == NULL)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOARSTS_RUNNING)
		return VO_ERR_NONE;

	snd_pcm_start(m_hdev);
	snd_pcm_prepare(m_hdev);

	//m_dwSysTime = 0;
	//m_nBufferTime = 0;
	m_nPlayingTime = 0;
	/*if (m_arstatus == VOARSTS_STOPPED)
		snd_pcm_start(m_hdev);
		else if (m_arstatus == VOARSTS_PAUSED)
		snd_pcm_pause(m_hdev, 0);*/

	m_status = VOARSTS_RUNNING;
	return VO_ERR_NONE;
}

VO_U32 CALSARender::Pause (void) {

	if (m_hdev == NULL)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOARSTS_PAUSED)
		return VO_ERR_NONE;

	if (m_status != VOARSTS_RUNNING)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	m_status = VOARSTS_PAUSED;
	return VO_ERR_NONE;
}

VO_U32 CALSARender::Stop (void) {
	if (m_hdev == NULL)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOARSTS_STOPPED)
		return VO_ERR_NONE;

	snd_pcm_drain(m_hdev);
	m_status = VOARSTS_STOPPED;

	return VO_ERR_NONE;
}

VO_U32 CALSARender::Render(VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait) {

	if (m_status != VOARSTS_RUNNING)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_bstereo4mono) {
		mono2stero(m_pmonobuf, pBuffer, nSize);
		pBuffer = m_pmonobuf;
		nSize *= 2;
	}

	m_nPlayingTime = nStart + nSize * 1000 / m_pvoaparam->dwAvgBytesPerSec;
	output(pBuffer, nSize);

	return VO_ERR_NONE;
}

VO_U32 CALSARender::Flush (void) {

	m_status = VOARSTS_FLUSHED;
	snd_pcm_reset(m_hdev);

	voCAutoLock lockTime (&m_csTime);
	m_nPlayingTime = 0;

	return VO_ERR_NONE;
}

#include <sys/time.h>
static inline unsigned long timeGetTime() {
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
}

VO_U32 CALSARender::GetPlayingTime (VO_S64	* pPlayingTime)
{
	voCAutoLock lockTime (&m_csTime);

	int delay = delayed();
	*pPlayingTime = m_nPlayingTime - delay * 1000 / m_pvoaparam->dwAvgBytesPerSec;

	return VO_ERR_NONE;
}

VO_U32 CALSARender::GetBufferTime (VO_S32 * pBufferTime)
{
	voCAutoLock lockTime (&m_csTime);
	*pBufferTime = delayed() * 1000 / m_pvoaparam->dwAvgBytesPerSec;
	//VOINFON(*pBufferTime);
	return VO_ERR_NONE;
}

VO_U32 CALSARender::SetParam (VO_U32 nID, VO_PTR pValue)
{
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_AUDIO;
}

VO_U32 CALSARender::GetParam (VO_U32 nID, VO_PTR pValue)
{
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_AUDIO;
}


bool CALSARender::InitDevice (void) {

	CloseDevice();

	/*int err;
	snd_ctl_t *handle_ctl;
	if ((err = snd_ctl_open(&handle_ctl, "default", 0)) < 0) {
		printf("control open: %s", snd_strerror(err));
		return -1;
	}
	err = snd_ctl_pcm_prefer_subdevice(handle_ctl, 0);
	if (err < 0) {
		printf("control open: %s", snd_strerror(err));
		return -1;
	}*/

	//VOINFO(m_szswdev);
	if (snd_pcm_open(&m_hdev, m_szswdev, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		VOINFO("cannot open audio device");
		return -1;
	}

	if (setprm() < 0) {
		VOINFO("set params error");
		return -1;
	}

	int rc = 0;
	rc = snd_pcm_prepare(m_hdev);
	if (rc < 0) {
		VOINFO("%s", snd_strerror(rc));
		return -1;
	}

  /*if (snd_mixer_open(&m_hmixer, 0) < 0) {
    VOINFO("failed to open mixer");
		return -1;
	}

  if (snd_mixer_attach(m_hmixer, "default") < 0) {
    VOINFO("failed to attach mixer");
		return -1;
	}

  if ((snd_mixer_selem_register(m_hmixer, NULL, NULL)) < 0) {
    VOINFO ("failed to snd_mixer_selem_register");
		return -1;
	}

	snd_mixer_load(m_hmixer);

  snd_mixer_selem_id_t *psid;
	snd_mixer_selem_id_alloca(&psid);
	snd_mixer_selem_id_set_name(psid, m_szswctl);
	m_pmixerelem = snd_mixer_find_selem(m_hmixer, psid);*/
	return true;
}

bool CALSARender::CloseDevice (void) {

	if (m_phwparams) {
		snd_pcm_hw_params_free(m_phwparams);
		m_phwparams = NULL;
	}

	if (m_pswparams) {
		snd_pcm_sw_params_free(m_pswparams);
		m_pswparams = NULL;
	}

	if (m_hdev != NULL) {
		int rc;
		//snd_pcm_drain(m_hdev);
		if ((rc = snd_pcm_close(m_hdev)) < 0)
			VOINFO("%s", snd_strerror(rc));
		m_hdev = NULL;
	}

	if (m_hmixer != NULL) {
		snd_mixer_free(m_hmixer);
		snd_mixer_close(m_hmixer);
		m_hmixer = NULL;
		m_pmixerelem = NULL;
	}

	if (m_pmonobuf != NULL){
		delete m_pmonobuf;
		m_pmonobuf = NULL;
	}

	m_bstereo4mono = false;
	return true;
}

int CALSARender::output(const VO_PBYTE pbuf, const VO_U32 dwsize) {

	//VOINFON(dwsize);
	int written=0, offsetBuffer=0;
	int totalframes = dwsize/m_wsamplesize;
	
	while (totalframes > 0) {
		if (m_status != VOARSTS_RUNNING) {
			return written;
		}

		written = snd_pcm_writei(m_hdev, pbuf+(offsetBuffer*m_wsamplesize), totalframes);

		if (written < 0) {
			if (written == -EAGAIN) {
				VOINFO("again...");
				//snd_pcm_wait(m_hdev, 2);
				//usleep(1200);
				continue;
			} else if (written == -EPIPE) {
				//VOINFO("ALSA underrun...");
				snd_pcm_wait(m_hdev, 10);
				//snd_pcm_recover(m_hdev, written, 0);
				snd_pcm_prepare(m_hdev);
				continue;
			} else if (written == -ESTRPIPE) {
					while ((written = snd_pcm_resume(m_hdev)) == -EAGAIN) {
						VOINFO("resume error...");
						usleep(10000);       /* wait until the suspend flag is released */
					}
					if (written < 0) {
						written = snd_pcm_prepare(m_hdev);
					}
				VOINFO("Cannot send any data to the audio device");
				return -1;
			}
		}

		totalframes -= written;
		offsetBuffer += written;
	}

	return 0;
}

bool CALSARender::mono2stero(VO_PBYTE pdst, const VO_PBYTE psrc, const int nsize) {
	VO_BYTE *ps=psrc, *pd=pdst;
	for (int i=0; i<nsize/2; ++i) {
		*((short*)pd) = *((short*)ps);
		*((short*)(pd+2)) = *((short*)ps);
		ps += 2;
		pd += 4;
	}

	return true;
}

int CALSARender::delayed() {
	int rc = 0;
	snd_pcm_sframes_t fdelayed = 0;
	if ((rc = snd_pcm_delay(m_hdev, &fdelayed)) < 0) {
		VOINFO("%s", snd_strerror(rc));
		//return -1;
	}

	//snd_pcm_avail_update(m_hdev);
	return m_wsamplesize*fdelayed;
}

int CALSARender::setprm() {
	if (sethwparams() != 0) 
		return -1;

	if (setswparams() != 0)
		return -1;

	return 0;
}

int CALSARender::sethwparams() {

	int rc = snd_pcm_hw_params_malloc(&m_phwparams);
	VOALSA_HWERR_RETURN;

	rc = snd_pcm_hw_params_any(m_hdev, m_phwparams);
	VOALSA_HWERR_RETURN;

	rc = snd_pcm_hw_params_set_rate_resample(m_hdev, m_phwparams, 1);
	VOALSA_HWERR_RETURN;

	snd_pcm_access_t access;
	if (m_pvoaparam->binterleaved)
		access = SND_PCM_ACCESS_RW_INTERLEAVED;
	else
		access = SND_PCM_ACCESS_RW_NONINTERLEAVED;
	rc = snd_pcm_hw_params_set_access(m_hdev, m_phwparams, access);
	VOALSA_HWERR_RETURN;

	//set the sample format
	snd_pcm_format_t format = getfmt();
	rc=snd_pcm_hw_params_set_format(m_hdev, m_phwparams, format);
	VOALSA_HWERR_RETURN;

	m_bstereo4mono = false;
	unsigned int dwchannels = 2;//always stereo
	rc = snd_pcm_hw_params_set_channels(m_hdev, m_phwparams, dwchannels);
	VOALSA_HWERR_RETURN;

	if (1 == m_pvoaparam->wchannels) {
		m_bstereo4mono = true;
		printf("stero for mono...");
	}

	if (m_bstereo4mono) {
		if (NULL != m_pmonobuf) {
			delete m_pmonobuf;
			m_pmonobuf = NULL;
		}

		m_pmonobuf = new VO_BYTE[m_pvoaparam->dwAvgBytesPerSec*2];
		if (m_pmonobuf == NULL)
			printf("no enough memory");

		m_pvoaparam->dwAvgBytesPerSec *= 2;
	}

	int dir = 0;
	unsigned int dwrate = m_pvoaparam->dwsamplerate;
	
#if 1
	rc = snd_pcm_hw_params_set_rate(m_hdev, m_phwparams, dwrate, dir);
#else
	rc = snd_pcm_hw_params_set_rate_near(m_hdev, m_phwparams,(PUINT)&dwrate, 0);
#endif
	VOALSA_HWERR_RETURN;

	snd_pcm_uframes_t max = m_pvoaparam->dwsamplerate;
	snd_pcm_uframes_t min = m_pvoaparam->dwsamplerate / 5;
	snd_pcm_hw_params_set_buffer_size_minmax(m_hdev, m_phwparams, &max, &min);
	VOALSA_HWERR_RETURN;

	rc = snd_pcm_hw_params(m_hdev, m_phwparams);
	VOALSA_HWERR_RETURN;


	snd_pcm_uframes_t size = m_pvoaparam->dwsamplerate;
	rc = snd_pcm_hw_params_get_buffer_size(m_phwparams, &size);
	VOALSA_HWERR_RETURN;
	m_buffersize = size;
	//VOINFON(m_buffersize);

	dir = 0;
	rc = snd_pcm_hw_params_get_period_size(m_phwparams, &size, &dir);
	VOALSA_HWERR_RETURN;
	//VOINFON(size);
	m_periodsize = size;

	if (m_phwparams) {
		snd_pcm_hw_params_free(m_phwparams);
		m_phwparams = NULL;
	}

	return 0;
}

int CALSARender::setswparams() {

	int rc = snd_pcm_sw_params_malloc(&m_pswparams);
	VOALSA_SWERR_RETURN;

	rc = snd_pcm_sw_params_current(m_hdev, m_pswparams);
	VOALSA_SWERR_RETURN;

	snd_pcm_uframes_t val = m_buffersize / 5;
	rc = snd_pcm_sw_params_set_start_threshold(m_hdev, m_pswparams, val);
	VOALSA_SWERR_RETURN;

	rc = snd_pcm_sw_params_set_avail_min(m_hdev, m_pswparams, 1);
	//rc = snd_pcm_sw_params_set_avail_min(m_hdev, m_pswparams, 8192);
	//rc = snd_pcm_sw_params_set_avail_min(m_hdev, m_pswparams, m_periodsize);
	VOALSA_SWERR_RETURN;

	rc = snd_pcm_sw_params(m_hdev, m_pswparams);
	VOALSA_SWERR_RETURN;

	rc = snd_pcm_sw_params_current(m_hdev, m_pswparams);
	VOALSA_SWERR_RETURN;

	if (m_pswparams) {
		snd_pcm_sw_params_free(m_pswparams);
		m_pswparams = NULL;
	}

	return 0;
}

snd_pcm_format_t CALSARender::getfmt() {

	snd_pcm_format_t snd_pcm_format;
	switch (m_pvoaparam->wbitspersample) {
		case 8:
			if (m_pvoaparam->edatasigned == VO_NumericalDataSigned)
				snd_pcm_format = SND_PCM_FORMAT_S8;
			else
				snd_pcm_format = SND_PCM_FORMAT_U8;
			break;

		case 16:
			if (m_pvoaparam->edatasigned == VO_NumericalDataSigned) {
				if (m_pvoaparam->eendiantype ==  VO_EndianLittle)
					snd_pcm_format = SND_PCM_FORMAT_S16_LE;
				else
					snd_pcm_format = SND_PCM_FORMAT_S16_BE;
			}else if (m_pvoaparam->edatasigned == VO_NumericalDataUnsigned) {
				if (m_pvoaparam->eendiantype ==  VO_EndianLittle)
					snd_pcm_format = SND_PCM_FORMAT_U16_LE;
				else
					snd_pcm_format = SND_PCM_FORMAT_U16_BE;
			}
			break;

		case 24:
			if (m_pvoaparam->edatasigned == VO_NumericalDataSigned) {
				if (m_pvoaparam->eendiantype ==  VO_EndianLittle)
					snd_pcm_format = SND_PCM_FORMAT_S24_LE;
				else
					snd_pcm_format = SND_PCM_FORMAT_S24_BE; 
			}else if (m_pvoaparam->edatasigned == VO_NumericalDataUnsigned) {
				if (m_pvoaparam->eendiantype ==  VO_EndianLittle)
					snd_pcm_format = SND_PCM_FORMAT_U24_LE;
				else
					snd_pcm_format = SND_PCM_FORMAT_U24_BE;
			}
			break;

		case 32:
			if (m_pvoaparam->edatasigned == VO_NumericalDataSigned) {
				if (m_pvoaparam->eendiantype ==  VO_EndianLittle)
					snd_pcm_format = SND_PCM_FORMAT_S32_LE;
				else
					snd_pcm_format = SND_PCM_FORMAT_S32_BE;
			} else if (m_pvoaparam->edatasigned == VO_NumericalDataUnsigned) {
				if (m_pvoaparam->eendiantype ==  VO_EndianLittle)
					snd_pcm_format = SND_PCM_FORMAT_U32_LE;
				else
					snd_pcm_format = SND_PCM_FORMAT_U32_BE;
			}
			break;

			default:
				break;
	}
	
	return snd_pcm_format;
}
