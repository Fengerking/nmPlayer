#pragma once
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2007				*
*																		*
************************************************************************/
/*******************************************************************************
File:		voDLNA_Param.h

Contains:	handle DLNA CTT test Parameters set/get 

Written by:	Stony

Change History (most recent first):
2012-03-30		Stony		Create file

*******************************************************************************/

#ifndef __voDLNA_PARAMETERS_H__
#define __voDLNA_PARAMETERS_H__

#include "voString.h"

class voDLNA_Param
{
public:
	voDLNA_Param(void);
	~voDLNA_Param(void);

	VO_VOID set_dlna(VO_BOOL dlna = VO_TRUE);
	VO_VOID set_byterange_support(VO_U32 support = 1);
	VO_VOID set_connection_stalling_on_play(VO_U32 stalling = 0);
	VO_VOID set_connection_stalling_on_pause(VO_U32 stalling = 1);

	VO_VOID set_connection_timeout( VO_U32 timeout );
	VO_VOID set_read_timeout(VO_U32 timeout);
	VO_U32  get_connection_timeout();
	VO_U32  get_read_timeout();

	VO_BOOL is_dlna();
	VO_BOOL is_byterange_support();
	VO_BOOL is_connection_stalling_on_pause();
	VO_BOOL is_connection_stalling_on_play();

	VO_BOOL is_range_limit();

	VO_VOID reset();

	VO_VOID append_reqitem(VO_CHAR* pitem);

	VO_CHAR* get_reqstr();

	VO_VOID set_server_response(VO_CHAR* presponse);

	VO_CHAR* get_server_response();

private:
	VO_BOOL	m_is_dlna;

	VO_BOOL m_is_connection_stalling_on_play;
	VO_BOOL m_is_connection_stalling_on_pause;

	VO_BOOL m_is_byterange_supported;

	//unit :micro seconds
	VO_U32	m_connection_timeout;
	//unit :micro seconds
	VO_U32	m_read_timeout;

	VO_CHAR m_request_append[2048];

	VO_CHAR m_server_response[4096];
};

#endif
