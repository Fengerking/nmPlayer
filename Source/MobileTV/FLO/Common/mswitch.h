#ifndef _SWITCH_H_
#define _SWITCH_H_

#define _ENABLE_DROP_B_FRAME    1  //default:	1
#define _ENABLE_DROP_FRAME      1  //	        1
#define _ENABLE_RENDERING       1  //	        1

#define _ENABLE_LOG             0  //	        0

#define _DUMP_264INOUT          0  //			0

#define _DUMP_TIMEINFO          0  //           0

#define _DUMP_H264FRAME         0  //			0            0

#define _DUMP_YUV               0  //			0            0

#define _DUMP_AVTIMESTAMP       0  //			0            1

#define _DUMP_FRAMERATE         0  //           0            1

#define _DUMP_RENDTIMESTAMP     0  //           0            0

#define _DUMP_AV_RTP            1  //           0            1

#define _DUMP_RTSP				0  //           0            1

#define _TEST_AVSYNC            0  //			0


#define _RESET_264DEC_LOOP      1  //           1
const unsigned int ResetInterval = 10*60*1000; //10 minutes   

#define _DUMP_264BITSTREAM      0  //           0            1



#define _TEST_FRAMERATE         1  //			1
#define _PRINT_FRAMERATE        0  //			0


#define _TEST_OCC_TIME          1  //			1
#define _PRINT_OCC_TIME         0  //			0


#define _ENABLE_ADJUSTTIMESTAMP 0  //           0

#define _PRINT_H264PROCESSERROR 0  //           0            


#if _DUMP_FRAMERATE

#endif //_DUMP_FRAMERATE


#if _TEST_OCC_TIME

class COCCTimeInfo
{
public:
	static COCCTimeInfo * CreateInstance();
	static void DestroyInstance();
	
	static COCCTimeInfo * m_pOCCTimeInfo;
	
protected:
	COCCTimeInfo();
	~COCCTimeInfo();
	
public:	
	void Reset();
	int GetTimeofDay(unsigned int & sec, unsigned int & usec);

public:
	int nOCCStartTime;
	int nOCCStopTime;
	int nOCCFirstVideoRTPReceivedTime;
	int nOCCFirstAudioRTPReceivedTime;
	int nOCCImageReadyTime;
	int nOCCImageRenderedTime;
	
	unsigned int nOCCStartTime_sec;
	unsigned int nOCCStopTime_sec;
	unsigned int nOCCFirstVideoRTPReceivedTime_sec;
	unsigned int nOCCFirstAudioRTPReceivedTime_sec;
	unsigned int nOCCImageReadyTime_sec;
	unsigned int nOCCImageRenderedTime_sec;
	
	unsigned int nOCCStartTime_usec;
	unsigned int nOCCStopTime_usec;
	unsigned int nOCCFirstVideoRTPReceivedTime_usec;
	unsigned int nOCCFirstAudioRTPReceivedTime_usec;
	unsigned int nOCCImageReadyTime_usec;
	unsigned int nOCCImageRenderedTime_usec;
};

#endif //_TEST_OCC_TIME


const char * GetLogDir();

#endif //_SWITCH_H_
