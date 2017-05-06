/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: cpltrees.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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

#include "RA_tni_priv.h"
#ifndef _HUFFMAN_TABLES_


#include "coder.h"

#ifdef  _PROFILE_TREES_
TreeProfile PROFILE_cpl_tree_2;
#endif //_PROFILE_TREES_

const HUFFNODE cpl_tree_2[3] = {
  {  -2,  1  },	  {  -1,  -3  },	  {  0,  0  },
};

#ifdef  _PROFILE_TREES_
TreeProfile PROFILE_cpl_tree_3;
#endif //_PROFILE_TREES_

const HUFFNODE cpl_tree_3[7] = {
  {  -4,  1  },	  {  -3,  2  },	  {  -5,  3  },	  {  -6,  4  },
  {  -2,  5  },	  {  -1,  -7  },	  {  0,  0  },
};

#ifdef  _PROFILE_TREES_
TreeProfile PROFILE_cpl_tree_4 ;
#endif //_PROFILE_TREES_

const HUFFNODE cpl_tree_4[15] = {
  {  -8,  1  },	  {  12,  2  },	  {  11,  3  },	  {  10,  4  },
  {  9,  5  },	  {  8,  6  },	  {  7,  13  },	  {  -1,  -2  },
  {  -3,  -13  },	  {  -4,  -12  },	  {  -5,  -11  },	  {  -6,  -10  },
  {  -7,  -9  },	  {  -14,  -15  },	  {  0,  0  },
};


#ifdef  _PROFILE_TREES_
TreeProfile PROFILE_cpl_tree_5;
#endif //_PROFILE_TREES_

const HUFFNODE cpl_tree_5[31] = {
  {  -16,  1  },	  {  21,  2  },	  {  19,  3  },	  {  17,  4  },
  {  15,  5  },	  {  13,  6  },	  {  11,  7  },	  {  8,  27  },
  {  9,  10  },	  {  -1,  -2  },	  {  -3,  -4  },	  {  12,  26  },
  {  -5,  -6  },	  {  14,  25  },	  {  -7,  -8  },	  {  16,  24  },
  {  -9,  -10  },	  {  18,  23  },	  {  -11,  -12  },	  {  20,  22  },
  {  -13,  -14  },	  {  -15,  -17  },	  {  -18,  -19  },	  {  -20,  -21  },
  {  -22,  -23  },	  {  -24,  -25  },	  {  -26,  -27  },	  {  28,  29  },
  {  -28,  -29  },	  {  -30,  -31  },	  {  0,  0  },
};

#ifdef  _PROFILE_TREES_
TreeProfile PROFILE_cpl_tree_6;
#endif //_PROFILE_TREES_

const HUFFNODE cpl_tree_6[63] = {
  {  -32,  1  },	  {  46,  2  },	  {  43,  3  },	  {  39,  4  },
  {  35,  5  },	  {  29,  6  },	  {  24,  7  },	  {  19,  8  },
  {  22,  9  },	  {  61,  10  },	  {  18,  11  },	  {  17,  12  },
  {  16,  13  },	  {  -62,  14  },	  {  -2,  15  },	  {  -1,  -63  },
  {  -3,  -61  },	  {  -4,  -60  },	  {  -5,  -59  },	  {  59,  20  },
  {  -54,  21  },	  {  -6,  -7  },	  {  23,  60  },	  {  -8,  -9  },
  {  25,  27  },	  {  -49,  26  },	  {  -10,  -11  },	  {  28,  58  },
  {  -12,  -13  },	  {  30,  33  },	  {  31,  32  },	  {  -14,  -15  },
  {  -16,  -17  },	  {  34,  57  },	  {  -18,  -46  },	  {  36,  54  },
  {  37,  38  },	  {  -19,  -20  },	  {  -21,  -22  },	  {  40,  51  },
  {  41,  42  },	  {  -23,  -24  },	  {  -25,  -26  },	  {  49,  44  },
  {  45,  50  },	  {  -27,  -28  },	  {  -31,  47  },	  {  -33,  48  },
  {  -29,  -30  },	  {  -34,  -35  },	  {  -36,  -37  },	  {  52,  53  },
  {  -38,  -39  },	  {  -40,  -41  },	  {  55,  56  },	  {  -42,  -43  },
  {  -44,  -45  },	  {  -47,  -48  },	  {  -50,  -51  },	  {  -52,  -53  },
  {  -55,  -56  },	  {  -57,  -58  },	  {  0,  0  },
};

const HUFFNODE *const RA_TNI_cpl_tree_tab[7] = {
	(const HUFFNODE *)0,
	(const HUFFNODE *)0,
	cpl_tree_2, cpl_tree_3,
	cpl_tree_4, cpl_tree_5,
	cpl_tree_6
};


#ifdef  _PROFILE_TREES_
TreeProfile	PROFILE_Null_cpl_tree ;
#endif //_PROFILE_TREES_

#ifdef  _PROFILE_TREES_
TreeProfile* const PROFILE_cpl_tree_tab[7] = {
	&PROFILE_Null_cpl_tree,
	&PROFILE_Null_cpl_tree,
	&PROFILE_cpl_tree_2, 
	&PROFILE_cpl_tree_3,
	&PROFILE_cpl_tree_4, 
	&PROFILE_cpl_tree_5,
	&PROFILE_cpl_tree_6
};
#endif //_PROFILE_TREES_

#endif // _HUFFMAN_TABLES_

