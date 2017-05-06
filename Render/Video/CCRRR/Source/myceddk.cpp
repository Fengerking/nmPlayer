#include <windows.h>
#include "myceddk.h"

static HMODULE g_hCEDDK = NULL;
static HMODULE g_hCOREDLL = NULL;

HALALLOCATECOMMONBUFFER HalAllocateCommonBuffer = NULL;
HALFREECOMMONBUFFER HalFreeCommonBuffer = NULL;
MMMAPIOSPACE MmMapIoSpace = NULL;
MMUNMAPIOSPACE MmUnmapIoSpace = NULL;

VIRTUALCOPY VirtualCopy = NULL;
ALLOCPHYSMEM AllocPhysMem = NULL;
FREEPHYSMEM FreePhysMem = NULL;

VIRTUALSETATTRIBUTES VirtualSetAttributes = NULL;
CACHERANGEFLUSH		 CacheRangeFlush = NULL;

LOCKPAGES LockPages = NULL;
UNLOCKPAGES UnlockPages = NULL;

#if	(_WIN32_WCE >= 600)	
VIRTUALALLOCCOPYEX VirtualAllocCopyEx = NULL;
GETDIRECTCALLERPROCESSID GetDirectCallerProcessId = NULL;
#endif //(_WIN32_WCE >= 600)	

BOOL OpenCEDDK()
{
	if (g_hCEDDK)
		return TRUE;
	g_hCEDDK = LoadLibrary(_T("ceddk.dll")); 
	if (g_hCEDDK == NULL)
		return FALSE;
	HalAllocateCommonBuffer = (HALALLOCATECOMMONBUFFER) GetProcAddress(g_hCEDDK, L"HalAllocateCommonBuffer");
	HalFreeCommonBuffer = (HALFREECOMMONBUFFER) GetProcAddress(g_hCEDDK, L"HalFreeCommonBuffer");
	MmMapIoSpace = (MMMAPIOSPACE) GetProcAddress(g_hCEDDK, L"MmMapIoSpace");
	MmUnmapIoSpace = (MMUNMAPIOSPACE) GetProcAddress(g_hCEDDK, L"MmUnmapIoSpace");
	if ((HalAllocateCommonBuffer == NULL) || (HalFreeCommonBuffer == NULL)
		|| (MmMapIoSpace == NULL) || (MmUnmapIoSpace == NULL) )
		return FALSE;

	g_hCOREDLL = LoadLibrary(_T("coredll.dll"));
	if (g_hCOREDLL == NULL)
		return FALSE;
	VirtualCopy = (VIRTUALCOPY) GetProcAddress(g_hCOREDLL, L"VirtualCopy");
	if (VirtualCopy == NULL)
		return FALSE;

	AllocPhysMem = (ALLOCPHYSMEM) GetProcAddress(g_hCOREDLL, L"AllocPhysMem");
	FreePhysMem = (FREEPHYSMEM) GetProcAddress(g_hCOREDLL, L"FreePhysMem");
	VirtualSetAttributes = (VIRTUALSETATTRIBUTES) GetProcAddress(g_hCOREDLL, L"VirtualSetAttributes");
	CacheRangeFlush = (CACHERANGEFLUSH)GetProcAddress(g_hCOREDLL, L"CacheRangeFlush");
	if ((AllocPhysMem == NULL) || (FreePhysMem == NULL) || (VirtualSetAttributes == NULL) || (CacheRangeFlush == NULL))
		return FALSE;

	LockPages = (LOCKPAGES) GetProcAddress(g_hCOREDLL, L"LockPages");
	UnlockPages = (UNLOCKPAGES) GetProcAddress(g_hCOREDLL, L"UnlockPages");
	if ((LockPages == NULL) || (UnlockPages == NULL))
		return FALSE;

#if	(_WIN32_WCE >= 600)	
	VirtualAllocCopyEx = (VIRTUALALLOCCOPYEX) GetProcAddress(g_hCOREDLL, L"VirtualAllocCopyEx");
	if (VirtualAllocCopyEx == NULL)
		return FALSE;
	GetDirectCallerProcessId = (GETDIRECTCALLERPROCESSID) GetProcAddress(g_hCOREDLL, L"GetDirectCallerProcessId");
	if (GetDirectCallerProcessId == NULL)
		return FALSE;
#endif //(_WIN32_WCE >= 600)	

	return TRUE;
}

void CloseCEDDK()
{
	if (g_hCOREDLL)
	{
		FreeLibrary(g_hCOREDLL);
		g_hCOREDLL = NULL;
	}

	if (g_hCEDDK)
	{
		FreeLibrary(g_hCEDDK);
		g_hCEDDK = NULL;
	}
}

#if 0

PVOID MapPhysicalMemory(ULONG physAddr, int size)
{
	PVOID virt = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
	BOOL b = VirtualCopy(virt, (LPVOID)(physAddr >> 8), size, PAGE_PHYSICAL | PAGE_READWRITE);
	if (b)
		return virt;
	VirtualFree(virt, size, MEM_RELEASE);
	return NULL;
}

void UnmapPhysicalMemory(PVOID virtAddr, int size)
{
	VirtualFree(virtAddr, size, MEM_RELEASE);
}

#else

PVOID MapPhysicalMemory(ULONG physAddr, int size)
{
	PHYSICAL_ADDRESS pa = { physAddr, 0 };
	return MmMapIoSpace(pa, size, FALSE);
}

void UnmapPhysicalMemory(PVOID virtAddr, int size)
{
	MmUnmapIoSpace(virtAddr, size);
}

#endif