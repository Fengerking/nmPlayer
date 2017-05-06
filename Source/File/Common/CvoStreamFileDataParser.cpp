#include "CvoStreamFileDataParser.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CvoStreamFileDataParser::CvoStreamFileDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
: CvoFileDataParser(pFileChunk, pMemOp)
, m_nFileSize(0)
{
}

CvoStreamFileDataParser::~CvoStreamFileDataParser(void)
{
}

