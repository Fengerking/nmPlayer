


#include "subtitleTrack.h"
#include "isomscan.h"
#include "mpxutil.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

TrackSubtitle::TrackSubtitle()
: Track()
,m_nCurTextLength(0)
{
	memset(&m_stTx3gDescription, 0, sizeof(Tx3gDescription));
	memset(&m_stTx3gTextSample, 0, sizeof(Tx3gTextSample));
	_index_cache = 0;
	_count_real_samples = 0;
	_pre_sample_size = 0;
}
MP4RRC TrackSubtitle::Close()
{
	VOLOGI("+Close");
	_descriptor_data = NULL;
	 MP4RRC  rrc = Track::Close();
	 VOLOGI("-Close");
	return rrc;
}
TrackSubtitle::~TrackSubtitle()
{
		VOLOGI("+~TrackSubtitle");
	m_3gpTTextOp.ReleaseSample(&m_stTx3gTextSample);
	
	m_stTx3gDescription.stFontTable.nEntryCount = m_3gpTTextOp.ReleaseBuf<Tx3gFontRecord>(m_stTx3gDescription.stFontTable.nEntryCount, &m_stTx3gDescription.stFontTable.pStFontRecord );
 	//RELBUF( m_stTx3gDescription.stFontTable.nEntryCount, m_stTx3gDescription.stFontTable.pStFontRecord, Tx3gFontRecord);
	VOLOGI("-~TrackSubtitle");
}


#ifdef _SUPPORT_TTEXT
uint32 TrackSubtitle::GetBitrate()
{
	return 0;
}
MP4RRC TrackSubtitle::SetDrmCallBack(CvoDrmCallback *pDrmCallback)
{
	//return _fragmentBox.SetDrmCallBack((VO_PTR)pDrmCallback);
	return MP4RRC_OK;
}
MP4RRC TrackSubtitle::Open(Reader* r)
{
	MP4RRC rcc = Track::Open(r);
	if(rcc != MP4RRC_OK) return rcc;

	m_pReader = r;
	return MP4RRC_OK;
}

uint32 TrackSubtitle::GetMaxSampleSize()
{
	return sizeof(Tx3gTextSample);
}

uint32 TrackSubtitle::GetSampleData(int index, void* buffer, int buffersize)
{

	if(!buffer) return 0;

	uint32 addrSample = GetSampleAddress(index);
#ifdef _CHECK_READ
	if (addrSample == ERR_CHECK_READ)
	{
		//VOLOGI("GetSampleData failed");
		return ERR_CHECK_READ;
	}
#endif //_CHECK_READ
	if (_current_sample_chunk != _prev_sample_chunk)
	{
		if (!_data_stream.Init2(_current_chunk_size, _current_chunk_address))
			return ERR_CHECK_READ; //return a special error code indicate the I/O error
		_prev_sample_chunk = _current_sample_chunk;
	}
	uint32 sizeSample = GetMaxSampleSize();

	m_pReader->SetPosition(addrSample);

	if(ParseTextSample(m_pReader, (Tx3gTextSample*)buffer, _current_chunk_size) != MP4RRC_OK)
		return 0;

	return sizeSample;

}
uint32 TrackSubtitle::GetSampleAddress(int index)
{
	//VOLOGI("index:%d", index);
	int chunkSampleIndex;
	int chunkSampleCount;
	int nRC = GetSampleChunk(index, chunkSampleIndex, chunkSampleCount);
#ifdef _CHECK_READ
	if (nRC != MP4RRC_OK)
	{
		VOLOGE("GetSampleAddress failed 1");
		return ERR_CHECK_READ;
	}
#endif //_CHECK_READ

	//VOLOGI("21");
	uint64 addr = _current_chunk_address = uint64(GetChunkOffset(index));
	_current_chunk_size = 0;
	int i;
	for (i = chunkSampleIndex; i < chunkSampleIndex + chunkSampleCount ; i++)
	{
		uint32 size = GetSampleSize(i);
#ifdef _CHECK_READ
		if (size == ERR_CHECK_READ)
		{
			VOLOGE("GetSampleAddress failed 3");
			return ERR_CHECK_READ;
		}
#endif //_CHECK_READ
		if(i < chunkSampleIndex + chunkSampleCount-1) addr += size;
		_current_chunk_size += GetSampleSize(i);
	}

	_current_sample_index = index;
	_current_sample_address = addr;

	//VOLOGI("addr: %d", addr);
	return uint32(addr);

}
MP4RRC TrackSubtitle::ParseKaraokeBox(Reader* r, Tx3gTextKaraokeBox *ptr)
{
	r->Read(&ptr->nHighlightStarttime);
	r->Read(&ptr->nEntryCount);
	m_3gpTTextOp.CreateStructBuffer<Tx3gKaraokeRecod>(ptr->nEntryCount, &ptr->pStKaraokeRecod);
	//RESETBUF(ptr->nEntryCount, ptr->pStKaraokeRecod, Tx3gKaraokeRecod);
	for(int i= 0; i< ptr->nEntryCount; i++){
		Tx3gKaraokeRecod  *kk = &ptr->pStKaraokeRecod[i];
		r->Read(&kk->nHighlightEndtime);
		r->Read(&kk->nStartCharoffset);
		r->Read(&kk->nEndCharoffset);
	}
	return MP4RRC_OK;
}

MP4RRC TrackSubtitle::ParseTextSample(Reader* r, Tx3gTextSample *pTextSample, int bodysize)
{

	uint32 size, type;
	//	memset(pTextSample, 0, sizeof(Tx3gTextSample));
	Tx3gTextSample *pSample = pTextSample;
	int16 len =0;
	if(!r->Read(&len)) return MP4RRC_SEEK_FAILED;
	//text size is no more than 2048
	pSample->nTextLength = len;// >= LENGTH_TEXT? LENGTH_TEXT : len;
	m_3gpTTextOp.CreateStructBuffer<unsigned char>(pSample->nTextLength, m_nCurTextLength, &pSample->pStrText);
	//	RESETBUF1(pSample->nTextLength, m_nCurTextLength, pSample->pStrText, unsigned char);

	r->Read(pSample->pStrText, pSample->nTextLength );

	bodysize -= len+2 ;
	VOLOGI("TextData: (%d),%s,(CurTextLength:%d)", pSample->nTextLength, pSample->pStrText,m_nCurTextLength);

	m_3gpTTextOp.ReleaseSampleModifierBox(pSample->pStTxtSampleModifierBox, pSample->nEntryCount);
	pSample->nEntryCount = 0;
	Tx3gTextSampleModifierBox **pCurTsmb = &pSample->pStTxtSampleModifierBox;
	VOLOGI("BodySize:%d", bodysize);
	while (bodysize >0) 	{
		if (!r->Read(&size) || !r->Read(&type) || size <= 0) 
		{
			return MP4RRC_OK;
		}
		if ((int)size > bodysize)
			return MP4RRC_BAD_DESCRIPTOR;

		switch(type)
		{
		case FOURCC2_styl:
			{
				VOLOGI("+ TX3G_MODIFIERTYPE_STYL");
				Tx3gTextStyleBox *pStyl   = m_3gpTTextOp.AddSampleModifierBox<Tx3gTextStyleBox>(pCurTsmb, type);
				//Tx3gTextStyleBox *pStyl  = (Tx3gTextStyleBox*)pCurTsmb->pTxtModifier;
				r->Read(&pStyl->nEntryCount);
				VOLOGI("EntryCount:%d", pStyl->nEntryCount);
				m_3gpTTextOp.CreateStructBuffer<Tx3gStyleRecord>(pStyl->nEntryCount, &pStyl->pStTextStyles);
				VOLOGI("2");
				//	RESETBUF(pStyl->nEntryCount, pStyl->pStTextStyles, Tx3gStyleRecord);
				for(int i = 0;i < pStyl->nEntryCount ; i++){
					ParseStyleRecord(r, &pStyl->pStTextStyles[i]);
				}
				VOLOGI("- TX3G_MODIFIERTYPE_STYL");
			}
			break;
		case FOURCC2_hlit:
			{
				Tx3gTextHighlightBox *pHilt  = m_3gpTTextOp.AddSampleModifierBox<Tx3gTextHighlightBox>(pCurTsmb, type);
				//Tx3gTextHighlightBox *pHilt = (Tx3gTextHighlightBox *)pCurTsmb->pTxtModifier;

				r->Read(&pHilt->nStartCharoffset);
				r->Read(&pHilt->nEndCharoffset);
			}
			break;
		case FOURCC2_hclr:
			{
				Tx3gTextHilightColorBox *pHclr  = m_3gpTTextOp.AddSampleModifierBox<Tx3gTextHilightColorBox>(pCurTsmb, type);
				//Tx3gTextHilightColorBox *pHclr = (Tx3gTextHilightColorBox *)pCurTsmb->pTxtModifier;
				ParseRGBAColor(r, &pHclr->stHighlightColor);
			}
			break;
		case FOURCC2_krok:
			{
				Tx3gTextKaraokeBox *pKrok  = m_3gpTTextOp.AddSampleModifierBox<Tx3gTextKaraokeBox>(pCurTsmb, type);
				//Tx3gTextKaraokeBox *pKrok = (Tx3gTextKaraokeBox *)pCurTsmb->pTxtModifier;
				ParseKaraokeBox(r, pKrok);
			}
			break;
		case FOURCC2_dlay:
			{
				Tx3gTextScrollDelayBox *pDlay = m_3gpTTextOp.AddSampleModifierBox<Tx3gTextScrollDelayBox>(pCurTsmb, type);
				//Tx3gTextScrollDelayBox *pDlay = (Tx3gTextScrollDelayBox *)pCurTsmb->pTxtModifier;
				r->Read(&pDlay->scrollDelay);
			}
			break;
		case FOURCC2_href:
			{
				Tx3gTextHyperTextBox *pHref = m_3gpTTextOp.AddSampleModifierBox<Tx3gTextHyperTextBox>(pCurTsmb, type);
				//Tx3gTextHyperTextBox *pHref = (Tx3gTextHyperTextBox *)pCurTsmb->pTxtModifier;
				ParseHrefBox(r, pHref);
			}
			break;
		case FOURCC2_tbox:
			{
				Tx3gTextboxBox *pTBox = m_3gpTTextOp.AddSampleModifierBox<Tx3gTextboxBox>(pCurTsmb, type);
				//Tx3gTextboxBox *pTBox = (Tx3gTextboxBox *)pCurTsmb->pTxtModifier;
				ParseBoxRecord(r, &pTBox->stTextBox);
			}
			break;
		case FOURCC2_blnk:
			{
				Tx3gBlinkBox *pBlnk = m_3gpTTextOp.AddSampleModifierBox<Tx3gBlinkBox>(pCurTsmb, type);
				//Tx3gBlinkBox *pBlnk = (Tx3gBlinkBox *)pCurTsmb->pTxtModifier;
				r->Read(&pBlnk->nStartCharoffset);
				r->Read(&pBlnk->nEndCharoffset);
			}
			break;
		case FOURCC2_twrp:
			{
				Tx3gTextWrapBox *pTwrp = m_3gpTTextOp.AddSampleModifierBox<Tx3gTextWrapBox>(pCurTsmb, type);
				//Tx3gTextWrapBox *pTwrp = (Tx3gTextWrapBox *)pCurTsmb->pTxtModifier;
				r->Read(&pTwrp->nWrapFlag);
			}
			break;
		default:
			return MP4RRC_SEEK_FAILED;
		}
		bodysize -=size + 2;

		pSample->nEntryCount ++;
	}

	return MP4RRC_OK;
}


MP4RRC TrackSubtitle::ParseHrefBox(Reader* r, Tx3gTextHyperTextBox *ptr)
{
	r->Read(&ptr->nStartCharoffset);
	r->Read(&ptr->nEndCharoffset);
	r->Read(&ptr->nURLLength);
	m_3gpTTextOp.CreateStructBuffer<unsigned char>(ptr->nURLLength, &ptr->pStrURL);

	//	RESETBUF(ptr->nURLLength, ptr->pStrURL, unsigned char);
	r->Read(ptr->pStrURL, ptr->nURLLength);
	r->Read(&ptr->nAltLength);
	m_3gpTTextOp.CreateStructBuffer<unsigned char>(ptr->nAltLength, &ptr->pStrAltString);

	//RESETBUF(ptr->nAltLength, ptr->pStrAltString, unsigned char);
	r->Read(ptr->pStrAltString, ptr->nAltLength);

	return MP4RRC_OK;
}

#endif
/* 12/10/2011, leon for subtitle*/

MP4RRC TrackSubtitle::ParseTextSampleEntry(Reader* r, int bodysize)
{
	VOLOGI("+ParseTextSampleEntry");
	Tx3gDescription *tx3g = &m_stTx3gDescription;
	r->Read(&tx3g->nDisplayFlags);
	r->Read(&tx3g->nHorizontalJustification);
	r->Read(&tx3g->nVerticalJustification);

	ParseRGBAColor(r, &tx3g->stBackgroundColor);
	ParseBoxRecord(r, &tx3g->stDefaultTextBox);
	ParseStyleRecord(r, &tx3g->stDefaultStyle);

	MP4RRC nRC = ParseDescriptor(r, bodysize);
	VOLOGI("- ParseTextSampleEntry.nRc:0x%08x",nRC);
	return nRC;
}

MP4RRC TrackSubtitle::ParseDescriptor(Reader* r, int bodysize)
{
	uint32 size, type;
	int leftsize =(int)(bodysize - r->Position());
	while (true) 	{
		if (!r->Read(&size) || !r->Read(&type) || size == 0) 
		{
			_descriptor_size = 0;
			return MP4RRC_OK;
		}
		if ((int)size > leftsize)
			return MP4RRC_BAD_DESCRIPTOR;
		if (type == FOURCC2_ftab  ) 
			break;
		else
			return MP4RRC_SEEK_FAILED;
	}

	_descriptor_type = type;
	_descriptor_size = size - 8; //8 byte header,
	if (_descriptor_size > 0){
		Tx3gDescription *tx3g = &m_stTx3gDescription;
		r->Read(&tx3g->stFontTable.nEntryCount);
		m_3gpTTextOp.CreateStructBuffer<Tx3gFontRecord>(tx3g->stFontTable.nEntryCount, &tx3g->stFontTable.pStFontRecord);

		//RESETBUF(tx3g->stFontTable.nEntryCount, tx3g->stFontTable.pStFontRecord, Tx3gFontRecord);
		for(int i = 0;i < tx3g->stFontTable.nEntryCount ; i++){
			ParseFontRecord(r, &tx3g->stFontTable.pStFontRecord[i]);
			VOLOGI("FontRecord. size:%d , name :%s", tx3g->stFontTable.pStFontRecord[i].nNameLength,  tx3g->stFontTable.pStFontRecord[i].strName)
		}
		_descriptor_data = (uint8*)&m_stTx3gDescription;
		_descriptor_size = sizeof(m_stTx3gDescription);
	}

	return MP4RRC_OK;
}

MP4RRC TrackSubtitle::ParseRGBAColor(Reader* r, Tx3gRGBAColor *ptr)
{
	r->Read(&ptr->nRed);
	r->Read(&ptr->nGreen);
	r->Read(&ptr->nBlue);
	r->Read(&ptr->nTransparency);
	return MP4RRC_OK;
}
MP4RRC TrackSubtitle::ParseBoxRecord(Reader* r, Tx3gBoxRecord *ptr)
{
	r->Read(&ptr->nTop);
	r->Read(&ptr->nLeft);
	r->Read(&ptr->nBottom);
	r->Read(&ptr->nRight);

	return MP4RRC_OK;
}

MP4RRC TrackSubtitle::ParseStyleRecord(Reader* r, Tx3gStyleRecord *ptr)
{
	r->Read(&ptr->nStartChar);
	r->Read(&ptr->nEndChar);
	r->Read(&ptr->nFontID);
	r->Read(&ptr->nFontFace);
	r->Read(&ptr->nFontSize);
	r->Read(&ptr->stFontColor.nRed);
	r->Read(&ptr->stFontColor.nGreen);
	r->Read(&ptr->stFontColor.nBlue);
	r->Read(&ptr->stFontColor.nTransparency);
	return MP4RRC_OK;
}
MP4RRC TrackSubtitle::ParseFontRecord(Reader* r, Tx3gFontRecord *ptr)
{
	r->Read(&ptr->nFontID);
	r->Read(&ptr->nNameLength);
	r->Read(ptr->strName, ptr->nNameLength);
	return MP4RRC_OK;
}
