/*
 * NOTE: Porting candidate.
 */

#ifndef __MKBase_Char_h__
#define __MKBase_Char_h__

#include <MKBase/Platform.h>
#include <MKBase/Config.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>

#include <ctype.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Tests the characters aCh1 and aCh2 for equality.
 *
 * The I version does a case-insensitive equality comparison.
 */
MK_INLINE MK_Bool MK_Char_Eq(MK_Char aCh1, MK_Char aCh2);
MK_INLINE MK_Bool MK_Char_EqI(MK_Char aCh1, MK_Char aCh2);

/*
 * Compares the characters aCh1 and aCh2. Returns 0 if the characters are equal,
 * otherwise a negative value is returned to if aCh1 < aCh2 and positive value
 * if aCh1 > aCh2. Unsigned comparison is used.
 *
 * The I version does a case-insensitive comparison.
 */
MK_INLINE MK_S32 MK_Char_Cmp(MK_Char aCh1, MK_Char aCh2);
MK_INLINE MK_S32 MK_Char_CmpI(MK_Char aCh1, MK_Char aCh2);

/******************************************************************************/

/*
 * Test if aCh is a 7-bit ASCII character.
 */
MK_INLINE MK_Bool MK_Char_IsASCII(MK_Char aCh);

/*
 * Test if aCh is a EOL character (e.g. '\n', '\r').
 */
MK_INLINE MK_Bool MK_Char_IsEOL(MK_Char aCh);

/*
 * Test if aCh is a space or tab character (e.g. '\t', ' ').
 */
MK_INLINE MK_Bool MK_Char_IsBlank(MK_Char aCh);

/*
 * Test if aCh is a white-space character (e.g. '\t', '\n', '\v', '\f', '\r', ' ').
 */
MK_INLINE MK_Bool MK_Char_IsSpace(MK_Char aCh);

/*
 * Test if aCh is a octal digit character (e.g. '0' - '7').
 */
MK_INLINE MK_Bool MK_Char_IsOctal(MK_Char aCh);

/*
 * Test if aCh is a decimal digit character (e.g. '0' - '9').
 */
MK_INLINE MK_Bool MK_Char_IsDecimal(MK_Char aCh);

/*
 * Test if aCh is a hexadecimal digit character (e.g. '0' - '9', 'A' - 'Z', 'a' - 'z').
 */
MK_INLINE MK_Bool MK_Char_IsHex(MK_Char aCh);

/*
 * Test if aCh is a uppercase letter (e.g. 'A' - 'Z').
 */
MK_INLINE MK_Bool MK_Char_IsUpper(MK_Char aCh);

/*
 * Test if aCh is a lowercase letter (e.g. 'a' - 'z').
 */
MK_INLINE MK_Bool MK_Char_IsLower(MK_Char aCh);

/*
 * Test if aCh is a letter (e.g. 'A' - 'Z', 'a' - 'z').
 */
MK_INLINE MK_Bool MK_Char_IsAlpha(MK_Char aCh);

/*
 * Test if aCh is a letter or decimal digit (e.g. '0' - '9', 'A' - 'Z', 'a' - 'z').
 */
MK_INLINE MK_Bool MK_Char_IsAlphaNum(MK_Char aCh);

/******************************************************************************/

/*
 * Finds the corresponding uppercase character for aCh and returns it. If there
 * is no uppercase character the orginal value is returned.
 */
MK_INLINE MK_Char MK_Char_ToUpper(MK_Char aCh);

/*
 * Finds the corresponding lowercase character for aCh and returns it. If there
 * is no lowercase character the orginal value is returned.
 */
MK_INLINE MK_Char MK_Char_ToLower(MK_Char aCh);

/******************************************************************************/

/*
 * Returns the octal value which aCh represents or -1 if there is none.
 */
MK_INLINE MK_S8 MK_Char_ToOctal(MK_Char aCh);

/*
 * Returns the decimal value which aCh represents or -1 if there is none.
 */
MK_INLINE MK_S8 MK_Char_ToDecimal(MK_Char aCh);

/*
 * Returns the hexadecimal value which aCh represents or -1 if there is none.
 */
MK_INLINE MK_S8 MK_Char_ToHex(MK_Char aCh);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

MK_INLINE MK_Bool MK_Char_Eq(MK_Char aCh1, MK_Char aCh2)
{
    return MK_ToU8(aCh1) == MK_ToU8(aCh2);
}

MK_INLINE MK_Bool MK_Char_EqI(MK_Char aCh1, MK_Char aCh2)
{
    return MK_Char_ToUpper(aCh1) == MK_Char_ToUpper(aCh2);
}

MK_INLINE MK_S32 MK_Char_Cmp(MK_Char aCh1, MK_Char aCh2)
{
    return MK_ToS32(MK_ToU8(aCh1) - MK_ToU8(aCh2));
}

MK_INLINE MK_S32 MK_Char_CmpI(MK_Char aCh1, MK_Char aCh2)
{
    return MK_Char_Cmp(MK_Char_ToUpper(aCh1), MK_Char_ToUpper(aCh2));
}

/******************************************************************************/

MK_INLINE MK_Bool MK_Char_IsASCII(MK_Char aCh)
{
    #if 0 && MK_HAVE_ISASCII
        return MK_ToBool(isascii(aCh));
    #else
        return 0 <= MK_ToS8(aCh);
    #endif
}

MK_INLINE MK_Bool MK_Char_IsEOL(MK_Char aCh)
{
    return '\n' == aCh || '\r' == aCh;
}

MK_INLINE MK_Bool MK_Char_IsBlank(MK_Char aCh)
{
    #if 0 && MK_HAVE_ISBLANK
        return MK_ToBool(isblank(aCh));
    #else
        return '\t' == aCh || ' ' == aCh;
    #endif
}

MK_INLINE MK_Bool MK_Char_IsSpace(MK_Char aCh)
{
    #if 0 && MK_HAVE_ISSPACE
        return MK_ToBool(isspace(aCh));
    #else
        return '\t' == aCh || '\n' == aCh || '\v' == aCh || '\f' == aCh || '\r' == aCh || ' ' == aCh;
    #endif
}

MK_INLINE MK_Bool MK_Char_IsOctal(MK_Char aCh)
{
    return '0' <= aCh && '7' >= aCh;
}

MK_INLINE MK_Bool MK_Char_IsDecimal(MK_Char aCh)
{
    #if 0 && MK_HAVE_ISDIGIT
        return MK_ToBool(isdigit(aCh));
    #else
        return '0' <= aCh && '9' >= aCh;
    #endif
}

MK_INLINE MK_Bool MK_Char_IsHex(MK_Char aCh)
{
    #if 0 && MK_HAVE_ISXDIGIT
        return MK_ToBool(isxdigit(aCh));
    #else
        return ('0' <= aCh && '9' >= aCh) ||
               ('A' <= aCh && 'F' >= aCh) ||
               ('a' <= aCh && 'f' >= aCh);
    #endif
}

MK_INLINE MK_Bool MK_Char_IsUpper(MK_Char aCh)
{
    #if 0 && MK_HAVE_ISUPPER
        return MK_ToBool(isupper(aCh));
    #else
        return ('A' <= aCh && 'Z' >= aCh);
    #endif
}

MK_INLINE MK_Bool MK_Char_IsLower(MK_Char aCh)
{
    #if 0 && MK_HAVE_ISLOWER
        return MK_ToBool(islower(aCh));
    #else
        return ('a' <= aCh && 'z' >= aCh);
    #endif
}

MK_INLINE MK_Bool MK_Char_IsAlpha(MK_Char aCh)
{
    #if 0 && MK_HAVE_ISALPHA
        return MK_ToBool(isalpha(aCh));
    #else
        return MK_Char_IsUpper(aCh) ||
               MK_Char_IsLower(aCh);
    #endif
}

MK_INLINE MK_Bool MK_Char_IsAlphaNum(MK_Char aCh)
{
    #if 0 && MK_HAVE_ISALPHANUM
        return MK_ToBool(isalnum(aCh));
    #else
        return MK_Char_IsDecimal(aCh) ||
               MK_Char_IsAlpha(aCh);
    #endif
}

/******************************************************************************/

MK_INLINE MK_Char MK_Char_ToUpper(MK_Char aCh)
{
    #if 0 && MK_HAVE_TOUPPER
        return MK_ToChar(toupper(aCh));
    #else
        return MK_Char_IsLower(aCh) ? aCh - 'a' + 'A' : aCh;
    #endif
}

MK_INLINE MK_Char MK_Char_ToLower(MK_Char aCh)
{
    #if 0 && MK_HAVE_TOLOWER
        return MK_ToChar(tolower(aCh));
    #else
        return MK_Char_IsUpper(aCh) ? aCh - 'A' + 'a' : aCh;
    #endif
}

/******************************************************************************/

MK_INLINE MK_S8 MK_Char_ToOctal(MK_Char aCh)
{
    return MK_ToS8(
        ('0' <= aCh && '7' >= aCh) ? aCh - '0' :
        -1
    );
}

MK_INLINE MK_S8 MK_Char_ToDecimal(MK_Char aCh)
{
    return MK_ToS8(
        ('0' <= aCh && '9' >= aCh) ? aCh - '0' :
        -1
    );
}

MK_INLINE MK_S8 MK_Char_ToHex(MK_Char aCh)
{
    return MK_ToS8(
        ('0' <= aCh && '9' >= aCh) ? aCh - '0' :
        ('A' <= aCh && 'F' >= aCh) ? aCh - 'A' + 10 :
        ('a' <= aCh && 'f' >= aCh) ? aCh - 'a' + 10 :
        -1
    );
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
