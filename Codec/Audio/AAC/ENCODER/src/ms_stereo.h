/*
   MS stereo processing
 */
#ifndef __MS_STEREO_H__
#define __MS_STEREO_H__
#include "typedef.h"

void MsStereoProcessing(Word32       *sfbEnergyLeft,
                        Word32       *sfbEnergyRight,
                        const Word32 *sfbEnergyMid,
                        const Word32 *sfbEnergySide,
                        Word32       *mdctSpectrumLeft,
                        Word32       *mdctSpectrumRight,
                        Word32       *sfbThresholdLeft,
                        Word32       *sfbThresholdRight,
                        Word32       *sfbSpreadedEnLeft,
                        Word32       *sfbSpreadedEnRight,
                        Word16       *msDigest,
                        Word16       *msMask,
                        const Word16  sfbCnt,
                        const Word16  sfbPerGroup,
                        const Word16  maxSfbPerGroup,
                        const Word16 *sfbOffset,
                        Word16       *weightMsLrPeRatio);


#endif
