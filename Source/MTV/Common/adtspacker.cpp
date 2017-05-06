#include <memory.h>
#include "adtspacker.h"
#include "adtsparser.h"

#if defined LINUX
#include <string.h>
#endif

#define MAX_HEAD_SIZE 9
#define MAX_DATA_SIZE 2048

#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK

CADTSPacker::CADTSPacker()
: _listener(0),
  _status(STATUS_DATA_FINISHED)
{
	_framebuf = new uint8[MAX_DATA_SIZE];
	_datasize = 0;
	_dataexpect = 0;
}

CADTSPacker::~CADTSPacker()
{
	delete[] _framebuf;
}

void CADTSPacker::SetListener(IADTSPackListener* listener)
{
	_listener = listener;
}

bool CADTSPacker::AddData(uint8* data, int size)
{
	uint8* p = data;
	int cLeft = size;
	while (cLeft)
	{
		if (_status == STATUS_DATA_FINISHED)
		{
			if (cLeft >= MAX_HEAD_SIZE)
			{
				CADTSParser parser;
				if (!parser.Parse(p))
				{
					//error tolerance
					uint8* pHead;
					if (!CADTSParser::FindHead(p, cLeft, (void**)&pHead))
						return false;
					int cSkipped = pHead - p;
					p += cSkipped;
					cLeft -= cSkipped;
					continue;
				}
				_dataexpect = parser.GetFrameLen();
				_status = STATUS_HEAD_FINISHED;
			}
			else
			{
				memcpy(_framebuf, p, cLeft);
				_datasize = cLeft;
				_status = STATUS_HEAD_PENDING;
				return true;
			}
		}

		if (_status == STATUS_HEAD_FINISHED)
		{
			if (cLeft >= _dataexpect)
			{
				if (_datasize == 0)
				{
					if (_listener)
						_listener->OnNewADTSPacket(p, _dataexpect);
				}
				else
				{
					memcpy(_framebuf + _datasize, p, _dataexpect);
					_datasize += _dataexpect;
					if (_listener)
						_listener->OnNewADTSPacket(_framebuf, _datasize);
					_datasize = 0;
				}
				p += _dataexpect;
				cLeft -= _dataexpect;
				//_dataexpect = 0;
				_status = STATUS_DATA_FINISHED;
				continue;
			}
			else
			{
				memcpy(_framebuf + _datasize, p, cLeft);
				_datasize += cLeft;
				_dataexpect -= cLeft;
				_status = STATUS_DATA_PENDING;
				return true;
			}
		}

		if (_status == STATUS_DATA_PENDING)
		{
			if (cLeft >= _dataexpect)
			{
				memcpy(_framebuf + _datasize, p, _dataexpect);
				_datasize += _dataexpect; //???_dataexpect;
				if (_listener)
					_listener->OnNewADTSPacket(_framebuf, _datasize);
				_datasize = 0;
				p += _dataexpect;
				cLeft -= _dataexpect;
				//_dataexpect = 0;
				_status = STATUS_DATA_FINISHED;
				continue;
			}
			else
			{
				memcpy(_framebuf + _datasize, p, cLeft);
				_datasize += cLeft;
				_dataexpect -= cLeft;
				//_status = STATUS_DATA_PENDING;
				return true;
			}
		}

		if (_status == STATUS_HEAD_PENDING)
		{
			int needed = MAX_HEAD_SIZE - _datasize;
			if (cLeft >= needed)
			{
				memcpy(_framebuf + _datasize, p, needed);
				CADTSParser parser;
				if (!parser.Parse(_framebuf))
					return false;
				cLeft -= needed;
				_datasize += needed;
				p += needed;
				_dataexpect = parser.GetFrameLen() - _datasize;
				_status = STATUS_HEAD_FINISHED;
				continue;
			}
			else
			{
				memcpy(_framebuf + _datasize, p, cLeft);
				_datasize += cLeft;
				//_status = STATUS_HEAD_PENDING;
				return true;
			}
		}

	}

	return true;
}

