/****************************************************************************
*
*   Module Title :     Huffman.h
*
*   Description  :     Huffman Coding header file.
*
****************************************************************************/
#ifndef __INC_HUFFMAN_H
#define __INC_HUFFMAN_H

/****************************************************************************
*  Header Files
****************************************************************************/
#include "type_aliases.h"
#include "boolhuff.h"


#ifdef NDS_NITRO
#define __NEW_HUFF__
#endif

/****************************************************************************
*  Module Statics
****************************************************************************/
#define HUFF_LUT_LEVELS 6

/****************************************************************************
*  Types
****************************************************************************/  
typedef struct _tokenorptr
{
    unsigned short selector : 1;   // 1 bit selector 0->ptr, 1->token
    unsigned short value : 7;
} tokenorptr;

typedef struct _ehuffnode
{
	union
	{
		char l;
		tokenorptr left;
    } leftunion; 
	union
	{
		char r;
		tokenorptr right;
    } rightunion; 
} E_HUFF_NODE;

#ifdef __NEW_HUFF__

/* HUFF_NODE for decoder */
typedef struct _HUFF_NODE
{
    unsigned short left; // 1 bit tells whether its a pointer or value
    unsigned short right;// 1 bit tells whether its a pointer or value
} HUFF_NODE;

#else

typedef struct _dhuffnode
{
	union
	{
		char l;
		tokenorptr left;
    } leftunion; 
	union
	{
		char r;
		tokenorptr right;
    } rightunion; 
//	unsigned char freq;

} HUFF_NODE;

#endif

/****************************************************************************
*   Data structures
****************************************************************************/
typedef struct _HUFF_TALBE_NODE
{
    unsigned short flag     :1;      // bit 0: 1-Token, 0-Index
    unsigned short value    :5;      // value: the value of the Token or the Index to the huffman tree
    unsigned short unused   :6;      // not used for now
    unsigned short length   :4;      // Huffman code length of the token
} HUFF_TABLE_NODE;

/****************************************************************************
*  Functions
****************************************************************************/

extern void VP6_BuildHuffLookupTable ( HUFF_NODE * HuffTreeRoot, UINT16 * HuffTable );
extern void VP6_BuildHuffTree ( HUFF_NODE *hn, unsigned int *counts, int values );

#endif
