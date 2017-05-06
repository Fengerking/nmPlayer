
#ifndef __CBaseBitmap_H__
#define __CBaseBitmap_H__

#define	SIZE_MASKS	12
const DWORD bits565[3] = {0X0000F800, 0X000007E0, 0X0000001F,};

class CBaseBitmap
{
public:
	CBaseBitmap(void);
	virtual ~CBaseBitmap(void);

	virtual int		CreateBitmapInfo (HDC hDC, int nBits, int nWidth, int nHeight);
	virtual int		CreateBitmap (HDC hDC, int nBits, int nWidth, int nHeight);

	virtual LPBYTE	GetBitmapBuffer (void);
	virtual HBITMAP	GetBitmapHandle (HDC hDC);

	virtual int		GetWidth (void) {return m_nWidth;}
	virtual int		GetHeight (void) {return abs (m_nHeight);}

	BITMAPINFO*		GetBmpInfo(){return m_pBmpInfo;};
	bool			IsWide(){return m_bWide;};
	void			SetWide(bool bWide){m_bWide = bWide;};
	void			SetBkColor(COLORREF clr);
	virtual void	Release (void);
	int				GetBmpDataWidth (void);
	bool			m_bCloseHandleWhenRelease;
	bool			ResampleBiLinearGeneral(CBaseBitmap* pSrc, int heightDirect);
	HRGN			CreateRgn (int clrMask, int nLeft , int nTop);

protected:
	virtual bool	CreateBitmapInfo (void);

protected:
	int				m_nWidth;
	int				m_nHeight;
	int				m_nBits;

	LPBYTE 			m_pData;
	HBITMAP 		m_pHandle;

	BITMAPINFO *	m_pBmpInfo;
	int				m_nBmpSize;

	bool			m_bWide;
};

#endif // _H_CBaseBitmap_