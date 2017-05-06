// TSWriter.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TSWriter.h"


// This is an example of an exported variable
TSWRITER_API int nTSWriter=0;

// This is an example of an exported function.
TSWRITER_API int fnTSWriter(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see TSWriter.h for the class definition
CTSWriter::CTSWriter()
{
	return;
}
