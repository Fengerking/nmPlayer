#ifndef __VO_IREADER_LOADER_H__
#define __VO_IREADER_LOADER_H__

typedef VO_S32 (VO_API* voReadAPI) (VO_PTR   pReadHandle, VO_U32 uFlag);
class IReaderLoader
{
public:
	virtual ~IReaderLoader(){};
	virtual voReadAPI GetAPIEntry()=0;
};
class ReaderLoaderFactory
{
public:
	static IReaderLoader*  CreateOneReaderLoader(TCHAR* dllName,TCHAR* readerAPI,VO_PTR hInst);
	static void					  DestroyOneReaderLoader(IReaderLoader*);
};




#endif// __VO_IREADER_LOADER_H__