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
/* voJSON */

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "voJSON.h"

static const char *ep;

const char *voJSON_GetErrorPtr() {return ep;}

static int voJSON_strcasecmp(const char *s1,const char *s2)
{
	if (!s1) return (s1==s2)?0:1;
	if (!s2) return 1;
	for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)
		if (*s1 == 0)	
			return 0;

	return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}

static void *(*voJSON_malloc)(size_t sz) = malloc;
static void (*voJSON_free)(void *ptr) = free;

static char* voJSON_strdup(const char* str)
{
	size_t len;
	char* copy;

	len = strlen(str) + 1;
	if (!(copy = (char*)voJSON_malloc(len))) return 0;
	memcpy(copy,str,len);
	return copy;
}

void voJSON_InitHooks(voJSON_Hooks* hooks)
{
	if (!hooks) { /* Reset hooks */
		voJSON_malloc = malloc;
		voJSON_free = free;
		return;
	}

	voJSON_malloc = (hooks->malloc_fn)?hooks->malloc_fn:malloc;
	voJSON_free	 = (hooks->free_fn)?hooks->free_fn:free;
}

/* Internal constructor. */
static voJSON *voJSON_New_Item()
{
	voJSON* node = (voJSON*)voJSON_malloc(sizeof(voJSON));
	if (node) memset(node,0,sizeof(voJSON));
	return node;
}

/* Delete a voJSON structure. */
void voJSON_Delete(voJSON *c)
{
	voJSON *next;
	while (c)
	{
		next=c->next;
		if (!(c->type&voJSON_IsReference) && c->child) voJSON_Delete(c->child);
		if (!(c->type&voJSON_IsReference) && c->valuestring) voJSON_free(c->valuestring);
		if (c->string) voJSON_free(c->string);
		voJSON_free(c);
		c=next;
	}
}

/* Parse the input text to generate a number, and populate the result into item. */
static const char *parse_number(voJSON *item,const char *num)
{
	double n=0,sign=1,scale=0;int subscale=0,signsubscale=1;

	/* Could use sscanf for this? */
	if (*num=='-') sign=-1,num++;	/* Has sign? */

	if (*num=='0') num++;			/* is zero */
	
	if (*num>='1' && *num<='9')	
		do	{
			n=(n*10.0)+(*num++ -'0');	
		} while (*num>='0' && *num<='9');	/* Number? */

	if (*num=='.' && num[1]>='0' && num[1]<='9') {
		num++;		
		do	n=(n*10.0)+(*num++ -'0'),scale--; 
		while (*num>='0' && *num<='9');
	}	/* Fractional part? */

	if (*num=='e' || *num=='E')		/* Exponent? */
	{	
		num++;if (*num=='+') num++;	else if (*num=='-') signsubscale=-1,num++;		/* With sign? */
		while (*num>='0' && *num<='9') subscale=(subscale*10)+(*num++ - '0');	/* Number? */
	}

	n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */

	item->valuedouble=n;
	item->valueint=(int)n;
	item->type=voJSON_Number;
	return num;
}

/* Render the number nicely from the given item into a string. */
static char *print_number(voJSON *item)
{
	char *str;
	double d=item->valuedouble;
	if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN)
	{
		str=(char*)voJSON_malloc(21);	/* 2^64+1 can be represented in 21 chars. */
		if (str) sprintf_s(str, 21, "%d",item->valueint);
	}
	else
	{
		str=(char*)voJSON_malloc(64);	/* This is a nice tradeoff. */
		if (str)
		{
			if (fabs(floor(d)-d)<=DBL_EPSILON)			
				sprintf_s(str, 64, "%.0f",d);
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)	
				sprintf_s(str, 64, "%e",d);
			else										
				sprintf_s(str, 64, "%f",d);
		}
	}
	return str;
}

/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static const char *parse_string(voJSON *item,const char *str)
{
	const char *ptr=str+1;
	char *ptr2;
	char *out;
	int len=0; 
	unsigned uc,uc2;

	if (*str!='\"') {ep=str;return 0;}	/* not a string! */

	while (*ptr!='\"' && *ptr && ++len) if (*ptr++ == '\\') ptr++;	/* Skip escaped quotes. */

	out=(char*)voJSON_malloc(len+1);	/* This is how long we need for the string, roughly. */
	if (!out) return 0;

	ptr=str+1;ptr2=out;
	while (*ptr!='\"' && *ptr)
	{
		if (*ptr!='\\') *ptr2++=*ptr++;
		else
		{
			ptr++;
			switch (*ptr)
			{
			case 'b': *ptr2++='\b';	break;
			case 'f': *ptr2++='\f';	break;
			case 'n': *ptr2++='\n';	break;
			case 'r': *ptr2++='\r';	break;
			case 't': *ptr2++='\t';	break;
			case 'u':	 /* transcode utf16 to utf8. */
				sscanf_s(ptr+1,"%4x", &uc); ptr+=4;	/* get the unicode char. */

				if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)	break;	// check for invalid.

				if (uc>=0xD800 && uc<=0xDBFF)	// UTF16 surrogate pairs.
				{
					if (ptr[1]!='\\' || ptr[2]!='u')	
						break;	// missing second-half of surrogate.

					sscanf_s(ptr+3, "%4x", &uc2);
					ptr+=6;
					if (uc2<0xDC00 || uc2>0xDFFF)	
						break;	// invalid second-half of surrogate.

					uc=0x10000 | ((uc&0x3FF)<<10) | (uc2&0x3FF);
				}

				len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;

				switch (len) {
				case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
				case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
				case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
				case 1: *--ptr2 =(uc | firstByteMark[len]);
				}
				ptr2+=len;
				break;
			default:  *ptr2++=*ptr; break;
			}
			ptr++;
		}
	}
	*ptr2=0;
	if (*ptr=='\"') ptr++;
	item->valuestring=out;
	item->type=voJSON_String;
	return ptr;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char *print_string_ptr(const char *str)
{
	const char *ptr;char *ptr2,*out;int len=0;unsigned char token;

	if (!str) return voJSON_strdup("");
	ptr=str;while ((token=*ptr) && ++len) {if (strchr("\"\\\b\f\n\r\t",token)) len++; else if (token<32) len+=5;ptr++;}

	out=(char*)voJSON_malloc(len+3);
	if (!out) return 0;

	ptr2=out;ptr=str;
	*ptr2++='\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\')
			*ptr2++=*ptr++;
		else
		{
			*ptr2++='\\';
			switch (token=*ptr++)
			{
			case '\\':	*ptr2++='\\';	break;
			case '\"':	*ptr2++='\"';	break;
			case '\b':	*ptr2++='b';	break;
			case '\f':	*ptr2++='f';	break;
			case '\n':	*ptr2++='n';	break;
			case '\r':	*ptr2++='r';	break;
			case '\t':	*ptr2++='t';	break;
			default: sprintf_s(ptr2, ptr2 - out, "u%04x",token);ptr2+=5;	break;	/* escape and print */
			}
		}
	}
	*ptr2++='\"';*ptr2++=0;
	return out;
}
/* Invote print_string_ptr (which is useful) on an item. */
static char *print_string(voJSON *item)	{
	return print_string_ptr(item->valuestring);
}

/* Predeclare these prototypes. */
static const char *parse_value(voJSON *item,const char *value);
static char *print_value(voJSON *item,int depth,int fmt);
static const char *parse_array(voJSON *item,const char *value);
static char *print_array(voJSON *item,int depth,int fmt);
static const char *parse_object(voJSON *item,const char *value);
static char *print_object(voJSON *item,int depth,int fmt);

/* Utility to jump whitespace and cr/lf */
static const char *skip(const char *in) {while (in && *in && (unsigned char)*in<=32) in++; return in;}

/* Parse an object - create a new root, and populate. */
voJSON *voJSON_Parse(const char *value)
{
	voJSON *c=voJSON_New_Item();
	ep=0;
	if (!c) return 0;       /* memory fail */

	if (!parse_value(c,skip(value))) {voJSON_Delete(c);return 0;}
	return c;
}

/* Render a voJSON item/entity/structure to text. */
char *voJSON_Print(voJSON *item)				{return print_value(item,0,1);}
char *voJSON_PrintUnformatted(voJSON *item)	{return print_value(item,0,0);}

/* Parser core - when encountering text, process appropriately. */
static const char *parse_value(voJSON *item,const char *value)
{
	if (!value)						return 0;	/* Fail on null. */
	if (!strncmp(value,"null",4))	{ item->type=voJSON_NULL;  return value+4; }
	if (!strncmp(value,"false",5))	{ item->type=voJSON_False; return value+5; }
	if (!strncmp(value,"true",4))	{ item->type=voJSON_True; item->valueint=1;	return value+4; }
	if (*value=='\"')				{ return parse_string(item,value); }
	if (*value=='-' || (*value>='0' && *value<='9'))	{ return parse_number(item,value); }
	if (*value=='[')				{ return parse_array(item,value); }
	if (*value=='{')				{ return parse_object(item,value); }

	ep=value;
	return 0;	/* failure. */
}

/* Render a value to text. */
static char *print_value(voJSON *item,int depth,int fmt)
{
	char *out=0;
	if (!item) return 0;
	switch ((item->type)&255)
	{
	case voJSON_NULL:	out=voJSON_strdup("null");	break;
	case voJSON_False:	out=voJSON_strdup("false");break;
	case voJSON_True:	out=voJSON_strdup("true"); break;
	case voJSON_Number:	out=print_number(item);break;
	case voJSON_String:	out=print_string(item);break;
	case voJSON_Array:	out=print_array(item,depth,fmt);break;
	case voJSON_Object:	out=print_object(item,depth,fmt);break;
	}
	return out;
}

/* Build an array from input text. */
static const char *parse_array(voJSON *item,const char *value)
{
	voJSON *child;
	if (*value!='[')	{ep=value;return 0;}	/* not an array! */

	item->type=voJSON_Array;
	value=skip(value+1);
	if (*value==']') return value+1;	/* empty array. */

	item->child=child=voJSON_New_Item();
	if (!item->child) return 0;		 /* memory fail */
	value=skip(parse_value(child,skip(value)));	/* skip any spacing, get the value. */
	if (!value) return 0;

	while (*value==',')
	{
		voJSON *new_item;
		if (!(new_item=voJSON_New_Item())) return 0; 	/* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_value(child,skip(value+1)));
		if (!value) return 0;	/* memory fail */
	}

	if (*value==']') return value+1;	/* end of array */
	ep=value;return 0;	/* malformed. */
}

/* Render an array to text */
static char *print_array(voJSON *item,int depth,int fmt)
{
	char **entries;
	char *out=0,*ptr,*ret;int len=5;
	voJSON *child=item->child;
	int numentries=0,i=0,fail=0;

	/* How many entries in the array? */
	while (child) numentries++,child=child->next;
	/* Allocate an array to hold the values for each */
	entries=(char**)voJSON_malloc(numentries*sizeof(char*));
	if (!entries) return 0;
	memset(entries,0,numentries*sizeof(char*));
	/* Retrieve all the results: */
	child=item->child;
	while (child && !fail)
	{
		ret=print_value(child,depth+1,fmt);
		entries[i++]=ret;
		if (ret) len+=strlen(ret)+2+(fmt?1:0); else fail=1;
		child=child->next;
	}

	/* If we didn't fail, try to malloc the output string */
	if (!fail) out=(char*)voJSON_malloc(len);
	/* If that fails, we fail. */
	if (!out) fail=1;

	/* Handle failure. */
	if (fail)
	{
		for (i=0;i<numentries;i++) 
			if (entries[i])
				voJSON_free(entries[i]);
		voJSON_free(entries);
		return 0;
	}

	/* Compose the output array. */
	*out='[';
	ptr=out+1;
	*ptr=0;
	for (i=0;i<numentries;i++)
	{
		strcpy_s(ptr, out+len-ptr, entries[i]);
		ptr+=strlen(entries[i]);
		if (i!=numentries-1) {
			*ptr++=',';
			if (fmt) 
				*ptr++=' ';
			*ptr=0;
		}
		voJSON_free(entries[i]);
	}
	voJSON_free(entries);
	*ptr++=']';
	*ptr++=0;
	return out;	
}

/* Build an object from the text. */
static const char *parse_object(voJSON *item,const char *value)
{
	voJSON *child;
	if (*value!='{')	{ep=value;return 0;}	/* not an object! */

	item->type=voJSON_Object;
	value=skip(value+1);
	if (*value=='}') return value+1;	/* empty array. */

	item->child=child=voJSON_New_Item();
	if (!item->child) return 0;
	value=skip(parse_string(child,skip(value)));
	if (!value) return 0;
	child->string=child->valuestring;child->valuestring=0;
	if (*value!=':') {ep=value;return 0;}	/* fail! */
	value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
	if (!value) return 0;

	while (*value==',')
	{
		voJSON *new_item;
		if (!(new_item=voJSON_New_Item()))	return 0; /* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_string(child,skip(value+1)));
		if (!value) return 0;
		child->string=child->valuestring;child->valuestring=0;
		if (*value!=':') {ep=value;return 0;}	/* fail! */
		value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
		if (!value) return 0;
	}

	if (*value=='}') return value+1;	/* end of array */
	ep=value;
	return 0;	/* malformed. */
}

/* Render an object to text. */
static char *print_object(voJSON *item,int depth,int fmt)
{
	char **entries=0,**names=0;
	char *out=0,*ptr,*ret,*str;int len=7,i=0,j;
	voJSON *child=item->child;
	int numentries=0,fail=0;
	/* Count the number of entries. */
	while (child) numentries++,child=child->next;
	/* Allocate space for the names and the objects */
	entries=(char**)voJSON_malloc(numentries*sizeof(char*));
	if (!entries) return 0;
	names=(char**)voJSON_malloc(numentries*sizeof(char*));
	if (!names) {
		voJSON_free(entries);
		return 0;
	}
	memset(entries,0,sizeof(char*)*numentries);
	memset(names,0,sizeof(char*)*numentries);

	/* Collect all the results into our arrays: */
	child=item->child;depth++;if (fmt) len+=depth;
	while (child)
	{
		names[i]=str=print_string_ptr(child->string);
		entries[i++]=ret=print_value(child,depth,fmt);
		if (str && ret) 
			len+=strlen(ret)+strlen(str)+2+(fmt? 2+depth : 0); 
		else 
			fail=1;

		child=child->next;
	}

	/* Try to allocate the output string */
	if (!fail) 
		out=(char*)voJSON_malloc(len);

	if (!out) fail=1;

	/* Handle failure */
	if (fail)
	{
		for (i=0;i<numentries;i++) {
			if (names[i]) 
				voJSON_free(names[i]);
			if (entries[i])
				voJSON_free(entries[i]);
		}
		voJSON_free(names);
		voJSON_free(entries);
		return 0;
	}

	/* Compose the output: */
	*out='{';ptr=out+1;
	if (fmt)
		*ptr++='\n';
	*ptr=0;
	for (i=0; i<numentries; i++)
	{
		if (fmt) {
			for (j=0;j<depth;j++)
				*ptr++='\t';
		}

		strcpy_s(ptr, out+len-ptr, names[i]);
		ptr += strlen(names[i]);
		*ptr++=':';
		if (fmt)
			*ptr++='\t';

		strcpy_s(ptr, out+len-ptr, entries[i]);
		ptr += strlen(entries[i]);
		if (i != numentries-1) 
			*ptr++=',';
		if (fmt)
			*ptr++='\n';
		*ptr=0;
		voJSON_free(names[i]);
		voJSON_free(entries[i]);
	}

	voJSON_free(names);
	voJSON_free(entries);
	if (fmt) 
		for (i=0;i<depth-1;i++) 
			*ptr++='\t';

	*ptr++='}';
	*ptr++=0;
	return out;	
}

/* Get Array size/item / object item. */
int    voJSON_GetArraySize(voJSON *array) {

	voJSON *c= NULL;
	int i=0;
	if (array == NULL)
		return 0;

	c= array->child;
	while(c) {
		i++;
		c=c->next;
	}
	return i;
}

voJSON *voJSON_GetArrayItem(voJSON *array,int item)	{

	voJSON *c = NULL;
	if (array == NULL)
		return NULL;

	c=array->child;  
	while (c && item>0) {
		item--;
		c=c->next;
	}
	return c;
}
voJSON *voJSON_GetObjectItem(voJSON *object,const char *string)	{
	
	voJSON *c = NULL;
	if (object == NULL)
		return NULL;

	c=object->child; 
	while (c && voJSON_strcasecmp(c->string,string))
		c=c->next; 
	return c;
}

/* Utility for array list handling. */
static void suffix_object(voJSON *prev,voJSON *item) {

	if (prev != NULL)
		prev->next=item;

	if (item != NULL)
		item->prev=prev;
}
/* Utility for handling references. */
static voJSON *create_reference(voJSON *item) {

	voJSON *ref=voJSON_New_Item();
	if (!ref) return 0;

	memcpy(ref,item,sizeof(voJSON));
	ref->string=0;
	ref->type|=voJSON_IsReference;
	ref->next=ref->prev=0;
	return ref;
}

/* Add item to array/object. */
void   voJSON_AddItemToArray(voJSON *array, voJSON *item) {

	voJSON *c = NULL;
	if (array == NULL || item == NULL) 
		return;

	c=array->child;

	if (!c) {
		array->child=item;
	} else {
		while (c && c->next) 
			c=c->next; 
		suffix_object(c,item);
	}
}

void   voJSON_AddItemToObject(voJSON *object,const char *string,voJSON *item)	{
	if (!item) return; 
	if (item->string) 
		voJSON_free(item->string);
	item->string=voJSON_strdup(string);
	voJSON_AddItemToArray(object,item);
}

void	voJSON_AddItemReferenceToArray(voJSON *array, voJSON *item)	{
	voJSON_AddItemToArray(array,create_reference(item));
}

void	voJSON_AddItemReferenceToObject(voJSON *object,const char *string,voJSON *item)	{
	voJSON_AddItemToObject(object,string,create_reference(item));
}


voJSON *voJSON_DetachItemFromArray(voJSON *array,int which)	{
	voJSON *c=array->child;
	while (c && which>0) {
		c=c->next;
		which--;
	}
	if (!c) 
		return 0;

	if (c->prev) 
		c->prev->next=c->next;
	if (c->next) 
		c->next->prev=c->prev;
	if (c==array->child) 
		array->child=c->next;
	c->prev=c->next=0;
	return c;
}

void   voJSON_DeleteItemFromArray(voJSON *array,int which) {
	voJSON_Delete(voJSON_DetachItemFromArray(array,which));
}

voJSON *voJSON_DetachItemFromObject(voJSON *object,const char *string) {

	int i=0;
	voJSON *c = NULL;
	if (object == NULL)
		return NULL;

	c=object->child;
	while (c && voJSON_strcasecmp(c->string,string)) {
		i++;
		c=c->next;
	}
	if (c) 
		return voJSON_DetachItemFromArray(object,i);

	return 0;
}

void   voJSON_DeleteItemFromObject(voJSON *object,const char *string) {
	voJSON_Delete(voJSON_DetachItemFromObject(object,string));
}

/* Replace array/object items with new ones. */
void   voJSON_ReplaceItemInArray(voJSON *array,int which,voJSON *newitem) {

	voJSON *c = NULL;
	if (array == NULL)
		return;

	c=array->child;
	while (c && which>0) {
		c=c->next;
		which--;
	}
	if (!c) return;

	newitem->next=c->next;
	newitem->prev=c->prev;
	if (newitem->next) 
		newitem->next->prev=newitem;

	if (c==array->child) 
		array->child=newitem;
	else 
		newitem->prev->next=newitem;
	c->next=c->prev=0;
	voJSON_Delete(c);
}

void   voJSON_ReplaceItemInObject(voJSON *object,const char *string,voJSON *newitem) {

	int i=0;
	voJSON *c=NULL;
	if (object == NULL)
		return;

	c=object->child;
	while(c && voJSON_strcasecmp(c->string,string)) {
		i++;
		c=c->next;
	}

	if (c) {
		newitem->string=voJSON_strdup(string);
		voJSON_ReplaceItemInArray(object,i,newitem);
	}
}

/* Create basic types: */
voJSON *voJSON_CreateNull()	{
	voJSON *item=voJSON_New_Item();
	if (item)
		item->type=voJSON_NULL;
	return item;
}

voJSON *voJSON_CreateTrue()	{
	voJSON *item=voJSON_New_Item();
	if (item)item->type=voJSON_True;
	return item;
}

voJSON *voJSON_CreateFalse() {
	voJSON *item=voJSON_New_Item();
	if (item)
		item->type=voJSON_False;
	return item;
}

voJSON *voJSON_CreateBool(int b) {
	voJSON *item=voJSON_New_Item();
	if (item)item->type=b?voJSON_True:voJSON_False;
	return item;
}

voJSON *voJSON_CreateNumber(double num)	{

	voJSON *item=voJSON_New_Item();
	if (item) {
		item->type=voJSON_Number;
		item->valuedouble=num;
		item->valueint=(int)num;
	}

	return item;
}

voJSON *voJSON_CreateString(const char *string)	{
	voJSON *item=voJSON_New_Item();
	if (item) {
		item->type=voJSON_String;
		item->valuestring = voJSON_strdup(string);
	}
	return item;
}

voJSON *voJSON_CreateArray() {
	voJSON *item=voJSON_New_Item();
	if (item)
		item->type=voJSON_Array;
	return item;
}

voJSON *voJSON_CreateObject() {
	voJSON *item=voJSON_New_Item();
	if (item)
		item->type=voJSON_Object;
	return item;
}

/* Create Arrays: */
voJSON *voJSON_CreateIntArray(int *numbers,int count) {
	int i;
	voJSON *n=0,*p=0,*a=voJSON_CreateArray();
	for(i=0;a && i<count;i++) {
		n=voJSON_CreateNumber(numbers[i]);
		if (!i)
			a->child=n;
		else
			suffix_object(p,n);
		p=n;
	}
	return a;
}

voJSON *voJSON_CreateFloatArray(float *numbers,int count) {
	int i;
	voJSON *n=0,*p=0,*a=voJSON_CreateArray();
	for (i=0;a && i<count;i++) {
		n=voJSON_CreateNumber(numbers[i]);
		if (!i)
			a->child=n;
		else
			suffix_object(p,n);
		p=n;
	}
	return a;
}

voJSON *voJSON_CreateDoubleArray(double *numbers,int count)		{
	int i;
	voJSON *n=0,*p=0,*a=voJSON_CreateArray();
	for(i=0;a && i<count;i++){
		n=voJSON_CreateNumber(numbers[i]);
		if (!i)
			a->child=n;
		else 
			suffix_object(p,n);
		p=n;
	}
	return a;
}

voJSON *voJSON_CreateStringArray(const char **strings,int count)	{
	int i;
	voJSON *n=0,*p=0,*a=voJSON_CreateArray();
	for(i=0;a && i<count;i++){
		n=voJSON_CreateString(strings[i]);
		if (!i)
			a->child=n;
		else 
			suffix_object(p,n);
		p=n;
	}
	return a;
}
