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
	//创建函数
	VO_BOOL	FCreate(VO_PTR hFile, VO_U64 ullFilePos = 0, VO_U32 dwChunkSize = VO_FILE_CHUNK_SIZE);

	//销毁函数
	VO_VOID	FDestroy();

	//将文件位置定位到dwPosInFile位置处
	VO_BOOL	FLocate(VO_U64 ullFilePos, VO_BOOL bForceMove = VO_FALSE);

	//从文件当前位置处读取dwLen长度的数据到内存pData里
	//只有读取到的数据长度等于dwLen，才返回true，即完全读取
	VO_BOOL	FRead(VO_PTR pData, VO_U32 dwLen);

	//从文件dwFilePos位置处读取dwLen长度的数据到内存pData里
	//只有读取到的数据长度等于dwLen，才返回true，即完全读取
	VO_BOOL	FRead2(VO_PTR pData, VO_U32 dwLen, VO_U64 ullFilePos);

	//从文件当前位置处读取dwLen长度的数据到内存pData里, 读取的实际长度赋值给(*pdwReader)
	//只要读取到的数据长度大于0，就返回true，即不完全读取
	VO_BOOL	FRead3(VO_PTR pData, VO_U32 dwLen, VO_U32* pdwReaded);

	//从文件dwFilePos位置处读取dwLen长度的数据到内存pData里, 读取的实际长度赋值给(*pdwReader)
	//只要读取到的数据长度大于0，就返回true，即不完全读取
	VO_BOOL	FRead4(VO_PTR pData, VO_U32 dwLen, VO_U64 ullFilePos, VO_U32* pdwReaded);

	//向前跳过ullSkip
	VO_BOOL	FSkip(VO_U64 ullSkip);

	//向后返回ullBack
	VO_BOOL	FBack(VO_U64 ullBack);

	//得到当前文件位置
	VO_U64	FGetFilePos()	{return m_ullFilePos;}

	//设置一个记录点
	VO_VOID	FStartRecord()	{m_ullRecordPos = m_ullFilePos;}

	//结束记录点
	VO_VOID	FStopRecord()	{m_ullRecordPos = -1;}

	//得到记录点
	VO_U64	FGetRecord()	{return m_ullRecordPos;}

	//得到开始记录后读取的字节数
	VO_U64	FGetLastRead()	{return (m_ullFilePos - m_ullRecordPos);}

	//直接获得操作内存指针，如果超出范围，返回VO_NULL，慎用
	VO_PBYTE	FGetFileContentPtr(VO_U32 dwLen);
	VO_S64		FGetFileSize(){return m_llFileSize;}
protected:
	//实现读文件基本功能的函数
	//return: true - read data; false - read no data
	VO_BOOL	ReadFileB(VO_PBYTE pBuffer, VO_S32 nToRead, VO_S32* pnReaded);

	//实现定位文件基本功能的函数
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
