#include "xml_document.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
xml_document::xml_document(void)
{
}

xml_document::~xml_document(void)
{
}

void xml_document::serialize( indent_file_writer * ptr_writer )
{
	ptr_writer->write_string( "<?xml version='1.0' encoding='UTF-8'?>" );
	xml_element::serialize( ptr_writer );
}
