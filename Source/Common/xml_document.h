#ifndef __XML_DOCUMENT_H__

#define __XML_DOCUMENT_H__

#include "voYYDef_SrcCmn.h"
#include "xml_element.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
class xml_document :
	public xml_element
{
public:
	xml_document(void);
	virtual ~xml_document(void);

	virtual void serialize( indent_file_writer * ptr_writer );
};
#ifdef _VONAMESPACE
}
#endif
#endif
