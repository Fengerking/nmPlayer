#include "OpenMAXVideoFrameSplit.h"
#include "string.h"



#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
static const VO_U8 VO_UE_VAL[512]={
	31,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9,10,10,10,10,11,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const VO_U8 VO_BIG_LEN_TAB[256]={
	0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};


signed long vo_big_len(unsigned long val);
unsigned char* GetNextFrame(unsigned char* currPos,int size);

signed long vo_big_len(unsigned long val)
{
	int len = 0;
	if (val & 0xffff0000) {
		val >>= 16;
		len += 16;
	}
	if (val & 0xff00) {
		val >>= 8;
		len += 8;
	}
	len += VO_BIG_LEN_TAB[val];

	return len;
}


unsigned char* GetNextFrame(unsigned char* currPos,int size)
{
    unsigned char* endPos = currPos+size-4;
    unsigned char* p = currPos;  
	unsigned char * tmp_ptr = NULL;
	unsigned int bFind = 0;
	unsigned int value;

	do 
	{
		for (; p < endPos; p++)
		{
			if (XRAW_IS_ANNEXB(p))
			{
				tmp_ptr = (unsigned char *)(p + 3);
				break;
			}
			if (XRAW_IS_ANNEXB2(p))
			{
				tmp_ptr = (unsigned char *)(p + 4);
				break;
			}
		}
		if(p>=endPos || tmp_ptr>=(unsigned char*)endPos)	//need 5 bytes to parse
			return NULL;
		value = (*tmp_ptr++)&0x1F;
		// printf("find nalu, type %d\r\n", value);
		if (value == 1 || value == 5)	//slice or IDR
		{
			value = (((unsigned int)(*tmp_ptr))<<24) | ((unsigned int)(*(tmp_ptr+1))<<16) | ((unsigned int)(*(tmp_ptr+2))<<8) | ((unsigned int)*(tmp_ptr+3));
			// printf("value %08X\r\n", value);
			if(value >=(1<<27))
			{
				value >>= 32-9;
				value = VO_UE_VAL[value];
			}
			else
			{
				value >>= 2*vo_big_len(value)-31;
				value--;
			}
			// printf("start mb nr %d\r\n", value);
			if (value == 0)	//start_mb_nr = 0, find a new pic
			{
				if (bFind)
				{
					break;
				}
				else
				{
					bFind = 1;
				}
			}
		}
		else if (bFind)
		{
			break;
		}
		p+=3;
	} while (1);

	return p;
}


CVideoFrameSplit::CVideoFrameSplit()
{
    m_pWorkBuffer = new VO_BYTE[512*1024];
    m_pOutputBuffer = new VO_BYTE[512*1024];
    if(m_pWorkBuffer == NULL || m_pOutputBuffer == NULL)
	{
		return;
	}

	m_ulOutputBufferLength = 512*1024;
    m_ulWorkBufferMaxLength = 512*1024;
    m_ulCurrentLength = 0;
    m_ulFrameCount = 0;
    
    memset(m_aTimeStampArray, 0, sizeof(VO_U64)*128);
    m_ulCurrentTimeStampFrontIndex = 0;
    m_ulCurrentTimeStampTailIndex = 0;
}

CVideoFrameSplit::~CVideoFrameSplit()
{
    if(m_pWorkBuffer != NULL)
    {
		delete[] m_pWorkBuffer;
        m_pWorkBuffer = NULL;
    }
    
	if(m_pOutputBuffer != NULL)
    {
		delete[] m_pOutputBuffer;
		m_pOutputBuffer = NULL;
    }
}

VO_U32 CVideoFrameSplit::InputFrameInfo(VO_BYTE*  pData, VO_U32 ulDataLength, VO_U64 ullTimeStamp)
{
    VO_BYTE*   pNewWorkBuffer = NULL;
    if((ulDataLength+m_ulCurrentLength)>m_ulWorkBufferMaxLength)
	{
        pNewWorkBuffer = new VO_BYTE(2*(ulDataLength+m_ulCurrentLength));
		if(pNewWorkBuffer == NULL)
        {
			return 1;
		}
		else
		{
			memset(pNewWorkBuffer, 0, 2*(ulDataLength+m_ulCurrentLength));
			memcpy(pNewWorkBuffer, m_pWorkBuffer, m_ulCurrentLength);
			memcpy(pNewWorkBuffer+m_ulCurrentLength, pData, ulDataLength);
			m_ulCurrentLength +=ulDataLength;
			m_ulWorkBufferMaxLength =  2*(ulDataLength+m_ulCurrentLength);
			delete[] m_pWorkBuffer;
			m_pWorkBuffer = pNewWorkBuffer;
            
			m_aTimeStampArray[m_ulCurrentTimeStampTailIndex] = ullTimeStamp;
			m_ulCurrentTimeStampTailIndex = (m_ulCurrentTimeStampTailIndex+1)%128;
			m_ulFrameCount++;
		}
	}
	else
	{
		memcpy(m_pWorkBuffer+m_ulCurrentLength, pData, ulDataLength);
        m_ulCurrentLength += ulDataLength;
		m_aTimeStampArray[m_ulCurrentTimeStampTailIndex] = ullTimeStamp;
		m_ulCurrentTimeStampTailIndex = (m_ulCurrentTimeStampTailIndex+1)%128;
		m_ulFrameCount++;
	}

	return 0;
}
VO_U32 CVideoFrameSplit::GetFrameInfo(VO_BYTE**  pDataOutput, VO_U32* pulDataLength, VO_U64* pullTimeStamp)
{
	VO_BYTE*    pFrameStart = NULL;    
    VO_U32      ulFrameLength = 0;
    VO_U64      ullTimeStamp = 0;

    if(m_ulFrameCount < 2)
    {
		return 1;
    }
	else
	{
		//Skip the first  00 00 00 01 09
        pFrameStart = GetNextFrame(m_pWorkBuffer, m_ulCurrentLength);
		if(pFrameStart == m_pWorkBuffer)
        {
			pFrameStart += 3;
			pFrameStart = GetNextFrame(m_pWorkBuffer, m_ulCurrentLength);
		}


		if(NULL != pFrameStart)
		{
			ulFrameLength = pFrameStart-m_pWorkBuffer;
			memcpy(m_pOutputBuffer, m_pWorkBuffer, ulFrameLength);
			ullTimeStamp = m_aTimeStampArray[m_ulCurrentTimeStampFrontIndex];
			m_ulCurrentTimeStampFrontIndex = (m_ulCurrentTimeStampFrontIndex+1)%128;
            *pDataOutput = m_pOutputBuffer;
			*pulDataLength = ulFrameLength;
			*pullTimeStamp = ullTimeStamp;


            memmove(m_pWorkBuffer, m_pWorkBuffer+ulFrameLength, (m_ulCurrentLength-ulFrameLength));
			m_ulCurrentLength -= ulFrameLength;

			//Remove the Padding
			RemovePadding(m_pOutputBuffer, ulFrameLength, &ulFrameLength);
			*pulDataLength = ulFrameLength;
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

VO_VOID  CVideoFrameSplit::RemovePadding(VO_BYTE*  pData, VO_U32 ulDataLength, VO_U32*  pDataOutputLength)
{
    VO_BYTE*   pStart = NULL;
    VO_BYTE*   pEnd = NULL;

	pStart = pData;
	pEnd = pData+(ulDataLength-1);
	while (pEnd > (pStart+1) && *pEnd == 0x00) 
	{
		--pEnd;
	}

	*pDataOutputLength = pEnd-pStart+1;
}




VO_U32 CVideoFrameSplit::ResetContext()
{
	m_ulOutputBufferLength = 512*1024;
	m_ulWorkBufferMaxLength = 512*1024;
	m_ulCurrentLength = 0;
	m_ulFrameCount = 0;

	memset(m_aTimeStampArray, 0, sizeof(VO_U64)*128);
	m_ulCurrentTimeStampFrontIndex = 0;
	m_ulCurrentTimeStampTailIndex = 0;
	return 0;
}
