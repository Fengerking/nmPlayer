#ifndef _3gpTTextOp_H
#define _3gpTTextOp_H


#include "isomrd.h"
#include "subtitleType.h"

#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#ifdef _MODIFIERBOX
class CModifierBox
{
public:
	CModifierBox();
	virtual ~CModifierBox();
public:
	virtual void *GetBox() = 0 ;
	virtual int GetCurBoxLength() { return m_nCurLength;}
	virtual void CreateBox() = 0;
protected:
	int m_nCurLength;
};

class CTx3gTextStyleBox:public CModifierBox
{
public:
	CTx3gTextStyleBox();
	
	virtual void *GetBox() { return (void*)&m_stStyl;} 
	virtual void CreateBox();
private:
	Tx3gTextStyleBox m_stStyl;
};
class CTx3gTextHighlightBox:public CModifierBox
{
public:
	CTx3gTextHighlightBox();

	virtual void *GetBox() { return (void*)&m_stStyl;} 
private:
	Tx3gTextHighlightBox m_stStyl;
};

class CTx3gTextHilightColorBox:public CModifierBox
{
public:
	CTx3gTextHilightColorBox();

	virtual void *GetBox() { return (void*)&m_stStyl;} 
private:
	Tx3gTextHilightColorBox m_stStyl;
};
class CTx3gTextKaraokeBox:public CModifierBox
{
public:
	CTx3gTextKaraokeBox();

	virtual void *GetBox() { return (void*)&m_stStyl;} 
	virtual void CreateBox();
private:
	Tx3gTextKaraokeBox m_stStyl;
};

class CTx3gTextScrollDelayBox:public CModifierBox
{
public:
	CTx3gTextScrollDelayBox();

	virtual void *GetBox() { return (void*)&m_stStyl;} 
	virtual void CreateBox();
private:
	Tx3gTextScrollDelayBox m_stStyl;
};
class CTx3gTextHyperTextBox:public CModifierBox
{
public:
	CTx3gTextHyperTextBox();

	virtual void *GetBox() { return (void*)&m_stStyl;} 
	virtual void CreateBox();
private:
	Tx3gTextHyperTextBox m_stStyl;
};
class CTx3gTextboxBox:public CModifierBox
{
public:
	CTx3gTextboxBox();

	virtual void *GetBox() { return (void*)&m_stStyl;} 
	virtual void CreateBox();
private:
	Tx3gTextboxBox m_stStyl;
};
class CTx3gBlinkBox:public CModifierBox
{
public:
	CTx3gBlinkBox();

	virtual void *GetBox() { return (void*)&m_stStyl;} 
	virtual void CreateBox();
private:
	Tx3gBlinkBox m_stStyl;
};
class CTx3gTextWrapBox:public CModifierBox
{
public:
	CTx3gTextWrapBox();

	virtual void *GetBox() { return (void*)&m_stStyl;} 
	virtual void CreateBox();
private:
	Tx3gTextWrapBox m_stStyl;
};
#endif

class Tx3gTTextOp
{
public:
	Tx3gTTextOp();
	~Tx3gTTextOp();
public:
	void CreateEntryBuffer(int size, void **ptr);
	void ReleaseSampleModifierBox(Tx3gTextSampleModifierBox *tsmb, int nCount, int flag = 1);
	void ReleaseSample(Tx3gTextSample *pTsample);

template <typename T>
	T *AddSampleModifierBox(Tx3gTextSampleModifierBox **tsmb, uint32 nType)
	{
		Tx3gTextSampleModifierBox *pMb = NULL;
		VOLOGI("+AddSampleModifierBox. Type: 0x%08x", nType);
		if(!(*tsmb) || !(*tsmb)->pNext )
		{
			pMb = new Tx3gTextSampleModifierBox;
			memset(pMb,0 ,sizeof(Tx3gTextSampleModifierBox));
			if(!(*tsmb))
				*tsmb = pMb;
			else
				(*tsmb)->pNext = pMb;
			m_nCurModifierBoxCount++;
		}
		else
			pMb = (*tsmb)->pNext;

		T *ptr = new T;
		memset(ptr, 0x00, sizeof(T));
		pMb->nModifierType = nType;
		pMb->pTxtModifier = (void*)ptr;
		VOLOGI("-AddSampleModifierBox");
		return ptr;
	}

template <typename T>
	void CreateStructBuffer(int nSize, int &nCurSize, T **pBuffer)
	{
		if(nSize >= nCurSize)
		{
			VOLOGI("New Buffer.%d",nSize);
			nCurSize = nSize +1;
			if(*pBuffer) delete []*pBuffer;
			*pBuffer = new T[nCurSize];
			memset(*pBuffer, 0, sizeof(T) * nCurSize);
		}
	}
template <typename T>
	void CreateStructBuffer(int nSize, T **pBuffer)
	{
		if(*pBuffer) delete []*pBuffer;
		*pBuffer = new T[nSize +1];
		memset(*pBuffer, 0, sizeof(T) * (nSize +1));
	}
template <typename T>
	int ReleaseBuf(int nSize, T **pBuffer)
	{
		if(*pBuffer) delete []*pBuffer;
		*pBuffer = NULL;
		return 0;
	}

private:
	uint32 ParseStyleRecord(Reader* r, Tx3gStyleRecord *ptr);

	
// 	int m_nCurTextLength;
// 	int m_nCurStyleRecordCount;
// 	int m_nCurKrokRecordCount;
// 	int m_nCurHyperURLLength;
// 	int m_nCurHyperAltLength;
// 
// 	int m_nCurFontNameLength;
// 	int m_nCurFontRecordCount;

	int m_nCurModifierBoxCount;
	
};
#ifdef _VONAMESPACE
}
#endif


#endif	//_3gTTextOp_H