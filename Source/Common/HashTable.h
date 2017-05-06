#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

const int HASH_KEY_STRING = 0;
const int HASH_KEY_WORD = 1;

const int SMALL_HASH_TABLE_SIZE = 4;
const int REBUILD_MULTIPLIER = 3;

class CHashTable
{
public:
	CHashTable(int hashKeyType);
	virtual ~CHashTable();

public:
	virtual void * Add(const char * key, void * value);
	virtual bool Remove(const char * key);
	virtual void * Lookup(const char * key);
	virtual unsigned int EntriesCount();
	bool IsEmpty() { return EntriesCount() == 0; }
	void * RemoveNext();

private:
	class CHashTableEntry
	{
	public:
		CHashTableEntry * m_nextEntry;
		const char * key;
		void * value;
	};

public:
	class Iterator 
	{
	public:
		Iterator(CHashTable & hashTable);
		virtual ~Iterator();

	public:
		virtual void * Next(const char *& key);

	protected:
		CHashTable      & m_hashTable;
		unsigned int      m_nextIndex;
		CHashTableEntry * m_nextEntry;
	};

	friend class CHashTable::Iterator;

protected:
	CHashTableEntry * LookupKey(const char * key, unsigned int & index) const;
	bool KeyMatches(const char * key1, const char * key2) const;

	CHashTableEntry * InsertNewEntry(unsigned int index, const char * key);
	void AssignKey(CHashTableEntry * entry, const char * key);

	void DeleteEntry(unsigned int index, CHashTableEntry * entry);
	void DeleteKey(CHashTableEntry * entry);

	void RebuildHashTable();

	unsigned int HashIndexFromKey(const char * key) const;
	unsigned int RandomIndex(unsigned long index) const 
	{
		return (((index * 1103515245) >> m_downShift) & m_mask);
	}

private:
	CHashTableEntry ** m_buckets;
	CHashTableEntry  * m_staticBuckets[SMALL_HASH_TABLE_SIZE];
	unsigned int	   m_bucketsCount;
	unsigned int	   m_entriesCount;
	unsigned int       m_rebuildSize;
	unsigned int       m_downShift;
	unsigned int       m_mask;
	int                m_hashKeyType;
};

#ifdef _VONAMESPACE
}
#endif


#endif //__HASHTABLE_H__
