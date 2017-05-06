
#include "voProgramInfo.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


VO_VOID CopyTrackInfoOP_EX( _TRACK_INFO *pSourceInfo, VO_SOURCE2_TRACK_INFO **ppDestInfo)
{

	if( !pSourceInfo )
		return;

	if( *ppDestInfo )
	{
		ReleaseTrackInfoOP_T( *ppDestInfo );
		*ppDestInfo = NULL;
	}

	VO_SOURCE2_TRACK_INFO  *pTmpTrackInfo = *ppDestInfo = new VO_SOURCE2_TRACK_INFO;
	memset( *ppDestInfo, 0x00, sizeof( VO_SOURCE2_TRACK_INFO ) );
	if( pSourceInfo->uTrackType == VO_SOURCE2_TT_AUDIO || pSourceInfo->uTrackType == VO_SOURCE2_TT_AUDIOGROUP )
	{
		pTmpTrackInfo->sAudioInfo.sFormat = pSourceInfo->sAudioInfo.sFormat;
		memcpy( pTmpTrackInfo->sAudioInfo.chLanguage, pSourceInfo->sAudioInfo.chLanguage, sizeof( pSourceInfo->sAudioInfo.chLanguage ) );
	}
	else if( pSourceInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE || pSourceInfo->uTrackType == VO_SOURCE2_TT_SUBTITLEGROUP )
	{
		memcpy( pTmpTrackInfo->sSubtitleInfo.Align, pSourceInfo->sSubtitleInfo.Align, sizeof( pSourceInfo->sSubtitleInfo.Align ) );
		pTmpTrackInfo->sSubtitleInfo.uCodingType = pSourceInfo->sSubtitleInfo.uCodingType;
		memcpy( pTmpTrackInfo->sSubtitleInfo.chLanguage, pSourceInfo->sSubtitleInfo.chLanguage, sizeof( pSourceInfo->sSubtitleInfo.chLanguage ) );
	}
	else if( pSourceInfo->uTrackType == VO_SOURCE2_TT_VIDEO || pSourceInfo->uTrackType == VO_SOURCE2_TT_VIDEOGROUP )
	{
		pTmpTrackInfo->sVideoInfo.sFormat = pSourceInfo->sVideoInfo.sFormat;
		memcpy( pTmpTrackInfo->sVideoInfo.strVideoDesc, pSourceInfo->sVideoInfo.strVideoDesc, sizeof( pSourceInfo->sVideoInfo.strVideoDesc ) );
		pTmpTrackInfo->sVideoInfo.uAngle = pSourceInfo->sVideoInfo.uAngle;
	}

	memcpy( pTmpTrackInfo->strFourCC, pSourceInfo->strFourCC, sizeof( pSourceInfo->strFourCC ) );

	pTmpTrackInfo->uBitrate = pSourceInfo->uBitrate;

	pTmpTrackInfo->uChunkCounts = pSourceInfo->uCodec;

	pTmpTrackInfo->uDuration = pSourceInfo->uDuration;

	pTmpTrackInfo->uHeadSize = pSourceInfo->uHeadSize;

	pTmpTrackInfo->uSelInfo = pSourceInfo->uSelInfo;
	pTmpTrackInfo->uCodec = pSourceInfo->uCodec;

#ifdef _new_programinfo
	pTmpTrackInfo->uTrackID = pSourceInfo->uOutSideTrackID;
#else
	pTmpTrackInfo->uTrackID = pSourceInfo->uTrackID;
#endif
	pTmpTrackInfo->uTrackType = pSourceInfo->uTrackType;

	pTmpTrackInfo->pHeadData = NULL;
#ifdef _cc_info
        pTmpTrackInfo->nVideoClosedCaptionDescDataLen = pSourceInfo->nVideoClosedCaptionDescDataLen;
	pTmpTrackInfo->pVideoClosedCaptionDescData = NULL;
#endif
	//	VOLOGR("3");
	if( pTmpTrackInfo->uHeadSize > 0 )
	{
		pTmpTrackInfo->pHeadData = new VO_BYTE[ pTmpTrackInfo->uHeadSize ];
		memset( pTmpTrackInfo->pHeadData, 0x00, sizeof( VO_BYTE ) * pTmpTrackInfo->uHeadSize );
		memcpy( pTmpTrackInfo->pHeadData, pSourceInfo->pHeadData, pSourceInfo->uHeadSize );
	}
#ifdef _cc_info
	if(pTmpTrackInfo->nVideoClosedCaptionDescDataLen > 0)
	{
		pTmpTrackInfo->pVideoClosedCaptionDescData = new VO_BYTE[pTmpTrackInfo->nVideoClosedCaptionDescDataLen];
		memset(pTmpTrackInfo->pVideoClosedCaptionDescData,0x00,sizeof(VO_BYTE) * pTmpTrackInfo->nVideoClosedCaptionDescDataLen);
		memcpy(pTmpTrackInfo->pVideoClosedCaptionDescData, pSourceInfo->pVideoClosedCaptionDescData, pSourceInfo->nVideoClosedCaptionDescDataLen);
	}
#endif
}

VO_VOID CopyTrackInfoOP_EX2( VO_SOURCE2_TRACK_INFO *pSourceInfo, _TRACK_INFO **ppDestInfo)
{

	if( !pSourceInfo )
		return;

	if( *ppDestInfo )
	{
		ReleaseTrackInfoOP_T( *ppDestInfo );
		*ppDestInfo = NULL;
	}

	_TRACK_INFO  *pTmpTrackInfo = *ppDestInfo = new _TRACK_INFO;
	memset( *ppDestInfo, 0x00, sizeof( _TRACK_INFO ) );
	if( pSourceInfo->uTrackType == VO_SOURCE2_TT_AUDIO || pSourceInfo->uTrackType == VO_SOURCE2_TT_AUDIOGROUP )
	{
		pTmpTrackInfo->sAudioInfo.sFormat = pSourceInfo->sAudioInfo.sFormat;
		memcpy( pTmpTrackInfo->sAudioInfo.chLanguage, pSourceInfo->sAudioInfo.chLanguage, sizeof( pSourceInfo->sAudioInfo.chLanguage ) );
	}
	else if( pSourceInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE || pSourceInfo->uTrackType == VO_SOURCE2_TT_SUBTITLEGROUP )
	{
		memcpy( pTmpTrackInfo->sSubtitleInfo.Align, pSourceInfo->sSubtitleInfo.Align, sizeof( pSourceInfo->sSubtitleInfo.Align ) );
		pTmpTrackInfo->sSubtitleInfo.uCodingType = pSourceInfo->sSubtitleInfo.uCodingType;
		memcpy( pTmpTrackInfo->sSubtitleInfo.chLanguage, pSourceInfo->sSubtitleInfo.chLanguage, sizeof( pSourceInfo->sSubtitleInfo.chLanguage ) );
	}
	else if( pSourceInfo->uTrackType == VO_SOURCE2_TT_VIDEO || pSourceInfo->uTrackType == VO_SOURCE2_TT_VIDEOGROUP )
	{
		pTmpTrackInfo->sVideoInfo.sFormat = pSourceInfo->sVideoInfo.sFormat;
		memcpy( pTmpTrackInfo->sVideoInfo.strVideoDesc, pSourceInfo->sVideoInfo.strVideoDesc, sizeof( pSourceInfo->sVideoInfo.strVideoDesc ) );
		pTmpTrackInfo->sVideoInfo.uAngle = pSourceInfo->sVideoInfo.uAngle;
	}

	memcpy( pTmpTrackInfo->strFourCC, pSourceInfo->strFourCC, sizeof( pSourceInfo->strFourCC ) );

	pTmpTrackInfo->uBitrate = pSourceInfo->uBitrate;

	pTmpTrackInfo->uChunkCounts = pSourceInfo->uCodec;

	pTmpTrackInfo->uDuration = pSourceInfo->uDuration;

	pTmpTrackInfo->uHeadSize = pSourceInfo->uHeadSize;

	pTmpTrackInfo->uSelInfo = pSourceInfo->uSelInfo;
	pTmpTrackInfo->uCodec = pSourceInfo->uCodec;

#ifdef _new_programinfo
	pTmpTrackInfo->uOutSideTrackID = pSourceInfo->uTrackID;
#else
	pTmpTrackInfo->uTrackID = pSourceInfo->uTrackID;
#endif
	pTmpTrackInfo->uTrackType = pSourceInfo->uTrackType;

	pTmpTrackInfo->pHeadData = NULL;
#ifdef _cc_info
        pTmpTrackInfo->nVideoClosedCaptionDescDataLen = pSourceInfo->nVideoClosedCaptionDescDataLen;
	pTmpTrackInfo->pVideoClosedCaptionDescData = NULL;
#endif

	if( pTmpTrackInfo->uHeadSize > 0 )
	{
		pTmpTrackInfo->pHeadData = new VO_BYTE[ pTmpTrackInfo->uHeadSize ];
		memset( pTmpTrackInfo->pHeadData, 0x00, sizeof( VO_BYTE ) * pTmpTrackInfo->uHeadSize );
		memcpy( pTmpTrackInfo->pHeadData, pSourceInfo->pHeadData, pSourceInfo->uHeadSize );
	}
#ifdef _cc_info
	if(pTmpTrackInfo->nVideoClosedCaptionDescDataLen > 0)
	{
		pTmpTrackInfo->pVideoClosedCaptionDescData = new VO_BYTE[pTmpTrackInfo->nVideoClosedCaptionDescDataLen];
		memset(pTmpTrackInfo->pVideoClosedCaptionDescData,0x00,sizeof(VO_BYTE) * pTmpTrackInfo->nVideoClosedCaptionDescDataLen);
		memcpy(pTmpTrackInfo->pVideoClosedCaptionDescData, pSourceInfo->pVideoClosedCaptionDescData, pSourceInfo->nVideoClosedCaptionDescDataLen);
	}
#endif

}

VO_VOID CopyStreamInfoOP_EX( _STREAM_INFO *pSourceInfo, VO_SOURCE2_STREAM_INFO **ppDestInfo )
{
	if( !pSourceInfo )
		return;

	if( *ppDestInfo )
	{
		ReleaseStreamInfoOP_T( *ppDestInfo );
		*ppDestInfo = NULL;
	}

	//streaminfo *
	VO_SOURCE2_STREAM_INFO * pTmpStreamInfo = *ppDestInfo = new VO_SOURCE2_STREAM_INFO;
	memset( *ppDestInfo, 0x00, sizeof( VO_SOURCE2_STREAM_INFO ) );
	pTmpStreamInfo->uBitrate = pSourceInfo->uBitrate;
	pTmpStreamInfo->uSelInfo = pSourceInfo->uSelInfo;
	pTmpStreamInfo->uStreamID = pSourceInfo->uStreamID;
	pTmpStreamInfo->uTrackCount = pSourceInfo->uTrackCount;

	//trackinfo **
	pTmpStreamInfo->ppTrackInfo = CreateInfo_T( pTmpStreamInfo->ppTrackInfo, pTmpStreamInfo->uTrackCount );
	//VOLOGR("2");
	for( VO_U32 i = 0; i < pTmpStreamInfo->uTrackCount; i++ )
	{
		CopyTrackInfoOP_EX( pSourceInfo->ppTrackInfo[i], &pTmpStreamInfo->ppTrackInfo[i] );
	}

}


VO_VOID CopyProgramInfoOP_EX( _PROGRAM_INFO *pSourceProgInfo, VO_SOURCE2_PROGRAM_INFO **ppDestProgInfo )
{

	if( *ppDestProgInfo )
		ReleaseProgramInfoOP_T( *ppDestProgInfo );
	//programinfo *
	VO_SOURCE2_PROGRAM_INFO * pTmpProgInfo = *ppDestProgInfo = new VO_SOURCE2_PROGRAM_INFO;
	memset( pTmpProgInfo , 0x00, sizeof( VO_SOURCE2_PROGRAM_INFO ) );
	pTmpProgInfo->uProgramID = pSourceProgInfo->uProgramID;
	pTmpProgInfo->uSelInfo = pSourceProgInfo->uSelInfo;
	pTmpProgInfo->sProgramType = pSourceProgInfo->sProgramType;
	memcpy(pTmpProgInfo->strProgramName, pSourceProgInfo->strProgramName, sizeof( pSourceProgInfo->strProgramName ) );
	pTmpProgInfo->uStreamCount = pSourceProgInfo->uStreamCount;

	//streaminfo **
	pTmpProgInfo->ppStreamInfo = CreateInfo_T( pTmpProgInfo->ppStreamInfo, pTmpProgInfo->uStreamCount );
	//VOLOGR("1");
	for( VO_U32 i = 0; i < pTmpProgInfo->uStreamCount; i++ )
	{
		CopyStreamInfoOP_EX( pSourceProgInfo->ppStreamInfo[i], &pTmpProgInfo->ppStreamInfo[i] );
	}

}

#ifdef _VONAMESPACE
}
#endif 