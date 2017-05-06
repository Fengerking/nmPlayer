	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voDownloader.h

	Contains:	data type define header file

	Written by:	Yi Yang

	Change History (most recent first):
	2012-03-16		Yi Yang			Create file

*******************************************************************************/

#ifndef __VODOWNLOADER_H__

#define __VODOWNLOADER_H__

#define VO_DOWNLOAD_OPENFLAG_URL 0x1

struct VODOWNLOAD_FUNC
{
	//Open one url for download, if this is HTTP protocol please ensure that you analyzed the HTTP response before return
	//param pHandle [out] download handle
	//param pSource [in] download descriptor( it is maybe url, some handle ), the type of this descriptor will be indicate by flag
	//param flag [in] indicate the download descriptor type, for url it should be VO_DOWNLOAD_OPENFLAG_URL
	int			( * Open ) ( void ** pHandle , void * pSource , int flag );

	//Stop download
	//param Handle [in] download handle
	int			( * Close ) ( void* Handle );

	//Read data from the downloader
	//ret long long, indicate the size that get from the read operation, the size must be equals to the request size
	//if the ret size is smaller than request size or the ret size is -1, it indicates we read to the end of the download file
	//if the request size is bigger than buffer size inside and it is still downloading, please return -2 to indicate that we need retry read again
	//param Handle [in] download handle
	//param pBuffer [in] the buffer pointor for get the data
	//param llSize [in] read size
	long long	( * Read ) ( void* Handle , unsigned char * pBuffer , long long llSize );

	//Get the Content size, if right now you do not have the content size value please return -2
	//param Handle [in] download handle
	long long	( * GetContentSize ) ( void* Handle );

	//Get the time cost for the download, this should be called after Read is done( ret is -1 or smaller than the request size ) otherwise return -1
	//param Handle [in] download handle
	//param pTime [out] time cost for the download
	int			( * GetDownloadTime )( void* Handle , unsigned int * pTime );
};

#endif