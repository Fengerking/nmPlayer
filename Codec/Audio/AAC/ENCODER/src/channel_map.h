/*
   channel mapping functionality
 */
#ifndef _CHANNEL_MAP_H
#define _CHANNEL_MAP_H

#include "psy_const.h"
#include "qc_data.h"

Word16 InitElementInfo (Word16 nChannels, ELEMENT_INFO* elInfo);

Word16 InitElementBits(ELEMENT_BITS *elementBits,
                       ELEMENT_INFO elInfo,
                       Word32 bitrateTot,
                       Word16 averageBitsTot,
                       Word16 staticBitsTot);

#endif /* CHANNEL_MAP_H */
