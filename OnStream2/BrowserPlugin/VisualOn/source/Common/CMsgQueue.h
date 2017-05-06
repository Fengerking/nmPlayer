
/*
 *
 * VisualOn 
 * david
 * 2011-07-06
 *
 */

#if !defined __CMsgQueue__
#define __CMsgQueue__

#include <WTypes.h>

#define VOQU_OK  (0)
#define VOQU_ERR (-1)

#define MAX_MSG_COUNT	1024

typedef struct __VOCBMSG__
{
    unsigned int				nID;
    unsigned int 			    nValue1;
    unsigned int			    nValue2;
    void setJSONObject(LPSTR object)
    {
        if(jsonObject)
        {
            delete []jsonObject;
            jsonObject = NULL;
        }

        if(NULL==object || strlen(object)==0)
            return;

        if (object)
        {
            int iLength = strlen(object);
            jsonObject = new CHAR[iLength+1];
            if(jsonObject)
            {
                strcpy(jsonObject,object);
            }
        }
    }

    LPSTR getJSONObject()
    {
        return jsonObject;
    }

	__VOCBMSG__()
	{
		nID = 0;
		nValue1 = 0;
		nValue2 = 0;
        jsonObject = NULL;
	}
    ~__VOCBMSG__()
    {
        if (jsonObject)
        {
            delete []jsonObject;
            jsonObject = NULL;
        }
    }

	__VOCBMSG__& operator= (const __VOCBMSG__& msg)
	{ 
		nID = msg.nID;
		nValue1 = msg.nValue1;
		nValue2 = msg.nValue2;
        setJSONObject(msg.jsonObject);        
		return *this;
	} 

private:
    LPSTR               jsonObject;
} VOCBMSG;

class CMsgQueue {

public:
	CMsgQueue(const int size = 0);
	~CMsgQueue();

	void setCapability(const int);
	void clearAll();
	
	int insert2Head(const VOCBMSG& msg);
	int enqueue(const VOCBMSG& msg);
	int dequeue(VOCBMSG *msg);
	int get(int &pos, VOCBMSG &msg);
	int getCount() { return m_ncount; }
	int getCapability() { return m_ncapability; }
	int getHeadPosition();
	int getTailPosition();

	bool isFull();
	bool isEmpty();

private:

	int			m_nhead;
	int			m_ntail;
	int			m_ncount;
	int			m_ncapability;
	VOCBMSG*	m_pdata;
};

template<typename T> 
class CVoQueue {

public:
	CVoQueue(const int size = 0);
	~CVoQueue();

	void setCapability(const int);
	void clearAll();

	int insert2Head(const T& item);
	int enqueue(const T& item);
	int dequeue(T *item);
	int get(int &pos, T& item);
	int remove(T item);
	int getCount() { return m_ncount; }
	int getCapability() { return m_ncapability; }
	int getHeadPosition();
	int getTailPosition();

	bool isFull();
	bool isEmpty();

private:

	int			m_nhead;
	int			m_ntail;
	int			m_ncount;
	int			m_ncapability;
	T*			m_pdata;
};

#endif // __CMsgQueue__

