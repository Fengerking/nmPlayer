/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef __VO_OSMP_DRM_H__
#define __VO_OSMP_DRM_H__

enum VO_OSMP_DRM_ERRORTYPE
{
	VO_OSMP_DRM_ERR_NONE       = 0X00000000,
	VO_OSMP_DRM_ERR_BASE       = 0x86600000,

	/*!< specified DRM module not found */
	VO_OSMP_DRM_ERR_UNSUPPORT  = (VO_OSMP_DRM_ERR_BASE | 0x0001),

	/*!< parameter is not valid */
	VO_OSMP_DRM_ERR_PARAMETER  = (VO_OSMP_DRM_ERR_BASE | 0x0002),

	/*!< error state */
	VO_OSMP_DRM_ERR_STATUS     = (VO_OSMP_DRM_ERR_BASE | 0x0003),

	/*!< unable to parse the DRM information in file successfully */
	VO_OSMP_DRM_ERR_DRMINFO    = (VO_OSMP_DRM_ERR_BASE | 0x0004),

	/*!< unable to decrypt the data successfully */
	VO_OSMP_DRM_ERR_DRMDATA    = (VO_OSMP_DRM_ERR_BASE | 0x0005),

	/*!< operation cancelled by user */
	VO_OSMP_DRM_ERR_USERCANCEL = (VO_OSMP_DRM_ERR_BASE | 0x0006),

	/*!< resolution is not support */
	VO_OSMP_DRM_ERR_RESOLUTION = (VO_OSMP_DRM_ERR_BASE | 0x0007),

	/*!< the data is not encryted */
	VO_OSMP_DRM_ERR_UNENCRYPT  = (VO_OSMP_DRM_ERR_BASE | 0x0008),

	/*!< unable to find DRM interfaces */
	VO_OSMP_DRM_ERR_NOAPI      = (VO_OSMP_DRM_ERR_BASE | 0x0009),

	/*!< retry the operation */
	VO_OSMP_DRM_ERR_RETRY      = (VO_OSMP_DRM_ERR_BASE | 0x0010),

	VO_OSMP_DRM_ERR_OUTPUT_BUFFER_SMALL = (VO_OSMP_DRM_ERR_BASE | 0x0011),
	VO_OSMP_DRM_ERR_NOT_IMPLEMENTED     = (VO_OSMP_DRM_ERR_BASE | 0x0012),
	VO_OSMP_DRM_ERR_NOT_MODIFIED        = (VO_OSMP_DRM_ERR_BASE | 0x0013),

	/*!< Undefined error */
	VO_OSMP_DRM_ERR_MAX              = (VO_OSMP_DRM_ERR_BASE | 0xFFFF)
};

/**
* Enumeration for source container
*/
enum VO_OSMP_DRM_SOURCE_TYPE {
	VO_OSMP_DRM_SOURCE_UNKNOWN,

	VO_OSMP_DRM_SOURCE_FILE      = 0x80,
	VO_OSMP_DRM_SOURCE_ENVELOPE  = VO_OSMP_DRM_SOURCE_FILE | 0x1,

	VO_OSMP_DRM_SOURCE_AS        = 0x800,
	VO_OSMP_DRM_SOURCE_HLS       = VO_OSMP_DRM_SOURCE_AS | 0x1,
	VO_OSMP_DRM_SOURCE_SSTR      = VO_OSMP_DRM_SOURCE_AS | 0x2,
	VO_OSMP_DRM_SOURCE_DASH      = VO_OSMP_DRM_SOURCE_AS | 0x3,

	VO_OSMP_DRM_SOURCE_FR        = 0x8000,
	VO_OSMP_DRM_SOURCE_PIFF      = VO_OSMP_DRM_SOURCE_FR | 0x1,
	VO_OSMP_DRM_SOURCE_ASF       = VO_OSMP_DRM_SOURCE_FR | 0x2,

	VO_OSMP_DRM_SOURCE_MAX       = 0x7FFFFFFF
};


/**
* Structure of assist DRM parameter for HLS
*/        
struct VO_OSMP_DRM_HLS_INFO
{
	char  szCurURL[2048];    // Refer URL
	char  szKeyString[2048]; // #EXT-X-KEY string
	int   iSequenceNum;      // num of chunk sequence
	void* pReserved;
};

/**
* Structure of assist DRM parameter for PIFF
*/        
struct VO_OSMP_DRM_PIFF_INFO
{
	// sample encryption box
	unsigned char* pSampleEncryptionBox;        

	// the size of Sample encryption box
	int            iSampleEncryptionBox; 

	// index of this sample data within this movie fragment.
	int            iSampleIndex;

	// Track encryption box
	unsigned char* pTrackEncryptionBox;

	// the size of Track encryption box
	int            iTrackEncryptionBox; 

	// the unique identifier of track
	int            iTrackID;
};

class VOOSMPDRM {
public: 
	VOOSMPDRM(void) {}
	virtual ~VOOSMPDRM(void) {}

	/**
	* initialize underlying DRM engine
	*
	* @return VO_OSMP_DRM_ERR_NONE if successful.
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, the playback must be aborted.
	*/
	virtual int init() = 0;

	/**
	* destroy underlying DRM engine. Release all resources allocated by
	* the underlying DRM engine.
	*
	* @return VO_OSMP_DRM_ERR_NONE if successful
	*/
	virtual int destroy() = 0;

	/**
	* calling the underlying DRM engine to process input URI 
	*
	* @param[in]      eType     type data source (VO_OSMP_DRM_SOURCE_TYPE)
	* @param[in]      szSrcURI  original URI
	* @param[out]     pDesURI   output buffer to hold the modified URI
	* @param[in/out]  piDesURI  as an input argument, it is the size of 
	*                           output buffer pDesURI; as an output 
	*                           argument, it returns the acutal length of 
	*                           modified URI
	*
	* @return VO_OSMP_DRM_ERR_NONE if input URI is processed and the 
	*         modified URI is set to pDesURI successfully
	*
	* @return VO_OSMP_DRM_ERR_NOT_MODIFIED if input URI does not need
	*         to be processed. Output buffer pDesURI is not changed and
	*         piDesURI is set to 0.
	*
	* @return VO_OSMP_DRM_ERR_OUTPUT_BUFFER_SMALL if output buffer pDesURI 
	*         is too small to hold the modified URI. The required buffer
	*         size is returned via piDesURI argument. The caller needs to
	*         prepared argument pDesURI witn an appropriate size and then
	*         calls again.
	*
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*/
	virtual int processURI(VO_OSMP_DRM_SOURCE_TYPE eType, 
		const char* szSrcURI, 
		char* pDesURI, int* piDesURI) = 0;

	/**
	* Process playlist for adaptive streaming soruces.
	*
	* @param[in]      eType          type of data source
	* @param[in_out]  szSrcPlaylist  original playlist, including main 
	*                                and sub playlist
	* @param[in]      iSrcPlaylist   size of original playlist
	* @param[out]     pDesPlaylist   output buffer to hold processed 
	*                                playlist 
	* @param[in/out]  piDesPlaylist  as an input argument, it is the size 
	*                                of output buffer pDesURI; as an output
	*                                argument, it returns the acutal length
	*                                of processed playlist
	*
	* @return VO_OSMP_DRM_ERR_NONE if input playlist is processed and the 
	*         processed playlist is set to pDesPlaylist successfully
	*
	* @return VO_OSMP_DRM_ERR_NOT_MODIFIED if input playlist does not 
	*         need to be processed. Output buffer pDesPlaylist is not 
	*         changed and piDesURI is set to 0.
	*
	* @return VO_OSMP_DRM_ERR_OUTPUT_BUFFER_SMALL if output buffer 
	*         pDesPlaylist is too small to hold the modified playlist. 
	*         The required buffer size is returned via piDesPlaylist 
	*         argument. The caller needs to prepared argument pDesPlaylist
	*         witn an appropriate size and then calls again.
	* 
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*/
	virtual int processPlaylist(VO_OSMP_DRM_SOURCE_TYPE eType, 
		const char* szSrcPlaylist, int iSrcPlaylist,
		char* pDesPlaylist, int* piDesPlaylist) = 0;

	/**
	* process DRM header such as 
	*   - main playlist of an adaptive streaming source
	*   - protection system specific header box (PIFF) 
	*   - content encryption object (PYV) 
	*   - extended contentEncryption object (WMDRM)
	*
	* This function is only needed to be called for specific DRM headers
	*
	* @param[in]  eType        type data source (VO_OSMP_DRM_SOURCE_TYPE)
	* @param[in]  pHeader      DRM header
	* @param[in]  iHeaderSize  size of DRM header 
	*
	* @return VO_OSMP_DRM_ERR_NONE if successful.  
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, the playback must be aborted.
	*/
	virtual int processHeader(VO_OSMP_DRM_SOURCE_TYPE eType,
		const unsigned char* pHeader,
		int iHeaderSize) = 0;

	/**
	* return the location of DRM header. This function is only needed to be
	* called when the whole file is encrypted (e.g.envelop).
	*
	* @param[out]  piPosition  starting position of DRM header
	* @param[out]  piSize      size of DRM header
	*
	* @return VO_OSMP_DRM_ERR_NONE for success
	*
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	* 
	* All other returned values are fatal failures. If content is 
	* encrypted, the playback must be aborted.
	*/
	virtual int getHeaderLocation(long long* piPosition, int* piSize) = 0;

	/**
	* return the location of actual data to be processed by file parsers 
	*
	* @param[in_out]  piPosition  as an input argument, it is the location
	*                             of the data which is about to be 
	*                             processed; as an output arguemnt, it 
	*                             returns the actual location of the data
	*                             to be processed.
	* @param[in_out]  piSize      as an input argument, it is the size of 
	*                             an the data which is about to be 
	*                             processed; as an output argument,
	*                             it is the size of the actual data to be
	*                             processed.
	*
	* @return VO_OSMP_DRM_ERR_NONE if successful.  
	*
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, the playback must be aborted.
	*/
	virtual int getDataLocation(long long* piPosition, int* piSize) = 0;

	/**
	* return the size of actual data in a file
	*
	* @param[in_out]  piSize  as an input argument, it is the size of the 
	*                         encrypted file; as an output argument, it is
	*                         the size of the actual file 
	*
	* @return VO_OSMP_DRM_ERR_NONE if successful.  

	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, the playback must be aborted.
	*/
	virtual int getDataFileSize(long long *piSize) = 0;

	/**
	* indicate to start descrypting a group of data
	*
	* @param[in]  identifer        unique identifier of a group of data 
	*                              (e.g. track ID or file ID)
	* @param[in]  pAdditionalInfo  auxiliary information corresponding 
	*                              to different source type (e.g. 
	*                              VO_OSMP_DRM_PIFF_INFO for PIFF or 
	*                              VO_OSMP_DRM_HLS_INFO for HLS, etc.)
	*
	* @return VO_OSMP_DRM_ERR_NONE if successful. 
	*
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, the playback must be aborted.
	*/
	virtual int beginSegment(int identifer, void* pAdditionalInfo) = 0;

	/**
	* decrypt one block from a group of data. This function is used to 
	* decrypt a block where the descrypted data is written back to 
	* the same buffer
	*
	* @param[in]      identifer        unique identifier of a group of data 
	*                                  (e.g. track ID or file ID)
	* @param[in]      llPosition       the offset of this block of data 
	*                                  from beginning of this group of 
	*                                  data
	* @param[in_out]  pData            as an input argument, it points to
	*                                  a block of encrypted data; as an 
	*                                  output argument, it points to the 
	*                                  descrypted data
	* @param[in_out]  piSize           as an input argument, it is the size 
	*                                  of encrypted data; as an output 
	*                                  argument, it is the size of 
	*                                  descrypted data
	* @param[in]      bLastBlock       true if this is the last block of 
	*                                  data in this group
	* @param[in]      pAdditionalInfo  auxiliary information corresponding 
	*                                  to different source type (e.g. 
	*                                  VO_OSMP_DRM_PIFF_INFO for PIFF or 
	*                                  VO_OSMP_DRM_HLS_INFO for HLS, etc.)
	*
	* @return VO_OSMP_DRM_ERR_NONE if data has been decrypted and result
	*         has been written back to pData successfully. piSize is set 
	*         to the size of decrypted data. 
	*
	* @return VO_OSMP_DRM_ERR_OUTPUT_BUF_SMALL if the size of pData is 
	*         not sufficient to hold the descrypted data. In this case,
	*         piSize is set to the required buffer size for holding the 
	*         decrypted data. 
	* 
	*         The caller need to prepare a buffer to store the descrypted 
	*         data with the size returned via piSize and then descrypt 
	*         the block again by calling the version of function that 
	*         accepts separate input and output buffers. 
	*
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, this group of data must be skipped 
	*/
	virtual int decryptSegment(int identifer, long long llPosition, 
		unsigned char* pData, int* piSize, 
		bool bLastBlock, void* pAdditionalInfo) = 0;

	/**
	* decrypt one block from a group of data. This function is used to 
	* decrypt a block where the descrypted data is written to a designated 
	* output buffer
	*
	* @param[in]      identifer        unique identifier of a group of data 
	*                                  (e.g. track ID or file ID)
	* @param[in]      llPosition       the offset of this block from 
	*                                  beginning of this group of data
	* @param[in_out]  pSrcData         block of data to be decrypted
	* @param[in]      iSrcSize         size of pSrcData
	* @param[in]      bLastBlock       true if this is the last block of 
	*                                  data in this group
	* @param[out]     pDesData         output buffer to hold the descrypted
	*                                  data
	* @param[in_out]  piDesSize        as an input argument, it is the 
	*                                  capacity of pDesData; as an output
	*                                  argument, it is the size of 
	*                                  decrypted data stored in pDesData
	* @param[in]      pAdditionalInfo  auxiliary information corresponding 
	*                                  to different source type (e.g. 
	*                                  VO_OSMP_DRM_PIFF_INFO for PIFF or 
	*                                  VO_OSMP_DRM_HLS_INFO for HLS, etc.)
	*
	* @return VO_OSMP_DRM_ERR_NONE if data has been decrypted and result
	*         has been written to pDesData successfully. piDesSize is set
	*         to the size of decrypted data. 
	*
	* @return VO_OSMP_DRM_ERR_OUTPUT_BUF_SMALL if the size of pDesData is 
	*         not sufficient to hold the descrypted data. In this case, 
	*         piDesSize is set to the required buffer size to hold the 
	*         decrypted data. 
	* 
	*         The caller need to prepare a buffer to store the descrypted 
	*         data with the size returned via piDesData and then descrypt 
	*         the block again by calling this function again
	*
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, this group of data must be skipped 
	*/
	virtual int decryptSegment(int identifer, long long llPosition, 
		unsigned char* pSrcData, int iSrcSize, 
		bool bLastBlock, unsigned char* pDesData, 
		int* piDesSize, void* pAdditionalInfo) = 0;

	/**
	* indicate to stop descrypting a group of data
	*
	* @param[in]  identifer  unique identifier of a group of data (e.g. 
	*                        track ID or file ID)
	*
	* @return VO_OSMP_DRM_ERR_NONE if successful.
	*
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, the playback must be aborted.
	*/
	virtual int endSegment(int identifer) = 0;

	/**
	* decrypt media sample.  This function is used to descrypted samples 
	* where a decrypted sample is written back to the same buffer.
	*
	* @param[in]      iPosition        offset from start point
	* @param[in/out]  pData            as an input argument, it is the 
	*                                  encrypted sample buffer; as an 
	*                                  output argument, it is the decrypted 
	*                                  sample buffer.
	* @param[in/out]  piSize           as an input argument, it is the 
	*                                  size of the encrypted buffer;
	*                                  as an output argument, it is the 
	*                                  size of decrypted sample
	* @param[in]      pAdditionalInfo  auxiliary information corresponding 
	*                                  to different source type (e.g. 
	*                                  VO_OSMP_DRM_PIFF_INFO for PIFF or 
	*                                  VO_OSMP_DRM_HLS_INFO for HLS, etc.)
	*
	* @return VO_OSMP_DRM_ERR_NONE if sample has been decrypted and result
	*         has been written back to pData successfully. piSize is set
	*         to the size of decrypted sample. 
	*
	* @return VO_OSMP_DRM_ERR_OUTPUT_BUF_SMALL if the size of pData is not
	*         sufficient to hold the descrypted sample. In this case, 
	*         piSize is set to the required buffer size to hold the 
	*         decrypted sample. 
	* 
	*         The caller need to prepare buffer to store the descrypted 
	*         sample with the size returned via piSize and then descrypt 
	*         the sample again by calling the version of function that 
	*         accepts separate input and output sample buffers. 
	*
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, this sample must be skipped
	*/
	virtual int decryptSample(int iPosition, unsigned char* pData, 
		int* piSize, void* pAdditionalInfo) = 0;

	/**
	* decrypt media sample.  This function is used to descrypted samples 
	* where a decrypted sample is written to a designated output buffer
	*
	* @param[in]      iPosition        offset from start point
	* @param[in_out]  pSrcData         buffer containing encrypted sample 
	* @param[in]      iSrcSize         size of pSrcData
	* @param[out]     pDesData         output buffer to hold the descrypted
	*                                  sample
	* @param[in_out]  piDesSize        as an input argument, it is the 
	*                                  capacity of pDesData; as an output
	*                                  argument, it is the size of 
	*                                  decrypted sample stored in pDesData
	* @param[in]      pAdditionalInfo  auxiliary information corresponding 
	*                                  to different source type (e.g. 
	*                                  VO_OSMP_DRM_PIFF_INFO for PIFF or 
	*                                  VO_OSMP_DRM_HLS_INFO for HLS, etc.)
	*
	* @return VO_OSMP_DRM_ERR_NONE if sample has been decrypted and result
	*         has been written to pDesData successfully. piDesSize is set
	*         to the size of decrypted sample. 
	*
	* @return VO_OSMP_DRM_ERR_OUTPUT_BUF_SMALL if the size of pDesData is
	*         not sufficient to hold the descrypted sample. In this case, 
	*         piDesSize is set to the required buffer size to hold the 
	*         decrypted sample. 
	* 
	*         The caller need to prepare a buffer to store the descrypted 
	*         sample with the size returned via piDesSize and then descrypt 
	*         the sample again by calling this function again
	*
	* @return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED if this function is not
	*         applicable to the underlying DRM engine
	*
	* All other returned values are fatal failures. If content is 
	* encrypted, this sample must be skipped
	*/
	virtual int decryptSample(int iPosition, unsigned char* pSrcData, 
		int iSrcSize, unsigned char* pDesData, 
		int* piDesSize, void* pAdditionalInfo) = 0;
};

#endif //__VO_OSMP_DRM_H__