/*-------------------------------------------------------------------*
 *                         RANDOM.C									 *
 *-------------------------------------------------------------------*
 * Signed 16 bits random generator.								     *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "count.h"


#if (!FUNC_RANDOM_MACRO)
#ifdef MSVC_AMRWBPLUS
Word16 Random(Word16 * seed)
#elif EVC_AMRWBPLUS
Word16 Random_evc(Word16 * seed)
#endif
// end
{
    /* static Word16 seed = 21845; */

#if (FUNC_RANDOME_OPT)

    *seed = (Word16)((*seed) * 31821 + 13849L);

#else

    *seed = extract_l(L_add(L_shr(L_mult(*seed, 31821), 1), 13849L));   

#endif

    return (*seed);
}
#endif
