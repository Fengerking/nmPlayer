/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef __voStageFright_H__
#define __voStageFright_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/* define the error ID */
#define VOSF_ERR_NONE								0x00000000
#define VOSF_ERR_EOS								0x00000001
#define VOSF_ERR_FORMAT_CHANGED						0x00000002
#define VOSF_ERR_NEED_MOREBUFF						0x00000003
#define VOSF_ERR_FAILED								0x80000001
#define VOSF_ERR_OUTOF_MEMORY						0x80000002
#define VOSF_ERR_NOT_IMPLEMENT						0x80000003
#define VOSF_ERR_INVALID_ARG						0x80000004
#define VOSF_ERR_WRONG_STATUS						0x80000007
#define VOSF_ERR_WRONG_PARAM_ID						0x80000008

#define OMX_VO_COLOR_FormatYUV420StridePlanar		0x7F100001

#define VOSF_DATA_URL								0x01
#define VOSF_DATA_SOURCE							0x02

#define VOSF_PID_NotifyFunc				0x0001	/*!<pValue1 VOSFNOTIFY *, pValue2 pUserData */
#define VOSF_PID_MemVideoOperater		0x0002  /*!<pValue1 VO_MEM_VIDEO_OPERATOR *  pValue2 NULL */
#define VOSF_CID_NotifyEvent			0x0001	/*!<pParam1 vosfEventType *  */


/**
	 * Video data buffer, usually used as iutput or output of video codec.
 */
	typedef struct
	{
		int 			nMsg;				/*!< msg ID */
		int				nExt1;				/*!< first param */
		int				nExt2;				/*!< secont param */
	} vosfEventType;

/**
	 * Video data buffer, usually used as iutput or output of video codec.
 */
	typedef struct
	{
		char * 			Buffer[3];			/*!< Buffer pointer */
		int 			Stride[3];			/*!< Buffer stride */
		int				ColorType;			/*!< Color Type */
		long long		Time;				/*!< The time of the buffer */
	} vosfVideoBuffer;

	typedef struct VOSF_DATASOURCETYPE
	{
		int		nSize;
		int		nVersion;
		void *	pDataSource;
		int		nFlag;
		int		nReserve;

	// the data source function set
		int		(* initCheck) (void * pDataSource);
		int		(* readAt) (void * pDataSource, long long llOffset, void * pData, int nSize);
		bool	(* getUInt16) (void * pDataSource, long long llOffset,  unsigned short * pValue);
		int		(* getSize) (void * pDataSource, long long * pSize);
		int		(* flags) (void * pDataSource);
	} VOSF_DATASOURCETYPE;

	typedef struct VOSF_MEDIAEXTRACTORTYPE
	{
		int		nSize;
		int		nVersion;
		void *	pMediaExtractor;
		int		nFlag;
		int		nReserve;

	// the media extractor function set
		int		(* countTracks) (void * pExtractor);
		void *	(* getTrack) (void * pExtractor, int nIndex);
		void *	(* getTrackMetaData) (void * pExtractor, int nIndex, int nFlag);
		void *	(* getMetaData) (void * pExtractor);
		int		(* flags) (void * pExtractor);

	} VOSF_MEDIAEXTRACTORTYPE;

	typedef enum
	{
		PlayMode =0,
		MedataMode ,
	} VOSF_MODE;
	
	typedef struct VOSF_MEDIASOURCETYPE
	{
		int		nSize;
		int		nVersion;
		void *	pMediaSource;
		int		nFlag;
		int		nReserve;

	// the media source function set
		int		(* start) (void * pMediaSource, void * pParams);
		int		(* stop) (void * pMediaSource);
		void *	(* getFormat) (void * pMediaSource);
		int		(* read) (void * pMediaSource, void ** ppBuffer, void * pOptions);
	} VOSF_MEDIASOURCETYPE;

	typedef struct VOSF_READOPTIONTYPE
	{
		int		nSize;
		int		nVersion;
		void *	pOptions;

	// the media source function set
		void 		(* reset) (void * pOptions);
		void 		(* setSeekTo) (void * pOptions, long long llPos  , int mode);
		void 		(* clearSeekTo) (void * pOptions);
		bool 		(* getSeekTo) (void * pOptions, long long * llPos , int *mode);
		void		(* clearSkipFrame)(void * pOptions);
		bool		(* getSkipFrame)(void * pOptions , long long *timeUs) ;
		void		(* setSkipFrame)(void * pOptions , long long timeUs);
		void 		(* setLateBy) (void * pOptions, long long llPos);
		long long	(* getLateBy) (void * pOptions);
	} VOSF_READOPTIONTYPE;

	typedef struct VOSF_MEDIABUFFERTYPE
	{
		int		nSize;
		int		nVersion;

	// the media source function set
		void *	(* create) (void * pData, int nSize);
		void *	(* createBySize) (int nSize);
		void	(* release) (void * pMediaBuffer);
		void	(* add_ref) (void * pMediaBuffer);
		void *	(* data) (void * pMediaBuffer);
		int		(* size) (void * pMediaBuffer);
		int		(* range_offset) (void * pMediaBuffer);
		int		(* range_length) (void * pMediaBuffer);
		void	(* set_range) (void * pMediaBuffer, int nOffset, int nLength);
		void *	(* meta_data) (void * pMediaBuffer);
		void	(* reset) (void * pMediaBuffer);
		void	(* setObserver) (void * pMediaBuffer, void * pObserver);
		void *	(* clone) (void * pMediaBuffer);
		int		(* refcount) (void * pMediaBuffer);
	} VOSF_MEDIABUFFERTYPE;

	typedef struct VOSF_METADATATYPE
	{
		int		nSize;
		int		nVersion;

	// the media source function set
		void *	(* create) (void);
		void *	(* createCopy) (void * pMetaData);
		void	(* clear) (void * pMetaData);
		bool	(* remove) (void * pMetaData, int nKey);
		bool	(* setCString) (void * pMetaData, int nKey, const char * pValue);
		bool	(* setInt32) (void * pMetaData, int nKey, int nValue);
		bool	(* setInt64) (void * pMetaData, int nKey, long long nValue);
		bool	(* setFloat) (void * pMetaData, int nKey, float dValue);
		bool	(* setPointer) (void * pMetaData, int nKey, void * pValue);
		bool	(* findCString) (void * pMetaData, int nKey, const char ** ppValue);
		bool	(* findInt32) (void * pMetaData, int nKey, int * nValue);
		bool	(* findInt64) (void * pMetaData, int nKey, long long * nValue);
		bool	(* findFloat) (void * pMetaData, int nKey, float * dValue);
		bool	(* findPointer) (void * pMetaData, int nKey, void ** ppValue);
		bool	(* setData) (void * pMetaData, int nKey, int nType, const void * pData, int nSize);
		bool	(* findData) (void * pMetaData, int nKey, int * nType, const void ** pData, int * nSize);
	} VOSF_METADATATYPE;


	typedef int (* VOSFNOFITY) (void * pUserData, int nID, void * pParam1, void * pParam2);

	void *	vosfCreateMediaExtractor (void * pDataSource, int nFlag, VOSF_MEDIABUFFERTYPE * pMediaBufferOP,
									  VOSF_METADATATYPE * pMetaDataOP);

	int		vosfGetTrackNum (void * pExtractor);

	void *	vosfGetTrackSource (void * pExtractor, int nTrack);

	void *	vosfGetTrackMetaData (void * pExtractor, int nTrack, int nFlag);

	void *	vosfGetMetaData (void * pExtractor);

	int		vosfGetFlag (void * pExtractor);

	int		vosfGetExtractorParam (void * pExtractor, int nParamID, void * pValue1, void * pValue2);

	int		vosfSetExtractorParam (void * pExtractor, int nParamID, void * pValue1, void * pValue2);

	int		vosfCloseMediaExtractor (void * pExtractor);

	void    vosfSetMode(void * pExtractor, VOSF_MODE inMode);

	void *	vosfCreateMediaSource (void * pMediaSource, int nFlag, VOSF_MEDIABUFFERTYPE * pMediaBufferOP,
								   VOSF_METADATATYPE * pMetaDataOP, void *pMetaData);

	int		vosfStart (void * pSource, void * pParam);

	int		vosfStop (void * pSource);

	void *	vosfGetFormat (void * pSource);

	int		vosfRead (void * pSource, void ** ppBuffer, void * pOptions);

	int		vosfGetMediaSourceParam (void * pSource, int nParamID, void * pValue1, void * pValue2);

	int		vosfSetMediaSourceParam (void * pSource, int nParamID, void * pValue1, void * pValue2);

	int		vosfCloseMediaSource (void * pSource);
	

	typedef void *	(* VOSFCREATEMEDIAEXTRACTOR) (void * pDataSource, int nFlag, VOSF_MEDIABUFFERTYPE * pMediaBufferOP,
					   VOSF_METADATATYPE * pMetaDataOP);
	typedef int		(* VOSFGETTRACKNUM) (void * pExtractor);
	typedef void *	(* VOSFGETTRACKSOURCE) (void * pExtractor, int nTrack);
	typedef void *	(* VOSFGETTRACKMETADATA) (void * pExtractor, int nTrack, int nFlag);
	typedef void *	(* VOSFGETMETADATA) (void * pExtractor);
	typedef int		(* VOSFGETFLAG) (void * pExtractor);
	typedef int		(* VOSFGETEXTRACTORPARAM) (void * pExtractor, int nParamID, void * pValue1, void * pValue2);
	typedef int		(* VOSFSETEXTRACTORPARAM) (void * pExtractor, int nParamID, void * pValue1, void * pValue2);
	typedef int		(* VOSFCLOSEMEDIAEXTRACTOR) (void * pExtractor);
	typedef void    (* VOSFSETMODE)(void* pExtractor, VOSF_MODE inMode);
	
	typedef void *	(* VOSFCREATEMEDIASOURCE) (void * pMediaSource, int nFlag, VOSF_MEDIABUFFERTYPE * pMediaBufferOP,
					   VOSF_METADATATYPE * pMetaDataOP, void *pMetaData);
	typedef int		(* VOSFSTART) (void * pSource, void * pParam);
	typedef int		(* VOSFSTOP) (void * pSource);
	typedef void *	(* VOSFGETFORMAT) (void * pSource);
	typedef int		(* VOSFREAD) (void * pSource, void ** ppBuffer, void * pOptions);
	typedef int		(* VOSFGETMEDIASOURCEPARAM) (void * pExtractor, int nParamID, void * pValue1, void * pValue2);
	typedef int		(* VOSFSETMEDIASOURCEPARAM) (void * pExtractor, int nParamID, void * pValue1, void * pValue2);
	typedef void *	(* VOSFCLOSEMEDIASOURCE) (void * pSource);
	

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__voStageFright_H__
