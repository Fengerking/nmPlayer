	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CCMMCRTPLocalSrc.h

	Contains:	CCMMCRTPLocalSrc header file

	Written by: 	Thomas Liang

	Change History (most recent first):
	2011-03-09		Thomas Liang			Create file

*******************************************************************************/
#ifndef __CCMMCRTPLOCALSRC_H__
#define __CCMMCRTPLOCALSRC_H__

#include "CCMCCRTPSrc.h"
//#include "pcap.h"

class CCMCCRTPLocalSrc:public CCMCCRTPSrc
{
public:
	CCMCCRTPLocalSrc();
	~CCMCCRTPLocalSrc();
	
	void loop();
private:
	
private:
	//struct pcap_pkthdr *m_pFileHeader;
//	pcap_t *			m_pFp;
};
#endif //__CCMMCRTPLOCALSRC_H__
