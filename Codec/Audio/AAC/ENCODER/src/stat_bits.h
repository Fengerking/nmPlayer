/*
   Static bit counter
 */
#ifndef __STAT_BITS_H
#define __STAT_BITS_H

#include "psy_const.h"
#include "interface.h"

Word16 countStaticBitdemand(PSY_OUT_CHANNEL psyOutChannel[MAX_CHANNELS],
                            PSY_OUT_ELEMENT *psyOutElement,
                            Word16 nChannels, 
							Word16 adtsUsed);

#endif /* __STAT_BITS_H */
