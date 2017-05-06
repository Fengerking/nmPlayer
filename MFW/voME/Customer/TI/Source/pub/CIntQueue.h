
/*
 *
 * VisualOn 
 * david
 * 2011-07-06
 *
 */

#if !defined __CIntQueue__
#define __CIntQueue__

class CIntQueue {

public:
	CIntQueue(const int size = 0);
	~CIntQueue();

	void setCapability(const int);
	void clearAll();

	int enqueue(const int nval);
	int dequeue(int *pval = NULL);
	int get(int &pos, int &val);
	int getCount() { return m_ncount; }
	int getHeadPosition();
	int getTailPosition();

	bool isFull();
	bool isEmpty();

private:

	int  m_nhead;
	int  m_ntail;
	int	 m_ncount;
	int  m_ncapability;
	int* m_pdata;
};

#endif // __CIntQueue__

