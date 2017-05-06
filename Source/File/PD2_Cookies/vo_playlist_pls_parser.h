
#include "vo_playlist_parser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

struct PLS_MEDIA_ITEM
{
	VO_CHAR path[1024];
	VO_CHAR title[1024];
	VO_S32 duration;
	VO_S32 sequence_number;

	PLS_MEDIA_ITEM * ptr_next;
};

class vo_playlist_pls_parser :
	public vo_playlist_parser
{
public:
	vo_playlist_pls_parser(void);
	virtual ~vo_playlist_pls_parser(void);

	virtual VO_BOOL parse();

	VO_VOID print();

	virtual VO_BOOL get_url(VO_S32 index, VO_PCHAR strurl);

private:
	PLS_MEDIA_ITEM * getitem_by_sequencebnumber( VO_S32 sequence_number );

private:

	PLS_MEDIA_ITEM * m_playlist_items_head;
	PLS_MEDIA_ITEM * m_playlist_items_tail;
};

#ifdef _VONAMESPACE
}
#endif