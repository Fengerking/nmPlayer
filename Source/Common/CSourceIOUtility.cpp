#include "CSourceIOUtility.h"

#include "voString.h"
#include "voOSFunc.h"
#include "voSource2.h"
#include "voLog.h"
#include "CDataBox.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#ifdef _REDRESS_DOWNLOADSIZE
	VO_U32 ReadFixedSize(DownloadDataCollector *pDataColletor, VO_SOURCE2_IO_API * ptr_api , VO_HANDLE handle , VO_PBYTE ptr_buffer , VO_U32* ptr_size , VO_BOOL * ptr_cancel, VO_U64 timeout, VO_SOURCEIO_EVENTCALLBACK *pCallback)
	{
		//	VO_U32 readed = 0;
		VO_PBYTE ptr_orgbuffer = ptr_buffer;
		VO_U32 ret = VO_SOURCE2_IO_OK;
		VO_U64 download_time = 0;
		VO_U32 uShouldRead = *ptr_size;
		*ptr_size = 0;
		while( *ptr_size <  uShouldRead && !(ptr_cancel && *ptr_cancel) )
		{
			VO_U32 readsize = 0;

			ret = ptr_api->Read( handle ,  ptr_buffer , uShouldRead - *ptr_size , &readsize );
			pDataColletor->AddSize(readsize);
			if( ret == VO_SOURCE2_IO_EOS )
			{
				*ptr_size += readsize;
				break;
			}
			else if( ret == VO_SOURCE2_IO_RETRY )
			{
				voOS_Sleep( 10 );
				continue;
			}
			else if( ret != VO_SOURCE2_IO_OK )
				return ret;
			if(timeout != -1 && timeout != 0 )
			{
				ret = ptr_api->GetParam( handle , VO_SOURCE2_IO_PARAMID_HTTPDOWNLOADTIME , &download_time );
				if( ret != VO_SOURCE2_IO_OK || download_time == 0 )
				{
					download_time = 1;
				}
				if(download_time > timeout)		//120% now
				{
					VOLOGW("Download slow,download time is %d,timeout is %d",(VO_U32)download_time,(VO_U32)timeout);
					if(pCallback != NULL)
					{						
						VO_U32 tmp = pCallback->NotifyEvent(pCallback->pUserData, VO_DATASOURCE_EVENTID_IO_DOWNLOADSLOW, VO_U32(pDataColletor->GetSize() * 8000./download_time), 0);
						if(tmp == VO_DATASOURCE_RET_DOWNLOAD_SLOW)
						{
							VOLOGR("After BA checked, Stop download.");
							return VO_DATASOURCE_RET_DOWNLOAD_SLOW;
						}						
						voOS_Sleep( 10 );
						VOLOGR("After BA checked, Download continuously.");
					}
					else
						return VO_DATASOURCE_RET_DOWNLOAD_SLOW;
				}
			}
			if( readsize < ( uShouldRead - *ptr_size ) / 10 )
			{
				voOS_Sleep( 10 );
			}

			*ptr_size += readsize;
			ptr_buffer = ptr_buffer + readsize;
		}

		if(ret == VO_SOURCE2_IO_EOS)
			return ret;
		else
			return (VO_TRUE == *ptr_cancel)?VO_SOURCE2_IO_FAIL: VO_SOURCE2_IO_OK;
	}

#endif

	VO_U32 ReadFixedSize( VO_SOURCE2_IO_API * ptr_api , VO_HANDLE handle , VO_PBYTE ptr_buffer , VO_U32* ptr_size , VO_BOOL * ptr_cancel, VO_U64 timeout)
	{
		//	VO_U32 readed = 0;
		VO_PBYTE ptr_orgbuffer = ptr_buffer;
		VO_U32 ret = VO_SOURCE2_IO_OK;
		VO_U64 download_time = 0;
		VO_U32 uShouldRead = *ptr_size;
		*ptr_size = 0;
		while( *ptr_size <  uShouldRead && !(ptr_cancel && *ptr_cancel) )
		{
			VO_U32 readsize = 0;

			ret = ptr_api->Read( handle ,  ptr_buffer , uShouldRead - *ptr_size , &readsize );
			if( ret == VO_SOURCE2_IO_EOS )
			{
				*ptr_size += readsize;
				break;
			}
			else if( ret == VO_SOURCE2_IO_RETRY )
			{
				voOS_Sleep( 20 );
				continue;
			}
			else if( ret != VO_SOURCE2_IO_OK )
				return ret;
			if(timeout != -1 && timeout != 0 )
			{
				ret = ptr_api->GetParam( handle , VO_SOURCE2_IO_PARAMID_HTTPDOWNLOADTIME , &download_time );
				if( ret != VO_SOURCE2_IO_OK || download_time == 0 )
				{
					download_time = 1;
				}
				if(download_time > timeout)		//120% now
				{
					VOLOGW("Download slow,download time is %d,timeout is %d",(VO_U32)download_time,(VO_U32)timeout);
					return VO_DATASOURCE_RET_DOWNLOAD_SLOW;
				}
			}
			if( readsize < ( uShouldRead - *ptr_size ) / 10 )
			{
				voOS_Sleep( 20 );
			}

		*ptr_size += readsize;
		ptr_buffer = ptr_buffer + readsize;
	}

	if(ret == VO_SOURCE2_IO_EOS)
		return ret;
	else
		return (VO_TRUE == *ptr_cancel)?VO_SOURCE2_IO_FAIL: VO_SOURCE2_IO_OK;
}

VO_U32  GetTheAbsolutePath(VO_CHAR* pstrDes, VO_CHAR* pstrInput, VO_CHAR* pstrRefer)
{
	VO_CHAR*    pFindForRefer = NULL;
	VO_CHAR*    pFindForInput = NULL;
	VO_CHAR*    pFindForReferForParam = NULL;
	VO_CHAR     strHelp[MAXURLLEN] = {0};
	memset( strHelp, 0x00, sizeof(strHelp) * sizeof(VO_CHAR) );
	if(pstrDes == NULL || pstrInput == NULL || pstrRefer == NULL)
	{
		return 0;
	}

	if (strstr(pstrRefer, "http://") != NULL ||
		strstr(pstrRefer, "https://") != NULL)
	{
		if(strstr(pstrInput, "http://") != NULL ||
           strstr(pstrInput, "https://") != NULL ||
           strstr(pstrInput, "iprm://") != NULL ||
		   strstr(pstrInput, "vrkp://") != NULL ||
            strstr(pstrInput, "vrkps://") != NULL)
		{
			memcpy(pstrDes, pstrInput, strlen(pstrInput));
			return 0;
		}
		else
		{
			if((*pstrInput) == '/')
			{
				pFindForRefer = strstr(pstrRefer+strlen("https://"), "/");
				if(pFindForRefer == NULL)
				{
					memcpy(pstrDes, pstrRefer, strlen(pstrRefer));
					memcpy(pstrDes+strlen(pstrRefer), pstrInput, strlen(pstrInput));
					return 0;
				}
				else
				{
					memcpy(pstrDes, pstrRefer, pFindForRefer-pstrRefer);
					memcpy(pstrDes+(pFindForRefer-pstrRefer), pstrInput, strlen(pstrInput));
					return 0;
				}
			}
			else
			{
				pFindForReferForParam = strrchr(pstrRefer, '?');
				pFindForRefer = strrchr(pstrRefer, '/');
				if(pFindForReferForParam != NULL && (pFindForRefer > pFindForReferForParam))
				{
					memcpy(strHelp, pstrRefer, pFindForReferForParam-pstrRefer);
					pFindForRefer = strrchr(strHelp, '/');
					memcpy(pstrDes, strHelp, pFindForRefer-strHelp+1);
					memcpy(pstrDes+(pFindForRefer-strHelp+1), pstrInput, strlen(pstrInput));
				}
				else
				{
					memcpy(pstrDes, pstrRefer, pFindForRefer-pstrRefer+1);
					memcpy(pstrDes+(pFindForRefer-pstrRefer+1), pstrInput, strlen(pstrInput));
				}

				return 0;
			}
		}
	}
	else
	{
		if(strstr(pstrInput, "http://") != NULL ||
           strstr(pstrInput, "https://") != NULL ||           
		   strstr(pstrInput, "iprm://") != NULL ||
		   strstr(pstrInput, "vrkp://") != NULL ||
            strstr(pstrInput, "vrkps://") != NULL)
		{
			VOLOGE("Main Path is local path,the reltive path is http");
			memcpy(pstrDes, pstrInput, strlen(pstrInput));
			return 0;
		}
		VOLOGE("strlen==%d",strlen(pstrRefer));
		VOLOGE("char array==%x:%x:%x:%x:%x:%x",pstrRefer[0],pstrRefer[1],pstrRefer[2],pstrRefer[3],pstrRefer[4],pstrRefer[5]);
		if((*pstrInput) == '/')
		{
		    if((*pstrRefer) == '/')
		    {
		        memcpy(pstrDes, pstrInput, strlen(pstrInput)); 
		        return 0;
		    }
		}
		
		VO_CHAR * pDesChar = NULL;
		pDesChar = strrchr(pstrRefer,'\\');
		if (!pDesChar)
		{
			pDesChar = strrchr(pstrRefer,'/');
		}
		
		if (!pDesChar)
		{
			VOLOGE("Main Path is error");
			return 0;
		}
		memcpy(pstrDes,pstrRefer,pDesChar - pstrRefer + 1);
		memcpy(pstrDes+(pDesChar - pstrRefer + 1),pstrInput,strlen(pstrInput));
	}
	return 0;
}



/* Merge the base URI and Reference URI into the pstrPath*/
VO_U32 MergeURIPath(VO_PTCHAR pstrPath ,VO_PTCHAR pstrAuthorityBase,VO_PTCHAR pstrPathBase, VO_PTCHAR pstrPathRefer)
{
	VOLOGR("+++MergeURIPath+++:pstrAuthorityBase=%s,pstrPathBase=%s,pstrPathRefer=%s",pstrAuthorityBase,pstrPathBase,pstrPathRefer);
	memset(pstrPath,0,sizeof(VO_TCHAR)*vostrlen(pstrPath));
	if (pstrAuthorityBase && vostrlen(pstrAuthorityBase) && (!pstrPathBase || !vostrlen(pstrPathBase)))
	{	
		///<return a string consisting of "/" concatenated with the reference's path
		vostrcat(pstrPath,_T("/"));
		vostrcat(pstrPath,pstrPathRefer);
		return 0;
	}
	///<return a string consisting of the reference's path component appended to all but the last segment of the
	///<base URI's path
	VO_PTCHAR pLastSlash = vostrrchr(pstrPathBase,'/');
	if (pLastSlash)
	{
		vostrncpy(pstrPath,pstrPathBase,vostrlen(pstrPathBase) - vostrlen(pLastSlash) + 1);
		vostrcat(pstrPath,pstrPathRefer);
	}
	else
	{
		vostrcpy(pstrPath,pstrPathRefer);
	}
	VOLOGR("---RemoveDotSegments---:pstrPath=%s",pstrPath);
	return 0;
}
/*
remove the dot sgement from input path and fill into output path
*/
VO_U32 RemoveDotSegments(VO_PTCHAR pstrOutputPath,VO_PTCHAR pstrInputPath)
{
	if (!pstrOutputPath || !pstrInputPath && !vostrlen(pstrInputPath))
	{
		VOLOGE("RemoveDotSegments:Input or Out string is NULL");
		return 0;
	}
	VOLOGR("+++RemoveDotSegments+++:pstrInputPath=%s",pstrInputPath);

	VO_PTCHAR pCurInput = pstrInputPath;
	while(pCurInput && vostrlen(pCurInput))
	{
		VO_U32 uCurLen = vostrlen(pCurInput);
		if (!vostrncmp(pCurInput,_T("./"),2) || !vostrncmp(pCurInput,_T("../"),3))
		{
			///<remove the "."or ".." before char'/'
			if (pCurInput[1] == '/')
			{
				pCurInput += 2;
			}
			else
				pCurInput += 3;
		}
		else if (!vostrncmp(pCurInput,_T("/."),2))
		{
			if (uCurLen >= 3 && pCurInput[2] == '.')///< "/.."
			{
				if (uCurLen >= 4 && pCurInput[3] == '/')///< "/../"
				{
					///<replace the "/../" with '/'
					vostrcpy(pCurInput+1,pCurInput + 4);			
				}
				else
				{
					///<replace the "/.." with '/'
					vostrcpy(pCurInput+1,pCurInput + 3);///< "/..a or anyother except '/'"
				}

				///<remove the last segment of output path
				VO_PTCHAR pLastSegment= vostrrchr(pstrOutputPath,'/');
				if (pLastSegment)
				{
					*pLastSegment = 0;///<remove all string after pLastSegment including pLastSegment char
				}
				else
					pstrOutputPath[0] = 0;
			}
			else if (uCurLen >= 3 && pCurInput[2] == '/')
			{
				///<replace the "/./" with '/'
				vostrcpy(pCurInput+1,pCurInput + 3);
			}
			else
			{
				///<replace the "/." with '/'
				vostrcpy(pCurInput+1,pCurInput + 2);
			}
		}
		else
		{
			VO_PTCHAR pTmp = pCurInput;
			while (*pTmp == '.')
			{
				pTmp++;
			}
			if (*pTmp == 0)
			{
				///<if all chars in path's string are '.',remove all
				*pCurInput = 0;
			}
			else
			{
				VO_PTCHAR pFirstSegment = pCurInput;
				VO_PTCHAR pNextSegment = uCurLen >2 ? vostrchr(pCurInput+1,'/') : NULL;

				///<append first segment of input path to end of output path
				if (pNextSegment)
				{
					vostrncpy(pstrOutputPath+vostrlen(pstrOutputPath),pFirstSegment,pNextSegment - pFirstSegment);
				}
				else
				{
					vostrcat(pstrOutputPath,pFirstSegment);
				}
				pCurInput = pNextSegment;
			}
		}
	}
	VOLOGR("---RemoveDotSegments---:pstrOutputPath=%s,pstrInputPath=%s",pstrOutputPath,pstrInputPath);
	return 0;
}

/*Generate every component from URI*/

VO_U32 GetComponentOfURI(VO_PTCHAR pstrScheme,VO_PTCHAR pstrAuthority,VO_PTCHAR pstrPath,VO_PTCHAR pstrQuery,VO_PTCHAR pstrFragment,VO_PTCHAR pstrURI)
{
	VOLOGR("+++GetComponentOfURI+++:pstrURI=%s",pstrURI);
	if (!pstrURI || !vostrlen(pstrURI))
	{
		return 0;
	}
	
	VO_PTCHAR pstrTmpScheme = NULL;
	VO_PTCHAR pstrTmpAuthority = NULL;
	VO_PTCHAR pstrTmpPath = NULL;
	VO_PTCHAR pstrTmpQuery = NULL;
	VO_PTCHAR pstrTmpFragment = NULL;


	pstrTmpScheme = vostrchr(pstrURI,':');
	if (pstrTmpScheme)
	{
		///<check if it is actual end of scheme
		VO_TCHAR strTmp[MAXURLLEN] = {0};
		vostrncpy(strTmp , pstrURI,pstrTmpScheme - pstrURI);
		if (vostrchr(strTmp,'/') || vostrchr(strTmp,'?') || vostrchr(strTmp,'#'))
		{
			pstrTmpScheme = NULL;
		}
		///<fill the Scheme if there is scheme
		if (pstrTmpScheme && pstrScheme )
		{
			vostrcpy(pstrScheme , strTmp);
		}
	}
	if (pstrTmpScheme && vostrlen(pstrTmpScheme) <= 1)
	{
		///<there is no other component
		return 0;
	}
	pstrTmpAuthority = pstrTmpScheme ? pstrTmpScheme : pstrURI;
	pstrTmpAuthority = vostrstr(pstrTmpAuthority,_T("//"));
	if (pstrTmpAuthority)
	{
		pstrTmpAuthority += 2;
		///<If a URI contains an authority component, then the path component must either be empty or begin with a slash
		///<("/") character
		pstrTmpPath = vostrlen(pstrTmpAuthority) ? vostrchr(pstrTmpAuthority,'/') : NULL;
		
	}
	else
	{
		pstrTmpPath = pstrTmpScheme ? pstrTmpScheme + 1 : pstrURI;
	}

	VO_PTCHAR pstrTmpBegin = pstrTmpPath ? pstrTmpPath : pstrTmpAuthority ? pstrTmpAuthority : pstrURI;
	pstrTmpQuery = vostrchr(pstrTmpBegin,'?');

	pstrTmpBegin = pstrTmpQuery ? pstrTmpQuery : pstrTmpBegin;
	pstrTmpFragment = vostrchr(pstrTmpBegin,'#');

	///<verify the path
	if ((pstrTmpQuery && pstrTmpPath >= pstrTmpQuery) || (pstrTmpFragment && pstrTmpPath >= pstrTmpFragment))
	{
		pstrTmpPath = NULL;
	}

	VO_PTCHAR pstrTmpEnd = NULL;
	if (pstrTmpAuthority)
	{
		pstrTmpEnd = pstrTmpPath ? pstrTmpPath : pstrTmpQuery ? pstrTmpQuery : pstrTmpFragment ? pstrTmpFragment : pstrURI + vostrlen(pstrURI);
		if (pstrAuthority)
		{
			vostrncpy(pstrAuthority , pstrTmpAuthority,pstrTmpEnd - pstrTmpAuthority);
		}
	}

	if (pstrTmpPath)
	{
		pstrTmpEnd = pstrTmpQuery ? pstrTmpQuery : pstrTmpFragment ? pstrTmpFragment : pstrURI + vostrlen(pstrURI);
		if (pstrPath)
		{
			vostrncpy(pstrPath , pstrTmpPath,pstrTmpEnd - pstrTmpPath);
		}
	}
	if (pstrTmpQuery)
	{
		pstrTmpQuery += 1;///<skip the '?'
		pstrTmpEnd = pstrTmpFragment ? pstrTmpFragment : pstrURI + vostrlen(pstrURI);
		if (pstrQuery)
		{
			vostrncpy(pstrQuery , pstrTmpQuery,pstrTmpEnd - pstrTmpQuery);
		}
	}
	if(pstrTmpFragment)
	{
		pstrTmpFragment += 1;///<skip the '#'

		pstrTmpEnd = pstrURI + vostrlen(pstrURI);
		if (pstrFragment)
		{
			vostrncpy(pstrFragment , pstrTmpFragment,pstrTmpEnd - pstrTmpFragment);
		}
	}
	VOLOGR("---GetComponentOfURI---:Sc=%s,Au=%s,Pa=%s,Qu=%s,Fr=%s,",pstrScheme,pstrAuthority,pstrPath,pstrQuery,pstrFragment);
	return 0;

}

/*Generate the target URI by the five component*/
VO_U32 GeneateURI(VO_PTCHAR pstrScheme,VO_PTCHAR pstrAuthority,VO_PTCHAR pstrPath,VO_PTCHAR pstrQuery,VO_PTCHAR pstrFragment,VO_PTCHAR pstrDes)
{
	if (!pstrDes)
	{
		VOLOGE("GeneateURI:Out URI string is NULL");
		return 0;
	}
	memset(pstrDes,0,sizeof(VO_TCHAR)*vostrlen(pstrDes));
	if (pstrScheme && vostrlen(pstrScheme))
	{
		vostrcat(pstrDes,pstrScheme);
		vostrcat(pstrDes,_T(":"));
	}
	if (pstrAuthority && vostrlen(pstrAuthority))
	{
		vostrcat(pstrDes,_T("//"));
		vostrcat(pstrDes,pstrAuthority);	
	}
	if (pstrPath)
	{
		vostrcat(pstrDes,pstrPath);	
	}
	if (pstrQuery && vostrlen(pstrQuery))
	{
		vostrcat(pstrDes,_T("?"));
		vostrcat(pstrDes,pstrQuery);	
	}
	if (pstrFragment && vostrlen(pstrFragment))
	{
		vostrcat(pstrDes,_T("#"));
		vostrcat(pstrDes,pstrFragment);	
	}
	return 0;
}
/**
	* Get the absolute path by baseURI and reference URI.
	* \param pstrDes [IN][OUT] the target URI
	* \param pstrRefer [IN] reference URI
	* \param pstrBase [IN]	base URI
	* \retval 0 Succeeded.
	*/
VO_U32  GetTheAbsolutePath2(VO_PTCHAR pstrDes, VO_PTCHAR pstrRefer, VO_PTCHAR pstrBase)
{
	VOLOGR("+++GetTheAbsolutePath2+++:pstrRefer=%s,pstrBase=%s",pstrRefer,pstrBase);
	if (!pstrDes)
	{
		VOLOGE("GetTheAbsolutePath2:Out string is NULL");
		return 0;
	}
	VO_TCHAR strScheme[MAXURLLEN] = {0};
	VO_TCHAR strAuthority[MAXURLLEN] = {0};
	VO_TCHAR strPath[MAXURLLEN] = {0};
	VO_TCHAR strQuery[MAXURLLEN] = {0};
	VO_TCHAR strFragment[MAXURLLEN] = {0};
	VO_TCHAR strOutPath[MAXURLLEN] = {0};

	GetComponentOfURI(strScheme,strAuthority,strPath,strQuery,strFragment,pstrRefer);

	if (vostrlen(strScheme))
	{
		///<remove dot segment of refer URI as des URI
		RemoveDotSegments(strOutPath,strPath);
	}
	else
	{
		///<take scheme of Base URI as des URI's
		GetComponentOfURI(strScheme,NULL,NULL,NULL,NULL,pstrBase);
		
		if (vostrlen(strAuthority))
		{
			///<remove dot segment of refer URI as des URI
			RemoveDotSegments(strOutPath,strPath);
		}
		else
		{
			///<take authority of Base URI as des URI's
			GetComponentOfURI(NULL,strAuthority,NULL,NULL,NULL,pstrBase);

			if (!vostrlen(strPath))///<strPath of Refer URI is empty
			{
				///<take path of Base URI as des URI's
				GetComponentOfURI(NULL,NULL,strPath,NULL,NULL,pstrBase);
	
				if (!vostrlen(strQuery))
				{
					///<take query of Base URI as des URI's
					GetComponentOfURI(NULL,NULL,NULL,strQuery,NULL,pstrBase);
				}
			}
			else
			{
				if (!vostrncmp(strPath,_T("/"),1))
				{
					///<remove dot segment of refer URI as des URI
					RemoveDotSegments(strOutPath,strPath);
				}
				else
				{
					VO_TCHAR strPathBaseURI[MAXURLLEN] = {0};
					VO_TCHAR strAuthorityBaseURI[MAXURLLEN] = {0};
					///<get authority && path of base URI
					GetComponentOfURI(NULL,strAuthorityBaseURI,strPathBaseURI,NULL,NULL,pstrBase);
					///<merge the path of base URI and refer URI
					MergeURIPath(strOutPath,strAuthorityBaseURI,strPathBaseURI,strPath);
					///<remove dot segment of merged URI as des URI
					VO_TCHAR strTmp[MAXURLLEN] = {0};
					RemoveDotSegments(strTmp,strOutPath);
					vostrcpy(strOutPath,strTmp);
				}
			}
		}
	}
	if (vostrlen(strOutPath))
	{
		vostrcpy(strPath,strOutPath);
	}
	GeneateURI(strScheme,strAuthority,strPath,strQuery,strFragment,pstrDes);

	VOLOGR("---GetTheAbsolutePath2---:Des=%s,Sc=%s,Au=%s,Pa=%s,Qu=%s,Fr=%s,",pstrDes,strScheme,strAuthority,strPath,strQuery,strFragment);
	return 0;
}


VO_U32 DownloadItem_II( VO_SOURCE2_IO_API *pIoApi, VO_SOURCE2_VERIFICATIONINFO * pVerificationInfo,  VO_SOURCE2_IO_HTTPCALLBACK * pIOHttpCallback,
					   VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_CHAR **ppBuffer, VO_U32 *pSize, VO_U32 *pUsedSize,VO_BOOL * pCancel, VO_BOOL bChangeURL   )
{
	VO_HANDLE h;
	VO_U32 ret = 0;

	VO_CHAR url[MAXURLLEN] = {0};
	memset( url, 0x00, sizeof(url) * sizeof(VO_CHAR) );
	if( bChangeURL )
		GetTheAbsolutePath( url , pData->szUrl , pData->szRootUrl );
	else
		memcpy( url, pData->szUrl, MAXURLLEN);
	voAutoIOInit init_obj( pIoApi , url , &h );

	if( init_obj.m_ret != VO_SOURCE2_IO_OK )
		return init_obj.m_ret;


	if( pVerificationInfo )
	{
		pIoApi->SetParam( h , VO_SOURCE2_IO_PARAMID_DOHTTPVERIFICATION , pVerificationInfo );
	}
	if( pIOHttpCallback )
	{
		pIoApi->SetParam(h, VO_SOURCE2_IO_PARAMID_HTTPIOCALLBACK, pIOHttpCallback );
	}

//	voAutoIOOpen open_obj( pIoApi , h , VO_FALSE );
	//using async open for sourceIO
	voAutoIOOpen open_obj( pIoApi , h , VO_TRUE );

	if( open_obj.m_ret != VO_SOURCE2_IO_OK )
		return open_obj.m_ret;

	VO_U64 size = 0;
	while(true)
	{
		ret = pIoApi->GetSize( h , &size );
	//	VOLOGI("Getsize return 0x%08x, size is %d",ret,(int)size);
		if(VO_SOURCE2_IO_FAIL == ret)
			return ret;
		if(VO_SOURCE2_IO_OK == ret || (*pCancel))
			break;
		voOS_Sleep(20);
	}

	if( !(*ppBuffer) || *pSize < 3 * size )
//	if( !(*ppBuffer) || *pSize < size+1 )
	{
		if( (*ppBuffer) )
			delete [](*ppBuffer);

		(*ppBuffer) = new VO_CHAR[ 3 * size ];
		*pSize = 3 * size;
		//memset( (*ppBuffer) , 0 , *pSize * sizeof( VO_CHAR ) );
	}

	memset( (*ppBuffer) , 0 , ( *pSize ) * sizeof( VO_CHAR ) );

	*pUsedSize = (VO_U32)size;

	ret = ReadFixedSize( pIoApi , h , (VO_PBYTE)(*ppBuffer) , pUsedSize , pCancel);

	if( ret != VO_SOURCE2_IO_OK )
	{
		return ret;
	}

	pData->pData = (VO_PBYTE)(*ppBuffer);
	pData->uDataSize = *pUsedSize;
	pData->uFullDataSize = 3 * size;


	// 	VO_ADAPTIVESTREAM_PLAYLISTDATA data;
	// 	memset( &data , 0 , sizeof( VO_ADAPTIVESTREAM_PLAYLISTDATA ) );
	// 	data.pData = (VO_PBYTE)buffer;
	// 	data.uDataSize = readsize;

	VO_SOURCE2_IO_HTTP_REDIRECT_URL redirecturl;
	memset( &redirecturl , 0 , sizeof( VO_SOURCE2_IO_HTTP_REDIRECT_URL ) );
	redirecturl.ptr_url = url;
	redirecturl.orig_size = strlen( url );
	redirecturl.ptr_redirect_url = pData->szNewUrl;
	redirecturl.redirect_size = sizeof( pData->szNewUrl );

	VO_U32 ioret = pIoApi->GetParam( h , VO_SOURCE2_IO_PARAMID_HTTPREDIRECTURL , &redirecturl );

	if( ioret != VO_SOURCE2_IO_OK )
	{
		strcpy( pData->szNewUrl , url );
	}
	
	if( *pCancel )
		return VO_RET_SOURCE2_FAIL;

	return ret;
}


VO_U32 DownloadItem_III( VO_SOURCE2_IO_API *pIoApi, VO_SOURCE2_VERIFICATIONINFO * pVerificationInfo,  VO_SOURCE2_IO_HTTPCALLBACK * pIOHttpCallback,
					   VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_BOOL * pCancel, VO_BOOL bChangeURL   )
{
	VO_HANDLE h;
	VO_U32 ret = 0;

	VO_CHAR url[MAXURLLEN] = {0};
	memset( url, 0x00, sizeof(url) * sizeof(VO_CHAR) );
	if( bChangeURL )
		GetTheAbsolutePath( url , pData->szUrl , pData->szRootUrl );
	else
		memcpy( url, pData->szUrl, MAXURLLEN);
	voAutoIOInit init_obj( pIoApi , url , &h );

	if( init_obj.m_ret != VO_SOURCE2_IO_OK )
		return init_obj.m_ret;


	if( pVerificationInfo )
	{
		pIoApi->SetParam( h , VO_SOURCE2_IO_PARAMID_DOHTTPVERIFICATION , pVerificationInfo );
	}
	if( pIOHttpCallback )
	{
		pIoApi->SetParam(h, VO_SOURCE2_IO_PARAMID_HTTPIOCALLBACK, pIOHttpCallback );
	}

//	voAutoIOOpen open_obj( pIoApi , h , VO_FALSE );
	//using async open for sourceIO
	voAutoIOOpen open_obj( pIoApi , h , VO_TRUE );

	if( open_obj.m_ret != VO_SOURCE2_IO_OK )
		return open_obj.m_ret;

	VO_U64 size = 0;
	while(true)
	{
		ret = pIoApi->GetSize( h , &size );
		//VOLOGI("Getsize return 0x%08x, size is %d",ret,(int)size);
		if(VO_SOURCE2_IO_FAIL == ret)
			return ret;
		if(VO_SOURCE2_IO_OK == ret || (*pCancel))
			break;
		voOS_Sleep(20);
	}
	
	if(!pData->pReserve){
		return VO_RET_SOURCE2_FAIL;
	}

	VO_DATABOX_CALLBACK* pCallBack = (VO_DATABOX_CALLBACK*)pData->pReserve;
	pCallBack->MallocData(pCallBack->pUserData, &pData->pData, 3 * size);
	pData->uDataSize = size;
	pData->uFullDataSize = 3 * size;
	ret = ReadFixedSize( pIoApi , h , pData->pData , &pData->uDataSize , pCancel);

	if( ret != VO_SOURCE2_IO_OK )
	{
		return ret;
	}

//	pData->pData = (VO_PBYTE)(*ppBuffer);
//	pData->uDataSize = *pUsedSize;


	// 	VO_ADAPTIVESTREAM_PLAYLISTDATA data;
	// 	memset( &data , 0 , sizeof( VO_ADAPTIVESTREAM_PLAYLISTDATA ) );
	// 	data.pData = (VO_PBYTE)buffer;
	// 	data.uDataSize = readsize;

	VO_SOURCE2_IO_HTTP_REDIRECT_URL redirecturl;
	memset( &redirecturl , 0 , sizeof( VO_SOURCE2_IO_HTTP_REDIRECT_URL ) );
	redirecturl.ptr_url = url;
	redirecturl.orig_size = strlen( url );
	redirecturl.ptr_redirect_url = pData->szNewUrl;
	redirecturl.redirect_size = sizeof( pData->szNewUrl );

	VO_U32 ioret = pIoApi->GetParam( h , VO_SOURCE2_IO_PARAMID_HTTPREDIRECTURL , &redirecturl );

	if( ioret != VO_SOURCE2_IO_OK )
	{
		strcpy( pData->szNewUrl , url );
	}

	if( *pCancel )
		return VO_RET_SOURCE2_FAIL;

	return ret;
}

// VO_U32 DownloadItem_II( VO_SOURCE2_IO_API *pIoApi, VO_SOURCE2_VERIFICATIONINFO * pVerificationInfo, VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_CHAR **ppBuffer, VO_U32 *pSize, VO_U32 *pUsedSize,VO_BOOL * pCancel, VO_BOOL bChangeURL   )
// {
// 	VO_HANDLE h;
// 	VO_U32 ret = 0;
// 
// 	VO_CHAR url[MAXURLLEN] = {0};
// 	if( bChangeURL )
// 		GetTheAbsolutePath( url , pData->szUrl , pData->szRootUrl );
// 	else
// 		memcpy( url, pData->szUrl, MAXURLLEN);
// 	voAutoIOInit init_obj( pIoApi , url , &h );
// 
// 	if( init_obj.m_ret != VO_SOURCE2_IO_OK )
// 		return init_obj.m_ret;
// 
// 
// 	if( pVerificationInfo )
// 	{
// 		pIoApi->SetParam( h , VO_SOURCE2_IO_PARAMID_DOHTTPVERIFICATION , pVerificationInfo );
// 	}
// 
// 	voAutoIOOpen open_obj( pIoApi , h , VO_FALSE );
// 
// 	if( open_obj.m_ret != VO_SOURCE2_IO_OK )
// 		return open_obj.m_ret;
// 
// 	VO_U64 size;
// 	while( VO_SOURCE2_IO_OK != pIoApi->GetSize( h , &size ) && !(*pCancel) )
// 		voOS_Sleep( 20 );
// 
// 	if( !(*ppBuffer) || *pSize < 2 * size )
// 		//	if( !(*ppBuffer) || *pSize < size+1 )
// 	{
// 		if( (*ppBuffer) )
// 			delete [](*ppBuffer);
// 
// 		(*ppBuffer) = new VO_CHAR[ 2 * size ];
// 		*pSize = 2 * size;
// 		memset( (*ppBuffer) , 0 , *pSize * sizeof( VO_CHAR ) );
// 	}
// 
// 	memset( (*ppBuffer) , 0 , ( size + 1 ) * sizeof( VO_CHAR ) );
// 
// 	VO_U32 readsize = (VO_U32)size;
// 
// 	ret = ReadFixedSize( pIoApi , h , (VO_PBYTE)(*ppBuffer) , &readsize , pCancel );
// 
// 	if( ret != VO_SOURCE2_IO_OK )
// 	{
// 		return ret;
// 	}
// 
// 	pData->pData = (VO_PBYTE)(*ppBuffer);
// 	pData->uDataSize = readsize;
// 
// 
// 	// 	VO_ADAPTIVESTREAM_PLAYLISTDATA data;
// 	// 	memset( &data , 0 , sizeof( VO_ADAPTIVESTREAM_PLAYLISTDATA ) );
// 	// 	data.pData = (VO_PBYTE)buffer;
// 	// 	data.uDataSize = readsize;
// 
// 	VO_SOURCE2_IO_HTTP_REDIRECT_URL redirecturl;
// 	memset( &redirecturl , 0 , sizeof( VO_SOURCE2_IO_HTTP_REDIRECT_URL ) );
// 	redirecturl.ptr_url = url;
// 	redirecturl.orig_size = strlen( url );
// 	redirecturl.ptr_redirect_url = pData->szNewUrl;
// 	redirecturl.redirect_size = sizeof( pData->szNewUrl );
// 
// 	VO_U32 ioret = pIoApi->GetParam( h , VO_SOURCE2_IO_PARAMID_HTTPREDIRECTURL , &redirecturl );
// 
// 	if( ioret != VO_SOURCE2_IO_OK )
// 	{
// 		strcpy( pData->szNewUrl , url );
// 	}
// 
// 	return ret;
// }
#ifdef _VONAMESPACE
}
#endif ///< _VONAMESPACE
