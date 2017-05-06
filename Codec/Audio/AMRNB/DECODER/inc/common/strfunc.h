/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : strfunc.h
*      Purpose          : string <-> mode and string <-> traffic channel
*                         conversion functions
*
*****************************************************************************
*/

#ifndef strfunc_h
#define strfunc_h "$Id $"
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "mode.h"
#include "frame.h"
#include "voAMRNBDecID.h"

/*
*****************************************************************************
*                         DECLARATION OF PROTOTYPES
*****************************************************************************
*/

int voAMRNBDecstr2mode(const char* str, enum Mode    *mode);
int voAMRNBDecmode2str(enum Mode    mode, char** str);
int voAMRNBDecrxframe2str(enum RXFrameType ft, char** str);
int voAMRNBDectxframe2str(enum TXFrameType ft, char** str);

#endif
