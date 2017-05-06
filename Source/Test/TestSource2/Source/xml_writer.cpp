#include "xml_writer.h"
#include "voString.h"

xml_writer::xml_writer( const VO_CHAR * str_filename , XML_ENCODING_TYPE encodeing_type )
:m_ptr_cur_element(0)
,m_ptr_doc(0)
,m_writer( str_filename )
{
}

xml_writer::~xml_writer(void)
{
	delete m_ptr_doc;
}

void xml_writer::write_document_start( const VO_CHAR * str_doc_name )
{
	m_ptr_cur_element = m_ptr_doc = new xml_document;
	m_ptr_doc->set_name( str_doc_name );
}

void xml_writer::write_document_end()
{
	m_ptr_doc->serialize( &m_writer );
}

void xml_writer::write_element_start( const VO_CHAR * str_element_name )
{
	m_ptr_cur_element = m_ptr_cur_element->create_child();
	m_ptr_cur_element->set_name( str_element_name );
}

void xml_writer::write_element_value( const VO_CHAR * str_value )
{
	m_ptr_cur_element->set_value( str_value );
}

void xml_writer::write_element_value( VO_U64 value )
{
	VO_CHAR temp[255];
	memset( temp , 0 , 255 );
	sprintf( temp , "%lld" , value );

	write_element_value( temp );
}

void xml_writer::write_element_attribute( const VO_CHAR * str_element_attrib_name , const VO_CHAR * str_value )
{
	xml_element_attrib * ptr = m_ptr_cur_element->create_attrib();
	ptr->set_name( str_element_attrib_name );
	ptr->set_value( str_value );
}

void xml_writer::write_element_attribute( const VO_CHAR * str_element_attrib_name , VO_U64 value )
{
	VO_CHAR temp[255];
	memset( temp , 0 , 255 );
	sprintf( temp , "%lld" , value );

	write_element_attribute( str_element_attrib_name , temp );
}

void xml_writer::write_element_end()
{
	m_ptr_cur_element = m_ptr_cur_element->get_parent();
}
