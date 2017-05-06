#ifndef _CPush_Mode_Type_
#define _CPush_Mode_Type_

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef enum{
	TYPE_ID3v2,
	TYPE_ADTS,
	TYPE_HEADDATE,
	TYPE_MAX
}DATATYPE;

typedef struct{
	VO_U32	nStartPos;
	VO_U32 	nLength;
	DATATYPE	 nType;
}HEADPOINT;


typedef enum{
	ERR_Valid_Data,
	ERR_Invalid_Data,	
	ERR_Not_Enough_Data
}ERR_TYPE;

#define _CHECK_NULL_POINTER_(cls)	if(!cls){return VO_FALSE;}
#define _CHECK_NULL_POINTER_EX(cls,result)	if(!cls){return result;}


#ifdef _VONAMESPACE
}
#endif
#endif

