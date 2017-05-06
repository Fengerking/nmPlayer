	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		QcpFileDataStruct.h

Contains:	Data Structure Of QCP file

Written by:	East

Reference:	RFC 3625 - The QCP File Format and Media Types for Speech Data.doc

Change History (most recent first):
2009-06-26		East			Create file

*******************************************************************************/
#ifndef __Qcp_File_Data_Struct
#define __Qcp_File_Data_Struct

#include "RiffFileDataStruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/*
<QLCM-form>
RIFF( 'QLCM'
<format chunk 'fmt'>						// required
[<variable rate chunk - 'vrat'>]			// required
[<label chunk 'labl'>]						// optional
[<offsets in increments chunk - 'offs'>]	// optional
<codec packet data chunk - 'data'>			// required
[<configuration chunk - 'cnfg'>]			// optional
[<text chunk - 'text'>]						// optional
*/
#define FOURCC_QLCM					MAKEFOURCC('Q', 'L', 'C', 'M')
#define FOURCC_vrat					MAKEFOURCC('v', 'r', 'a', 't')	//[required]
#define FOURCC_labl					MAKEFOURCC('l', 'a', 'b', 'l')
#define FOURCC_offs					MAKEFOURCC('o', 'f', 'f', 's')
#define FOURCC_cnfg					MAKEFOURCC('c', 'n', 'f', 'g')
#define FOURCC_text					MAKEFOURCC('t', 'e', 'x', 't')

//QCP format
//after 'fmt ' chunk
#define QCP_FORMAT_SKIP_SIZE		40
typedef struct tagQcpFormat
{
	VO_U8		btMajorVersion;		// major version of this format
	VO_U8		btMinorVersion;		// minor version of this format
	VO_GUID		guidCodec;			// unique ID of this codec
	VO_U16		wCodecVersion;		// version of the codec
	VO_BYTE		btCodecName[80];	// proper name of the codec
	VO_U16		wAvgBitsPerSec;		// avg bps of the codec
	VO_U16		wMaxPacketSize;		// size of a packet in bytes (max rate), including header byte for that packet
	VO_U16		wSamplesPerBlock;	// size of a block in samples, encoder encodes one block into one packet, decoder decodes one packet into one block
	VO_U16		wSamplesPerSec;		// sps of input needed for the encoder and output from the decoder
	VO_U16		wBitsPerSample;		// bits per sample of the above contains rate header format info
	/*
	// contains rate header format info
	VO_U32		dwRates;			// of rates high byte = rate, low byte = size of following packet
	VO_U16		wPacketSize[8];
	VO_U32		dwReserved[5];		// reserved
	*/
} QcpFormat, *PQcpFormat;

#define QCP_PACKETDATA_FIXEDSIZE	0
typedef struct tagQcpVariableRate
{
	VO_U32		dwVariableRate;		// if ( == 0 ) packet data is fixed rate, else if ( < 0xFFFF0000 ) variable rate
	VO_U32		dwPacketNum;		// the data chunk size in packets
} QcpVariableRate, *PQcpVariableRate;

#define QCP_SAMPLETIME				20	//20<ms/sample>

#define DECLARE_USE_QCP_GLOBAL_VARIABLE\
	static const VO_U16	s_wQCPPacketSizes[16];\
	static const VO_U16	s_wEVCPacketSizes[16];

#define DEFINE_USE_QCP_GLOBAL_VARIABLE(cls)\
const VO_U16 cls::s_wQCPPacketSizes[16] = {1, 4, 8, 17, 35, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};\
const VO_U16 cls::s_wEVCPacketSizes[16] = {0, 3, 0, 11, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#ifdef _VONAMESPACE
}
#endif

#endif	//__Qcp_File_Data_Struct
