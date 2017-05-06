	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		fMarcos.h

Contains:	some macros for file parser define header file

Written by:	East Zhou

Change History (most recent first):
2009-03-30		East		Create file

*******************************************************************************/
#ifndef __File_Marcos_H__
#define __File_Marcos_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define VO_NULL			0

#define VO_MAXS8		0x7F
#define VO_MAXU8		0xFF
#define VO_MAXS16		0x7FFF
#define VO_MAXU16		0xFFFF
#define VO_MAXS32		0x7FFFFFFF
#define VO_MAXU32		0xFFFFFFFF
#define VO_MAXS64		0x7FFFFFFFFFFFFFFFLL
#define VO_MAXU64		0xFFFFFFFFFFFFFFFFLL

#define VO_MAX_PATH		260

#define VO_MIN(v1, v2) ((v1 < v2) ? v1 : v2)
#define VO_MAX(v1, v2) ((v1 > v2) ? v1 : v2)

#define VO_ABS(v) ((v < 0) ? -v : v);

#ifndef MAKETWOCC
#define MAKETWOCC(ch0, ch1)	((VO_U16)(VO_U8)(ch0) | ((VO_U16)(VO_U8)(ch1) << 8))
#endif	//MAKETWOCC

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)									\
                ((VO_U32)(VO_U8)(ch0) | ((VO_U32)(VO_U8)(ch1) << 8) |   \
                ((VO_U32)(VO_U8)(ch2) << 16) | ((VO_U32)(VO_U8)(ch3) << 24 ))
#endif	//MAKEFOURCC


#define VO_EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8)  \
	extern VO_GUID itf = {l1,s1,s2,{c1,c2,c3,c4,c5,c6,c7,c8}}

#define VO_DEFINE_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8)  \
	VO_GUID itf = {l1,s1,s2,{c1,c2,c3,c4,c5,c6,c7,c8}}

#define voIsEqualGUID(rguid1, rguid2) (!MemCompare(&rguid1, &rguid2, sizeof(VO_GUID)))
//////////////////////////////////////////////////////////////////////////
//safe free macros
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)\
if(x)\
{\
	x->Release();\
	x = 0;\
}
#endif	//SAFE_RELEASE

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)\
if(x)\
{\
	delete x;\
	x = 0;\
}
#endif	//SAFE_DELETE

#ifndef SAFE_ARRAYDELETE
#define SAFE_ARRAYDELETE(x)\
if(x)\
{\
	delete [] x;\
	x = 0;\
}
#endif	//SAFE_ARRAYDELETE

#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE(h)\
if(h)\
{\
	CloseHandle( h );\
	h = 0;\
}
#endif	//SAFE_CLOSEHANDLE

#ifndef SAFE_SYSFREESTRING
#define SAFE_SYSFREESTRING(x)\
if(x)\
{\
	SysFreeString(x);\
	x = 0;\
}
#endif	//SAFE_SYSFREESTRING

#ifndef SAFE_VIRTUALFREE
#define SAFE_VIRTUALFREE(x)\
if(x)\
{\
	VirtualFree(x, 0, MEM_RELEASE);\
	x = 0;\
}
#endif	//SAFE_VIRTUALFREE

#ifndef SAFE_DELETE_CHAIN
#define SAFE_DELETE_CHAIN(s, phead)\
if(phead)\
{\
	s* pCur = phead;\
	s* pDel;\
	while(pCur)\
	{\
		pDel = pCur;\
		pCur = pCur->next;\
		delete pDel;\
	}\
	phead = 0;\
}
#endif	//SAFE_DELETE_CHAIN

#ifndef SAFE_RELEASE_CHAIN
#define SAFE_RELEASE_CHAIN(s, phead)\
if(phead)\
{\
	s* pCur = phead;\
	s* pDel;\
	while(pCur)\
	{\
		pDel = pCur;\
		pCur = pCur->next;\
		MemFree(pDel);\
	}\
	phead = 0;\
}
#endif	//SAFE_RELEASE_CHAIN

#ifndef SAFE_MEM_FREE
#define SAFE_MEM_FREE(x)\
if(x)\
{\
	MemFree(x);\
	x = 0;\
}
#endif	//SAFE_MEM_FREE

//end of safe free macros
//////////////////////////////////////////////////////////////////////////

#ifndef ADD_TO_CHAIN
#define	ADD_TO_CHAIN(s, phead, pnew)\
{\
	if(!phead)\
		phead = pnew;\
	else\
	{\
		s* ptmp = phead;\
		while(ptmp->next)\
			ptmp = ptmp->next;\
		ptmp->next = pnew;\
	}\
}
#endif	//ADD_TO_CHAIN

#ifndef NEW_OBJ
#define	NEW_OBJ(s)		(s*)MemAlloc(sizeof(s))
#endif	//NEW_OBJ

#ifndef NEW_OBJS
#define	NEW_OBJS(s, c)	(s*)MemAlloc(c * sizeof(s))
#endif	//NEW_OBJS

#ifndef	NEW_BUFFER
#define	NEW_BUFFER(c)	(VO_PBYTE)MemAlloc(c)
#endif	//NEW_BUFFER

//////////////////////////////////////////////////////////////////////////
//macros for CGFileChunk
//You must use m_chunk(CGFileChunk) as class member variable
//If you want to use m_pFileChunk as class member variable, please use ptr_XXX
#define read_pointer(p, l)\
{\
	if(!m_chunk.FRead(p, l))\
		return VO_FALSE;\
}

#define ptr_read_pointer(p, l)\
{\
	if(!m_pFileChunk->FRead(p, l))\
		return VO_FALSE;\
}

#define skip(l)\
{\
	if(!m_chunk.FSkip(l))\
		return VO_FALSE;\
}

#define ptr_skip(l)\
{\
	if(!m_pFileChunk->FSkip(l))\
		return VO_FALSE;\
}

//////////////////////////////////////////////////////////////////////////
//Big Endian
//when you use following macro of Big Endian, please must write use_big_endian_read
//else you will meet compiler error
#define use_big_endian_read		VO_PBYTE tp;

#define read_word_b(w)\
{\
	tp = (VO_PBYTE)&w;\
	read_pointer(tp + 1, 1);\
	read_pointer(tp, 1);\
}

#define ptr_read_word_b(w)\
{\
	tp = (VO_PBYTE)&w;\
	ptr_read_pointer(tp + 1, 1);\
	ptr_read_pointer(tp, 1);\
}

#define read_3byte_dword_b(d)\
{\
	tp = (VO_PBYTE)&d;\
	tp[3] = 0;\
	read_pointer(tp + 2, 1);\
	read_pointer(tp + 1, 1);\
	read_pointer(tp, 1);\
}

#define ptr_read_3byte_dword_b(d)\
{\
	tp = (VO_PBYTE)&d;\
	tp[3] = 0;\
	ptr_read_pointer(tp + 2, 1);\
	ptr_read_pointer(tp + 1, 1);\
	ptr_read_pointer(tp, 1);\
}

#define read_dword_b(d)\
{\
	tp = (VO_PBYTE)&d;\
	read_pointer(tp + 3, 1);\
	read_pointer(tp + 2, 1);\
	read_pointer(tp + 1, 1);\
	read_pointer(tp, 1);\
}

#define ptr_read_dword_b(d)\
{\
	tp = (VO_PBYTE)&d;\
	ptr_read_pointer(tp + 3, 1);\
	ptr_read_pointer(tp + 2, 1);\
	ptr_read_pointer(tp + 1, 1);\
	ptr_read_pointer(tp, 1);\
}

#define read_qword_b(q)\
{\
	tp = (VO_PBYTE)&q;\
	read_pointer(tp + 7, 1);\
	read_pointer(tp + 6, 1);\
	read_pointer(tp + 5, 1);\
	read_pointer(tp + 4, 1);\
	read_pointer(tp + 3, 1);\
	read_pointer(tp + 2, 1);\
	read_pointer(tp + 1, 1);\
	read_pointer(tp, 1);\
}

#define ptr_read_qword_b(q)\
{\
	tp = (VO_PBYTE)&q;\
	ptr_read_pointer(tp + 7, 1);\
	ptr_read_pointer(tp + 6, 1);\
	ptr_read_pointer(tp + 5, 1);\
	ptr_read_pointer(tp + 4, 1);\
	ptr_read_pointer(tp + 3, 1);\
	ptr_read_pointer(tp + 2, 1);\
	ptr_read_pointer(tp + 1, 1);\
	ptr_read_pointer(tp, 1);\
}

//End of Big Endian
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Little Endian
#define read_word_l(w)				read_pointer(&w, 2)

#define ptr_read_word_l(w)			ptr_read_pointer(&w, 2)

#define read_3byte_dword_l(d)		read_pointer(&d, 3) 

#define ptr_read_3byte_dword_l(d)	ptr_read_pointer(&d, 3)

#define read_dword_l(d)				read_pointer(&d, 4)

#define ptr_read_dword_l(d)			ptr_read_pointer(&d, 4)

#define read_qword_l(q)				read_pointer(&q, 8)

#define ptr_read_qword_l(q)			ptr_read_pointer(&q, 8)
//End of Little Endian
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Common
#define read_byte(b)			read_pointer(&b, 1)

#define ptr_read_byte(b)		ptr_read_pointer(&b, 1)

#ifdef VO_FILE_USE_BIG_ENDIAN
//(ISOBaseMedia, REAL, MPEG, FLV, MP3, AAC, FLAC, ...)
#define read_word(w)			read_word_b(w)

#define ptr_read_word(w)		ptr_read_word_b(w)

#define read_3byte_dword(d)		read_3byte_dword_b(d)

#define ptr_read_3byte_dword(d)	ptr_read_3byte_dword_b(d)

#define read_dword(d)			read_dword_b(d)

#define ptr_read_dword(d)		ptr_read_dword_b(d)

#define read_qword(q)			read_qword_b(q)

#define ptr_read_qword(q)		ptr_read_qword_b(q)
#else	//VO_FILE_USE_BIG_ENDIAN
//(AVI, ASF, OGG, WAV, QCP, ...)
#define read_word(w)			read_word_l(w)

#define ptr_read_word(w)		ptr_read_word_l(w)

#define read_3byte_dword(d)		read_3byte_dword_l(d)

#define ptr_read_3byte_dword(d)	ptr_read_3byte_dword_l(d)

#define read_dword(d)			read_dword_l(d)

#define ptr_read_dword(d)		ptr_read_dword_l(d)

#define read_qword(q)			read_qword_l(q)

#define ptr_read_qword(q)		ptr_read_qword_l(q)
#endif	//VO_FILE_USE_BIG_ENDIAN

#define read_fcc(f)				read_pointer(&f, 4)

#define ptr_read_fcc(f)			ptr_read_pointer(&f, 4)

#define read_guid(g)			read_pointer(&g, 16)

#define ptr_read_guid(g)		ptr_read_pointer(&g, 16)
//End of Common
//////////////////////////////////////////////////////////////////////////

//end of macros for CGFileChunk
//////////////////////////////////////////////////////////////////////////

#ifdef _VONAMESPACE
}
#endif
#endif	//__File_Marcos_H__
