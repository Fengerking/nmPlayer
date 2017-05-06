/****************************************************************************
*
*   Module Title :     PB_Globals.c
*
*   Description  :     Video CODEC Demo: playback dll global declarations
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"
#include "on2_mem.h"


/****************************************************************************
*  Exports
****************************************************************************/


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DeletePBInstance
 *
 *  INPUTS        :     PB_INSTANCE **pbi : Pointer to the pointer to the 
 *                                          decoder instance.
 *
 *  OUTPUTS       :     PB_INSTANCE **pbi : Pointer to the pointer to the 
 *                                          decoder instance. Set to 0 on exit.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     De-allocates the decoder instance data structure.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void 
VP6_DeletePBInstance ( PB_INSTANCE **pbi )
{
    if ( *pbi )
    {
        // Delete any other dynamically allocaed temporary buffers
		VP6_DeleteTmpBuffers(*pbi);
		VP6_DeleteQuantizer(&(*pbi)->quantizer);
		VP6_DeleteHuffman (*pbi);
    }

    // dealoocate and reset pointer to NULL
	duck_free ( *pbi );
	*pbi = 0;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_CreatePBInstance
 *
 *  INPUTS        :     None.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     PB_INSTANCE *: Pointer to allocated decoder instance.
 *
 *  FUNCTION      :     Allocates space for and initializes decoder instance.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
PB_INSTANCE *
VP6_CreatePBInstance ( void )
{
	PB_INSTANCE *pbi = 0;
    CONFIG_TYPE ConfigurationInit = { 0,0,0,0,8,8,0,0,0,0,0,0,0,0 };
	int pbi_size = sizeof(PB_INSTANCE);

    pbi = (PB_INSTANCE *) duck_memalign(32, pbi_size );
    if(pbi == (PB_INSTANCE *)0)
        return 0;

	// initialize whole structure to 0
	duck_memset ( (unsigned char *)pbi, 0, pbi_size );
	duck_memcpy ( (void *)&pbi->Configuration, (void *)&ConfigurationInit, sizeof(CONFIG_TYPE) );

	if(VP6_AllocateTmpBuffers(pbi) < 0)
    {
        duck_free(pbi);
        return 0;
    }

	pbi->CPUFree = 70;
//sjl    pbi->idct    = idct;

	// Initialise Entropy related data structures.
	duck_memset( pbi->DcProbs, 0, sizeof(pbi->DcProbs) );
	duck_memset( pbi->AcProbs, 0, sizeof(pbi->AcProbs) );

	return pbi;
}