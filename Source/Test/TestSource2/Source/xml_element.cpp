#include "xml_element.h"

#include "voString.h"

xml_element_attrib::xml_element_attrib()
:m_str_name(0)
,m_name_size(0)
,m_str_value(0)
,m_value_size(0)
{
}

xml_element_attrib::~xml_element_attrib()
{
	if( m_str_name )
		delete []m_str_name;

	m_str_name = 0;
	m_name_size = 0;

	if( m_str_value )
		delete []m_str_value;

	m_str_value = 0;
	m_value_size = 0;
}

void xml_element_attrib::set_name( const VO_CHAR * str_name )
{
	int len = strlen( str_name );

	if( len == 0 )
		len = 1;

	if( len > m_name_size )
	{
		delete []m_str_name;
		m_str_name = new VO_CHAR[len + 1];
		m_name_size = len + 1;
	}

	memset( m_str_name , 0 , sizeof( VO_CHAR ) * m_name_size );

	strcpy( m_str_name , str_name );
}

void xml_element_attrib::set_value( const VO_CHAR * str_value )
{
	int len = strlen( str_value );

	if( len == 0 )
		len = 1;

	if( len > m_value_size )
	{
		delete []m_str_value;
		m_str_value = new VO_CHAR[len + 1];
		m_value_size = len + 1;
	}

	memset( m_str_value , 0 , sizeof( VO_CHAR ) * m_value_size );

	strcpy( m_str_value , str_value );
}

VO_CHAR * xml_element_attrib::get_name()
{
	return m_str_name;
}

VO_CHAR * xml_element_attrib::get_value()
{
	return m_str_value;
}

xml_element::xml_element(void)
:m_str_name(0)
,m_name_size(0)
,m_ptr_parent(0)
,m_str_value(0)
,m_value_size(0)
,m_is_value_set(false)
{
}

xml_element::~xml_element(void)
{
	if( m_str_name )
		delete []m_str_name;

	m_str_name = 0;
	m_name_size = 0;

	vo_doublelink_list< xml_element * >::iterator children_iter = m_children.begin();
	vo_doublelink_list< xml_element * >::iterator children_itere = m_children.end();

	while( children_iter != children_itere )
	{
		delete *children_iter;
		children_iter++;
	}

	vo_doublelink_list< xml_element_attrib * >::iterator attribs_iter = m_attribs.begin();
	vo_doublelink_list< xml_element_attrib * >::iterator attribs_itere = m_attribs.end();

	while( attribs_iter != attribs_itere )
	{
		delete *attribs_iter;
		attribs_iter++;
	}
}

void xml_element::set_name( const VO_CHAR * str_name )
{
	int len = strlen( str_name );

	if( len == 0 )
		len = 1;

	if( len > m_name_size )
	{
		delete []m_str_name;
		m_str_name = new VO_CHAR[len + 1];
		m_name_size = len + 1;
	}

	memset( m_str_name , 0 , sizeof( VO_CHAR ) * m_name_size );

	strcpy( m_str_name , str_name );
}

void xml_element::set_value( const VO_CHAR * str_value )
{
	int len = strlen( str_value );

	if( len == 0 )
		len = 1;

	if( len > m_value_size )
	{
		delete []m_str_value;
		m_str_value = new VO_CHAR[len + 1];
		m_value_size = len + 1;
	}

	memset( m_str_value , 0 , sizeof( VO_CHAR ) * m_value_size );

	strcpy( m_str_value , str_value );

	m_is_value_set = true;
}

xml_element * xml_element::create_child()
{
	xml_element * ptr_temp = new xml_element;
	ptr_temp->set_parent( this );

	m_children.push_back( ptr_temp );

	return ptr_temp;
}

xml_element_attrib * xml_element::create_attrib()
{
	xml_element_attrib * ptr_temp = new xml_element_attrib;

	m_attribs.push_back( ptr_temp );

	return ptr_temp;
}

xml_element * xml_element::get_parent()
{
	return m_ptr_parent;
}

void xml_element::serialize( indent_file_writer * ptr_writer )
{
	ptr_writer->add_indent();

	VO_CHAR * ptr_cache = new VO_CHAR[1024];
	memset( ptr_cache , 0 , 1024 );

	strcat( ptr_cache , "<" );
	strcat( ptr_cache , m_str_name );

	vo_doublelink_list< xml_element_attrib * >::iterator attribs_iter = m_attribs.begin();
	vo_doublelink_list< xml_element_attrib * >::iterator attribs_itere = m_attribs.end();

	while( attribs_iter != attribs_itere )
	{
		strcat( ptr_cache , " " );
		strcat( ptr_cache , (*attribs_iter)->get_name() );
		strcat( ptr_cache , "=\"" );
		strcat( ptr_cache , (*attribs_iter)->get_value() );
		strcat( ptr_cache , "\"" );

		attribs_iter++;
	}

	strcat( ptr_cache , ">" );

	if( m_is_value_set )
		strcat( ptr_cache , m_str_value );

	if( m_children.count() > 0 )
	{
		ptr_writer->write_string( ptr_cache );
		memset( ptr_cache , 0 , 1024 );
	}

	vo_doublelink_list< xml_element * >::iterator children_iter = m_children.begin();
	vo_doublelink_list< xml_element * >::iterator children_itere = m_children.end();

	while( children_iter != children_itere )
	{
		(*children_iter)->serialize( ptr_writer );
		children_iter++;
	}

	strcat( ptr_cache , "</" );
	strcat( ptr_cache , m_str_name );
	strcat( ptr_cache , ">" );

	ptr_writer->write_string( ptr_cache );

	delete []ptr_cache;

	ptr_writer->release_indent();
}

void xml_element::set_parent( xml_element * ptr_element )
{
	m_ptr_parent = ptr_element;
}
