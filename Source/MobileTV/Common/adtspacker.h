#ifndef _ADTS_PACKER_H
#define _ADTS_PACKER_H

#include "voitypes.h"
class IADTSPackListener
{
public:
	virtual void OnNewADTSPacket(uint8* data, int size) = 0;
};

class CADTSPacker
{
public:
	CADTSPacker();
	~CADTSPacker();

public:
	void SetListener(IADTSPackListener* listener);
	bool AddData(uint8* data, int size);

protected:
	IADTSPackListener* _listener;

protected:
	enum
	{
		STATUS_INIT = 0,
		STATUS_DATA_FINISHED,  // before a new packet, that is, finish a packet
		STATUS_HEAD_PENDING,  // head pending
		STATUS_HEAD_FINISHED,
		STATUS_DATA_PENDING  //data pending
	} _status;

protected:
	uint8* _framebuf;
	int _datasize; //how many data in databuf now
	int _dataexpect; //data except for current packet

};

#endif // _ADTS_PACKER_H
