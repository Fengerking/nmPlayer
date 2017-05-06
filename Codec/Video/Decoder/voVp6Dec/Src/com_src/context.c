/****************************************************************************
*
*   Module Title :     context.c
*
*   Description  :     
*
****************************************************************************/

/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"

/****************************************************************************
*  Typedefs
****************************************************************************/     

/****************************************************************************
*  Module constants
****************************************************************************/     

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_ResetLeftContext
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Updates the left contexts.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void 
VP6_ResetLeftContext ( PB_INSTANCE *pbi)
{
	((INT32 *)&pbi->fc.LeftY)[0] = 0;
	((INT32 *)&pbi->fc.LeftY)[1] = 0;

	((INT32 *)&pbi->fc.LeftU)[0] = 0;
	((INT32 *)&pbi->fc.LeftV)[0] = 0;

	pbi->fc.LeftY[0].Frame = 4;
	pbi->fc.LeftY[1].Frame = 4;
	pbi->fc.LeftU.Frame    = 4;
	pbi->fc.LeftV.Frame    = 4;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_ResetAboveContext
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Updates the above contexts.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void 
VP6_ResetAboveContext ( PB_INSTANCE *pbi )
{
	UINT32 i;

    for ( i = 0 ; i < pbi->HFragments+8;i++)
	{
        pbi->fc.AboveY[i].Frame = 4;
		pbi->fc.AboveY[i].Dc =0;
		pbi->fc.AboveY[i].Token=0;
	}
	for ( i = 0 ; i < pbi->HFragments/2 + 8;i++)
	{        
        pbi->fc.AboveU[i].Frame = 4;
		pbi->fc.AboveU[i].Token=0;
		pbi->fc.AboveU[i].Dc=0;

        pbi->fc.AboveV[i].Frame = 4;  
		pbi->fc.AboveV[i].Token=0;
		pbi->fc.AboveV[i].Dc=0;
	}

	if(pbi->Vp3VersionNo < 6)
	{
        pbi->fc.AboveU[1].Frame = 0;
        pbi->fc.AboveV[1].Frame = 0;                
	}

	pbi->fc.LastDcY[0] = 0;
	pbi->fc.LastDcU[0] = 128;
	pbi->fc.LastDcV[0] = 128;
	for ( i = 1 ; i < 3 ; i++)
	{
		pbi->fc.LastDcY[i] = 0;
		pbi->fc.LastDcU[i] = 0;
		pbi->fc.LastDcV[i] = 0;
	}
}

