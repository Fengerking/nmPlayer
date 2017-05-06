
#ifndef __XML_ELEMENT_H__

#define __XML_ELEMENT_H__

#include "voYYDef_SrcCmn.h"
#include "vo_doublelink_list.hpp"
#include "indent_file_writer.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class xml_element_attrib
{
public:
	xml_element_attrib();
	~xml_element_attrib();

	void set_name( const VO_CHAR * str_name );
	void set_value( const VO_CHAR * str_value );

	VO_CHAR * get_name();
	VO_CHAR * get_value();

protected:
	VO_CHAR * m_str_name;
	VO_U32 m_name_size;

	VO_CHAR * m_str_value;
	VO_U32 m_value_size;
};

class xml_element
{
public:
	xml_element(void);
	~xml_element(void);

	void set_name( const VO_CHAR * str_name );
	void set_value( const VO_CHAR * str_value );

	xml_element * create_child();
	xml_element_attrib * create_attrib();

	xml_element * get_parent();

	virtual void serialize( indent_file_writer * ptr_writer );

protected:
	void set_parent( xml_element * ptr_element );

protected:
	vo_doublelink_list< xml_element * > m_children;
	vo_doublelink_list< xml_element_attrib * > m_attribs;

	VO_CHAR * m_str_name;
	VO_U32 m_name_size;

	VO_CHAR * m_str_value;
	VO_U32 m_value_size;

	bool m_is_value_set;

	xml_element * m_ptr_parent;
};


#ifdef _VONAMESPACE
}
#endif


#endif