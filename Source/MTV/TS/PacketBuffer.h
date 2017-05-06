#ifndef _PACKET_BUFFER_H
#define _PACKET_BUFFER_H

class CPacketBuffer
{
public:
	CPacketBuffer(void);
	~CPacketBuffer(void);

	bool SetMaxSize(int size);
	bool AddData(const unsigned char * pData, int size);
	inline void Reset();
	inline int GetSize() const { return m_cSize; }
	inline unsigned char* GetBuffer() { return m_pBuffer; }
	inline int GetMaxSize() const { return m_nMaxSize; }

	enum
	{
		INVALID_ID = -1,
	};
	inline int GetID() const { return m_id; }
	inline void SetID(int id) { m_id = id; }

	void Release();

protected:
	int m_id;
	int m_nMaxSize;
	int m_cSize;
	unsigned char* m_pBuffer;
};

//===============================================================

inline void CPacketBuffer::Reset()
{
	m_cSize = 0;
}


#endif //_PACKET_BUFFER_H

