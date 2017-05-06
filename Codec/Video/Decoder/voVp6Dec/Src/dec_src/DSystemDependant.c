/****************************************************************************
*
*   Module Title :     SystemDependant.c
*
*   Description  :     Miscellaneous system dependant functions.
*
****************************************************************************/



/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"

/****************************************************************************
 * 
 *
 ****************************************************************************/
void 
overridePredictFilteredFuncs(unsigned int isVP60)
{
    if(isVP60)
    {
//        reconFunc[0] = ReconInter_MB;
//        reconFunc[1] = ReconIntra_MB;
        reconFunc[2] = VP6_PredictFiltered_MB_60;
        reconFunc[3] = VP6_PredictFiltered_MB_60;
        reconFunc[4] = VP6_PredictFiltered_MB_60;
//        reconFunc[5] = ReconInter_MB;
        reconFunc[6] = VP6_PredictFiltered_MB_60;
        reconFunc[7] = VP6_PredictFiltered_MB_60;
        reconFunc[8] = VP6_PredictFiltered_MB_60;
        reconFunc[9] = VP6_PredictFiltered_MB_60;    
    }
    else
    {
//        reconFunc[0] = ReconInter_MB;
//        reconFunc[1] = ReconIntra_MB;
        reconFunc[2] = VP6_PredictFiltered_MB;
        reconFunc[3] = VP6_PredictFiltered_MB;
        reconFunc[4] = VP6_PredictFiltered_MB;
//        reconFunc[5] = ReconInter_MB;
        reconFunc[6] = VP6_PredictFiltered_MB;
        reconFunc[7] = VP6_PredictFiltered_MB;
        reconFunc[8] = VP6_PredictFiltered_MB;
        reconFunc[9] = VP6_PredictFiltered_MB;    
    }
}
/****************************************************************************
 * 
 *
 ****************************************************************************/
void 
VP6_DMachineSpecificConfig ( void )
{
    /* default MB recon func table */
    reconFunc[0] = ReconInter_MB;
    reconFunc[1] = ReconIntra_MB;
    reconFunc[2] = VP6_PredictFiltered_MB;
    reconFunc[3] = VP6_PredictFiltered_MB;
    reconFunc[4] = VP6_PredictFiltered_MB;
    reconFunc[5] = ReconInter_MB;
    reconFunc[6] = VP6_PredictFiltered_MB;
    reconFunc[7] = VP6_PredictFiltered_MB;
    reconFunc[8] = VP6_PredictFiltered_MB;
    reconFunc[9] = VP6_PredictFiltered_MB;    
}
