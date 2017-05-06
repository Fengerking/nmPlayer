#include "sdp_error.h"
#include "sdp_media.h"
#include "sdp_utility.h"
#if defined(LINUX)
#   include "vocrsapis.h"
#endif

CSDPMedia::CSDPMedia(CSDPSession * pSDPSession, int index)
: m_pSDPSession(pSDPSession)
, m_index(index)
{
}

CSDPMedia::~CSDPMedia()
{
	list_T<char *>::iterator iter;
	for(iter=m_listMediaDesLine.begin(); iter!=m_listMediaDesLine.end(); ++iter)
	{
		delete[] *iter;
	}
	m_listMediaDesLine.clear();
}

void CSDPMedia::AddMediaDescriptionLine(const char * _mediaDesLine)
{
	size_t len = SDP_GetLineLength(_mediaDesLine);
	if(len == 0)
		return;

	char * _line = SDP_StringDup_N(_mediaDesLine, len);
	if(_line != NULL)
		m_listMediaDesLine.push_back(_line);
}

char * CSDPMedia::SearchMediaDescriptionLine(const char * _searchLineName)
{
	char * _mediaDesLine = NULL;
	list_T<char *>::iterator iter;
	for(iter=m_listMediaDesLine.begin(); iter!=m_listMediaDesLine.end(); ++iter)
	{
		_mediaDesLine = *iter;
		if(_strnicmp(_mediaDesLine, _searchLineName, strlen(_searchLineName)) == 0)
			return _mediaDesLine;
	}
	return NULL;
}

char * CSDPMedia::SearchFmtpParameter(const char * _searchParamName)
{
	char * _mediaAttr_fmtp = SearchMediaDescriptionLine("a=fmtp:");
	if(_mediaAttr_fmtp == NULL)
		return NULL;

	char * _s = _mediaAttr_fmtp + strlen("a=fmtp:"); //skip "a=fmtp:"
	while(isdigit(*_s)) ++_s; //skip payload type 

	char _paramName[64];
	while(_s != NULL && *_s != '\0' && *_s != '\r' && *_s != '\n')
	{
		while(*_s == ' ') ++_s; //skip white space ' '

		if(sscanf(_s, "%[^=\r\n]", _paramName) != 1)
			return NULL;

		if(_stricmp(_paramName, _searchParamName) == 0)
		{
			char * _paramValue = strstr(_s, "=");
			if(_paramValue == NULL)
				return NULL;
			
			return ++_paramValue; //skip "=" 
		}
		else
		{
			while(*_s != '\0' && *_s != '\r' && *_s != '\n' && *_s != ';')
				++_s;

			if(*_s == ';') ++_s; // Skip the ';'
		}
	}

	return NULL;
}



bool CSDPMedia::ParseMediaType(char * _mediaType, size_t len)
{
	do
	{
		char * _mediaDes_m = SearchMediaDescriptionLine("m=");
		if(_mediaDes_m == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaDes_m, "m=%s", _mediaType) != 1)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseTransportPort(unsigned short * _transportPort)
{
	do
	{
		char * _mediaDes_m = SearchMediaDescriptionLine("m=");
		if(_mediaDes_m == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		char _mediaType[16];
		if(sscanf(_mediaDes_m, "m=%s %hu", _mediaType, _transportPort) != 2)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseTransportProtocol(char * _transportProtocol, size_t len)
{
	do
	{
		char * _mediaDes_m = SearchMediaDescriptionLine("m=");
		if(_mediaDes_m == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		char _mediaType[16];
		unsigned short transportPort = 0;
		if(sscanf(_mediaDes_m, "m=%s %hu %s", _mediaType, &transportPort, _transportProtocol) != 3)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseRTPPayloadType(unsigned int * _rtpPayloadType)
{
	do
	{
		char * _mediaDes_m = SearchMediaDescriptionLine("m=");
		if(_mediaDes_m == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		char _mediaType[16];
		unsigned short transportPort = 0;
		char _transportProtocol[16];
		if(sscanf(_mediaDes_m, "m=%s %hu %s %u", _mediaType, &transportPort, _transportProtocol, _rtpPayloadType) != 4)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAddressType(char * _addressType, size_t len)
{
	do
	{
		char * _mediaDes_c = SearchMediaDescriptionLine("c=");
		if(_mediaDes_c == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaDes_c, "c=IN %s", _addressType) != 1)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseConnectionAddress(char * _connectionAddress, size_t len)
{
	do
	{
		char * _mediaDes_c = SearchMediaDescriptionLine("c=");
		if(_mediaDes_c == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		char _addressType[16];
		if(sscanf(_mediaDes_c, "c=IN %s %[^/ \r\n]", _addressType, _connectionAddress) != 2)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseTTL(char * _ttl, size_t len)
{
	do
	{
		char * _mediaDes_c = SearchMediaDescriptionLine("c=");
		if(_mediaDes_c == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		char _addressType[16];
		char _connectionAddress[64];
		if(sscanf(_mediaDes_c, "c=IN %s %[^/ \r\n]/%[^/ \r\n]", _addressType, _connectionAddress, _ttl) != 3)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_rtpmap_encodingName(char * _encodingName, size_t len)
{
	do
	{
		char * _mediaAttr_rtpmap = SearchMediaDescriptionLine("a=rtpmap:");
		if(_mediaAttr_rtpmap == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		unsigned int payloadType = 0;
		if(sscanf(_mediaAttr_rtpmap, "a=rtpmap:%u %[^/ \r\n]", &payloadType, _encodingName) != 2)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_rtpmap_clockRate(unsigned int * _clockRate)
{
	do
	{
		char * _mediaAttr_rtpmap = SearchMediaDescriptionLine("a=rtpmap:");
		if(_mediaAttr_rtpmap == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		unsigned int payloadType = 0;
		char _encodingName[32];
		if(sscanf(_mediaAttr_rtpmap, "a=rtpmap:%u %[^/ \r\n]/%u", &payloadType, _encodingName, _clockRate) != 3)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_rtpmap_encodingParameters(unsigned int * _encodingParam)
{
	do
	{
		char * _mediaAttr_rtpmap = SearchMediaDescriptionLine("a=rtpmap:");
		if(_mediaAttr_rtpmap == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		unsigned int payloadType = 0;
		char _encodingName[32];
		unsigned int clockRate = 0;
		if(sscanf(_mediaAttr_rtpmap, "a=rtpmap:%u %[^/ \r\n]/%u/%u", &payloadType, _encodingName, &clockRate, _encodingParam) != 4)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_cliprect(int * _top, int * _left, int * _bottom, int * _right)
{
	do
	{
		char * _mediaAttr_cliprect = SearchMediaDescriptionLine("a=cliprect:");
		if(_mediaAttr_cliprect == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_cliprect, "a=cliprect:%d,%d,%d,%d", _top, _left, _bottom, _right) != 4)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_framesize(int * _width, int * _height)
{
	do
	{
		char * _mediaAttr_framesize = SearchMediaDescriptionLine("a=framesize:");
		if(_mediaAttr_framesize == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		unsigned int payloadType = 0;
		if(sscanf(_mediaAttr_framesize, "a=framesize:%u %d-%d", &payloadType, _width, _height) != 3)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_framerate(float * _framerate)
{
	do
	{
		char * _mediaAttr_framerate = SearchMediaDescriptionLine("a=framerate:");
		if(_mediaAttr_framerate == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_framerate, "a=framerate:%f", _framerate) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_mid(int * _mid)
{
	do
	{
		char * _mediaAttr_mid = SearchMediaDescriptionLine("a=mid:");
		if(_mediaAttr_mid == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_mid, "a=mid:%d", _mid) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_mpeg4_esid(int * _mpeg4_esid)
{
	do
	{
		char * _mediaAttr_mpeg4_esid = SearchMediaDescriptionLine("a=mpeg4-esid:");
		if(_mediaAttr_mpeg4_esid == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_mpeg4_esid, "a=mpeg4-esid:%d", _mpeg4_esid) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}



bool CSDPMedia::ParseAttribute_fmtp_packetization_mode(int * _packetization_mode)
{
	do
	{
		char * _mediaAttr_fmtp_packetization_mode = SearchFmtpParameter("packetization-mode");
		if(_mediaAttr_fmtp_packetization_mode == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_packetization_mode, "%d", _packetization_mode) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_profile_level_id(int * _profile_level_id)
{
	do
	{
		char * _mediaAttr_fmtp_profile_level_id = SearchFmtpParameter("profile-level-id");
		if(_mediaAttr_fmtp_profile_level_id == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_profile_level_id, "%d", _profile_level_id) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_profile_level_id(char * _profile_level_id, size_t len)
{
	do
	{
		char * _mediaAttr_fmtp_profile_level_id = SearchFmtpParameter("profile-level-id");
		if(_mediaAttr_fmtp_profile_level_id == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_profile_level_id, "%[^;\r\n]", _profile_level_id) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_sprop_parameter_sets(char * _sprop_parameter_sets, size_t len)
{
	do
	{
		char * _mediaAttr_fmtp_sprop_parameter_sets = SearchFmtpParameter("sprop-parameter-sets");
		if(_mediaAttr_fmtp_sprop_parameter_sets == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_sprop_parameter_sets, "%[^;\r\n]", _sprop_parameter_sets) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_config(char * _config, size_t len)
{
	do
	{
		char * _mediaAttr_fmtp_config = SearchFmtpParameter("config");
		if(_mediaAttr_fmtp_config == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_config, "%[^;\r\n]", _config) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_streamtype(int * _streamtype)
{
	do
	{
		char * _mediaAttr_fmtp_streamtype = SearchFmtpParameter("streamtype");
		if(_mediaAttr_fmtp_streamtype == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_streamtype, "%d", _streamtype) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_objectType(int * _objectType)
{
	do
	{
		char * _mediaAttr_fmtp_objectType = SearchFmtpParameter("objectType");
		if(_mediaAttr_fmtp_objectType == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_objectType, "%d", _objectType) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_object(int * _object)
{
	do
	{
		char * _mediaAttr_fmtp_object = SearchFmtpParameter("object");
		if(_mediaAttr_fmtp_object == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_object, "%d", _object) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_mode(char * _mode, size_t len)
{
	do
	{
		char * _mediaAttr_fmtp_mode = SearchFmtpParameter("mode");
		if(_mediaAttr_fmtp_mode == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_mode, "%[^;\r\n]", _mode) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_sizeLength(unsigned int * _sizeLength)
{
	do
	{
		char * _mediaAttr_fmtp_sizeLength = SearchFmtpParameter("sizeLength");
		if(_mediaAttr_fmtp_sizeLength == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_sizeLength, "%u", _sizeLength) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_indexLength(unsigned int * _indexLength)
{
	do
	{
		char * _mediaAttr_fmtp_indexLength = SearchFmtpParameter("indexLength");
		if(_mediaAttr_fmtp_indexLength == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_indexLength, "%u", _indexLength) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_indexDeltaLength(unsigned int * _indexDeltaLength)
{
	do
	{
		char * _mediaAttr_fmtp_indexDeltaLength = SearchFmtpParameter("indexDeltaLength");
		if(_mediaAttr_fmtp_indexDeltaLength == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_indexDeltaLength, "%u", _indexDeltaLength) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_cpresent(int * _cpresent)
{
	do
	{
		char * _mediaAttr_fmtp_cpresent = SearchFmtpParameter("cpresent");
		if(_mediaAttr_fmtp_cpresent == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_cpresent, "%d", _cpresent) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_octet_align(int * _octet_align)
{
	do
	{
		char * _mediaAttr_fmtp_octet_align = SearchFmtpParameter("octet-align");
		if(_mediaAttr_fmtp_octet_align == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_octet_align, "%d", _octet_align) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_interleaving(int * _interleaving)
{
	do
	{
		char * _mediaAttr_fmtp_interleaving = SearchFmtpParameter("interleaving");
		if(_mediaAttr_fmtp_interleaving == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_interleaving, "%d", _interleaving) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_robust_sorting(int * _robust_sorting)
{
	do
	{
		char * _mediaAttr_fmtp_robust_sorting = SearchFmtpParameter("robust-sorting");
		if(_mediaAttr_fmtp_robust_sorting == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_robust_sorting, "%d", _robust_sorting) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_crc(int * _crc)
{
	do
	{
		char * _mediaAttr_fmtp_crc = SearchFmtpParameter("crc");
		if(_mediaAttr_fmtp_crc == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_crc, "%d", _crc) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_framesize(int * _width, int * _height)
{
	do
	{
		char * _mediaAttr_fmtp_framesize = SearchFmtpParameter("framesize");
		if(_mediaAttr_fmtp_framesize == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_framesize, "%d-%d", _width, _height) != 2)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_constantSize(int * _constantSize)
{
	do
	{
		char * _mediaAttr_fmtp_constantSize = SearchFmtpParameter("constantSize");
		if(_mediaAttr_fmtp_constantSize == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_constantSize, "%d", _constantSize) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_constantDuration(unsigned int * _constantDuration)
{
	do
	{
		char * _mediaAttr_fmtp_constantDuration = SearchFmtpParameter("constantDuration");
		if(_mediaAttr_fmtp_constantDuration == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_constantDuration, "%u", _constantDuration) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_maxDisplacement(unsigned int * _maxDisplacement)
{
	do
	{
		char * _mediaAttr_fmtp_maxDisplacement = SearchFmtpParameter("maxDisplacement");
		if(_mediaAttr_fmtp_maxDisplacement == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_maxDisplacement, "%u", _maxDisplacement) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_de_interleaveBufferSize(unsigned int * _de_interleaveBufferSize)
{
	do
	{
		char * _mediaAttr_fmtp_de_interleaveBufferSize = SearchFmtpParameter("de-interleaveBufferSize");
		if(_mediaAttr_fmtp_de_interleaveBufferSize == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_de_interleaveBufferSize, "%u", _de_interleaveBufferSize) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_CTSDeltaLength(unsigned int * _CTSDeltaLength)
{
	do
	{
		char * _mediaAttr_fmtp_CTSDeltaLength = SearchFmtpParameter("CTSDeltaLength");
		if(_mediaAttr_fmtp_CTSDeltaLength == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_CTSDeltaLength, "%u", _CTSDeltaLength) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_DTSDeltaLength(unsigned int * _DTSDeltaLength)
{
	do
	{
		char * _mediaAttr_fmtp_DTSDeltaLength = SearchFmtpParameter("DTSDeltaLength");
		if(_mediaAttr_fmtp_DTSDeltaLength == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_DTSDeltaLength, "%u", _DTSDeltaLength) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_randomAccessIndication(unsigned int * _randomAccessIndication)
{
	do
	{
		char * _mediaAttr_fmtp_randomAccessIndication = SearchFmtpParameter("randomAccessIndication");
		if(_mediaAttr_fmtp_randomAccessIndication == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_randomAccessIndication, "%u", _randomAccessIndication) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_streamStateIndication(unsigned int * _streamStateIndication)
{
	do
	{
		char * _mediaAttr_fmtp_streamStateIndication = SearchFmtpParameter("streamStateIndication");
		if(_mediaAttr_fmtp_streamStateIndication == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_streamStateIndication, "%u", _streamStateIndication) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_auxiliaryDataSizeLength(unsigned int * _auxiliaryDataSizeLength)
{
	do
	{
		char * _mediaAttr_fmtp_auxiliaryDataSizeLength = SearchFmtpParameter("auxiliaryDataSizeLength");
		if(_mediaAttr_fmtp_auxiliaryDataSizeLength == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_auxiliaryDataSizeLength, "%u", _auxiliaryDataSizeLength) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypCryptoSuite(char * _ISMACrypCryptoSuite, size_t len)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypCryptoSuite = SearchFmtpParameter("ISMACrypCryptoSuite");
		if(_mediaAttr_fmtp_ISMACrypCryptoSuite == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypCryptoSuite, "%[^;\r\n]", _ISMACrypCryptoSuite) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypIVLength(unsigned int * _ISMACrypIVLength)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypIVLength = SearchFmtpParameter("ISMACrypIVLength");
		if(_mediaAttr_fmtp_ISMACrypIVLength == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypIVLength, "%u", _ISMACrypIVLength) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypDeltaIVLength(unsigned int * _ISMACrypDeltaIVLength)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypDeltaIVLength = SearchFmtpParameter("ISMACrypDeltaIVLength");
		if(_mediaAttr_fmtp_ISMACrypDeltaIVLength == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypDeltaIVLength, "%u", _ISMACrypDeltaIVLength) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypSelectiveEncryption(unsigned int * _ISMACrypSelectiveEncryption)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypSelectiveEncryption = SearchFmtpParameter("ISMACrypSelectiveEncryption");
		if(_mediaAttr_fmtp_ISMACrypSelectiveEncryption == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypSelectiveEncryption, "%u", _ISMACrypSelectiveEncryption) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypKeyIndicatorLength(unsigned int * _ISMACrypKeyIndicatorLength)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypKeyIndicatorLength = SearchFmtpParameter("ISMACrypKeyIndicatorLength");
		if(_mediaAttr_fmtp_ISMACrypKeyIndicatorLength == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypKeyIndicatorLength, "%u", _ISMACrypKeyIndicatorLength) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypKeyIndicatorPerAU(unsigned int * _ISMACrypKeyIndicatorPerAU)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypKeyIndicatorPerAU = SearchFmtpParameter("ISMACrypKeyIndicatorPerAU");
		if(_mediaAttr_fmtp_ISMACrypKeyIndicatorPerAU == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypKeyIndicatorPerAU, "%u", _ISMACrypKeyIndicatorPerAU) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypSalt(char * _ISMACrypSalt, size_t len)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypSalt = SearchFmtpParameter("ISMACrypSalt");
		if(_mediaAttr_fmtp_ISMACrypSalt == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypSalt, "%[^;\r\n]", _ISMACrypSalt) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypKey(char * _ISMACrypKey, size_t len)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypKey = SearchFmtpParameter("ISMACrypKey");
		if(_mediaAttr_fmtp_ISMACrypKey == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypKey, "%[^;\r\n]", _ISMACrypKey) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypKMSID(unsigned int * _ISMACrypKMSID)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypKMSID = SearchFmtpParameter("ISMACrypKMSID");
		if(_mediaAttr_fmtp_ISMACrypKMSID == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypKMSID, "%u", _ISMACrypKMSID) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypKMSVersion(unsigned int * _ISMACrypKMSVersion)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypKMSVersion = SearchFmtpParameter("ISMACrypKMSVersion");
		if(_mediaAttr_fmtp_ISMACrypKMSVersion == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypKMSVersion, "%u", _ISMACrypKMSVersion) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_ISMACrypKMSSpecificData(char * _ISMACrypKMSSpecificData, size_t len)
{
	do
	{
		char * _mediaAttr_fmtp_ISMACrypKMSSpecificData = SearchFmtpParameter("ISMACrypKMSSpecificData");
		if(_mediaAttr_fmtp_ISMACrypKMSSpecificData == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_ISMACrypKMSSpecificData, "%[^;\r\n]", _ISMACrypKMSSpecificData) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_Version_profile(unsigned int * _Version_profile)
{
	do
	{
		char * _mediaAttr_fmtp_Version_profile = SearchFmtpParameter("Version-profile");
		if(_mediaAttr_fmtp_Version_profile == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_Version_profile, "%u", _Version_profile) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPMedia::ParseAttribute_fmtp_Level(unsigned int * _Level)
{
	do
	{
		char * _mediaAttr_fmtp_Level = SearchFmtpParameter("Level");
		if(_mediaAttr_fmtp_Level == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_mediaAttr_fmtp_Level, "%u", _Level) != 1)
		{
			SDP_SetLastError(SDPERROR_UNKNOWN);
			break;
		}

		return true;

	}while(0);

	return false;
}
