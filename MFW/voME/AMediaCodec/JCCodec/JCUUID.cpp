#include <stdio.h>
#include <assert.h>

//#define __DEBUG
#include "JCUUID.h"

#define LOG_TAG "JCUUID"





JCUUID::JCUUID(JNIEnv* env):JCObject(env, "java/util/UUID"),
							mFromString(NULL),
							mNameUUIDFromBytes(NULL)
{
	
}

JCUUID::~JCUUID()
{
	
}

bool JCUUID::onInit()
{
	assert(mEnv != NULL);
	if(mEnv)
	{	
		struct _NameInfo
		{
			jmethodID&      id;
			const char*     name;
			const char*     sig;
		} infos[] =
		  {
			  {mFromString,          "fromString",          "(Ljava/lang/String;)Ljava/util/UUID;"},
			  {mNameUUIDFromBytes,   "nameUUIDFromBytes",   "([B)Ljava/util/UUID;"},
		  };
		for(int i = 0 ; i < sizeof(infos)/sizeof(infos[0]); i++)
		{
			infos[i].id = mEnv->GetStaticMethodID(mClass, infos[i].name, infos[i].sig);
			if(infos[i].id == NULL)
			{
				LOGE("can not find the method = %s sig = %s\n", infos[i].name, infos[i].sig);
				return false;
			}
		}
	}
	return true;
}

bool JCUUID::createFromString(const char* inStr)
{
	if(!mEnv || !mClass || !inStr || !mFromString)
		return false;
	
	return createObjFromChar(inStr, mFromString);
}
bool JCUUID::createFromBytes(const unsigned char* inBytes, int inLen)
{
	assert(inLen == 128);
	if(!mEnv || !mClass || !inBytes || !mNameUUIDFromBytes)
		return false;
	
	return createObjFromBytes(inBytes, inLen, mNameUUIDFromBytes);
}

