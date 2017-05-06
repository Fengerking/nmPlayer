	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXPipe.h

	Contains:	file operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voOMXPipe_H__
#define __voOMXPipe_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <OMX_Types.h>
#include <OMX_ContentPipe.h>

/** Open a content stream for reading or writing. */ 
CPresult voOMXPipeOpen (CPhandle* hContent, CPstring szURI, CP_ACCESSTYPE eAccess );

/** Close a content stream. */ 
CPresult voOMXPipeClose (CPhandle hContent );

/** Create a content source and open it for writing. */ 
CPresult voOMXPipeCreate (CPhandle *hContent, CPstring szURI );

/** Check the that specified number of bytes are available for reading or writing (depending on access type).*/
CPresult voOMXPipeCheckAvailableBytes (CPhandle hContent, CPuint nBytesRequested, CP_CHECKBYTESRESULTTYPE *eResult );

/** Seek to certain position in the content relative to the specified origin. */
CPresult voOMXPipeSetPosition (CPhandle  hContent, CPint nOffset, CP_ORIGINTYPE eOrigin);

/** Retrieve the current position relative to the start of the content. */
CPresult voOMXPipeGetPosition (CPhandle hContent, CPuint *pPosition);

/** Retrieve data of the specified size from the content stream (advance content pointer by size of data).
Note: pipe client provides pointer. This function is appropriate for small high frequency reads. */
CPresult voOMXPipeRead (CPhandle hContent, CPbyte *pData, CPuint nSize); 

/** Retrieve a buffer allocated by the pipe that contains the requested number of bytes. 
Buffer contains the next block of bytes, as specified by nSize, of the content. nSize also
returns the size of the block actually read. Content pointer advances the by the returned size. 
Note: pipe provides pointer. This function is appropriate for large reads. The client must call 
ReleaseReadBuffer when done with buffer. 

In some cases the requested block may not reside in contiguous memory within the
pipe implementation. For instance if the pipe leverages a circular buffer then the requested 
block may straddle the boundary of the circular buffer. By default a pipe implementation 
performs a copy in this case to provide the block to the pipe client in one contiguous buffer.
If, however, the client sets bForbidCopy, then the pipe returns only those bytes preceding the memory 
boundary. Here the client may retrieve the data in segments over successive calls. */
CPresult voOMXPipeReadBuffer (CPhandle hContent, CPbyte **ppBuffer, CPuint *nSize, CPbool bForbidCopy);

/** Release a buffer obtained by ReadBuffer back to the pipe. */
CPresult voOMXPipeReleaseReadBuffer (CPhandle hContent, CPbyte *pBuffer);

/** Write data of the specified size to the content (advance content pointer by size of data).
Note: pipe client provides pointer. This function is appropriate for small high frequency writes. */
CPresult voOMXPipeWrite (CPhandle hContent, CPbyte *data, CPuint nSize); 

/** Retrieve a buffer allocated by the pipe used to write data to the content. 
Client will fill buffer with output data. Note: pipe provides pointer. This function is appropriate
for large writes. The client must call WriteBuffer when done it has filled the buffer with data.*/
CPresult voOMXPipeGetWriteBuffer (CPhandle hContent, CPbyte **ppBuffer, CPuint nSize);

/** Deliver a buffer obtained via GetWriteBuffer to the pipe. Pipe will write the 
the contents of the buffer to content and advance content pointer by the size of the buffer */
CPresult voOMXPipeWriteBuffer (CPhandle hContent, CPbyte *pBuffer, CPuint nFilledSize);

/** Register a per-handle client callback with the content pipe. */
CPresult voOMXPipeRegisterCallback (CPhandle hContent, CPresult (*ClientCallback)(CP_EVENTTYPE eEvent, CPuint iParam));

CPresult voOMXPipeFillPointer (CP_PIPETYPE * pPipe);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voOMXPipe_H__
