#ifndef __CSourceIOSwitch_H__
#define __CSourceIOSwitch_H__

#include "voType.h"
#include "voString.h"
#include "voFile.h"
#include "voSource2_IO.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

typedef struct
{
	VO_PTR				phndIO2;
	VO_PTR				oprIO2;
	VO_BOOL				bDrm;
}voSourceSwitch;

typedef struct
{
	VO_SOURCE2_IO_API	oprIO2;
	VO_BOOL				bDrm;
}voSourceSwitchInfo;
/**
 * Open the file if it exist, or create the file if mode contains write
 * \param uID [in] The module ID
 * \param uSize [in] The file name
 * \param uMode [in] The file operate mode
 * \return value 0 failed. or file pointer
 */
VO_PTR	CSourceIOSwitchOpen (VO_FILE_SOURCE * pSource);

/**
 * Read file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnIOSwitchOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to read
 * \return value is read data size.
 */
VO_S32	CSourceIOSwitchRead (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);

/**
 * Write file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnIOSwitchOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to write
 * \return value is write data size.
 */
VO_S32	CSourceIOSwitchWrite (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);


/**
 * flush the file buffer to disk
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnIOSwitchOpen.
 * \return value non zero is successful.
 */
VO_S32	CSourceIOSwitchFlush (VO_PTR pHandle);


/**
 * Seek the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnIOSwitchOpen.
 * \param nPos [in] The seek position
 * \param uFlag [in] The start file position
 * \return value is current file position
 */
VO_S64	CSourceIOSwitchSeek (VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag);


/**
 * get the file szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnIOSwitchOpen.
 * \return value is the file size
 */
VO_S64	CSourceIOSwitchSize (VO_PTR pHandle);

/**
 * get the file saved szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnIOSwitchOpen.
 * \return value is the file size
 */
VO_S64	CSourceIOSwitchSave (VO_PTR pHandle);

/**
 * close the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnIOSwitchOpen.
 * \return value 0 is successful
 */
VO_S32	CSourceIOSwitchClose (VO_PTR pHandle);


/**
 * Fill the file operator function pointer
 * \param uID [in] The module ID
 * \return value 0 is successful
 */
VO_S32	CSourceIOSwitchFillPointer (VO_FILE_OPERATOR * pFileOP);

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#endif // CSourceIOSwitch