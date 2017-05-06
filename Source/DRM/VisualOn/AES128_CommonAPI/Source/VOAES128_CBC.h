#ifndef __VOAES_CBC_H__
#define __VOAES_CBC_H__
#include "voHLSDRM.h"
#include "AES_CBC.h"
#include "CSourceIOUtility.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class VOAES128_CBC
	: public CAES_CBC
{
public:
	class Delegate {
	public:
		virtual ~Delegate() {}

		virtual int getKey(char* pKeyURL, unsigned char* pKey, int* piKey) = 0;
	};

	VOAES128_CBC(void);
	~VOAES128_CBC(void);

	int Open(VOAES128_CBC::Delegate *pDelegate, int identifer, void* pAdditionalInfo);

	int Decrypt(long long llPosition, unsigned char* pSrcData, int iSrcDdata, bool bLastBlock, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo);

	int Close();

	int Identifer() { return m_identifer; }

private:
	VOAES128_CBC::Delegate* m_pDelegate;

	int				m_identifer;
	bool			m_bNeedDecrypt;

	int				m_iLen;
	unsigned char	m_Buf[752];

	char			m_PreKeyURI[2048];
};


#ifdef _VONAMESPACE
}
#endif

#endif //__CAES128SINGLEINSTANCE_H__