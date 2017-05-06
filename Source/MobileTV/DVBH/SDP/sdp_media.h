#ifndef __SDP_MEDIA_H__
#define __SDP_MEDIA_H__

#include "list_T.h"

class CSDPSession;

class CSDPMedia
{
public:
	CSDPMedia(CSDPSession * pSDPSession, int index);
	virtual ~CSDPMedia();

public:
	void   AddMediaDescriptionLine(const char * _mediaDesLine);
private:
	char * SearchMediaDescriptionLine(const char * _searchLineName);
	char * SearchFmtpParameter(const char * _searchParamName); 

public:
	CSDPSession * GetSDPSession() { return m_pSDPSession; }
	int GetIndex() { return m_index; }

public:
	// m=
	bool ParseMediaType(char * _mediaType, size_t len);
	bool ParseTransportPort(unsigned short * _transportPort);
	bool ParseTransportProtocol(char * _transportProtocol, size_t len);
	bool ParseRTPPayloadType(unsigned int * _rtpPayloadType);

	//c=:
	bool ParseAddressType(char * _addressType, size_t len);
	bool ParseConnectionAddress(char * _connectionAddress, size_t len);
	bool ParseTTL(char * _ttl, size_t len);

	//a=rtpmap:
	bool ParseAttribute_rtpmap_encodingName(char * _encodingName, size_t len);
	bool ParseAttribute_rtpmap_clockRate(unsigned int * _clockRate);
	bool ParseAttribute_rtpmap_encodingParameters(unsigned int * _encodingParam);

	//a=*
	bool ParseAttribute_cliprect(int * _top, int * _left, int * _bottom, int * _right); //a=cliprect:0,0,240,320
	bool ParseAttribute_framesize(int * _width, int * _height); //a=framesize:96 320-240
	bool ParseAttribute_framerate(float * _framerate); //a=framerate:25

	bool ParseAttribute_mid(int * _mid); // a=mid
	bool ParseAttribute_mpeg4_esid(int * _mpeg4_esid); // a=mpeg4-esid

	//a=fmtp:
	bool ParseAttribute_fmtp_packetization_mode(int * _packetization_mode);
	bool ParseAttribute_fmtp_profile_level_id(int * _profile_level_id);
	bool ParseAttribute_fmtp_profile_level_id(char * _profile_level_id, size_t len);
	bool ParseAttribute_fmtp_sprop_parameter_sets(char * _sprop_parameter_sets, size_t len);
	bool ParseAttribute_fmtp_config(char * _config, size_t len);
	bool ParseAttribute_fmtp_streamtype(int * _streamtype);
	bool ParseAttribute_fmtp_objectType(int * _objectType);
	bool ParseAttribute_fmtp_object(int * _object);
	bool ParseAttribute_fmtp_mode(char * _mode, size_t len);
	bool ParseAttribute_fmtp_sizeLength(unsigned int * _sizeLength);
	bool ParseAttribute_fmtp_indexLength(unsigned int * _indexLength);
	bool ParseAttribute_fmtp_indexDeltaLength(unsigned int * _indexDeltaLength);
	bool ParseAttribute_fmtp_cpresent(int * _cpresent);
	bool ParseAttribute_fmtp_octet_align(int * _octet_align);
	bool ParseAttribute_fmtp_interleaving(int * _interleaving);
	bool ParseAttribute_fmtp_robust_sorting(int * _robust_sorting);
	bool ParseAttribute_fmtp_crc(int * _crc);
	bool ParseAttribute_fmtp_framesize(int * _width, int * _height);
	bool ParseAttribute_fmtp_constantSize(int * _constantSize);
	bool ParseAttribute_fmtp_constantDuration(unsigned int * _constantDuration);
	bool ParseAttribute_fmtp_maxDisplacement(unsigned int * _maxDisplacement);
	bool ParseAttribute_fmtp_de_interleaveBufferSize(unsigned int * _de_interleaveBufferSize);
	bool ParseAttribute_fmtp_CTSDeltaLength(unsigned int * _CTSDeltaLength);
	bool ParseAttribute_fmtp_DTSDeltaLength(unsigned int * _DTSDeltaLength);
	bool ParseAttribute_fmtp_randomAccessIndication(unsigned int * _randomAccessIndication);
	bool ParseAttribute_fmtp_streamStateIndication(unsigned int * _streamStateIndication);
	bool ParseAttribute_fmtp_auxiliaryDataSizeLength(unsigned int * _auxiliaryDataSizeLength);
	bool ParseAttribute_fmtp_ISMACrypCryptoSuite(char * _ISMACrypCryptoSuite, size_t len);
	bool ParseAttribute_fmtp_ISMACrypIVLength(unsigned int * _ISMACrypIVLength);
	bool ParseAttribute_fmtp_ISMACrypDeltaIVLength(unsigned int * _ISMACrypDeltaIVLength);
	bool ParseAttribute_fmtp_ISMACrypSelectiveEncryption(unsigned int * _ISMACrypSelectiveEncryption);
	bool ParseAttribute_fmtp_ISMACrypKeyIndicatorLength(unsigned int * _ISMACrypKeyIndicatorLength);
	bool ParseAttribute_fmtp_ISMACrypKeyIndicatorPerAU(unsigned int * _ISMACrypKeyIndicatorPerAU);
	bool ParseAttribute_fmtp_ISMACrypSalt(char * _ISMACrypSalt, size_t len);
	bool ParseAttribute_fmtp_ISMACrypKey(char * _ISMACrypKey, size_t len);
	bool ParseAttribute_fmtp_ISMACrypKMSID(unsigned int * _ISMACrypKMSID);
	bool ParseAttribute_fmtp_ISMACrypKMSVersion(unsigned int * _ISMACrypKMSVersion);
	bool ParseAttribute_fmtp_ISMACrypKMSSpecificData(char * _ISMACrypKMSSpecificData, size_t len);
	bool ParseAttribute_fmtp_Version_profile(unsigned int * _Version_profile);
	bool ParseAttribute_fmtp_Level(unsigned int * _Level);

private:
	list_T<char *>    m_listMediaDesLine;
private:
	CSDPSession * m_pSDPSession;
private:
	int m_index;
};


#endif //__SDP_MEDIA_H__
