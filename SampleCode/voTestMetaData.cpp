#include "voMetaData.h"
#include "cmnFile.h"
#include "cmnMemory.h"

typedef VO_S32 (VO_API* VOGETMETADATAAPI)(VO_METADATA_READAPI* pReadHandle, VO_U32 uFlag);

void ProcessString(PMetaDataString pString)
{
	if(pString->dwFlags & VO_METADATA_DYNAMICSTRING)
	{
		//dynamic allocated string
	}

	if(pString->dwFlags & VO_METADATA_TE_ANSI)
	{
		//ANSI
	}
	else if(pString->dwFlags & VO_METADATA_TE_UTF8)
	{
		//UTF-8
	}
	else if(pString->dwFlags & VO_METADATA_TE_UTF16)
	{
		//UTF-16
	}
	else
	{
		//Unknown
	}
}

void ProcessImage(PMetaDataImage pImage)
{
	if(VO_METADATA_IT_JPEG == pImage->nImageType)
	{
		//Jpeg
	}
	else if(VO_METADATA_IT_BMP == pImage->nImageType)
	{
		//Bmp
	}
	else if(VO_METADATA_IT_PNG == pImage->nImageType)
	{
		//Png
	}
	else if(VO_METADATA_IT_GIF == pImage->nImageType)
	{
		//Gif
	}
	else
	{
		//Unknown
	}
}

void OnTest()
{
	HMODULE hDll = LoadLibrary(_T("voMetaData.dll"));
	VOGETMETADATAAPI fGetAPI = (VOGETMETADATAAPI)GetProcAddress (hDll, "voGetMetaDataReadAPI");

	if(fGetAPI)
	{
		VO_METADATA_READAPI fAPIs;
		fGetAPI(&fAPIs, 0);
		VO_PTR hFileParser = NULL;

		//You can set NULL
		VO_FILE_OPERATOR fileOp;
		fileOp.Open = cmnFileOpen;
		fileOp.Read = cmnFileRead;
		fileOp.Write = cmnFileWrite;
		fileOp.Flush = cmnFileFlush;
		fileOp.Seek = cmnFileSeek;
		fileOp.Size = cmnFileSize;
		fileOp.Save = cmnFileSave;
		fileOp.Close = cmnFileClose;

		//You can set NULL
		VO_MEM_OPERATOR memOp;
		memOp.Alloc = cmnMemAlloc;
		memOp.Free = cmnMemFree;
		memOp.Set = cmnMemSet;
		memOp.Copy = cmnMemCopy;
		memOp.Check = cmnMemCheck;
		memOp.Compare = cmnMemCompare;

		VO_SOURCE_OPENPARAM srcOpenParam;
		srcOpenParam.nFlag = VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR | VO_SOURCE_OPENPARAM_FLAG_SOURCENAME | VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE;
		srcOpenParam.pSourceOP = &fileOp;
//		srcOpenParam.pSourceOP = NULL;
		srcOpenParam.pMemOP = &memOp;
//		srcOpenParam.pMemOP = NULL;

		srcOpenParam.pSource = (VO_PTR)_T("E:\\test_clips\\ID3tagsample\\Brother.mp3");
//		srcOpenParam.pSource = (VO_PTR)_T("E:\\test_clips\\ID3tagsample\\ID3Tag_WMA.wma");
//		srcOpenParam.pSource = (VO_PTR)_T("E:\\test_clips\\ID3tagsample\\mp4(iTunes)\\31.m4a");
//		srcOpenParam.pSource = (VO_PTR)_T("E:\\test_clips\\ID3tagsample\\mp4(TS26244)\\31.3g2");
		fAPIs.Open(&hFileParser, &srcOpenParam);

		VO_U32 rc = VO_ERR_METADATA_OK;
		MetaDataString mds;
		mds.dwFlags = 0;
//		mds.dwFlags = VO_METADATA_NOTADDSTRINGTERMINATE;
		MetaDataImage mdi;

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_TITLE, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_ARTIST, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_ALBUM, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_GENRE, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_COMPOSER, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_TRACK, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_FRONTCOVER, &mdi);
		if(VO_ERR_METADATA_OK == rc)
			ProcessImage(&mdi);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_GRACENOTETAGID, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_COMMENT, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_DISK, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_PUBLISHER, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_ISRC, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_YEAR, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_BAND, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_CONDUCTOR, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_REMIXER, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_LYRICIST, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_RADIOSTATION, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_ORIGRELEASEYEAR, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_OWNER, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_ENCODER, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_ENCODESETTINGS, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_COPYRIGHT, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_ORIGARTIST, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		rc = fAPIs.GetParam(hFileParser, VO_PID_METADATA_ORIGALBUM, &mds);
		if(VO_ERR_METADATA_OK == rc)
			ProcessString(&mds);

		fAPIs.Close(hFileParser);
	}

	if(hDll)
		FreeLibrary(hDll);
}