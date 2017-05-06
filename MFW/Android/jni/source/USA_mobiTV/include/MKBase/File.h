#ifndef __MKBase_File_h__
#define __MKBase_File_h__

#include <MKBase/Platform.h>
#include <MKBase/Config.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>
#include <MKBase/Result.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

typedef struct _MK_File MK_File;

#define MK_FILE_INITVAL _MK_FILE_INITVAL

/******************************************************************************/

typedef MK_U32 MK_File_OFlag;

enum
{
    kFileWr = 1 << 0,   /* Open for writing */
    kFileRd = 1 << 1,   /* Open for reading */
    kFileNew = 1 << 2,  /* Create/truncate file if neccesary */
    kFileTemp = 1 << 3  /* Temporary file, delete on close */
};

typedef MK_U32 MK_File_SType;

enum
{
    kFileBeg = 0, /* Relative to beginning of file */
    kFileCur = 1, /* Relative to current position */
    kFileEnd = 2  /* Relative to end of file */
};

/******************************************************************************/

/*
 * Return an initialized MK_File structure.
 */
#if !(MK_WIN32 && __cplusplus)
    /* NOTE: Avoid VS bug when compiling as C++ */
    MK_INLINE MK_File MK_File_InitVal(void);
#endif

/*
 * Initialize aFile structure.
 */
MK_INLINE void MK_File_Init(MK_File* aFile);

/*
 * Swap the contents of two file structures.
 */
MK_INLINE void MK_File_Swap(MK_File* aFile1, MK_File* aFile2);

/*
 * Free any resources owned by aFile structure and reinitialize it (alias for
 * MK_File_Close function).
 */
MK_INLINE void MK_File_Destroy(MK_File* aFile);

/******************************************************************************/

/*
 * Open a file. Returns:
 *
 *   MK_S_OK           - Success
 *   MK_E_FILE_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_Result MK_File_Open(MK_File* aFile, const MK_Char* aPath, MK_File_OFlag aFlag);

/*
 * Free any resources owned by aFile structure and reinitialize it.
 */
void MK_File_Close(MK_File* aFile);

/******************************************************************************/

/*
 * Check validity of aFile. Returns MK_TRUE if valid and MK_FALSE otherwise.
 */
MK_INLINE MK_Bool MK_File_IsValid(MK_File* aFile);

/*
 * Returns number of bytes directly available for reading. A negatve value means
 * an error occured.
 *
 *   MK_E_FILE_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_INLINE MK_S64 MK_File_Avail(MK_File* aFile);

/*
 * Check if aFile is at EOF. Returns MK_TRUE if socket is at EOF and MK_FALSE
 * otherwise.
 */
MK_INLINE MK_Bool MK_File_PendingEOF(MK_File* aFile);

/******************************************************************************/

/*
 * Wait a maximum of aMSec (0 for IsWritable) millisecond for the file aFile
 * to become writable. Returns a value > 0 if the file became writable, 0 if the
 * timeout expired and < 0 if an error occured:
 *
 *   MK_E_FILE_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S32 MK_File_WaitWritable(MK_File* aFile, MK_U32 aMSec);
MK_INLINE MK_S32 MK_File_IsWritable(MK_File* aFile)
{
    return MK_File_WaitWritable(aFile, 0);
}

/*
 * Wait a maximum of aMSec (0 for IsReadable) millisecond for the file aFile
 * to become readable. Returns a value > 0 if the file became readable, 0 if the
 * timeout expired and < 0 if an error occured.
 *
 *   MK_E_FILE_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S32 MK_File_WaitReadable(MK_File* aFile, MK_U32 aMSec);
MK_INLINE MK_S32 MK_File_IsReadable(MK_File* aFile)
{
    return MK_File_WaitReadable(aFile, 0);
}

/******************************************************************************/

/*
 * Write aBufSz bytes of data from aBuf. Returns the number of bytes actually
 * written (which may be less than aBufSz). A return value equal to aBufSz
 * indicates success, a value < aBufSz indicates a partial write (due to
 * timeout, interruption or EOF) and a value < 0 indicates an error:
 *
 *   MK_E_AGAIN        - Would block/timeout
 *   MK_E_FILE_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S32 MK_File_Write(MK_File* aFile, const void* aBuf, MK_U32 aBufSz);

/*
 * Read aBufSz data into aBuf from aFile. Returns the number of bytes actually
 * read (which may be less than aBufSz). A return-value equal to aBufSz
 * indicates success, a value < aBufSz but > 0 indicates a partial read (due to
 * timeout, interruption or EOF), a value equal to 0 indicates EOF and a
 * value < 0 indicates an error:
 *
 *   MK_E_AGAIN        - Would block/timeout
 *   MK_E_FILE_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S32 MK_File_Read(MK_File* aFile, void* aBuf, MK_U32 aBufSz);

/******************************************************************************/

/*
 * Move the current position in the file to aTo with aFrom as the reference
 * point. Returns the new absolute position if successful and a negative result
 * code on error:
 *
 *   MK_E_FILE_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S64 MK_File_Seek(MK_File* aFile, MK_S64 aTo, MK_File_SType aFrom);

/*
 * Returns the current absolute position or a negative result code on error:
 *
 *   MK_E_FILE_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_INLINE MK_S64 MK_File_Pos(MK_File* aFile);

/*
 * Returns the current file size or a negative result code on error:
 *
 *   MK_E_FILE_FAIL    - Unspecified error (triggers LOGERR)
 */
MK_S64 MK_File_Size(MK_File* aFile);

/******************************************************************************/

/*
 * TODO:
 *  - Improve errors by adding support for more codes.
 *  - Add function to generate temporary names (e.g. mktemp() call)?
 */   

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

#if MK_WIN32
    #include <windows.h>
#endif

#if MK_WIN32
    #define _MK_FILE_T HANDLE
    #define _MK_FILE_INV INVALID_HANDLE_VALUE
#else
    #define _MK_FILE_T MK_S32
    #define _MK_FILE_INV -1
#endif

struct _MK_File
{
    _MK_FILE_T Hdl;
};

#define _MK_FILE_INITVAL {_MK_FILE_INV}

/******************************************************************************/

MK_INLINE MK_File MK_File_InitVal(void)
{
    MK_File lFile = MK_FILE_INITVAL;
    return lFile;
}

MK_INLINE void MK_File_Init(MK_File* aFile)
{
    MK_ASSERT(NULL != aFile);
    *aFile = MK_File_InitVal();
}

MK_INLINE void MK_File_Swap(MK_File* aFile1, MK_File* aFile2)
{
    MK_File lTmp;
    MK_ASSERT(NULL != aFile1 && NULL != aFile2);
    lTmp = *aFile1;
    *aFile1 = *aFile2;
    *aFile2 = lTmp;
}

MK_INLINE void MK_File_Destroy(MK_File* aFile)
{
    MK_File_Close(aFile);
}

/******************************************************************************/

MK_INLINE MK_Bool MK_File_IsValid(MK_File* aFile)
{
    return (NULL != aFile && _MK_FILE_INV != aFile->Hdl) ? MK_TRUE : MK_FALSE;
}

MK_INLINE MK_S64 MK_File_Avail(MK_File* aFile)
{
    MK_S64 lSz, lPos;
    if (0 > (lSz = MK_File_Size(aFile)))
    {
        return lSz;
    }
    if (0 > (lPos = MK_File_Pos(aFile)))
    {
        return lPos;
    }
    return lSz - lPos;
}

MK_INLINE MK_Bool MK_File_PendingEOF(MK_File* aFile)
{
    return (MK_File_IsValid(aFile) && MK_File_IsReadable(aFile) && 0 == MK_File_Avail(aFile)) ? MK_TRUE : MK_FALSE;
}

/******************************************************************************/

MK_INLINE MK_S64 MK_File_Pos(MK_File* aFile)
{
    return MK_File_Seek(aFile, 0, kFileCur);
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
