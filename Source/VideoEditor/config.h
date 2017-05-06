/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		config.h

Contains:	config header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-05		Leon			Create file
*******************************************************************************/

#ifndef INI_FILE_H_
#define  INI_FILE_H_ 
#include  "stdio.h"
#include  "stdlib.h" 
//#include  < assert.h > 
#include  "string.h"
#include  "ctype.h" 


int  read_profile_string(  const   char   * section,  const   char   * key, char   * value,  int  size,   const char *default_value,const   char   * file);
int  read_profile_int(  const   char   * section,  const   char   * key, int  default_value,  const   char   * file);
int  clean_ini_file(const char *file);
int  write_profile_string(  const   char   * section,  const   char   * key, const   char   * value,  const   char   * file);


#endif   // end of INI_FILE_H_ 
