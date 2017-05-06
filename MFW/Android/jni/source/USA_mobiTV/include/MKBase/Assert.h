/*
 * NOTE: Porting candidate.
 *
 * NOTE: Unlike other headers Assert.h may usefully be included multiple times,
 *       with and without NDEBUG defined.
 *
 * TODO: Consider using a NASSERT (or something similar) macro instead to allow
 *       more precise control (possible issue with assert.h using NDEBUG).
 */

/******************************************************************************/

#undef MK_ASSERT

/******************************************************************************/

#ifdef NDEBUG
    #define MK_ASSERT(aExpr) ((void)0)
#else
    #include <MKBase/Platform.h>
    #include <assert.h>
    #define MK_ASSERT(aExpr) assert(aExpr)
#endif

/******************************************************************************/
