	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CCMMCRTPLiveSrc.cpp

	Contains:	CCMMCRTPLiveSrc source file

	Written by: 	Thomas Liang

	Change History (most recent first):
	2011-03-10		Thomas Liang			Create file

*******************************************************************************/
#ifndef __CCMMCRTPLIVESRC_H__
#define __CCMMCRTPLIVESRC_H__
#include "CCMCCRTPSrc.h"

#ifdef LINUX
#include "netinet/in.h"
#endif 

/** fec ext header */
typedef struct tag_fec_ext_header
{
	/** pack base */
	unsigned int pack_base;
	/** pack index */
	unsigned short pack_index;
	/** factor_k */
	unsigned char factor_k;
	/** factor_n */
	unsigned char factor_n;
} FEC_EXT_HEADER, *PFEC_EXT_HEADER;

class CCMCCRTPLiveSrc:public CCMCCRTPSrc
{
public:
	CCMCCRTPLiveSrc();
	~CCMCCRTPLiveSrc();

	void		Reset();
	
	int			Start();
	int			Stop();

	void 		MainLoop();
	void 		ReceiveLoop();

	int			CreateSocket();
	void		CloseSocket();

	int			Select(unsigned short interval);

private:
	void		HandleRTPInFEC(MEM_NODE *node);
	void		SavePackByIndex(PFEC_EXT_HEADER header, MEM_NODE *node);

private:

	struct sockaddr_in 		m_sockAddr;
#ifdef LINUX
	int 					m_sockFD;
#elif defined WIN32
	SOCKET 					m_sockFD;
#endif //LINUX

	int						m_nMode;

	void *					m_hCodec;

	unsigned long			m_sessionID;
	unsigned short			m_packetID;

	unsigned char			m_factorK;
	unsigned char			m_factorN;

	unsigned char *			m_pktArray;
	int						m_bFirstPkt;

	NODE					m_invalidList;

};

#endif //__CCMMCRTPSRC_H__

