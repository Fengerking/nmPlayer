#include "commonheader.h"
#include "netbase.h"
#include "RTPPacket.h"

const int MTU_SIZE = 1500;

CRTPPacket::CRTPPacket()
: m_rtpPacketData(NULL)
{
}

CRTPPacket::~CRTPPacket()
{
}

void CRTPPacket::Reset()
{
//	m_readPointer = m_rtpPacketData;
//	m_rtpPacketSize = 0;

	m_rtpMarker = false;
	m_rtpTimestamp = 0;
	m_rtpSSRC = 0;
	m_rtpPayloadType = 0;
	m_rtpSeqNum = 0;
}

bool CRTPPacket::AssignRTPData(unsigned char * _rtpData, int rtpDataSize)
{
	Reset();

	m_rtpPacketData = _rtpData;
	m_rtpPacketSize = rtpDataSize;
	m_readPointer = _rtpData;
	return true;
}

bool CRTPPacket::ParseRTPCommonHeader()
{
	do
	{
		if(AvailDataSize() < 12)
			break;

		unsigned int rtpHeader = 0;
		Read(&rtpHeader, 4);
		rtpHeader = ntohl(rtpHeader);

		if((rtpHeader & 0xC0000000) != 0x80000000) // version: 2 bits
			break;

		m_rtpMarker = ((rtpHeader & 0x00800000) >> 23) != 0; // marker: 1 bit
		m_rtpPayloadType = (rtpHeader & 0x007F0000) >> 16;   // payload: 7 bits
		m_rtpSeqNum = (unsigned short)(rtpHeader & 0xFFFF);  // sequence number: 16 bits

		if(rtpHeader & 0x20000000) // padding: 1 bit
		{
			int paddingBytes = m_rtpPacketData[m_rtpPacketSize - 1];
			if(m_rtpPacketSize < paddingBytes)
				break;
			RemoveRTPPadding(paddingBytes);
		}    

		Read(&m_rtpTimestamp, 4);
		m_rtpTimestamp = ntohl(m_rtpTimestamp);

		Read(&m_rtpSSRC, 4);
		m_rtpSSRC = ntohl(m_rtpSSRC);

		unsigned int rtpCSRCCount = (rtpHeader >> 24) & 0xF;
		Skip(rtpCSRCCount * 4);

		if(rtpHeader & 0x10000000) // extension: 1 bit
		{
			unsigned int extRTPHeader = 0;
			Read(&extRTPHeader, 4);
			extRTPHeader = ntohl(extRTPHeader);

			unsigned int extRTPHeaderSize = (extRTPHeader & 0xFFFF) * 4;
			Skip(extRTPHeaderSize);
		}

		return true;

	}while(0);

	return false;
}

int CRTPPacket::Read(void * _buffer, int numOfBytesToRead)
{
	if(AvailDataSize() < numOfBytesToRead)
	{
		numOfBytesToRead = AvailDataSize();
	}
	memcpy(_buffer, m_readPointer, numOfBytesToRead);
	Skip(numOfBytesToRead);
	return numOfBytesToRead;
}

int CRTPPacket::Write(void * _buffer, int numOfBytesToWrite)
{
	if(AvailDataSize() < numOfBytesToWrite)
	{
		numOfBytesToWrite = AvailDataSize();
	}
	memcpy(m_readPointer, _buffer, numOfBytesToWrite);
	return numOfBytesToWrite;
}

int CRTPPacket::Peek(void * _buffer, int numOfBytesToPeek)
{
	if(AvailDataSize() < numOfBytesToPeek)
	{
		numOfBytesToPeek = AvailDataSize();
	}
	memcpy(_buffer, m_readPointer, numOfBytesToPeek);
	return numOfBytesToPeek;
}

void CRTPPacket::Skip(int num)
{
	m_readPointer += num;
	if(m_readPointer > m_rtpPacketData + m_rtpPacketSize)
	{
		m_readPointer = m_rtpPacketData + m_rtpPacketSize;
	}
}

void CRTPPacket::RemoveRTPPadding(int num)
{
	if(num > m_rtpPacketSize)
		num = m_rtpPacketSize;

	m_rtpPacketSize -= num;
}

unsigned char * CRTPPacket::RTPPacketData() 
{
	return m_rtpPacketData; 
}

int	CRTPPacket::RTPPacketSize()
{
	return m_rtpPacketSize; 
}

void CRTPPacket::SetRTPPacketSize(int rtpPacketSize)
{
	m_rtpPacketSize = rtpPacketSize;
}

unsigned char * CRTPPacket::ReadPointer()
{
	return m_readPointer;
}

void CRTPPacket::ResetReadPointer()
{
	m_readPointer = m_rtpPacketData;
}

int CRTPPacket::AvailDataSize()
{
	int availDataSize = (int)(m_rtpPacketData + m_rtpPacketSize - m_readPointer);
	return availDataSize;
}

bool CRTPPacket::Marker() 
{ 
	return m_rtpMarker; 
}

unsigned int CRTPPacket::Timestamp() 
{
	return m_rtpTimestamp;
}

unsigned int CRTPPacket::SSRC() 
{
	return m_rtpSSRC; 
}

unsigned int CRTPPacket::PayloadType()
{
	return m_rtpPayloadType;
}

unsigned short CRTPPacket::SeqNum() 
{ 
	return m_rtpSeqNum; 
}
