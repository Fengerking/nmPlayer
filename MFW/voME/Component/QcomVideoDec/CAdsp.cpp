	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAdsp.cpp

	Contains:	QCOM ADSP wrapper class file

	Written by:	East Zhou

	Change History (most recent first):
	2010-06-18		East		Create file

*******************************************************************************/
#include "CAdsp.h"

#ifdef _LINUX_ANDROID
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/msm_adsp.h>
#include <linux/msm_q6vdec.h>
#include <fcntl.h>
#include <errno.h>
#endif	//_LINUX_ANDROID

#define LOG_TAG "CAdsp"
#include "voLog.h"

CAdsp::CAdsp()
	: m_nFD(0)
	, m_nCpuDmaFD(0)
	, m_nCpuDmaFreq(1000)
{
	memset(&m_BufferRequest, 0, sizeof(m_BufferRequest));
}

CAdsp::~CAdsp()
{
}

VO_BOOL CAdsp::Open()
{
	VOLOGR("CAdsp::Open");

#ifdef _LINUX_ANDROID
	m_nFD = open("/dev/vdec", O_RDWR);
	if(m_nFD < 0)
	{
		VOLOGE("adsp: cannot open vdec, fd: %d, (%s)", m_nFD, strerror(errno));

		return VO_FALSE;
	}

	m_nCpuDmaFD = open("/dev/cpu_dma_latency", O_RDWR);
	if(m_nCpuDmaFD < 0)
	{
		VOLOGW("adsp: cannot open cpu_dma_latency, fd: %d (%s)", m_nCpuDmaFD, strerror(errno));
	}
	else
	{
		if(read(m_nCpuDmaFD, &m_nCpuDmaFreq, sizeof(m_nCpuDmaFreq)) < 0)
		{
			VOLOGW("adsp: Request cpu_dma_freq read failed");
		}

		VO_U32 nCpuDmaFreq = 1000;
		if(write(m_nCpuDmaFD, &nCpuDmaFreq, sizeof(nCpuDmaFreq)) < 0)
		{
			VOLOGW("adsp: Request cpu_dma_freq write failed");
		}
	}

#endif	//_LINUX_ANDROID
	return VO_TRUE;
}

VO_VOID CAdsp::Close()
{
	VOLOGR("CAdsp::Close");

#ifdef _LINUX_ANDROID
	if(m_nFD > 0)
	{
		if(close(m_nFD) < 0)
		{
			VOLOGW("*************close vdec ERROR!");
		}
		m_nFD = 0;
	}

	if(m_nCpuDmaFD > 0)
	{
		if(write(m_nCpuDmaFD, &m_nCpuDmaFreq, sizeof(m_nCpuDmaFreq)) < 0)
		{
			VOLOGW("ERROR - adsp: Request cpu_dma_freq write failed");
		}

		if(close(m_nCpuDmaFD) < 0)
		{
			VOLOGW("*************close cpu_dma_latency ERROR!");
		}
		m_nCpuDmaFD = 0;
	}
#endif	//_LINUX_ANDROID
}

VO_BOOL CAdsp::Start(VO_U32 nCoding, VO_U32 nFourcc, VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_U32 nWidth, VO_U32 nHeight)
{
	VOLOGR("CAdsp::Start coding %d, fourcc 0x%08X, seq header 0x%08X, seq length %d, width %d, height %d", 
		nCoding, nFourcc, pHeadData, nHeadSize, nWidth, nHeight);

#ifdef _LINUX_ANDROID
	if(m_nFD <= 0)
	{
		VOLOGE("/dev/vdec has not been opened.");
		return VO_FALSE;
	}

	vdec_init initParam;

	//config
	initParam.sps_cfg.cfg.width = nWidth;
	initParam.sps_cfg.cfg.height = nHeight;

	initParam.sps_cfg.cfg.order = 0;	//only DivX set 1, other set 0

	initParam.sps_cfg.cfg.notify_enable = 1;
	initParam.sps_cfg.cfg.fourcc = nFourcc;
	initParam.sps_cfg.cfg.vc1_rowbase = 0;

	//support H264 instead of AVC
	initParam.sps_cfg.cfg.h264_startcode_detect = 1;
	initParam.sps_cfg.cfg.h264_nal_len_size = 0;

	initParam.sps_cfg.cfg.postproc_flag = 0;
	initParam.sps_cfg.cfg.fruc_enable = 0;
	initParam.sps_cfg.cfg.reserved = 0;

	//sequence header
	initParam.sps_cfg.seq.header = pHeadData;
	initParam.sps_cfg.seq.len = nHeadSize;

	//buffer request, retrieved by HW
	vdec_buf_req bufReq;
	initParam.buf_req = &bufReq;

	//initialize...
	if(ioctl(m_nFD, VDEC_IOCTL_INITIALIZE, &initParam) < 0)
	{
		VOLOGE("VDEC_IOCTL_INITIALIZE failed");

		return VO_FALSE;
	}

	//save buffer informations
	m_BufferRequest.sInputBufferInfo.nMinNum = initParam.buf_req->input.num_min_buffers;
	m_BufferRequest.sInputBufferInfo.nMaxNum = initParam.buf_req->input.num_max_buffers;
	m_BufferRequest.sInputBufferInfo.nSize = initParam.buf_req->input.bufsize;

	m_BufferRequest.sOutputBufferInfo.nMinNum = initParam.buf_req->output.num_min_buffers;
	m_BufferRequest.sOutputBufferInfo.nMaxNum = initParam.buf_req->output.num_max_buffers;
	m_BufferRequest.sOutputBufferInfo.nSize = initParam.buf_req->output.bufsize;

	m_BufferRequest.sDecReq1BufferInfo.nMinNum = initParam.buf_req->dec_req1.num_min_buffers;
	m_BufferRequest.sDecReq1BufferInfo.nMaxNum = initParam.buf_req->dec_req1.num_max_buffers;
	m_BufferRequest.sDecReq1BufferInfo.nSize = initParam.buf_req->dec_req1.bufsize;

	m_BufferRequest.sDecReq2BufferInfo.nMinNum = initParam.buf_req->dec_req2.num_min_buffers;
	m_BufferRequest.sDecReq2BufferInfo.nMaxNum = initParam.buf_req->dec_req2.num_max_buffers;
	m_BufferRequest.sDecReq2BufferInfo.nSize = initParam.buf_req->dec_req2.bufsize;

	VOLOGR("input min %d, max %d, size %d", m_BufferRequest.sInputBufferInfo.nMinNum, m_BufferRequest.sInputBufferInfo.nMaxNum, m_BufferRequest.sInputBufferInfo.nSize);
	VOLOGR("output min %d, max %d, size %d", m_BufferRequest.sOutputBufferInfo.nMinNum, m_BufferRequest.sOutputBufferInfo.nMaxNum, m_BufferRequest.sOutputBufferInfo.nSize);
	VOLOGR("dec1 min %d, max %d, size %d", m_BufferRequest.sDecReq1BufferInfo.nMinNum, m_BufferRequest.sDecReq1BufferInfo.nMaxNum, m_BufferRequest.sDecReq1BufferInfo.nSize);
	VOLOGR("dec2 min %d, max %d, size %d", m_BufferRequest.sDecReq2BufferInfo.nMinNum, m_BufferRequest.sDecReq2BufferInfo.nMaxNum, m_BufferRequest.sDecReq2BufferInfo.nSize);
#endif	//_LINUX_ANDROID

	return VO_TRUE;
}

VO_VOID CAdsp::Stop()
{
	VOLOGR("CAdsp::Stop");

#ifdef _LINUX_ANDROID
	if(m_nFD <= 0)
		return;

	if(ioctl(m_nFD, VDEC_IOCTL_CLOSE, NULL) < 0)
	{
		VOLOGW("VDEC_IOCTL_CLOSE failed");
	}
#endif	//_LINUX_ANDROID
}

VO_VOID CAdsp::Flush(ADSP_VDEC_PORTTYPE nPort)
{
	VOLOGR("CAdsp::Flush Port %d", nPort);

#ifdef _LINUX_ANDROID
	if(m_nFD <= 0)
	{
		VOLOGE("/dev/vdec has not been opened.");
		return;
	}

	if(ioctl(m_nFD, VDEC_IOCTL_FLUSH, &nPort) < 0)
	{
		VOLOGW("VDEC_IOCTL_FLUSH Port %d failed", nPort);
	}
#endif	//_LINUX_ANDROID
}

VO_BOOL CAdsp::SetBuffers(VO_U32 nPMemID, ADSP_BUFFER_TYPE nBufferType, VO_U32 nBufferCount, VO_BOOL bIsLast, VO_U32 nBufferOffset, VO_U32 nBufferSize)
{
	VOLOGR("CAdsp::SetBuffers pmem ID 0x%08X, type %d, count %d, last %d, offset %d, size %d", 
		nPMemID, nBufferType, nBufferCount, bIsLast, nBufferOffset, nBufferSize);

#ifdef _LINUX_ANDROID
	if(m_nFD <= 0)
	{
		VOLOGE("/dev/vdec has not been opened.");
		return VO_FALSE;
	}

	vdec_buffer vdecBuffer;
	vdecBuffer.pmem_id = nPMemID;
	vdecBuffer.buf.buf_type = nBufferType;
	vdecBuffer.buf.num_buf = nBufferCount;
	vdecBuffer.buf.islast = (bIsLast == VO_TRUE) ? 1 : 0;

	vdecBuffer.buf.region.src_id = 0x0106e429;
	vdecBuffer.buf.region.offset = nBufferOffset;
	vdecBuffer.buf.region.size = nBufferSize;

	if(ioctl(m_nFD, VDEC_IOCTL_SETBUFFERS, &vdecBuffer) < 0)
	{
		VOLOGI("VDEC_IOCTL_SETBUFFERS failed");

		return VO_FALSE;
	}
#endif	//_LINUX_ANDROID

	return VO_TRUE;
}

VO_BOOL CAdsp::FreeBuffers(VO_U32 nPMemID, ADSP_BUFFER_TYPE nBufferType, VO_U32 nBufferCount, VO_BOOL bIsLast, VO_U32 nBufferOffset, VO_U32 nBufferSize)
{
	VOLOGR("CAdsp::FreeBuffers pmem ID 0x%08X, type %d, count %d, last %d, offset %d, size %d", 
		nPMemID, nBufferType, nBufferCount, bIsLast, nBufferOffset, nBufferSize);

#ifdef _LINUX_ANDROID
	if(m_nFD <= 0)
	{
		VOLOGE("/dev/vdec has not been opened.");
		return VO_FALSE;
	}

	vdec_buffer vdecBuffer;
	vdecBuffer.pmem_id = nPMemID;
	vdecBuffer.buf.buf_type = nBufferType;
	vdecBuffer.buf.num_buf = nBufferCount;
	vdecBuffer.buf.islast = (bIsLast == VO_TRUE) ? 1 : 0;

	vdecBuffer.buf.region.src_id = 0x0106e429;
	vdecBuffer.buf.region.offset = nBufferOffset;
	vdecBuffer.buf.region.size = nBufferSize;

	if(ioctl(m_nFD, VDEC_IOCTL_FREEBUFFERS, &vdecBuffer) < 0)
	{
		VOLOGE("VDEC_IOCTL_FREEBUFFERS failed");

		return VO_FALSE;
	}
#endif	//_LINUX_ANDROID

	return VO_TRUE;
}

VO_BOOL CAdsp::PostInputBuffer(VO_U32 nPMemID, VO_U32 nOffset, VO_U32 nSize, VO_S64 llTime, VO_U32 nFlags, VO_BOOL bIsEOS, VO_PTR pUserData)
{
	VOLOGR("CAdsp::PostInputBuffer pmem ID 0x%08X, offset %d, size %d, time %lld, flags 0x%08X, eos %d, userdata 0x%08X", 
		nPMemID, nOffset, nSize, llTime, nFlags, bIsEOS, pUserData);

#ifdef _LINUX_ANDROID
	if(m_nFD <= 0)
	{
		VOLOGE("/dev/vdec has not been opened.");
		return VO_FALSE;
	}

	if(bIsEOS)
	{
		if(ioctl(m_nFD, VDEC_IOCTL_EOS, NULL) < 0)
		{
			VOLOGE("VDEC_IOCTL_EOS failed");

			return VO_FALSE;
		}
	}
	else
	{
		vdec_input_buf vdecInputBuf;
		vdec_queue_status vdecQueueStatus;

		vdecInputBuf.pmem_id = nPMemID;
		vdecInputBuf.queue_status = &vdecQueueStatus;

		vdecInputBuf.buffer.avsync_state = 0;
		vdecInputBuf.buffer.data = (VO_U32)pUserData;
		vdecInputBuf.buffer.offset = nOffset;
		vdecInputBuf.buffer.size = nSize;
		vdecInputBuf.buffer.timestamp_lo = (int)(llTime & 0x00000000FFFFFFFFLL);
		vdecInputBuf.buffer.timestamp_hi = (int)((llTime & 0xFFFFFFFF00000000LL) >> 32);
		vdecInputBuf.buffer.flags = nFlags;

		if(ioctl(m_nFD, VDEC_IOCTL_QUEUE, &vdecInputBuf) < 0)
		{
			VOLOGE("VDEC_IOCTL_QUEUE failed");

			return VO_FALSE;
		}
	}
#endif	//_LINUX_ANDROID

	return VO_TRUE;
}

VO_BOOL CAdsp::ReleaseFrame(VO_U32 nBuffer)
{
	VOLOGR("CAdsp::ReleaseFrame %d", nBuffer);

#ifdef _LINUX_ANDROID
	if(m_nFD <= 0)
	{
		VOLOGE("/dev/vdec has not been opened.");
		return VO_FALSE;
	}

	if(ioctl(m_nFD, VDEC_IOCTL_REUSEFRAMEBUFFER, &nBuffer) < 0)
	{
		VOLOGE("VDEC_IOCTL_REUSEFRAMEBUFFER failed");

		return VO_FALSE;
	}
#endif	//_LINUX_ANDROID

	return VO_TRUE;
}

VO_U32 CAdsp::GetCurrentMessage(ADSP_MESSAGE* psMessage)
{
#ifdef _LINUX_ANDROID
	if(m_nFD <= 0)
	{
		VOLOGE("/dev/vdec has not been opened.");
		return 0;
	}

	vdec_msg vdecMsg;
	if(ioctl(m_nFD, VDEC_IOCTL_GETMSG, &vdecMsg) < 0)	//current no message!!
		return 2;

	if(vdecMsg.id == VDEC_MSG_REUSEINPUTBUFFER)
	{
		if(psMessage)
		{
			psMessage->eType = ADSP_MESSAGE_TYPE_BUFFERDONE;

			psMessage->sInfo.sBufferDoneInfo.nBufferID = vdecMsg.buf_id;
		}
	}
	else if(vdecMsg.id == VDEC_MSG_FRAMEDONE)
	{
		if(psMessage)
		{
			psMessage->eType = ADSP_MESSAGE_TYPE_FRAMEDONE;
			psMessage->sInfo.sFrameDoneInfo.nPMemID = vdecMsg.vfr_info.data2;
			psMessage->sInfo.sFrameDoneInfo.nPMemOffset = vdecMsg.vfr_info.offset;

			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.eStatus = (ADSP_VDEC_STATUS)vdecMsg.vfr_info.status;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nUserData1 = vdecMsg.vfr_info.data1;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nUserData2 = vdecMsg.vfr_info.data2;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.ullTimeStamp = ((VO_U64)vdecMsg.vfr_info.timestamp_hi << 32) | ((VO_U64)vdecMsg.vfr_info.timestamp_lo & 0x0FFFFFFFFLL);
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.ullCalculatedTimeStamp = ((VO_U64)vdecMsg.vfr_info.cal_timestamp_hi << 32) | ((VO_U64)vdecMsg.vfr_info.cal_timestamp_lo & 0x0FFFFFFFFLL);
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nDecPicWidth = vdecMsg.vfr_info.dec_width;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nDecPicHeight = vdecMsg.vfr_info.dec_height;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.sCroppingWindow.nLeft = vdecMsg.vfr_info.cwin.x1;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.sCroppingWindow.nTop = vdecMsg.vfr_info.cwin.y1;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.sCroppingWindow.nRight = vdecMsg.vfr_info.cwin.x2;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.sCroppingWindow.nBottom = vdecMsg.vfr_info.cwin.y2;
			for(VO_U32 i = 0; i < MAX_FIELDS; i++)
				psMessage->sInfo.sFrameDoneInfo.sFrameInfo.ePicType[i] = (ADSP_VDEC_PICTURETYPE)vdecMsg.vfr_info.picture_type[i];
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.ePicFormat = (ADSP_VDEC_PICTUREFORMAT)vdecMsg.vfr_info.picture_format;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nVC1RangeY = vdecMsg.vfr_info.vc1_rangeY;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nVC1RangeUV = vdecMsg.vfr_info.vc1_rangeUV;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.ePicResolution = (ADSP_VDEC_PICTURERESOLUTION)vdecMsg.vfr_info.picture_resolution;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nRepeatProgFrames = vdecMsg.vfr_info.frame_disp_repeat;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.bRepeatFirstField = vdecMsg.vfr_info.repeat_first_field;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.bTopFieldFirst = vdecMsg.vfr_info.top_field_first;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.bFrameInterpFlag = vdecMsg.vfr_info.interframe_interp;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.sVC1PanScan.nNum = vdecMsg.vfr_info.panscan.num;
			for(VO_U32 i = 0; i < MAX_VC1_PAN_SCAN_WINDOWS; i++)
			{
				psMessage->sInfo.sFrameDoneInfo.sFrameInfo.sVC1PanScan.nWidth[i] = vdecMsg.vfr_info.panscan.width[i];
				psMessage->sInfo.sFrameDoneInfo.sFrameInfo.sVC1PanScan.nHeight[i] = vdecMsg.vfr_info.panscan.height[i];
				psMessage->sInfo.sFrameDoneInfo.sFrameInfo.sVC1PanScan.nHorOffset[i] = vdecMsg.vfr_info.panscan.xoffset[i];
				psMessage->sInfo.sFrameDoneInfo.sFrameInfo.sVC1PanScan.nVerOffset[i] = vdecMsg.vfr_info.panscan.yoffset[i];
			}
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nPercentConcealedMacroblocks = vdecMsg.vfr_info.concealed_macblk_num;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nFlags = vdecMsg.vfr_info.flags;
			psMessage->sInfo.sFrameDoneInfo.sFrameInfo.nPerformanceStats = vdecMsg.vfr_info.performance_stats;
		}
	}
	else
	{
		VOLOGE("VDEC_IOCTL_GETMSG Unknown Message ID");

		return 0;
	}
#endif	//_LINUX_ANDROID

	return 1;
}