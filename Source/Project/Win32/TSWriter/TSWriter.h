// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TSWRITER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TSWRITER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TSWRITER_EXPORTS
#define TSWRITER_API __declspec(dllexport)
#else
#define TSWRITER_API __declspec(dllimport)
#endif

// This class is exported from the TSWriter.dll
class TSWRITER_API CTSWriter {
public:
	CTSWriter(void);
	// TODO: add your methods here.
};

extern TSWRITER_API int nTSWriter;

TSWRITER_API int fnTSWriter(void);
