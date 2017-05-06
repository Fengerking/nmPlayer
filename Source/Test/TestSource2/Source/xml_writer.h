#ifndef __XML_WRITER_H__

#define __XML_WRITER_H__

#include "voType.h"
#include "xml_element.h"
#include "xml_document.h"
#include "indent_file_writer.h"

enum XML_ENCODING_TYPE
{
	XML_ENCODING_TYPE_UTF8,
};

class xml_writer
{
public:
	xml_writer( const VO_CHAR * str_filename , XML_ENCODING_TYPE encodeing_type );
	~xml_writer(void);

	void write_document_start( const VO_CHAR * str_doc_name );
	void write_document_end();

	void write_element_start( const VO_CHAR * str_element_name );
	void write_element_value( const VO_CHAR * str_value );
	void write_element_value( VO_U64 value );
	void write_element_attribute( const VO_CHAR * str_element_attrib_name , const VO_CHAR * str_value );
	void write_element_attribute( const VO_CHAR * str_element_attrib_name , VO_U64 value );
	void write_element_end();

private:
	xml_document * m_ptr_doc;
	xml_element * m_ptr_cur_element;

	indent_file_writer m_writer;
};

#endif __XML_WRITER_H__
