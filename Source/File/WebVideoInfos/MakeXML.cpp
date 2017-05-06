#include "MakeXML.h"

CMakeXML::CMakeXML(void)
{
}

CMakeXML::~CMakeXML(void)
{
}

VO_U32 CMakeXML::SpecialCharacters(VO_PCHAR pSource)
{
	VO_CHAR szTile[256] = {0};
	VO_PCHAR poffset = szTile;

	memcpy(szTile, pSource, strlen(pSource));
	memset(pSource, 0, 256);

	while (*poffset != '\0')
	{		
		switch (*poffset)
		{
			case 0x3C:	//<
				{
					memcpy(pSource, "&lt;", 4);
					pSource += 4;

					break;
				}
			case 0x3E:	//>
				{
					memcpy(pSource, "&gt;", 4);
					pSource += 4;

					break;
				}
			case 0x26:	//&
				{
					if ( 0 == memcmp(poffset, "&lt;", 4) ||
						0 == memcmp(poffset, "&gt;", 4) ||
						0 == memcmp(poffset, "&amp;", 5) ||
						0 == memcmp(poffset, "&apos;", 6) ||
						0 == memcmp(poffset, "&quot;", 6) )
					{
						memcpy(pSource, poffset, 1);
						pSource += 1;
					}
					else
					{
						memcpy(pSource, "&amp;", 5);
						pSource += 5;
					}
					
					break;
				}
			case 0x27:	//'
				{
					memcpy(pSource, "&apos;", 6);
					pSource += 6;

					break;
				}
			case 0x22:	//"
				{
					memcpy(pSource, "&quot;", 6);
					pSource += 6;

					break;
				}
			default:
				{
					memcpy(pSource, poffset, 1);
					pSource += 1;				
				}
		}

		poffset++;
	}

	return 0;
}
