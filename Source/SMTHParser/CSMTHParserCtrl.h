
#ifndef _CSMTHPARSERCTRL_H_
#define  _CSMTHPARSERCTRL_H_
#ifdef _WIN32
	#include "stdintx.h"
#else
//	#include <stdint.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "voParser.h"
#include "voSmthBase.h"
#include "smth-def.h"
#include "voDrmCallback.h"
#include "voStreamingDRM.h"
#include "voSource2.h"

#ifdef WIN32
#include   <windows.h> 
#else
#pragma pack (4)
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


typedef struct 
{
	VO_BYTE	 *pcInitializationVector;
	VO_U16    nNumberOfEntries;
	ENTRIES_ST *pEntries;
}SAMPLE_ST;
typedef struct 
{
	VO_U32	 nIV_Size;
	VO_U32	 nSampleCount;
	SAMPLE_ST *pSample;
}IV_STURCT;
class CSMTHParserCtrl
{
public:
	CSMTHParserCtrl();
	~CSMTHParserCtrl();
	
	VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	VO_VOID	IssueParseResult(VO_U32 nType, VO_VOID* pValue, VO_VOID* pReserved=VO_NULL);

	VO_U32 Open(PARSERPROC pProc, VO_VOID* pUserData);
	VO_U32 Close();

	VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	
private:
	VO_S32 ToParser( VO_PBYTE pBuf,VO_U64 bufsize,VO_U32 inputtype);
	VO_S32 ParserFirstLevel(VO_SMTH_BOX *box);
	VO_S32 ParserSecondLevel(VO_SMTH_BOX *box);
	VO_S32 ParserThirdLevel(VO_SMTH_BOX *box);

	 VO_VOID ToRelease(VO_SMTH_FRAGMENT_INFO *f);

	 VO_S32 GetBoxType(VO_SMTH_BOX* box);
	 VO_S32 GetBoxValue( VO_U8 *pValue,VO_U32 size= sizeof(VO_U8));
	 VO_S32 GetBoxValue( VO_U16 *pValue,VO_U32 size= sizeof(VO_U16));
	 VO_S32 GetBoxValue( VO_U32 *pValue,VO_U32 size= sizeof(VO_U32));
	 VO_S32 GetBoxValue( VO_U64 *pValue,VO_U32 size= sizeof(VO_U64));
	 VO_S32 GetBoxBuf(VO_VOID *pValue, VO_U32 size = sizeof(VO_U32));

	
	//1
	 VO_S32 ParseBox_MOOF(VO_SMTH_BOX* box);
	 VO_S32 ParseBox_MDAT(VO_SMTH_BOX* box);
	 //2
	 VO_S32 ParseBox_MFHD(VO_SMTH_BOX* box);
	 VO_S32 ParseBox_TRAF(VO_SMTH_BOX* box);
	 //3
	 VO_S32 ParseBox_TFHD(VO_SMTH_BOX* box);
	 VO_S32 ParseBox_TRUN(VO_SMTH_BOX* box);
	 VO_S32 ParseBox_SDTP(VO_SMTH_BOX* box);
	 VO_S32 ParseBox_UUID(VO_SMTH_BOX* box);
	
	 VO_S32  CheckBoxFlag(VO_SMTH_BOX* box,VO_U64 &setting, VO_U32 flag,VO_U64 id);
	 VO_S32  CheckBoxFlag(VO_SMTH_BOX* box,VO_U32 &setting, VO_U32 flag,VO_U64 id);
	 VO_VOID CheckFor4B(VO_S64 &boxsize,VO_SMTH_BOX *root);
private:

	VO_SMTH_FRAGMENT m_Fragment;		  /** fragment buf */
	
	PARSERPROC	m_pProc;
	VO_VOID*	m_pUserData;
	VO_U64		m_u64Starttime;
	VO_U32		m_u32StreamID;
	VO_BOOL     m_bProcDownloadEnd;


	//piff
	CvoDrmCallback  *m_pDrmcallback;

	//new drm
//	VO_StreamingDRM_API *m_pNewDrmApi;
	VO_DATASOURCE_FILEPARSER_DRMCB m_fpDrmPtr;

	VOSMTH_DRM_TYPE	m_drmCustomerType;
//	VO_PBYTE m_pInitializationVector;
//	VO_U32      m_nIV_size ;

	IV_STURCT *m_pIVSturct;
private:

	VO_PBYTE m_pSampleEncyptionBox;
	VO_U32	 m_uSampleEncyptionSize;

	VO_U32   m_uTrackID;
};
#ifdef _VONAMESPACE
}
#endif

#endif
