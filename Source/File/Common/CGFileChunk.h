#ifndef __CGFileChunk_H__
#define __CGFileChunk_H__


#include "voYYDef_filcmn.h"
#include "CvoBaseFileOpr.h"
#include "CvoBaseMemOpr.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define VO_FILE_CHUNK_SIZE	0x10000	//64KB

class CGFileChunk
	: public CvoBaseFileOpr
	, public CvoBaseMemOpr
{
public:
	CGFileChunk(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp);
	virtual ~CGFileChunk();

public:
	VO_BOOL	IsValid() {return (0 != m_hFile) ? VO_TRUE: VO_FALSE;}
	VO_VOID	SetShareFileHandle(VO_BOOL bValue);

public:
	//��������
	VO_BOOL	FCreate(VO_PTR hFile, VO_U64 ullFilePos = 0, VO_U32 dwChunkSize = VO_FILE_CHUNK_SIZE);

	//���ٺ���
	VO_VOID	FDestroy();

	//���ļ�λ�ö�λ��dwPosInFileλ�ô�
	VO_BOOL	FLocate(VO_U64 ullFilePos, VO_BOOL bForceMove = VO_FALSE);

	//���ļ���ǰλ�ô���ȡdwLen���ȵ����ݵ��ڴ�pData��
	//ֻ�ж�ȡ�������ݳ��ȵ���dwLen���ŷ���true������ȫ��ȡ
	VO_BOOL	FRead(VO_PTR pData, VO_U32 dwLen);

	//���ļ�dwFilePosλ�ô���ȡdwLen���ȵ����ݵ��ڴ�pData��
	//ֻ�ж�ȡ�������ݳ��ȵ���dwLen���ŷ���true������ȫ��ȡ
	VO_BOOL	FRead2(VO_PTR pData, VO_U32 dwLen, VO_U64 ullFilePos);

	//���ļ���ǰλ�ô���ȡdwLen���ȵ����ݵ��ڴ�pData��, ��ȡ��ʵ�ʳ��ȸ�ֵ��(*pdwReader)
	//ֻҪ��ȡ�������ݳ��ȴ���0���ͷ���true��������ȫ��ȡ
	VO_BOOL	FRead3(VO_PTR pData, VO_U32 dwLen, VO_U32* pdwReaded);

	//���ļ�dwFilePosλ�ô���ȡdwLen���ȵ����ݵ��ڴ�pData��, ��ȡ��ʵ�ʳ��ȸ�ֵ��(*pdwReader)
	//ֻҪ��ȡ�������ݳ��ȴ���0���ͷ���true��������ȫ��ȡ
	VO_BOOL	FRead4(VO_PTR pData, VO_U32 dwLen, VO_U64 ullFilePos, VO_U32* pdwReaded);

	//��ǰ����ullSkip
	VO_BOOL	FSkip(VO_U64 ullSkip);

	//��󷵻�ullBack
	VO_BOOL	FBack(VO_U64 ullBack);

	//�õ���ǰ�ļ�λ��
	VO_U64	FGetFilePos()	{return m_ullFilePos;}

	//����һ����¼��
	VO_VOID	FStartRecord()	{m_ullRecordPos = m_ullFilePos;}

	//������¼��
	VO_VOID	FStopRecord()	{m_ullRecordPos = -1;}

	//�õ���¼��
	VO_U64	FGetRecord()	{return m_ullRecordPos;}

	//�õ���ʼ��¼���ȡ���ֽ���
	VO_U64	FGetLastRead()	{return (m_ullFilePos - m_ullRecordPos);}

	//ֱ�ӻ�ò����ڴ�ָ�룬���������Χ������VO_NULL������
	VO_PBYTE	FGetFileContentPtr(VO_U32 dwLen);
	VO_S64		FGetFileSize(){return m_llFileSize;}
protected:
	//ʵ�ֶ��ļ��������ܵĺ���
	//return: true - read data; false - read no data
	VO_BOOL	ReadFileB(VO_PBYTE pBuffer, VO_S32 nToRead, VO_S32* pnReaded);

	//ʵ�ֶ�λ�ļ��������ܵĺ���
	//return: true - seek success; false - seek fail
	VO_BOOL	SeekFileB(VO_S64 llFilePos, VO_FILE_POS fsPos);

protected:
	VO_PTR				m_hFile;		//file handle
	VO_S64				m_llActualFilePos;	//actual position in file
	//share file handle with other, so we should reset file position before do actual file operation!!
	VO_BOOL				m_bFileHandleToShare;
	VO_BOOL				m_bFileHandleShared;

	VO_PBYTE			m_pContent;		//chunk content
	VO_S32				m_nSize;		//chunk actual size
	VO_U32				m_dwChunkSize;	//chunk size
	VO_U32				m_dwReadPos;	//chunk read position in m_pContent
	VO_U64				m_ullFilePos;	//chunk read position in file
	VO_U64				m_ullRecordPos;	//position recorded
	
	VO_S64				m_llFileSize;
};

#ifdef _VONAMESPACE
}
#endif

#endif	//__CGFileChunk_H__
