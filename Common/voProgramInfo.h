/********************************************************************
* Copyright 2003 ~ 2012 by VisualOn Software, Inc.
* All modifications are confidential and proprietary information
* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
*********************************************************************
* File Name: 
*            voProgramInfo.h
*
* Project:
* contents/description: a serious of operation func for ProgramInfo
*            
***************************** Change History**************************
* 
*    DD/MMM/YYYY     Code Ver     Description             Author
*    -----------     --------     -----------             ------
*    01-06-2013        1.0							     Leon Huang
*                                             
**********************************************************************/

#ifndef __VOPROGRAMINFO_H__
#define __VOPROGRAMINFO_H__

#include "voYYDef_Common.h"
#include "voType.h"
#include "voSource2.h"
 

#ifdef _new_programinfo
#include "voDSType.h"
#else

#ifndef __PROGRAMINFO
#define __PROGRAMINFO
typedef		VO_SOURCE2_PROGRAM_INFO		_PROGRAM_INFO;		
typedef		VO_SOURCE2_TRACK_INFO		_TRACK_INFO;		
typedef		VO_SOURCE2_STREAM_INFO		_STREAM_INFO;	
#endif

#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_VOID CopyTrackInfoOP_EX( _TRACK_INFO *pSourceInfo, VO_SOURCE2_TRACK_INFO **ppDestInfo);

VO_VOID CopyTrackInfoOP_EX2( VO_SOURCE2_TRACK_INFO *pSourceInfo, _TRACK_INFO **ppDestInfo);

VO_VOID CopyStreamInfoOP_EX( _STREAM_INFO *pSourceInfo, VO_SOURCE2_STREAM_INFO **ppDestInfo );

VO_VOID CopyProgramInfoOP_EX( _PROGRAM_INFO *pSourceProgInfo, VO_SOURCE2_PROGRAM_INFO **ppDestProgInfo );


template< class T >
VO_VOID ReleaseTrackInfoOP_T( T *ppInfo)
{
	if( !ppInfo )
		return;

	if( ppInfo->pHeadData )
		delete []ppInfo->pHeadData;
	ppInfo->pHeadData = NULL;
#ifdef _cc_info
	if(ppInfo->pVideoClosedCaptionDescData)
		delete []ppInfo->pVideoClosedCaptionDescData;
	ppInfo->pVideoClosedCaptionDescData = NULL;
#endif
	delete ppInfo;
	ppInfo = NULL;

}

template< class T >
VO_VOID ReleaseStreamInfoOP_T( T *ppInfo)
{
	if( !ppInfo )
		return;

	for( VO_U32 i = 0; i < ppInfo->uTrackCount; i++)
	{
		ReleaseTrackInfoOP_T( ppInfo->ppTrackInfo[i] );
		ppInfo->ppTrackInfo[i] = NULL;
	}
	delete []ppInfo->ppTrackInfo;
	ppInfo->ppTrackInfo = NULL;

	delete ppInfo;
	ppInfo = NULL;

}

template< class T >
VO_VOID ReleaseProgramInfoOP_T( T *ppInfo )
{	
	if( !ppInfo )
		return;

	for( VO_U32 i = 0; i < ppInfo->uStreamCount; i++)
	{

		ReleaseStreamInfoOP_T( ppInfo->ppStreamInfo[i] );
		ppInfo->ppStreamInfo[i] = NULL;
	}
	delete []ppInfo->ppStreamInfo;
	ppInfo->ppStreamInfo = NULL;

	delete ppInfo;
	ppInfo = NULL;
}

template< class T >
T** CreateInfo_T( T **ppInfo, VO_U32 uCount )
{
	if( uCount <= 0 )
		return NULL;

	T **pp = new T *[ uCount ];
	memset( pp, 0x00, sizeof( T *) * uCount );
	return pp;
}
template< class T >
VO_VOID CopyTrackInfoOP_T( T *pSourceInfo, T **ppDestInfo)
{

	if( !pSourceInfo )
		return;

	if( *ppDestInfo )
	{
		ReleaseTrackInfoOP_T( *ppDestInfo );
		*ppDestInfo = NULL;
	}

	T  *pTmpTrackInfo = *ppDestInfo = new T;
	memset( *ppDestInfo, 0x00, sizeof( T ) );
	memcpy( *ppDestInfo, pSourceInfo, sizeof( T ) );
	pTmpTrackInfo->pHeadData = NULL;
#ifdef _cc_info
	pTmpTrackInfo->pVideoClosedCaptionDescData = NULL
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

template< class T >
VO_VOID CopyStreamInfoOP_T( T *pSourceInfo, T **ppDestInfo )
{
	if( !pSourceInfo )
		return;

	if( *ppDestInfo )
	{
		ReleaseStreamInfoOP_T( *ppDestInfo );
		*ppDestInfo = NULL;
	}

	//streaminfo *
	T * pTmpStreamInfo = *ppDestInfo = new T;
	memset( *ppDestInfo, 0x00, sizeof( T ) );
	memcpy( *ppDestInfo, pSourceInfo, sizeof( T ) );
	(*ppDestInfo)->ppTrackInfo = NULL;
	//trackinfo **
	pTmpStreamInfo->ppTrackInfo = CreateInfo_T( pTmpStreamInfo->ppTrackInfo, pTmpStreamInfo->uTrackCount );

	for( VO_U32 i = 0; i < pTmpStreamInfo->uTrackCount; i++ )
	{
		CopyTrackInfoOP_T( pSourceInfo->ppTrackInfo[i], &pTmpStreamInfo->ppTrackInfo[i] );
	}

}

template< class P >
VO_VOID CopyProgramInfoOP_T( P *pSourceInfo, P **ppDestInfo )
{
	if( !pSourceInfo )
		return;

	if( *ppDestInfo )
	{
		ReleaseProgramInfoOP_T( *ppDestInfo );
		*ppDestInfo = NULL;
	}
	//programinfo *
	P * pTmpProgInfo = *ppDestInfo = new P;
	memset( *ppDestInfo , 0x00, sizeof( P ) );
	memcpy( *ppDestInfo, pSourceInfo, sizeof( P ) );
	(*ppDestInfo)->ppStreamInfo = NULL;
	//streaminfo **
	pTmpProgInfo->ppStreamInfo = CreateInfo_T( pTmpProgInfo->ppStreamInfo, pTmpProgInfo->uStreamCount );

	for( VO_U32 i = 0; i < pTmpProgInfo->uStreamCount; i++ )
	{
		CopyStreamInfoOP_T( pSourceInfo->ppStreamInfo[i], &pTmpProgInfo->ppStreamInfo[i] );
	}

}

template< class S >
_TRACK_INFO* GetTrackInfoBy2IDOP_T( S *pInfo, VO_U32 uASTrackID, VO_U32 uFPTrackID )
{
	if( !pInfo )
		return NULL;
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{

#ifdef _new_programinfo
		if( ( pInfo->ppTrackInfo[i] )->uASTrackID == uASTrackID && ( pInfo->ppTrackInfo[i] )->uFPTrackID == uFPTrackID )
#else
		continue;
#endif
		{
			return pInfo->ppTrackInfo[i];
		}
	}
	return NULL;
}

template< class S, class T >
VO_U32 InsertTrackInfoOP2_T( S * pStreamInfo, T *pTrackInfo )
{
	if( !pStreamInfo || !pTrackInfo )
		return VO_RET_SOURCE2_FAIL;

	T ** ppTmpTrackInfo = pStreamInfo->ppTrackInfo;
	VO_U32 nOldCount = pStreamInfo->uTrackCount;
	_TRACK_INFO *pT = NULL;
#ifdef _new_programinfo
	pT = GetTrackInfoBy2IDOP_T( pStreamInfo, pTrackInfo->uASTrackID, pTrackInfo->uFPTrackID );
#else 
	pT = GetTrackInfoByIDOP_T(pStreamInfo, pTrackInfo->uTrackID );
#endif
	if( pT )
		return VO_RET_SOURCE2_FAIL;

	pStreamInfo->uTrackCount = nOldCount + 1;
	pStreamInfo->ppTrackInfo = new T *[ pStreamInfo->uTrackCount ];
	memset(pStreamInfo->ppTrackInfo, 0x00, sizeof( T * ) * pStreamInfo->uTrackCount );
	for( VO_U32 n = 0; n < nOldCount; n++) 
	{
		CopyTrackInfoOP_T( ppTmpTrackInfo[n], &pStreamInfo->ppTrackInfo[n] );
		ReleaseTrackInfoOP_T( ppTmpTrackInfo[n] );
		ppTmpTrackInfo[n] = NULL;
	}
	delete []ppTmpTrackInfo;
	CopyTrackInfoOP_T( pTrackInfo, &pStreamInfo->ppTrackInfo[pStreamInfo->uTrackCount - 1] );

	return VO_RET_SOURCE2_OK;
}

template< class S, class T >
VO_U32 InsertTrackInfoOP3_T( S * pStreamInfo, T *pTrackInfo )
{
	if( !pStreamInfo || !pTrackInfo )
		return VO_RET_SOURCE2_FAIL;

	T ** ppTmpTrackInfo = pStreamInfo->ppTrackInfo;
	VO_U32 nOldCount = pStreamInfo->uTrackCount;
	_TRACK_INFO *pT = NULL;

	pStreamInfo->uTrackCount = nOldCount + 1;
	pStreamInfo->ppTrackInfo = new T *[ pStreamInfo->uTrackCount ];
	memset(pStreamInfo->ppTrackInfo, 0x00, sizeof( T * ) * pStreamInfo->uTrackCount );
	for( VO_U32 n = 0; n < nOldCount; n++) 
	{
		CopyTrackInfoOP_T( ppTmpTrackInfo[n], &pStreamInfo->ppTrackInfo[n] );
		ReleaseTrackInfoOP_T( ppTmpTrackInfo[n] );
		ppTmpTrackInfo[n] = NULL;
	}
	delete []ppTmpTrackInfo;
	CopyTrackInfoOP_T( pTrackInfo, &pStreamInfo->ppTrackInfo[pStreamInfo->uTrackCount - 1] );

	return VO_RET_SOURCE2_OK;
}

template< class P, class S >
VO_U32 SelectStreamOP_T( P *pInfo, VO_U32 uStreamID, S ** ppStreamInfo )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	if( !pInfo )
		return ret;

	for( VO_U32 i = 0; i < pInfo->uStreamCount; i++ )
	{
		S *pStreamInfo = pInfo->ppStreamInfo[i];
		if(  pStreamInfo->uStreamID  == uStreamID )
		{

			pStreamInfo->uSelInfo |= VO_SOURCE2_SELECT_SELECTED;
			*ppStreamInfo = pStreamInfo;
			ret = VO_RET_SOURCE2_OK;
		}
		else
		{
			pStreamInfo->uSelInfo &= ~VO_SOURCE2_SELECT_SELECTED;
			pStreamInfo->uSelInfo |= VO_SOURCE2_SELECT_SELECTABLE;
		}
	}
	return ret;
}

template< class S, class T >
VO_U32 SelectTrackOP_T( S * pInfo, VO_U32 uOutSideTrackID, T **ppTrackInfo )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	if( !pInfo )
		return ret;

	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{
		T *pTmpTrackInfo = pInfo->ppTrackInfo[i];

#ifdef _new_programinfo
		if( pTmpTrackInfo->uOutSideTrackID == uOutSideTrackID )
#else
		if( pTmpTrackInfo->uTrackID == uOutSideTrackID )
#endif
		{

			pTmpTrackInfo->uSelInfo |= VO_SOURCE2_SELECT_SELECTED;

			*ppTrackInfo = pTmpTrackInfo;
			ret = VO_RET_SOURCE2_OK;
		}

	}

	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{
		T *pTmpTrackInfo = pInfo->ppTrackInfo[i];
#ifdef _new_programinfo
		if( pTmpTrackInfo->uOutSideTrackID != uOutSideTrackID 
#else
		if( pTmpTrackInfo->uTrackID != uOutSideTrackID 
#endif
			&& pTmpTrackInfo->uTrackType == (*ppTrackInfo)->uTrackType)
		{
			pTmpTrackInfo->uSelInfo &= ~VO_SOURCE2_SELECT_SELECTED;

			pTmpTrackInfo->uSelInfo |= VO_SOURCE2_SELECT_SELECTABLE;
		}
	}
	return VO_RET_SOURCE2_FAIL;
}

template< class S, class T >
VO_U32 SelectTrackBy2ID_T( S * pInfo, VO_U32 uASTrackID, VO_U32 uFPTrackID, T **ppTrackInfo )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
#ifndef _new_programinfo
	return VO_RET_SOURCE2_FAIL;
#endif

	if( !pInfo )
		return ret;

	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{
		T *pTmpTrackInfo = pInfo->ppTrackInfo[i];

		if( pTmpTrackInfo->uASTrackID == uASTrackID && pTmpTrackInfo->uFPTrackID == uFPTrackID )
		{

			pTmpTrackInfo->uSelInfo |= VO_SOURCE2_SELECT_SELECTED;

			*ppTrackInfo = pTmpTrackInfo;
			ret = VO_RET_SOURCE2_OK;
		}
	}

	return ret;
}
template< class T >
VO_U32 ResetTrackSelInfo_T( T *pInfo )
{
	if( !pInfo )
		return VO_RET_SOURCE2_FAIL;

	pInfo->uSelInfo &= ~VO_SOURCE2_SELECT_SELECTED;

	return VO_RET_SOURCE2_OK;
}
template< class S >
VO_U32 ResetStreamSelInfo_T( S *pInfo )
{
	if( !pInfo )
		return VO_RET_SOURCE2_FAIL;

	pInfo->uSelInfo &= ~VO_SOURCE2_SELECT_SELECTED;
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
		ResetTrackSelInfo_T( pInfo->ppTrackInfo[i] );

	return VO_RET_SOURCE2_OK;
}
template< class P >
VO_U32 ResetProgramSelInfo_T( P *pInfo )
{
	if( !pInfo )
		return VO_RET_SOURCE2_FAIL;

	pInfo->uSelInfo &= ~VO_SOURCE2_SELECT_SELECTED;
	for( VO_U32 i = 0; i < pInfo->uStreamCount; i++ )
		ResetStreamSelInfo_T( pInfo->ppStreamInfo[i] );

	return VO_RET_SOURCE2_OK;
}



template< class S, class T >
VO_U32 GetSelectedTrackOP_T( S * pInfo, T **ppTrackInfo, VO_U32 *pCount )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	if( !pInfo )
		return ret;
	T **ppTmpTrackInfo = ppTrackInfo;
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{

		T *pTrackInfo = pInfo->ppTrackInfo[i];

		if( pTrackInfo->uSelInfo & VO_SOURCE2_SELECT_SELECTED )
		{
			*ppTmpTrackInfo = pTrackInfo;
			ppTmpTrackInfo ++;
			*pCount ++;
			ret = VO_RET_SOURCE2_OK;
		}
	}
	return ret;
}

template< class S, class T >
VO_U32 GetSelectedTrackOP_T( S * pInfo, VO_SOURCE2_TRACK_TYPE uType,  T **ppTrackInfo )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	if( !pInfo )
		return ret;
	
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{

		T *pTrackInfo = pInfo->ppTrackInfo[i];

		if( (pTrackInfo->uSelInfo & VO_SOURCE2_SELECT_SELECTED) 
			&& (pTrackInfo->uTrackType == uType) )
		{
			*ppTrackInfo = pTrackInfo;
			return VO_RET_SOURCE2_OK;
		}
	}
	return ret;
}


template< class P, class S >
VO_U32 GetSelectedStreamOP_T( P * pInfo, S **ppStreamInfo )
{
	if( !pInfo )
		return VO_RET_SOURCE2_FAIL;

	for( VO_U32 i = 0; i < pInfo->uStreamCount; i++ )
	{
		S *pStreamInfo = pInfo->ppStreamInfo[i];
		if( pStreamInfo->uSelInfo & VO_SOURCE2_SELECT_SELECTED )
		{
			*ppStreamInfo = pStreamInfo;
			return VO_RET_SOURCE2_OK;
		}
	}
	return VO_RET_SOURCE2_FAIL;
}

template< class P, class T >
VO_VOID InsertTrackInfoOP_T( P *pProgInfo, VO_U32 uStreamID, T *pInfo )
{
	if( !pProgInfo )
		return;

	for( VO_U32 i = 0; i < pProgInfo->uStreamCount; i++ )
	{
		if( pProgInfo->ppStreamInfo[i]->uStreamID == uStreamID )
		{
			T ** ppTmpTrackInfo = pProgInfo->ppStreamInfo[i]->ppTrackInfo;
			VO_U32 nOldCount = pProgInfo->ppStreamInfo[i]->uTrackCount;

			VO_U32 nNewCount = pProgInfo->ppStreamInfo[i]->uTrackCount = nOldCount + 1;
			pProgInfo->ppStreamInfo[i]->ppTrackInfo = new T *[ nNewCount ];
			memset( pProgInfo->ppStreamInfo[i]->ppTrackInfo, 0x00, sizeof( T * ) * nNewCount );
			for( VO_U32 n = 0; n < nOldCount; n++) 
			{
				CopyTrackInfoOP_T( ppTmpTrackInfo[n], &( pProgInfo->ppStreamInfo[i]->ppTrackInfo[n] ) );
				ReleaseTrackInfoOP_T( ppTmpTrackInfo[n] );
				ppTmpTrackInfo[n] = NULL;
			}
			delete []ppTmpTrackInfo;
			CopyTrackInfoOP_T( pInfo, &( pProgInfo->ppStreamInfo[i]->ppTrackInfo[nOldCount] ));
		}
	}
}

template< class P, class S >
VO_VOID InsertStreamInfoOP_T( P *pProgInfo, S *pInfo )
{
	if( !pProgInfo )
		return;

	S ** ppTmpStreamInfo = pProgInfo->ppStreamInfo;
	VO_U32 nOldCount = pProgInfo->uStreamCount;

	pProgInfo->uStreamCount = nOldCount + 1;
	pProgInfo->ppStreamInfo = new S *[ pProgInfo->uStreamCount ];
	memset(pProgInfo->ppStreamInfo, 0x00, sizeof( S * ) * pProgInfo->uStreamCount );

	for( VO_U32 n = 0; n < nOldCount; n++) 
	{
		CopyStreamInfoOP_T( ppTmpStreamInfo[n], &pProgInfo->ppStreamInfo[n] );
		ReleaseStreamInfoOP_T( ppTmpStreamInfo[n] );
		ppTmpStreamInfo[n] = NULL;
	}
	delete []ppTmpStreamInfo;
	CopyStreamInfoOP_T( pInfo, &pProgInfo->ppStreamInfo[pProgInfo->uStreamCount - 1] );
}

template< class S, class T >
VO_U32 RemoveTrackFromStreamOP_T( S *pInfo, T *pTrackInfo )
{
	if( !pInfo || !pTrackInfo )
		return VO_RET_SOURCE2_FAIL;
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{
		if( pTrackInfo == pInfo->ppTrackInfo[i] )
		{
			ReleaseTrackInfoOP_T( pTrackInfo );
			pTrackInfo = NULL;
			pInfo->ppTrackInfo[i] = pInfo->ppTrackInfo[ pInfo->uTrackCount - 1 ];
			pInfo->ppTrackInfo[ pInfo->uTrackCount - 1 ] = NULL;
			pInfo->uTrackCount --;
			return VO_RET_SOURCE2_OK;
		}
	}
	return VO_RET_SOURCE2_FAIL;
}

template< class S >
VO_U32 RemoveTrackFromStreamOP_T( S *pInfo )
{
	if( !pInfo )
		return VO_RET_SOURCE2_FAIL;
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{
		ReleaseTrackInfoOP_T( pInfo->ppTrackInfo[i] );
		pInfo->ppTrackInfo[i] = NULL;
	}
	delete []pInfo->ppTrackInfo;
	pInfo->uTrackCount = 0;
	return VO_RET_SOURCE2_FAIL;
}

template< class P >
VO_U32 RemoveAllTrackFromProgramOP_T( P *pInfo )
{
	if( !pInfo )
		return VO_RET_SOURCE2_FAIL;
	for( VO_U32 i = 0; i < pInfo->uStreamCount; i++ )
	{
		RemoveTrackFromStreamOP_T( pInfo->ppStreamInfo[i] );
	}
	return VO_RET_SOURCE2_FAIL;
}

template< class S >
VO_U32 RemoveTrackFromStreamByASIDOP_T( S *pInfo, VO_U32 uASTrackID )
{
	if( !pInfo )
		return VO_RET_SOURCE2_FAIL;
	VO_U32 count = pInfo->uTrackCount;
	for( VO_U32 i = 0; i < count; )
	{
#ifdef _new_programinfo
		if( pInfo->ppTrackInfo[i] && uASTrackID == ( pInfo->ppTrackInfo[i] )->uASTrackID )
		{
			ReleaseTrackInfoOP_T( pInfo->ppTrackInfo[i] );
			pInfo->ppTrackInfo[i] = NULL;
			pInfo->ppTrackInfo[i] = pInfo->ppTrackInfo[ pInfo->uTrackCount - 1 ];
			pInfo->ppTrackInfo[ pInfo->uTrackCount - 1 ] = NULL;
			pInfo->uTrackCount --;
			i = 0;
		}
		else
			i++;
#else
		continue;
#endif
	}
	
	
	return VO_RET_SOURCE2_FAIL;
}

template< class P >
_STREAM_INFO* GetStreamInfoByIDOP_T( P *pInfo, VO_U32 uSteamID )
{
	if( !pInfo )
		return NULL;
	for( VO_U32 i = 0; i < pInfo->uStreamCount; i++ )
	{
		if( ( pInfo->ppStreamInfo[i] )->uStreamID ==uSteamID )
			return pInfo->ppStreamInfo[i] ;
	}
	return NULL;
}

template< class S >
_TRACK_INFO* GetTrackInfoByASIDOP_T( S *pInfo, VO_U32 uASTrackID )
{
	if( !pInfo )
		return NULL;
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{
		if(  (pInfo->ppTrackInfo[i])->uASTrackID == uASTrackID  )
		{
			return pInfo->ppTrackInfo[i];
		}
	}
	return NULL;
}

template< class S, class T >
VO_VOID GetTrackInfoByASIDOP2_T( S *pStreamInfo, VO_U32 uASTrackID, T **ppTrackInfo )
{
	if( !pStreamInfo )
		return ;
	int x = 0;
	for( VO_U32 i = 0; i < pStreamInfo->uTrackCount; i++ )
	{
		if(  (pStreamInfo->ppTrackInfo[i])->uASTrackID == uASTrackID  )
		{
			ppTrackInfo[x++] = pStreamInfo->ppTrackInfo[i];
		}
	}
	return ;
}

template< class S >
_TRACK_INFO* GetTrackInfoByASIDandTypeOP_T( S *pInfo, VO_U32 uASTrackID, VO_SOURCE2_TRACK_TYPE nType )
{
	if( !pInfo )
		return NULL;
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{
#ifdef _new_programinfo
		if(  (pInfo->ppTrackInfo[i])->uASTrackID == uASTrackID && (pInfo->ppTrackInfo[i])->uTrackType == nType )
#else
		continue;
#endif
		{
			return pInfo->ppTrackInfo[i];
		}
	}
	return NULL;
}


template< class S >
_TRACK_INFO* GetTrackInfoByIDOP_T( S *pInfo, VO_U32 uTrackID, VO_SOURCE2_TRACK_TYPE nType )
{
	if( !pInfo )
		return NULL;
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{

		if(  (pInfo->ppTrackInfo[i])->uTrackID == uTrackID && (pInfo->ppTrackInfo[i])->uTrackType == nType )
		{
			return pInfo->ppTrackInfo[i];
		}
	}
	return NULL;
}

template< class S >
_TRACK_INFO* GetTrackInfoByIDOP_T( S *pInfo, VO_U32 uTrackID )
{
	if( !pInfo )
		return NULL;
	for( VO_U32 i = 0; i < pInfo->uTrackCount; i++ )
	{
#ifdef _new_programinfo
		if(  (pInfo->ppTrackInfo[i])->uOutSideTrackID == uTrackID  )
#else
		if(  (pInfo->ppTrackInfo[i])->uTrackID == uTrackID  )
#endif
		{
			return pInfo->ppTrackInfo[i];
		}
	}
	return NULL;
}
template< class P,class T >
VO_VOID GetTrackInfoByIDOP_T( P *pProgram, VO_U32 uTrackID, T **pTrackInfo )
{
	if( !pProgram )
		return ;
	for( VO_U32 i = 0; i < pProgram->uStreamCount; i++ )
	{
		for( VO_U32 n = 0; n < pProgram->ppStreamInfo[i]->uTrackCount; n++ )
		{
			*pTrackInfo = GetTrackInfoByIDOP_T(pProgram->ppStreamInfo[i], uTrackID);
			if(*pTrackInfo)
				return;
		}
	}
	return ;
}

#ifdef _VONAMESPACE
}
#endif
#endif  //__FAUDIOHEADDATAINFO_H__








