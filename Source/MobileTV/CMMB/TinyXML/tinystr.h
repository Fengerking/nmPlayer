
#pragma once
#ifndef TIXML_USE_STL

#ifndef TIXML_STRING_INCLUDED
#define TIXML_STRING_INCLUDED

#include <assert.h>
#include <string.h>
/*	The support for explicit isn't that universal, and it isn't really
	required - it is used to check that the TiXmlString class isn't incorrectly
	used. Be nice to old compilers and macro it here:
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200 )
	// Microsoft visual studio, version 6 and higher.
	#define TIXML_EXPLICIT explicit
#elif defined(__GNUC__) && (__GNUC__ >= 3 )
	// GCC version 3 and higher.s
	#define TIXML_EXPLICIT explicit
#else
	#define TIXML_EXPLICIT
#endif


/*
   TiXmlString is an emulation of a subset of the std::string template.
   Its purpose is to allow compiling TinyXML on compilers with no or poor STL support.
   Only the member functions relevant to the TinyXML project have been implemented.
   The buffer allocation is made by a simplistic power of 2 like mechanism : if we increase
   a string and there's no more room, we allocate a buffer twice as big as we need.
*/
// The size type used
typedef size_t size_type;
typedef struct tagRep{
	size_type size, capacity;
	char str[1];
}Rep;


class TiXmlString
{
  public :
	static const size_type npos; // = -1;

	TiXmlString ();

	TiXmlString ( const TiXmlString & copy);
	TIXML_EXPLICIT TiXmlString ( const char * copy);
	TIXML_EXPLICIT TiXmlString ( const char * str, size_type len);

	~TiXmlString ();
	TiXmlString& operator = (const char * copy);
	TiXmlString& operator = (const TiXmlString & copy);
	TiXmlString& operator += (const char * suffix);
	TiXmlString& operator += (char single);
	TiXmlString& operator += (const TiXmlString & suffix);
	const char * c_str () const;
	const char * data () const;
	size_type length () const;
	size_type size () const; 
	bool empty () const; 
	size_type capacity () const;
	const char& at (size_type index) const;
	char& operator [] (size_type index) const;
	size_type find (char lookup) const;
	size_type find (char tofind, size_type offset) const;
	void clear ();
	void reserve (size_type cap);
	TiXmlString& assign (const char* str, size_type len);
	TiXmlString& append (const char* str, size_type len);
	void swap (TiXmlString& other);

  private:

	void init(size_type sz);
	void set_size(size_type sz); 
	char* start() const; 
	char* finish() const; 

	/*struct Rep
	{
		size_type size, capacity;
		char str[1];
	};*/

	void init(size_type sz, size_type cap);
	void quit();

	Rep * rep_;
	//static Rep nullrep_;
};

inline bool operator == (const TiXmlString & a, const TiXmlString & b)
{
	return    ( a.length() == b.length() )				// optimization on some platforms
	       && ( strcmp(a.c_str(), b.c_str()) == 0 );	// actual compare
}

inline bool operator < (const TiXmlString & a, const TiXmlString & b)
{
	return strcmp(a.c_str(), b.c_str()) < 0;
}

inline bool operator != (const TiXmlString & a, const TiXmlString & b) { return !(a == b); }
inline bool operator >  (const TiXmlString & a, const TiXmlString & b) { return b < a; }
inline bool operator <= (const TiXmlString & a, const TiXmlString & b) { return !(b < a); }
inline bool operator >= (const TiXmlString & a, const TiXmlString & b) { return !(a < b); }

inline bool operator == (const TiXmlString & a, const char* b) { return strcmp(a.c_str(), b) == 0; }
inline bool operator == (const char* a, const TiXmlString & b) { return b == a; }
inline bool operator != (const TiXmlString & a, const char* b) { return !(a == b); }
inline bool operator != (const char* a, const TiXmlString & b) { return !(b == a); }

TiXmlString operator + (const TiXmlString & a, const TiXmlString & b);
TiXmlString operator + (const TiXmlString & a, const char* b);
TiXmlString operator + (const char* a, const TiXmlString & b);


/*
   TiXmlOutStream is an emulation of std::ostream. It is based on TiXmlString.
   Only the operators that we need for TinyXML have been developped.
*/
class TiXmlOutStream : public TiXmlString
{
public :

	// TiXmlOutStream << operator.
	TiXmlOutStream & operator << (const TiXmlString & in)
	{
		*this += in;
		return *this;
	}

	// TiXmlOutStream << operator.
	TiXmlOutStream & operator << (const char * in)
	{
		*this += in;
		return *this;
	}

} ;

#endif	// TIXML_STRING_INCLUDED
#endif	// TIXML_USE_STL
