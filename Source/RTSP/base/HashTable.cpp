#include <string.h>
#include "utility.h"
#include "HashTable.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CHashTable::CHashTable(int hashKeyType)
: m_buckets(m_staticBuckets)
, m_bucketsCount(SMALL_HASH_TABLE_SIZE)
, m_entriesCount(0)
, m_rebuildSize(SMALL_HASH_TABLE_SIZE * REBUILD_MULTIPLIER)
, m_downShift(28) 
, m_mask(0x3) 
, m_hashKeyType(hashKeyType) 
{
	for(int i=0; i<SMALL_HASH_TABLE_SIZE; ++i)
	{
		m_staticBuckets[i] = NULL;
	}
}

CHashTable::~CHashTable() 
{
	for(unsigned int i=0; i<m_bucketsCount; ++i)
	{
		CHashTableEntry * entry;
		while((entry = m_buckets[i]) != NULL) 
		{
			DeleteEntry(i, entry);
		}
	}
	
	if(m_buckets != m_staticBuckets)
	{
		SAFE_DELETE_ARRAY(m_buckets);
	}
}

void * CHashTable::RemoveNext()
{
	Iterator * iter = new Iterator(*this);TRACE_NEW("CHashTable::RemoveNext ",iter);
	const char * key;

	void * removedValue = iter->Next(key);
	if(removedValue != 0)
		Remove(key);

	SAFE_DELETE(iter);

	return removedValue;
}

void * CHashTable::Add(const char * key, void * value)
{
	void * oldValue;
	unsigned int index;
	CHashTableEntry * entry = LookupKey(key, index);
	if(entry != NULL) 
	{
		oldValue = entry->value;
	}
	else
	{
		entry = InsertNewEntry(index, key);
		oldValue = NULL;
	}
	entry->value = value;
	
	if(m_entriesCount >= m_rebuildSize) 
		RebuildHashTable();
	
	return oldValue;
}

bool CHashTable::Remove(const char * key)
{
	unsigned int index;
	CHashTableEntry * entry = LookupKey(key, index);
	if(entry == NULL) 
		return false;
	
	DeleteEntry(index, entry);
	
	return true;
}

void * CHashTable::Lookup(const char * key)
{
	unsigned int index;
	CHashTableEntry * entry = LookupKey(key, index);
	if(entry == NULL)
		return NULL;

	return entry->value;
}

unsigned int CHashTable::EntriesCount()
{
	return m_entriesCount;
}

CHashTable::Iterator::Iterator(CHashTable & hashTable)
: m_hashTable(hashTable)
, m_nextIndex(0)
, m_nextEntry(NULL)
{
}

CHashTable::Iterator::~Iterator()
{
}

void * CHashTable::Iterator::Next(const char *& key)
{
	while(m_nextEntry == NULL) 
	{
		if(m_nextIndex >= m_hashTable.m_bucketsCount) 
			return NULL;
		
		m_nextEntry = m_hashTable.m_buckets[m_nextIndex++];
	}
	
	CHashTable::CHashTableEntry * entry = m_nextEntry;
	m_nextEntry = entry->m_nextEntry;
	
	key = entry->key;
	return entry->value;
}

CHashTable::CHashTableEntry * CHashTable::LookupKey(const char * key, unsigned int & index) const
{
	CHashTableEntry * entry;
	index = HashIndexFromKey(key);
	
	for(entry=m_buckets[index]; entry!=NULL; entry=entry->m_nextEntry) 
	{
		if(KeyMatches(key, entry->key)) 
			break;
	}
	
	return entry;
}

bool CHashTable::KeyMatches(const char * key1, const char * key2) const 
{
	if(m_hashKeyType == HASH_KEY_STRING)
	{
		return(strcmp(key1, key2) == 0);
	}
	else if(m_hashKeyType == HASH_KEY_WORD) 
	{
		return (key1 == key2);
	}
	return 0;
}

CHashTable::CHashTableEntry * CHashTable::InsertNewEntry(unsigned int index, const char * key)
{
	CHashTableEntry * entry = new CHashTableEntry();TRACE_NEW("CHashTable::InsertNewEntry ",entry);
	entry->m_nextEntry = m_buckets[index];
	m_buckets[index] = entry;
	
	++m_entriesCount;
	AssignKey(entry, key);
	
	return entry;
}

void CHashTable::AssignKey(CHashTableEntry * entry, const char * key) 
{
	if(m_hashKeyType == HASH_KEY_STRING)
	{
		//entry->key = strdup(key);	//contented by doncy 0813
		#if defined	UNDER_CE
				entry->key = _strdup(key);
		#else
				entry->key = strdup(key);
		#endif	//UNDER_CE
	}
	else if(m_hashKeyType == HASH_KEY_WORD)
	{
		entry->key = key;
	}
}

void CHashTable::DeleteEntry(unsigned index, CHashTableEntry * entry) 
{
	CHashTableEntry ** ep = &m_buckets[index];
	while(*ep != NULL)
	{
		if(*ep == entry)
		{
			*ep = entry->m_nextEntry;
			break;
		}
		ep = &((*ep)->m_nextEntry);
	}
	
	--m_entriesCount;
	DeleteKey(entry);
	SAFE_DELETE(entry);
}

void CHashTable::DeleteKey(CHashTableEntry * entry) 
{
	if(m_hashKeyType == HASH_KEY_STRING)
	{
		SAFE_DELETE_ARRAY(entry->key);
		entry->key = NULL;
	}
	else if(m_hashKeyType == HASH_KEY_WORD)
	{
		entry->key = NULL;
	}
}

void CHashTable::RebuildHashTable() 
{
	unsigned int oldBucketsCount = m_bucketsCount;
	CHashTableEntry ** oldBuckets = m_buckets;
	
	m_bucketsCount *= 4;
	m_buckets = new CHashTableEntry * [m_bucketsCount];TRACE_NEW("CHashTable::RebuildHashTable() ",m_buckets);
	for(unsigned int i=0; i<m_bucketsCount; ++i) 
	{
		m_buckets[i] = NULL;
	}
	m_rebuildSize *= 4;
	m_downShift -= 2;
	m_mask = (m_mask << 2) | 0x3;
	
	for(CHashTableEntry ** oldChainPtr=oldBuckets; oldBucketsCount > 0; --oldBucketsCount, ++oldChainPtr) 
	{
		for(CHashTableEntry * hPtr=*oldChainPtr; hPtr != NULL; hPtr=*oldChainPtr) 
		{
			*oldChainPtr = hPtr->m_nextEntry;
			
			unsigned int index = HashIndexFromKey(hPtr->key);
			
			hPtr->m_nextEntry = m_buckets[index];
			m_buckets[index] = hPtr;
		}
	}
	
	if(oldBuckets != m_staticBuckets)
	{
		SAFE_DELETE_ARRAY(oldBuckets);
	}
}

unsigned int CHashTable::HashIndexFromKey(const char * key) const 
{
	unsigned int result = 0;
	
	if(m_hashKeyType == HASH_KEY_STRING) 
	{
		while(1)
		{
			char c = *key++;
			if(c == 0)
				break;
			result += (result << 3) + (unsigned int)c;
		}
		result &= m_mask;
	}
	else if(m_hashKeyType == HASH_KEY_WORD)
	{
		result = RandomIndex((unsigned long)key);
	} 
	
	return result;
}
