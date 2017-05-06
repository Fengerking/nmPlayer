

#include "3gpTTextOp.h"
#include "isomscan.h"
#include "mpxutil.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

Tx3gTTextOp::Tx3gTTextOp()
:m_nCurModifierBoxCount(0)
// ,m_nCurTextLength(0)
// ,m_nCurStyleRecordCount(0)
// ,m_nCurKrokRecordCount(0)
// ,m_nCurHyperURLLength(0)
// ,m_nCurHyperAltLength(0)
// ,m_nCurFontNameLength(0)
// ,m_nCurFontRecordCount(0)
// ,m_nCurModifierBoxCount(0)
{

}
Tx3gTTextOp::~Tx3gTTextOp()
{

}


void Tx3gTTextOp::CreateEntryBuffer(int size, void **ptr)
{
	//RESETBUF(pStyl->nEntryCount, m_nCurStyleRecordCount, ptr->pStTextStyles, Tx3gStyleRecord);
}

void Tx3gTTextOp::ReleaseSample(Tx3gTextSample *pTsample)
{
	VOLOGI("+ ReleaseSample");
	if(pTsample->pStrText) delete []pTsample->pStrText;
	VOLOGI(" ReleaseSample 1");
	pTsample->pStrText = NULL;
	pTsample->nTextLength = 0;

	ReleaseSampleModifierBox(pTsample->pStTxtSampleModifierBox, m_nCurModifierBoxCount, 0);

	memset(pTsample, 0, sizeof(Tx3gTextSample));
	VOLOGI("- ReleaseSample");
}

void Tx3gTTextOp::ReleaseSampleModifierBox(Tx3gTextSampleModifierBox *tsmb, int nCount, int flag /*= 1 */)
{
		Tx3gTextSampleModifierBox *pMb = tsmb;
		for(int i =0;i< nCount; i++)
		{
			if(!pMb) break;

			switch (pMb->nModifierType)
			{
				case FOURCC2_styl:
					{
						Tx3gTextStyleBox *pPtr = (Tx3gTextStyleBox*)pMb->pTxtModifier;
						pPtr->nEntryCount = ReleaseBuf<Tx3gStyleRecord>(pPtr->nEntryCount, &pPtr->pStTextStyles);
						//RELBUF(pPtr->nEntryCount, pPtr->pStTextStyles, Tx3gStyleRecord);
						delete pPtr;
						break;
					}
				case FOURCC2_hlit:
					{
						Tx3gTextHighlightBox *pPtr = (Tx3gTextHighlightBox*)pMb->pTxtModifier;
						//RESETBUF(-1,0, pPtr, Tx3gTextHighlightBox);
						delete pPtr;
						break;
					}
				case FOURCC2_hclr:
					{
						Tx3gTextHilightColorBox *pPtr = (Tx3gTextHilightColorBox*)pMb->pTxtModifier;
						//RESETBUF(-1,0, pPtr, Tx3gTextHilightColorBox);
						delete pPtr;
						break;
					}
				case FOURCC2_krok:
					{
						Tx3gTextKaraokeBox *pPtr = (Tx3gTextKaraokeBox*)pMb->pTxtModifier;
						pPtr->nEntryCount = ReleaseBuf<Tx3gKaraokeRecod>(pPtr->nEntryCount, &pPtr->pStKaraokeRecod);
						delete pPtr;
						//	RESETBUF(pPtr->nEntryCount, pPtr->pStKaraokeRecod, Tx3gKaraokeRecod);
						break;
					}
				case FOURCC2_dlay:
					{
						Tx3gTextScrollDelayBox *pPtr = (Tx3gTextScrollDelayBox*)pMb->pTxtModifier;
						//	RESETBUF(-1,0, pPtr, Tx3gTextScrollDelayBox);
						delete pPtr;
						break;
					}
				case FOURCC2_href:
					{
						Tx3gTextHyperTextBox *pPtr = (Tx3gTextHyperTextBox*)pMb->pTxtModifier;
						pPtr->nURLLength = ReleaseBuf<unsigned char>(pPtr->nURLLength, &pPtr->pStrURL);
						pPtr->nAltLength = ReleaseBuf<unsigned char>(pPtr->nAltLength, &pPtr->pStrAltString);
						delete pPtr;

// 							RESETBUF(pPtr->nURLLength, pPtr->pStrURL, unsigned char);
// 							RESETBUF(pPtr->nAltLength, pPtr->pStrAltString, unsigned char);
						break;
					}
				case FOURCC2_tbox:
					{
						Tx3gTextboxBox *pPtr = (Tx3gTextboxBox*)pMb->pTxtModifier;
						delete pPtr;

						//	RESETBUF(-1,0, pPtr, Tx3gTextboxBox);
						break;
					}
				case FOURCC2_blnk:
					{
						Tx3gBlinkBox *pPtr = (Tx3gBlinkBox*)pMb->pTxtModifier;
						delete pPtr;

						//	RESETBUF(-1,0, pPtr, Tx3gBlinkBox);
						break;
					}
				case FOURCC2_twrp:
					{
						Tx3gTextWrapBox *pPtr = (Tx3gTextWrapBox*)pMb->pTxtModifier;
						delete pPtr;

						//	RESETBUF(-1,0, pPtr, Tx3gTextWrapBox);
						break;
					}
				default:
					break;
			}

			pMb->pTxtModifier = NULL;
			pMb->nModifierType = 0;

			Tx3gTextSampleModifierBox *pCur = pMb;

			pMb = pMb->pNext;
			if(flag == 0)
			{
				delete pCur; pCur = NULL;
			}
		}
}

// 
// Tx3gTextSampleModifierBox *Tx3gTTextOp::AddSampleModifierBox(Tx3gTextSampleModifierBox *tsmb, uint32 nType)
// {
// 
// 	Tx3gTextSampleModifierBox *pMb = NULL;
// 
// 	if(!tsmb || !tsmb->pNext )
// 	{
// 		pMb = new Tx3gTextSampleModifierBox;
// 		memset(pMb,0 ,sizeof(Tx3gTextSampleModifierBox));
// 		if(!tsmb)
// 			tsmb = pMb;
// 		else
// 			tsmb->pNext = pMb;
// 		m_nCurModifierBoxCount++;
// 	}
// 	else
// 		pMb = tsmb->pNext;
// 
// 	void *ptr =NULL;
// 	switch (nType)
// 	{
// 	case TX3G_MODIFIERTYPE_STYL:
// 		{
// 			Tx3gTextStyleBox *pPtr = new Tx3gTextStyleBox;
// 			ptr = (void*)pPtr;
// 		}
// 		break;
// 	case TX3G_MODIFIERTYPE_HLIT:
// 		{
// 			Tx3gTextHighlightBox *pPtr = new Tx3gTextHighlightBox;
// 			ptr = (void*)pPtr;
// 
// 		}
// 		break;
// 	case TX3G_MODIFIERTYPE_HCLR:
// 		{
// 			Tx3gTextHilightColorBox *pPtr = new Tx3gTextHilightColorBox;
// 			ptr = (void*)pPtr;
// 		}
// 		break;
// 	case TX3G_MODIFIERTYPE_KROK:
// 		{
// 			Tx3gTextKaraokeBox *pPtr = new Tx3gTextKaraokeBox;
// 			ptr = (void*)pPtr;
// 
// 		}
// 		break;
// 	case TX3G_MODIFIERTYPE_DLAY:
// 		{
// 			Tx3gTextScrollDelayBox *pPtr = new Tx3gTextScrollDelayBox;
// 			ptr = (void*)pPtr;
// 		}
// 		break;
// 	case TX3G_MODIFIERTYPE_HREF:
// 		{
// 			Tx3gTextHyperTextBox *pPtr = new Tx3gTextHyperTextBox;
// 			ptr = (void*)pPtr;
// 
// 		}
// 		break;
// 	case TX3G_MODIFIERTYPE_TBOX:
// 		{
// 			Tx3gTextboxBox *pPtr = new Tx3gTextboxBox;
// 			ptr = (void*)pPtr;
// 
// 		}
// 		break;
// 	case TX3G_MODIFIERTYPE_BLNK:
// 		{
// 			Tx3gBlinkBox *pPtr = new Tx3gBlinkBox;
// 			ptr = (void*)pPtr;
// 
// 		}
// 		break;
// 	case TX3G_MODIFIERTYPE_TWRP:
// 		{
// 			Tx3gTextWrapBox *pPtr = new Tx3gTextWrapBox;
// 			ptr = (void*)pPtr;
// 
// 		}
// 		break;
// 	}	
// 	pMb->nModifierType = nType;
// 	pMb->pTxtModifier = (void*)ptr;
// 	
// 	return pMb;
// }

#ifdef _MODIFIERBOX
CModifierBox::CModifierBox()
:m_nCurLength(0)
{
}


CTx3gTextStyleBox::CTx3gTextStyleBox()
:CModifierBox()
{
}

void CTx3gTextStyleBox::CreateBox()
{
	RESETBUF(m_stStyl.nEntryCount, m_nCurLength, m_stStyl.pStTextStyles, Tx3gStyleRecord );
}
void CTx3gTextKaraokeBox::CreateBox()
{
	RESETBUF(m_stStyl.nEntryCount, m_nCurLength, m_stStyl.pStKaraokeRecod, Tx3gKaraokeRecod);
}


#endif