
#ifndef TIXML_USE_STL
#include "tinystr.h"

static Rep nullrep_ = {0, 0, '\0'};
const size_type TiXmlString::npos = static_cast<size_type >(-1);

void TiXmlString::reserve(size_type cap) {
	if (cap > capacity()) {
		TiXmlString tmp;
		tmp.init(length(), cap);
		memcpy(tmp.start(), data(), length());
		swap(tmp);
	}
}

TiXmlString& TiXmlString::assign(const char* str, size_type len) {
	size_type cap = capacity();
	if (len > cap || cap > 3*(len + 8)) {
		TiXmlString tmp;
		tmp.init(len);
		memcpy(tmp.start(), str, len);
		swap(tmp);
	} else {
		memmove(start(), str, len);
		set_size(len);
	}
	return *this;
}


TiXmlString& TiXmlString::append(const char* str, size_type len) {
	size_type newsize = length() + len;
	if (newsize > capacity()) {
		reserve(newsize + capacity());
	}
	memmove(finish(), str, len);
	set_size(newsize);
	return *this;
}


TiXmlString operator +(const TiXmlString & a, const TiXmlString & b) {
	TiXmlString tmp;
	tmp.reserve(a.length() + b.length());
	tmp += a;
	tmp += b;
	return tmp;
}

TiXmlString operator +(const TiXmlString & a, const char* b) {
	TiXmlString tmp;
	size_type b_len = static_cast<size_type>( strlen(b) );
	tmp.reserve(a.length() + b_len);
	tmp += a;
	tmp.append(b, b_len);
	return tmp;
}

TiXmlString operator +(const char* a, const TiXmlString & b) {
	TiXmlString tmp;
	size_type a_len = static_cast<size_type>( strlen(a) );
	tmp.reserve(a_len + b.length());
	tmp.append(a, a_len);
	tmp += b;
	return tmp;
}


TiXmlString::TiXmlString() : rep_(&nullrep_) {}

TiXmlString::TiXmlString( const TiXmlString & copy) : rep_(0) {
	init(copy.length());
	memcpy(start(), copy.data(), length());
}

TiXmlString::TiXmlString( const char * copy) : rep_(0) {
		init( static_cast<size_type>( strlen(copy) ));
		memcpy(start(), copy, length());
}

TiXmlString::TiXmlString( const char * str, size_type len) : rep_(0) {
	init(len);
	memcpy(start(), str, len);
}

TiXmlString::~TiXmlString() {
		quit();
}

TiXmlString& TiXmlString::operator =(const char * copy) {
		return assign( copy,(size_type)strlen(copy));
}

TiXmlString& TiXmlString::operator =(const TiXmlString & copy) {
	return assign(copy.start(), copy.length());
}

TiXmlString& TiXmlString::operator +=(const char * suffix) {
	return append(suffix, static_cast<size_type>( strlen(suffix) ));
}

TiXmlString& TiXmlString::operator +=(char single) {
	return append(&single, 1);
}

TiXmlString& TiXmlString::operator +=(const TiXmlString & suffix) {
	return append(suffix.data(), suffix.length());
}

const char * TiXmlString::c_str() const { return rep_->str; }

const char * TiXmlString::data() const { return rep_->str; }

size_type TiXmlString::length() const { return rep_->size; }

size_type TiXmlString::size() const { return rep_->size; }

bool TiXmlString::empty() const { return rep_->size == 0; }

size_type TiXmlString::capacity() const { return rep_->capacity; }

const char& TiXmlString::at(size_type index) const{
	assert( index < length() );
	return rep_->str[ index ];
}

char& TiXmlString::operator [](size_type index) const{
	assert( index < length() );
	return rep_->str[ index ];
}

size_type TiXmlString::find(char lookup) const{
	return find(lookup, 0);
}

size_type TiXmlString::find(char tofind, size_type offset) const{
	if (offset >= length()) return npos;
	for (const char* p = c_str() + offset; *p != '\0'; ++p) {
	   if (*p == tofind) return static_cast< size_type >( p - c_str() );
	}
	return npos;
}

void TiXmlString::clear() {
	quit();
	init(0,0);
}


void TiXmlString::swap(TiXmlString& other) {
	Rep* r = rep_;
	rep_ = other.rep_;
	other.rep_ = r;
}

void TiXmlString::init(size_type sz) { init(sz, sz); }
void TiXmlString::set_size(size_type sz) { rep_->str[ rep_->size = sz ] = '\0'; }
char* TiXmlString::start() const { return rep_->str; }
char* TiXmlString::finish() const { return rep_->str + rep_->size; }


void TiXmlString::init(size_type sz, size_type cap) {
	if (cap) {
		const size_type bytesNeeded = sizeof(Rep) + cap;
		const size_type intsNeeded =( bytesNeeded + sizeof(int) - 1 ) / sizeof( int ); 
		rep_ = reinterpret_cast<Rep*>( new int[ intsNeeded ] );
		rep_->str[rep_->size=sz] = '\0';
		rep_->capacity=cap;
	} else {
		rep_ = &nullrep_;
	}
}

void TiXmlString::quit() {
	if (rep_ != &nullrep_)
		delete [](reinterpret_cast<int*>(rep_));
}

#endif	// TIXML_USE_STL


