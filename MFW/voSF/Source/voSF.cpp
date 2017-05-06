	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voStageFright.cpp

	Contains:	voStageFright class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#include "voSF.h"

#include "CFileMediaExtractor.h"
#include "CFileMediaSource.h"
#include "CMediaDecoderSource.h"
#include "CMediaEncoderSource.h"
#include "CFileMediaSink.h"

#define LOG_TAG "voSF"
#include "voLog.h"


void * vosfCreateMediaExtractor (void * pDataSource, int nFlag, VOSF_MEDIABUFFERTYPE * pMediaBufferOP,
								  VOSF_METADATATYPE * pMetaDataOP)
{
//	VOSF_DATASOURCETYPE * pSource = (VOSF_DATASOURCETYPE *)pDataSource;

//	VOLOGI ("pSource size %d, ver %p, Source %p, read %p", pSource->nSize, pSource->nVersion, pSource->pDataSource, pSource->readAt);

//	VOLOGI ("create %p", pMediaBufferOP->create);

	CBaseMediaExtractor * pExtractor = new CFileMediaExtractor (pDataSource, nFlag & 0X0F, pMediaBufferOP, pMetaDataOP);

	return pExtractor;
}

int vosfGetTrackNum (void * pExtractor)
{
//	VOLOGI ("");

	if (pExtractor == NULL)
		return VOSF_ERR_INVALID_ARG;

	return ((CBaseMediaExtractor *)pExtractor)->GetTrackNum ();
}

void * vosfGetTrackSource (void * pExtractor, int nTrack)
{
//	VOLOGI ("");

	if (pExtractor == NULL)
		return NULL;

	return ((CBaseMediaExtractor *)pExtractor)->GetTrackSource (nTrack);
}

void * vosfGetTrackMetaData (void * pExtractor, int nTrack, int nFlag)
{
//	VOLOGI ("");

	if (pExtractor == NULL)
		return NULL;

	return ((CBaseMediaExtractor *)pExtractor)->GetTrackMetaData (nTrack, nFlag);
}

void * vosfGetMetaData (void * pExtractor)
{
//	VOLOGI ("");

	if (pExtractor == NULL)
		return NULL;

	return ((CBaseMediaExtractor *)pExtractor)->GetMetaData ();
}

int vosfGetFlag (void * pExtractor)
{
//	VOLOGI ("");

	if (pExtractor == NULL)
		return VOSF_ERR_INVALID_ARG;

	return ((CBaseMediaExtractor *)pExtractor)->GetFlag ();
}

int vosfGetExtractorParam (void * pExtractor, int nParamID, void * pValue1, void * pValue2)
{
	if (pExtractor == NULL)
		return VOSF_ERR_INVALID_ARG;

	return ((CBaseMediaExtractor *)pExtractor)->GetParam (nParamID, pValue1, pValue2);;
}

int vosfSetExtractorParam (void * pExtractor, int nParamID, void * pValue1, void * pValue2)
{
	if (pExtractor == NULL)
		return VOSF_ERR_INVALID_ARG;

	return ((CBaseMediaExtractor *)pExtractor)->SetParam (nParamID, pValue1, pValue2);;
}

int vosfCloseMediaExtractor (void * pExtractor)
{
//	VOLOGI ("");

	if (pExtractor == NULL)
		return VOSF_ERR_INVALID_ARG;

	delete (CBaseMediaExtractor *)pExtractor;

	return VOSF_ERR_NONE;
}

void vosfSetMode (void * pExtractor, VOSF_MODE inMode)
{
	if (pExtractor == NULL)
		return ;
	
	((CBaseMediaExtractor *)pExtractor)->setMode(inMode);
}


void * vosfCreateMediaSource (void * pMediaSource, int nFlag, VOSF_MEDIABUFFERTYPE * pMediaBufferOP,
								  VOSF_METADATATYPE * pMetaDataOP  , void *pMetaData)
{
//	VOLOGI ("");

//	VOSF_MEDIASOURCETYPE * pMediaData = (VOSF_MEDIASOURCETYPE *)pMediaSource;
//	VOLOGI ("Ver %08X, Source %p, Read %p", pMediaData->nVersion, pMediaData->pMediaSource, pMediaData->read);

	CBaseMediaSource * pMediaDec = NULL;
	if(nFlag == 0)
		pMediaDec = new CMediaDecoderSource (pMediaSource, nFlag, pMediaBufferOP, pMetaDataOP);
	else
		pMediaDec = new CMediaEncoderSource (pMediaSource,pMediaBufferOP, pMetaDataOP , pMetaData);

//	VOLOGI ("CMediaDecoderSource is %p", pMediaDec);

	return pMediaDec;
}

int vosfStart (void * pSource, void * pParam)
{
//	VOLOGI ("pSource %p", pSource);

	if (pSource == NULL)
		return VOSF_ERR_INVALID_ARG;

//	VOLOGI ("pParam %p", pParam);

	return ((CBaseMediaSource *)pSource)->Start (pParam);
}

int vosfStop (void * pSource)
{
//	VOLOGI ("pSource %p", pSource);

	if (pSource == NULL)
		return VOSF_ERR_INVALID_ARG;

	return ((CBaseMediaSource *)pSource)->Stop ();
}

void * vosfGetFormat (void * pSource)
{
//	VOLOGI ("pSource %p", pSource);

	if (pSource == NULL)
		return NULL;

	return ((CBaseMediaSource *)pSource)->GetFormat ();
}

int vosfRead (void * pSource, void ** ppBuffer, void * pOptions)
{
//	VOLOGI ("pSource %p", pSource);

	if (pSource == NULL)
		return VOSF_ERR_INVALID_ARG;

	int nRC = ((CBaseMediaSource *)pSource)->Read (ppBuffer, pOptions);

//	VOLOGI ("vosfRead result is %d", nRC);

	return nRC;
}

int vosfGetMediaSourceParam (void * pSource, int nParamID, void * pValue1, void * pValue2)
{
	if (pSource == NULL)
		return VOSF_ERR_INVALID_ARG;

	return ((CBaseMediaSource *)pSource)->GetParam (nParamID, pValue1, pValue2);;
}

int vosfSetMediaSourceParam (void * pSource, int nParamID, void * pValue1, void * pValue2)
{
	if (pSource == NULL)
		return VOSF_ERR_INVALID_ARG;

	return ((CBaseMediaSource *)pSource)->SetParam (nParamID, pValue1, pValue2);;
}

int vosfCloseMediaSource (void * pSource)
{
	VOLOGI ("pSource %p", pSource);

	if (pSource == NULL)
		return VOSF_ERR_INVALID_ARG;

	delete (CBaseMediaSource *)pSource;

	return VOSF_ERR_NONE;
}

void*	vosfCreateWriter(int fd, int format, VOSF_MEDIABUFFERTYPE * pMediaBufferOP, 
						 VOSF_METADATATYPE * pMetaDataOP, VOSF_MEDIASOURCETYPE * pMediaSourceOP)
{
	return new CFileMediaSink(fd, format, pMediaBufferOP, pMetaDataOP, pMediaSourceOP);
}

int		vosfWriterAddSource(void* pWriter, void* pSource)
{
	CFileMediaSink* pSink = (CFileMediaSink*) pWriter;
	if(pSink)
		return pSink->AddSource(pSource);
	return VO_ERR_NOT_IMPLEMENT;
}

int     vosfWriterStart(void* pWriter, void *pMetaData)
{
	CFileMediaSink* pSink = (CFileMediaSink*) pWriter;
	if(pSink)
		return pSink->Start();
	return VO_ERR_NOT_IMPLEMENT;
}

int 	vosfWriterStop(void* pWriter)
{
	CFileMediaSink* pSink = (CFileMediaSink*) pWriter;
	if(pSink)
		return pSink->Stop();
	return VO_ERR_NOT_IMPLEMENT;
}

int 	vosfWriterPause(void* pWriter)
{
	CFileMediaSink* pSink = (CFileMediaSink*) pWriter;
	if(pSink)
		return pSink->Pause();
	return VO_ERR_NOT_IMPLEMENT;
}

bool    vosfWriterReachEOS(void* pWriter)
{
	CFileMediaSink* pSink = (CFileMediaSink*) pWriter;
	if(pSink)
		return pSink->ReachEOS();
	return true;
}

int		vosfCloseWriter(void* pWriter)
{
	CFileMediaSink* pSink = (CFileMediaSink*) pWriter;
	if(pSink)
		delete pSink;
	return VO_ERR_NONE;
}

