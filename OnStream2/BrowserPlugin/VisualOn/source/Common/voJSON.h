/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __voJSON_h__
#define __voJSON_h__

#ifdef __cplusplus
extern "C"
{
#endif

/* voJSON Types: */
#define voJSON_False 0
#define voJSON_True 1
#define voJSON_NULL 2
#define voJSON_Number 3
#define voJSON_String 4
#define voJSON_Array 5
#define voJSON_Object 6
	
#define voJSON_IsReference 256

/* The voJSON structure: */
typedef struct voJSON {
	struct voJSON *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct voJSON *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==voJSON_String */
	int valueint;				/* The item's number, if type==voJSON_Number */
	double valuedouble;			/* The item's number, if type==voJSON_Number */

	char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} voJSON;

typedef struct voJSON_Hooks {
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} voJSON_Hooks;

/* Supply malloc, realloc and free functions to voJSON */
extern void voJSON_InitHooks(voJSON_Hooks* hooks);


/* Supply a block of JSON, and this returns a voJSON object you can interrogate. Call voJSON_Delete when finished. */
extern voJSON *voJSON_Parse(const char *value);
/* Render a voJSON entity to text for transfer/storage. Free the char* when finished. */
extern char  *voJSON_Print(voJSON *item);
/* Render a voJSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char  *voJSON_PrintUnformatted(voJSON *item);
/* Delete a voJSON entity and all subentities. */
extern void   voJSON_Delete(voJSON *c);

/* Returns the number of items in an array (or object). */
extern int	  voJSON_GetArraySize(voJSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern voJSON *voJSON_GetArrayItem(voJSON *array,int item);
/* Get item "string" from object. Case insensitive. */
extern voJSON *voJSON_GetObjectItem(voJSON *object,const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when voJSON_Parse() returns 0. 0 when voJSON_Parse() succeeds. */
extern const char *voJSON_GetErrorPtr();
	
/* These calls create a voJSON item of the appropriate type. */
extern voJSON *voJSON_CreateNull();
extern voJSON *voJSON_CreateTrue();
extern voJSON *voJSON_CreateFalse();
extern voJSON *voJSON_CreateBool(int b);
extern voJSON *voJSON_CreateNumber(double num);
extern voJSON *voJSON_CreateString(const char *string);
extern voJSON *voJSON_CreateArray();
extern voJSON *voJSON_CreateObject();

/* These utilities create an Array of count items. */
extern voJSON *voJSON_CreateIntArray(int *numbers,int count);
extern voJSON *voJSON_CreateFloatArray(float *numbers,int count);
extern voJSON *voJSON_CreateDoubleArray(double *numbers,int count);
extern voJSON *voJSON_CreateStringArray(const char **strings,int count);

/* Append item to the specified array/object. */
extern void voJSON_AddItemToArray(voJSON *array, voJSON *item);
extern void	voJSON_AddItemToObject(voJSON *object,const char *string,voJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing voJSON to a new voJSON, but don't want to corrupt your existing voJSON. */
extern void voJSON_AddItemReferenceToArray(voJSON *array, voJSON *item);
extern void	voJSON_AddItemReferenceToObject(voJSON *object,const char *string,voJSON *item);

/* Remove/Detatch items from Arrays/Objects. */
extern voJSON *voJSON_DetachItemFromArray(voJSON *array,int which);
extern void   voJSON_DeleteItemFromArray(voJSON *array,int which);
extern voJSON *voJSON_DetachItemFromObject(voJSON *object,const char *string);
extern void   voJSON_DeleteItemFromObject(voJSON *object,const char *string);
	
/* Update array items. */
extern void voJSON_ReplaceItemInArray(voJSON *array,int which,voJSON *newitem);
extern void voJSON_ReplaceItemInObject(voJSON *object,const char *string,voJSON *newitem);

#define voJSON_AddNullToObject(object,name)	voJSON_AddItemToObject(object, name, voJSON_CreateNull())
#define voJSON_AddTrueToObject(object,name)	voJSON_AddItemToObject(object, name, voJSON_CreateTrue())
#define voJSON_AddFalseToObject(object,name)		voJSON_AddItemToObject(object, name, voJSON_CreateFalse())
#define voJSON_AddNumberToObject(object,name,n)	voJSON_AddItemToObject(object, name, voJSON_CreateNumber(n))
#define voJSON_AddStringToObject(object,name,s)	voJSON_AddItemToObject(object, name, voJSON_CreateString(s))

#ifdef __cplusplus
}
#endif

#endif
