/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010        *
*									                                    *
************************************************************************/
/************************************************************************
File:		voAPETag.h

Contains:	APE ID3 Tag Parser Header file

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

*************************************************************************/

#ifndef  __VOAPETAG_H__
#define  __VOAPETAG_H__

/**************************************************************************
ID3 v1.1 tag
***************************************************************************/
#define ID3_TAG_BYTES    128

typedef struct
{
	char Header[3];             // should equal 'TAG'
	char Title[30];             // title
	char Artist[30];            // artist
	char Album[30];             // album
	char Year[4];               // year
	char Comment[29];           // comment
	unsigned char Track;        // track
	unsigned char Genre;        // genre
}VO_ID3_TAG;

#define APE_TAG_FOOTER_BYTES    32

typedef struct
{
	char m_cID[8];              // should equal 'APETAGEX'    
	int m_nVersion;             // equals CURRENT_APE_TAG_VERSION
	int m_nSize;                // the complete size of the tag, including this footer (excludes header)
	int m_nFields;              // the number of fields in the tag
	int m_nFlags;               // the tag flags
	char m_cReserved[8];        // reserved for later use (must be zero)
}VO_APE_TAG;

#endif    //__VOAPETAG_H__

