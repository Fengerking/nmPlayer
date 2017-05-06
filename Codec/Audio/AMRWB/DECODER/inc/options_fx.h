#ifndef options_fx_h
#define options_fx_h

#ifdef WIN32
#pragma warning( disable : 4244 4761)  //4244: conversion from 'short ' to 'unsigned char ', possible loss of data;
                                       //4761: integral size mismatch in argument; (ex. shr( (0x3 & cnst),1) 
                                       // VCC compile this expression (0x3 & cnst) like a Word32...
#endif

/* AMR-WB+ input/output sampling rate filters */
/* Choice of filter should be selected according to the hardware specification */
/* Both can be selected if required */
/* FILTER_44khz allow 11025, 22050 and 44100 Hz sampling rates */
/* FILTER_48khz allow 8, 16, 24, 32 and 48 kHz sampling rates */
#define FILTER_44kHz

#define FILTER_48kHz

#endif
