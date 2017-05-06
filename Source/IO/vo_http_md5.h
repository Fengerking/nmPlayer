#pragma once
#include "voYYDef_SourceIO.h"
#include "voString.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

/* vo_http_md5 declaration. */
class vo_http_md5 {
public:
	vo_http_md5();
	void update(VO_BYTE* input, VO_U32 length);
	const VO_BYTE* digest();
	void reset();

private:
	void final();
	void transform(const VO_BYTE block[64]);
	void encode(const VO_U32* input, VO_BYTE* output, size_t length);
	void decode(const VO_BYTE* input, VO_U32* output, size_t length);

	/* class uncopyable */
	vo_http_md5(const vo_http_md5&);
	vo_http_md5& operator=(const vo_http_md5&);

private:
	VO_U32 m_state[4];	/* state (ABCD) */
	VO_U32 m_count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	VO_BYTE m_buffer[64];	/* input buffer */
	VO_BYTE m_digest[16];	/* message digest */
	bool m_finished;		/* calculate finished ? */

	static const VO_BYTE PADDING[64];	/* padding for calculate */
};

#ifdef _VONAMESPACE
}
#endif
