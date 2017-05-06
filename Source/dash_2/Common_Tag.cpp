#include "Common_Tag.h"

Common_Tag::Common_Tag(void)
{
}

Common_Tag::~Common_Tag(void)
{
}

int Common_Tag::StrCompare(const char* p1, const char* p2)
{
#if defined _WIN32
	return stricmp(p1, p2);
#elif defined LINUX
	return strcasecmp(p1, p2);
#elif defined _IOS
	return strcasecmp(p1, p2);
#elif defined _MAC_OS
	return strcasecmp(p1, p2);
#endif
}

VO_VOID Common_Tag::replace(char *source,const char *sub,const char *rep )
{

	int in,out;
	in = out = 0;
	int lsub = strlen(sub);
	int lsour = strlen(source);
	char tmp[155];
	char *p = source;
	char sourceclone[1024];
	memset(sourceclone,0x00,1024);
	memcpy(sourceclone,source,lsour);
	while(lsour >= lsub)
	{
		memset(tmp,0x00,155);
		memcpy(tmp,p,lsub);
		if(strcmp(tmp,sub)==0)
		{
			out = in +lsub;
			break;
		}
		in ++;
		p++;
		lsour -- ;
	}
	if(out >0 && in != out)
	{
		memset(source, 0 ,strlen(source));
		memcpy(source, sourceclone,in);
		strcat(source,rep);
		strcat(source, &sourceclone[out]);
	}
}
VO_S32 Common_Tag::lastIndex(const char *s,const char *t){
	const char *s1=s;
	const char *t1=t;
	int n1=strlen(s1);
	int n2=strlen(t1);
	int i=0;
	int j=0;
	bool find=false;
	int loc=0;
	while(i<n1&&j<n2)
	{

		if(*(s1+i)==*(t1+j))
		{
			const char *s2=s1+i+1;
			const char *t2=t1+j+1;
			while(*s2!='\0'&&*t2!='\0')
			{
				if(*s2==*t2)
				{
					s2++;
					t2++;
				}
				else
				{
					break;
				}
			}
	
			if(*t2=='\0')
			{
				find=true;
				loc=i;
			}
			i++;
		}
		
		else
		{
			i++;
		}
	}
	if(find)return loc;
	else return -1;

}
