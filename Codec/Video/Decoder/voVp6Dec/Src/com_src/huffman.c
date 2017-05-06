/****************************************************************************
*
*   Module Title :     Huffman.c
*
*   Description  :     Huffman coding routines.
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "huffman.h"
#include "pbdll.h"

/****************************************************************************
*  Typedefs
****************************************************************************/              
typedef struct _SORT_NODE
{
    int next;
    int freq;
    unsigned char value;
} SORT_NODE;

#ifndef __NEW_HUFF__
typedef struct _sortnode
{
    int next;
    int freq;
    tokenorptr value;
} sortnode;
#else
typedef struct _sortnode
{
    int next;
    int freq;
    unsigned short value;
} sortnode;
#endif

/****************************************************************************
 * 
 *  ROUTINE       :     InsertSorted
 *
 *  INPUTS        :     sortnode *sn   : Array of sort nodes.
 *                      int node       : Index of node to be inserted.
 *                      int *startnode : Pointer to head of linked-list.
 *
 *  OUTPUTS       :     int *startnode : Pointer to head of linked-list.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Inserts a node into a sorted linklist.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
static void InsertSorted ( sortnode *sn, int node, int *startnode )
{
    int which = *startnode;
    int prior = *startnode;

    // find the position at which to insert the node
    while( which != -1 && sn[node].freq > sn[which].freq )
    {
        prior = which;
        which = sn[which].next;
    }

    if(which == *startnode)
    {
        *startnode = node;
        sn[node].next = which;
    }
    else
    {
        sn[prior].next = node;
        sn[node].next = which;
    }
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_BuildHuffTree
 *
 *  INPUTS        :     int values           : Number of values in the tree.
 *                      unsigned int *counts : Histogram of token frequencies.
 *
 *  OUTPUTS       :     HUFF_NODE *hn        : Array of nodes (containing token frequency) 
 *                                             from which to create tree.
 *                      unsigned int *counts : Histogram of token frequencies (0 freq clipped to 1).
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Creates a Huffman tree data structure from list
 *                      of token frequencies.
 *
 *  SPECIAL NOTES :     Maximum of 256 nodes can be handled. 
 *
 ****************************************************************************/
void VP6_BuildHuffTree ( HUFF_NODE *hn, unsigned int *counts, int values )
{
    int i;
    sortnode sn[256];
    int sncount=0;
    int startnode=0;

    // NOTE:
    // Create huffman tree in reverse order so that the root will always be 0
    int huffptr=values-1;

    // Set up sorted linked list of values/pointers into the huffman tree
    for ( i=0; i<values; i++ )
    {
#ifndef __NEW_HUFF__
        sn[i].value.selector = 1;
        sn[i].value.value = i;
#else
        sn[i].value = (i << 1) | 1;
#endif
        if(counts[i] == 0)
            counts[i] = 1;
        sn[i].freq = counts[i];
        sn[i].next = -1;

    }

    sncount = values;

    // Connect above list into a linked list
    for ( i=1; i<values; i++ )
        InsertSorted ( sn, i, &startnode );

    // while there is more than one node in our linked list
    while ( sn[startnode].next != -1 )
    {
        int first = startnode;
        int second = sn[startnode].next;
        int sumfreq = sn[first].freq + sn[second].freq;

        // set-up new merged huffman node
        --huffptr;
#ifndef __NEW_HUFF__
        hn[huffptr].leftunion.left = sn[first].value;
        hn[huffptr].rightunion.right = sn[second].value;
#else
        // setup new merged huffman node
        hn[huffptr].left = sn[first].value;
        hn[huffptr].right = sn[second].value;
#endif
//        hn[huffptr].freq = 256 * sn[first].freq / sumfreq;

        // set up new merged sort node pointing to our huffnode
#ifndef __NEW_HUFF__
        sn[sncount].value.selector = 0;
        sn[sncount].value.value = huffptr;
#else
        sn[sncount].value = (huffptr << 1) | 0;
#endif

        sn[sncount].freq = sumfreq;
        sn[sncount].next = -1;


        // remove the two nodes we just merged from the linked list
        startnode = sn[second].next;

        // insert the new sort node into the proper location
        InsertSorted(sn, sncount, &startnode);

        // account for new nodes
        sncount++;
    }

    return;
}


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_BuildHuffLookupTable
 *
 *  INPUTS        :     HUFF_NODE *HuffTreeRoot : Pointer to root of Huffman tree. 
 *
 *  OUTPUTS       :     UINT16 *HuffTable       : Array (LUT) of Huffman codes.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Traverse Huffman tree to create LUT of Huffman codes.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
#ifndef __NEW_HUFF__
void 
VP6_BuildHuffLookupTable ( HUFF_NODE *HuffTreeRoot, UINT16 *HuffTable )
{
    int i, j;
    int bits;
    tokenorptr torp;

    for ( i=0; i<(1<<HUFF_LUT_LEVELS); i++ )
    {
        bits = i;        
        j=0;

        torp.value    = 0;
        torp.selector = 0;

        do
        {
            j++;
            if ( (bits>>(HUFF_LUT_LEVELS - j)) & 1 )
                torp = HuffTreeRoot[torp.value].rightunion.right;
            else
                torp = HuffTreeRoot[torp.value].leftunion.left;
        }
        while ( !(torp.selector) && (j < HUFF_LUT_LEVELS) );
        
//        HuffTable[i] = torp.value<<1 | torp.selector | (j << 12);
        ((HUFF_TABLE_NODE *)HuffTable)[i].value = torp.value;
        ((HUFF_TABLE_NODE *)HuffTable)[i].flag = torp.selector;
        ((HUFF_TABLE_NODE *)HuffTable)[i].length = j;
    }
}
#else
void 
VP6_BuildHuffLookupTable(HUFF_NODE *HuffTreeRoot,  UINT16 * HuffTable)
{
    short *h= (short *) HuffTreeRoot;    

    int i, bits,j;
    unsigned int nodeptr;

//    for(i = 0; i < (1 << NUM_LEVELS); i++)
    for(i = 0; i < (1 << HUFF_LUT_LEVELS); i++)
    {
        bits = i;        
        nodeptr=0;
        j=0;

        //ON2LOG("VP6_BuildHuffLookupTable() :: %d\n", i);
        do
        {
//            nodeptr += (bits >> j) & 1;
            j++;
            nodeptr += (bits >> (HUFF_LUT_LEVELS - j) ) & 1;
            nodeptr = ((unsigned int )h[nodeptr]);
        
        }while(!(nodeptr & 1) && (j < HUFF_LUT_LEVELS));
        
        //ON2LOG("VP6_BuildHuffLookupTable() :: %d flag = %x value = %x length = %x\n", i, nodeptr & 1, (nodeptr >> 1), j);
//        HuffTable[i] = nodeptr | 
  //                      (j << 13);

        HuffTable[i] = nodeptr | 
                        (j << 12);

    }
}

/*
typedef struct _HUFF_TALBE_NODE
{
    unsigned short flag     :1;      // bit 0: 1-Token, 0-Index
    unsigned short value    :5;      // value: the value of the Token or the Index to the huffman tree
    unsigned short unused   :6;      // not used for now
    unsigned short length   :4;      // Huffman code length of the token
} HUFF_TABLE_NODE;
*/

#endif

