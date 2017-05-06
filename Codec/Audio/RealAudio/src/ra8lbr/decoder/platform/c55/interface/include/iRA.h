/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: iRA.h,v 1.1 2005/02/18 23:30:41 hfrederickson Exp $ 
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
 *  ======== iRA.h ========
 *  This header defines all types, constants, and functions used in the 
 *  Real Audio decoder algorithm. This is the interface definition file.
 * 
 */
 
#ifndef IRA_
#define IRA_

#include <ialg.h>

/*
 *  ======== IRA_Obj ========
 *  This structure must be the first field of all RA instance objects.
 */
typedef struct IRA_Obj {
    struct IRA_Fxns *fxns;
} IRA_Obj; 

/*
 *  ======== IRA_Handle ========
 *  This handle is used to reference all RA instance objects.
 */
typedef struct IRA_Obj  *IRA_Handle; 


/*
 *  ======== IRA_Params ========
 *  This structure defines the creation parameters for all RA instance
 *  objects.
 */
typedef struct  IRA_Params {
	Int   size;             /* [in] Size of this structure */
	Short nSamples;         /* [in] Samples per frame : 1024, 512 or 256 */
	Short nFrameBits;       /* [in] No.of bits per frame */
	Uns   sampRate;         /* [in] Sampling Rate */
	Short nRegions;         /* [in] No.of regions per frame */
	Short nChannels;        /* [in] No. of channels */
	Int   lossRate;         /* [in] loss rate */
	Short cplStart;         /* [in] start of coupling info */
	Short cplQbits;         /* [in] coupling Q bits */
	Short codingDelay;      /* [out] Coding delay value returned by the algInit method*/
	                        /* RA_TNI decoder module */
} IRA_Params;

extern const IRA_Params IRA_PARAMS;	/* default params - defined in RA_TNI_iRA.c*/


/*
 *  ======== IRA_Status ========
 *  This structure defines the parameters that can be changed at runtime
 *  (read/write), and the instance status parameters (read-only).
 */
typedef struct IRA_Status{
	Int     size;       /* Size of this structure */

	Uns     frameLen;   /* Frame length in number of samples (read-only) */
	Int     fastFwdLen; /* # of error-free input words needed during fast-forward (read-only) */

	Bool    isValid;    /* are next fields valid? (read-only) */
	LgInt   bitRate;    /* Bitstream bit rate in bits per second (read-only) */
	LgInt   sampleRate; /* Bitstream sample rate in Hz (read-only) */
	Int     numChannels;/* 1 or 2 (read-only) */
	Int     frameSize;  /* number of bits in current frame (read-only) */
}IRA_Status;


/*
 *  ======== IRA_Cmd ========
 *  This structure defines the control commands for the RA module. 
 */
typedef  IALG_Cmd IRA_Cmd;


/*
 *  ======== IRA_Fxns ========
 *  This structure defines all of the operations on RA objects.
 *  These functions must be implemented by any module implementing the IRA interface
 *
 *      ialg            -   IALG_Fxns structure that IRA extends
 *      getStatus()     -   get the status of decoder at run-time. 
 *                          May be NULL; NULL => do nothing
 *      setStatus()     -   set the parameters of decoder at run-time.
 *                          May be NULL; NULL => do nothing
 *      reset()         -   reset the state of decoder. May be NULL; NULL => do nothing
 *      findSync()      -   find synchorization info. in the stream.
 *                          May be NULL; NULL => do nothing
 *      decode()        -   decode the stream. Must not be NULL
 *      idma            -   Pointer to IDMA2_Fxns structure
 */
typedef struct IRA_Fxns{
	IALG_Fxns ialg; /* IRA extends IALG */
	Void   (*getStatus)(IRA_Handle handle, IRA_Status *status);
	Void   (*setStatus)(IRA_Handle handle, const IRA_Status *status);
	Void   (*reset)(IRA_Handle handle);
	Int    (*findSync)(IRA_Handle handle, unsigned char *in);
	Int    (*decode)(IRA_Handle handle, unsigned char *inbuf, Int *outbuf, Int *lostflag);
	Void * idma;
}IRA_Fxns;


#endif	/* IRA_ */

