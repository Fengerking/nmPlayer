/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: RA_tni_priv.h,v 1.1 2005/02/18 23:30:39 hfrederickson Exp $ 
 * 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM   
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc.   
 * All Rights Reserved.   
 *   
 * The contents of this file, and the files included with this file, 
 * are subject to the current version of the RealNetworks Community 
 * Source License (the "RCSL"), including Attachment G and any 
 * applicable attachments, all available at 
 * http://www.helixcommunity.org/content/rcsl.  You may also obtain 
 * the license terms directly from RealNetworks.  You may not use this 
 * file except in compliance with the RCSL and its Attachments. There 
 * are no redistribution rights for the source code of this 
 * file. Please see the applicable RCSL for the rights, obligations 
 * and limitations governing use of the contents of the file. 
 *   
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the 
 * portions it created. 
 *   
 * This file, and the files included with this file, is distributed 
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS 
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET 
 * ENJOYMENT OR NON-INFRINGEMENT. 
 *   
 * Technology Compatibility Kit Test Suite(s) Location:   
 * https://rarvcode-tck.helixcommunity.org   
 *   
 * Contributor(s):   
 *   
 * ***** END LICENSE BLOCK ***** */   
 
/*
 *  ======== RA_tni_priv.h ========
 *  Internal vendor specific (TNI) interface header for Real Audio
 *  algorithm. Only the implementation source files include
 *  this header; this header is not shipped as part of the
 *  algorithm.
 *
 *  This header contains declarations that are specific to
 *  this implementation and which do not need to be exposed
 *  in order for an application to use the Real Audio algorithm.
 */
#ifndef _RA_TNI_PRVT_
#define _RA_TNI_PRVT_

#include <std.h>
#include <ialg.h>
#include <iRA.h>  
 
#if defined WORKING_BUF_DMA && !defined CONST_TABLE_DMA
#define CONST_TABLE_DMA
#endif
 
 
#ifdef CONST_TABLE_DMA
#include <idma2.h>
#endif


/* gain control - no. of attacks */
#define MAXNATS		8

#define MAXCHAN  2
#define MAXFBITS 2240
#define MAXNSAMP 1024

typedef unsigned char	UCHAR;
typedef unsigned short	USHORT;
/* typedef unsigned int	ULONG; */
typedef unsigned long	ULONG;

//-----------------------------------------------------------------------------------------
//not using DMA
#ifndef CONST_TABLE_DMA 
#define NUM_MEMORY_BLOCKS  6						// number of memory buffers 
#endif												// returned by algNumAlloc

// using DMA
#ifdef  CONST_TABLE_DMA
#define NUM_MEMORY_BLOCKS  7						// number of memory buffers 
													// returned by algNumAlloc

#define NUM_DMA_CHANNELS 1							// channels required for CONST_TABLE_DMA 
													// without WORKING_BUF_DMA
#ifdef  WORKING_BUF_DMA
#undef  NUM_DMA_CHANNELS
#define NUM_DMA_CHANNELS 4							// channels required for CONST_TABLE_DMA 
													// with WORKING_BUF_DMA
#endif

#endif
//-----------------------------------------------------------------------------------------




//-----------------------------------------------------------------------------------------
/* gain control info */
typedef	struct {
	short		nats;				/* number of attacks */
	USHORT		loc[MAXNATS];		/* location of attack */
	short		gain[MAXNATS];		/* gain code */
} GAINC;
            

//-----------------------------------------------------------------------------------------
//	===== RA_TNI_Obj ======
//
// Definition of algorithm instance object type
//
typedef struct {

	IALG_Obj ialg ;		// The parent IALG object. 
						// Must be first field of algorithm instance object

	long *decpcm; 	// must be 
					// set up to a buffer
					// size [2*MAXNSAMP] (max)
					// Or done dynamically (depending upon no. of
					// samples in stream)
					// NOTE : The buffer must be aligned such that
					// lg2(2*MAXNSAMP*2) lsbs of the address are 0 
					// 
					// DEBUG -> I think alignment of 2*MAXNSAMP(words) will do. Please verify !!
					//								

	short nsamples;
	short nchannels;
	short nregions;
	short nframebits;
	unsigned short samprate;
	short cplstart;
	short cplqbits;

	/* used in Decode --Venky */
	short oldlost;					/* delayed loss flag */
    
	/* bitpacking globals */
	ULONG *pkptr;					/* initialized to input code buffer */

#ifdef NOPACK
	ULONG pkbuf[(MAXFBITS+31)/32];		// NOT used : NOPACK is NOT defined !!
#endif

	
	
	
	/* used in coder.c -- Venky */
	GAINC dgainc[MAXCHAN][3];				/* one extra for lookahead */
	

	long *decmlt[MAXCHAN];				/* pointers to 2 mlt buffers in internal memory*/
								// These 2 pointers must be initialized to 2 arrays to longs 
								// size of each array	= 1024 (longs)
								// Block allocate 1 buffer of 2048*2 (words). algInit will do the rest
										
	long *overlap[MAXCHAN]	;	/* overlap delay for decode */
								// These 2 pointers must be initialized to 2 arrays to longs 
								// size 	= number of Samples for the stream
								// In case WORKING_BUF_DMA is used, these two ptrs pt to the 
								// DARAM temp. buffers in which they are paged from ext. mem.
								// They both pt. to the start of the working buffer and use the first 2k of the 4kw buffer
								
								
	long *buf;			/* pointer to temporary buffer used by IMLT */
						// Must be initialized to an array of longs, 
						// Length of array = number of samples per frame of stream
						// In case working buffer DMA is used, it pts to the working buffer in DARAM + 2048 words 
						//	i.e it occupies the last 2K of the 4K buffer 
						
	int numCalls;							/* DEBUG!!! -- Venky */
	
	short dualmono ; // dual mono flag 	
	
	long _lfsr	;			/* well-chosen seed for scalar dequant */                   

#ifdef CONST_TABLE_DMA
	// Pointers to constant tables
	long const *fixwindow_tab ;			// pointer to RA_TNI_fixwindow_tab in DARAM       
	long const *fixwindow_tab_extmem ;	// pointer to RA_TNI_fixwindow_tab in extmem
	long fixwindowsize ;					// size of the table 		
	
	long const *fixcos4_sin4tab_extmem;		// ptr to RA_TNI_fixcos4tab/RA_TNI_fixsin4tab  in extmem
	long fixcos4sin4size ;					// size of the 2 tables (combined)	
	long const *fixcos4tab	;			// ptr to RA_TNI_fixcos4tab  in DARAM       
    long const *fixsin4tab ; 			// ptr to RA_TNI_fixsin4tab  in DARAM       
                                                                   
	long const *fixcos1tab;				// ptr to RA_TNI_fixcos1tab in DARAM       
    long const *fixcos1tab_extmem;		// ptr to RA_TNI_fixcos1tab in extmem
	long fixcos1size ;					// size of the tables 
    
	long const *twidtab ;				// ptr to RA_TNI_twidtab in DARAM         
	long const *twidtab_extmem ;		// ptr to RA_TNI_twidtab in extmem  
	long twidtabsize ;					// size of the table 
	
	void const *sqvh_tab[7] ; 			// array of ptrs to  
										// const HuffmanTableNode RA_TNI_sqvh_table_x[] ;  
										
	// The DMA channel handle										
	IDMA2_Handle dmaChannel_0	;		// DMA channel 0

#ifdef WORKING_BUF_DMA   
	// Pointers to buffers in ext mem.
	
	long *decmlt_extmem[MAXCHAN];		/* pointers to 2 mlt buffers in EXTMEM*/
	
	long *overlap_extmem[MAXCHAN];	 // ptr to overlap buffers in extmem
	
	IDMA2_Handle dmaChannel_1	;		// DMA channel 1
	IDMA2_Handle dmaChannel_2	;		// DMA channel 2     
 	IDMA2_Handle dmaChannel_3	;		// DMA channel 3     

#endif // WORKING_BUF_DMA
																																							
	int currChannel;					// current channel (0 or 1)         
	int rdAgainFlg;						// flag indicating that one more channel is yet to be processed

#endif //CONST_TABLE_DMA   
    
    unsigned int odd_even_flag;
                                                          
} RA_TNI_Obj;

//-----------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------
// Prototypes of functions exported by  RA_TI algorithm thru the IRA and IALG interfaces
//-------------------------------------------------------------------------------------------------------------
extern Int  RA_TNI_algInit	(IALG_Handle Handle, const IALG_MemRec *memTab, IALG_Handle parent, const IALG_Params *params);
extern Int	RA_TNI_algNumAlloc (void);
extern Int	RA_TNI_algAlloc	(const IALG_Params * Params, IALG_Fxns ** fxns, IALG_MemRec * memTab);
extern Int  RA_TNI_algFree 	(IALG_Handle Handle, IALG_MemRec * mem);
extern Void RA_TNI_algMoved(IALG_Handle Handle,const IALG_MemRec *memTab, IALG_Handle parent, const IALG_Params *params) ;

extern Void RA_TNI_getStatus (IRA_Handle Handle, IRA_Status *status);
extern Void RA_TNI_setStatus (IRA_Handle Handle, const IRA_Status *status);
extern Void RA_TNI_reset (IRA_Handle Handle);
extern Int	RA_TNI_findSync 	(IRA_Handle Handle, UCHAR *in);
extern Int	RA_TNI_decode(IRA_Handle Handle, UCHAR *inbuf, Int *outbuf, Int *lostflag) ;


#ifdef CONST_TABLE_DMA 
extern Void RA_TNI_dmaChangeChannels(IALG_Handle handle , IDMA2_ChannelRec dmaTab[])  ;
extern Int  RA_TNI_dmaGetChannelCnt(Void);
extern Int  RA_TNI_dmaGetChannels(IALG_Handle handle , IDMA2_ChannelRec dmaTab[])  ;
extern Int  RA_TNI_dmaInit(IALG_Handle handle , IDMA2_ChannelRec dmaTab[])         ;
#endif
//-------------------------------------------------------------------------------------------------------------


#endif 		//_RA_TNI_PRVT_
