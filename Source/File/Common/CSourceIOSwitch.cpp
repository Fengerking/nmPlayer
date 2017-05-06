
#include "CSourceIOSwitch.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
VO_PTR CSourceIOSwitchOpen (VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL || pSource->nReserve == 0)
		return NULL;

	voSourceSwitch* pSwitchHandle = (voSourceSwitch*)pSource->nReserve;
	VO_SOURCE2_IO_API OperIO2 = *(VO_SOURCE2_IO_API*)pSwitchHandle->oprIO2;
	
	VO_U32 ret = OperIO2.Open(pSwitchHandle->phndIO2,VO_TRUE);
	if (ret != VO_SOURCE2_IO_OK)
	{
		return NULL;
	}
	return pSwitchHandle;
}

VO_S32 CSourceIOSwitchRead (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (!pHandle)
	{
		return -1;
	}

	voSourceSwitch* pSwitchHandle = (voSourceSwitch*)pHandle;
	VO_SOURCE2_IO_API OperIO2 = *(VO_SOURCE2_IO_API*)pSwitchHandle->oprIO2;

	VO_U32 nReadSize = 0;

	VO_U32 ret = OperIO2.Read(pSwitchHandle->phndIO2,pBuffer,uSize,&nReadSize);

	if (ret == VO_SOURCE2_IO_OK)
	{
		return nReadSize;
	}
	else
		return -1;
}

VO_S32 CSourceIOSwitchWrite (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (!pHandle)
	{
		return -1;
	}
	voSourceSwitch* pSwitchHandle = (voSourceSwitch*)pHandle;
	VO_SOURCE2_IO_API OperIO2 = *(VO_SOURCE2_IO_API*)(pSwitchHandle->oprIO2);

	VO_U32 nWriteSize = 0;

	VO_U32 ret = OperIO2.Write (pSwitchHandle->phndIO2, pBuffer, uSize, &nWriteSize);

	if (ret == VO_SOURCE2_IO_OK)
	{
		return nWriteSize;
	}
	else
		return -1;
}

VO_S32 CSourceIOSwitchFlush (VO_PTR pHandle)
{
	if (!pHandle)
	{
		return -1;
	}
	voSourceSwitch* pSwitchHandle = (voSourceSwitch*)pHandle;
	VO_SOURCE2_IO_API OperIO2 = *(VO_SOURCE2_IO_API*)pSwitchHandle->oprIO2;

	VO_U32 ret = OperIO2.Flush (pSwitchHandle->phndIO2);
	if (ret == VO_SOURCE2_IO_OK)
	{
		return 0;
	}
	else
		return -1;
}

VO_S64 CSourceIOSwitchSeek (VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag)
{
	if (!pHandle)
	{
		return -1;
	}
	voSourceSwitch* pSwitchHandle = (voSourceSwitch*)pHandle;
	VO_SOURCE2_IO_API OperIO2 = *(VO_SOURCE2_IO_API*)pSwitchHandle->oprIO2;

	VO_S64 nActualSize = 0;

	VO_SOURCE2_IO_POS RelativePos = VO_SOURCE2_IO_POS_BEGIN;
	switch(uFlag)
	{
	case VO_FILE_BEGIN:
		{
			RelativePos = VO_SOURCE2_IO_POS_BEGIN;
		}
		break;
	case VO_FILE_CURRENT:
		{
			RelativePos = VO_SOURCE2_IO_POS_CURRENT;
		}
		break;
	case VO_FILE_END:
		{
			RelativePos = VO_SOURCE2_IO_POS_END;
		}
		break;
	default:
		break;

	}

	VO_U32 ret = OperIO2.SetPos(pSwitchHandle->phndIO2, nPos, RelativePos, &nActualSize);
	if (ret == VO_SOURCE2_IO_OK)
	{
		return nActualSize;
	}
	else
		return -1;
}

VO_S64 CSourceIOSwitchSize (VO_PTR pHandle)
{
	if (!pHandle)
	{
		return -1;
	}
	voSourceSwitch* pSwitchHandle = (voSourceSwitch*)pHandle;
	VO_SOURCE2_IO_API OperIO2 = *(VO_SOURCE2_IO_API*)pSwitchHandle->oprIO2;

	VO_U64 llSize = 0;

	VO_U32 ret = OperIO2.GetSize(pSwitchHandle->phndIO2,&llSize);

	if (ret == VO_SOURCE2_IO_OK)
	{
		return llSize;
	}
	else
		return -1;
}

VO_S64 CSourceIOSwitchSave (VO_PTR pHandle)
{
	return 0;
}

VO_S32 CSourceIOSwitchClose (VO_PTR pHandle)
{
	if (!pHandle)
	{
		return -1;
	}
	voSourceSwitch* pSwitchHandle = (voSourceSwitch*)pHandle;
	VO_SOURCE2_IO_API *OperIO2 = (VO_SOURCE2_IO_API*)pSwitchHandle->oprIO2;

	OperIO2->Close (pSwitchHandle->phndIO2);
	return 0;
}

VO_S32	CSourceIOSwitchFillPointer (VO_FILE_OPERATOR * pFileOP)
{
	pFileOP->Open = CSourceIOSwitchOpen;
	pFileOP->Read = CSourceIOSwitchRead;
	pFileOP->Write = CSourceIOSwitchWrite;
	pFileOP->Flush = CSourceIOSwitchFlush;
	pFileOP->Seek = CSourceIOSwitchSeek;
	pFileOP->Size = CSourceIOSwitchSize;
	pFileOP->Save = CSourceIOSwitchSave;
	pFileOP->Close = CSourceIOSwitchClose;

	return 0;
}

#ifdef _VONAMESPACE
}
#endif