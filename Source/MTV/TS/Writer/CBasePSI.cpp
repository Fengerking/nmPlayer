#include "CBasePSI.h"
#include "cmnMemory.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CDescriptor::CDescriptor()
:descriptor_tag(0)
,descriptor_length(0)
,bUsed(VO_FALSE)
,pNext(NULL)
{
	
}
CDescriptor::~CDescriptor()
{
	if (pNext)
	{
		delete pNext;
		pNext = NULL;
	}
}

CRegistration_Descriptor::CRegistration_Descriptor()
:CDescriptor()
,format_identifier(0)
,additional_info_len(0)
,additional_identification_info(NULL)
{

}

CRegistration_Descriptor::~CRegistration_Descriptor()
{
	if (additional_identification_info)
	{
		delete []additional_identification_info;
	}
}


void CPmt::AddDescriptor(VO_U32 nStreamID,Pmt_Descriptor_Type eType,CDescriptor * pdescriptor)
{
	CDescriptor * pHead = NULL;
	if (eType == Pmt_Descriptor_Type_Program)
	{
		if (!pProgramdescriptorHead)
		{
			pProgramdescriptorHead = pdescriptor;
			return;
		}
		pHead = pProgramdescriptorHead;
		
	}
	else if (eType == Pmt_Descriptor_Type_Stream)
	{
		if (!pStreamdescriptorHead[nStreamID])
		{
			pStreamdescriptorHead[nStreamID] = pdescriptor;
			return;
		}
		pHead = pStreamdescriptorHead[nStreamID];
	}
	else
		return ;
	
	CDescriptor *pTmp = pHead;
	while(pHead->pNext)
	{
		pHead = pHead->pNext;
	}
	pHead->pNext = pdescriptor;
}

void CPmt::RemoveDescriptor(Pmt_Descriptor_Type eType)
{
	CDescriptor * pHead = NULL;
	if (eType == Pmt_Descriptor_Type_Program)
	{
		CDescriptor *pCur = pProgramdescriptorHead;
		CDescriptor *pTmp = pCur;
		while(pCur)
		{
			pTmp = pCur;
			pCur = pCur->pNext;
			delete pTmp;
		}
		pProgramdescriptorHead = NULL;
	}
	else if (eType == Pmt_Descriptor_Type_Stream)
	{
		VO_U32 nStreamID = 0;
		while(nStreamID < MAX_STREAM_TYPE_COUNT)
		{
			CDescriptor *pCur = pStreamdescriptorHead[nStreamID];
			CDescriptor *pTmp = pCur;
			while(pCur)
			{
				pTmp = pCur;
				pCur = pCur->pNext;
				delete pTmp;
			}
			pStreamdescriptorHead[nStreamID] = NULL;
			nStreamID++;
		}
	}
	else
		return ;

	
}


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/***********************************************
************************************************
*				CRC check                  *
************************************************
************************************************/
const int CRC8_SEED		= 0x31;
const int CRC16_SEED	= 0x8005;
const int CRC32_SEED	= 0x04C11DB7;
static unsigned long crc32_table[256];
static unsigned char crc8_table[256];

void BuildCrcTable()
{
	for (int i = 0; i < 256; i++ )
	{
		unsigned char nData = i;
		unsigned char nAccum = 0;
		for (VO_U32 j = 0; j < 8; j++ )
		{
			if ( ( nData ^ nAccum ) & 0x80 )
				nAccum = ( nAccum << 1 ) ^ CRC8_SEED;
			else
				nAccum <<= 1;
			nData <<= 1;
		}
		crc8_table[i] = nAccum;
	}

	for (VO_U32 i = 0; i < 256; i++ )
	{
		unsigned long nData = ( unsigned long )( i << 24 );
		unsigned long nAccum = 0;
		for (VO_U32 j = 0; j < 8; j++ )
		{
			if ( ( nData ^ nAccum ) & 0x80000000 )
				nAccum = ( nAccum << 1 ) ^ CRC32_SEED;
			else
				nAccum <<= 1;
			nData <<= 1;
		}
		crc32_table[i] = nAccum;
	}
}

unsigned char CalcCRC8(const VO_PBYTE pbuf, VO_U32 len) 
{ 
	unsigned char crc = 0; 
	for(unsigned i=0; i < len; i++) 
	{ 
		crc = crc8_table[crc ^ pbuf[i]]; 
	} 
	return crc; 
}

unsigned long CalcCRC32(const VO_PBYTE inData, VO_U32 inLen)
{
	unsigned long dwRegister = 0xFFFFFFFF;
	for(unsigned long i=0; i<inLen; i++ )
	{
		dwRegister = ((dwRegister) << 8) ^ crc32_table[(inData[i]) ^ (dwRegister>>24)];
	}
	return dwRegister;
}

inline unsigned long	GET32(const VO_PBYTE p)	{return (p[0]<<24) + (p[1] <<16) + (p[2]<<8) +  p[3];}

bool CheckCRC32(const VO_PBYTE lpIn, VO_U32 len)
{
	unsigned long dwCalCRC		= CalcCRC32(lpIn, len);
	unsigned long u32CRCValue	= GET32(lpIn + len);
	return dwCalCRC == u32CRCValue;
}

#ifdef _VONAMESPACE
}
#endif

