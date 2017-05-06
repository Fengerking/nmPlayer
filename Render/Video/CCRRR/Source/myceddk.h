#pragma once

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS; 

typedef enum _INTERFACE_TYPE {
    InterfaceTypeUndefined = -1,
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    ProcessorInternal,
    InternalPowerBus,
    PNPISABus,
    PNPBus,
    MaximumInterfaceType
} INTERFACE_TYPE, *PINTERFACE_TYPE;

//
// DMA Adapter Description
//
typedef struct _DMA_ADAPTER_OBJECT_
{
    USHORT ObjectSize;                  // Size of structure (versioning).
    INTERFACE_TYPE InterfaceType;       // Adapter bus interface.
    ULONG BusNumber;                    // Adapter bus number.
} DMA_ADAPTER_OBJECT, *PDMA_ADAPTER_OBJECT;

// HalAllocateCommonBuffer

typedef PVOID (*HALALLOCATECOMMONBUFFER) (
  PDMA_ADAPTER_OBJECT DmaAdapter,
  ULONG Length,
  PPHYSICAL_ADDRESS LogicalAddress,
  BOOLEAN CacheEnabled
);
extern HALALLOCATECOMMONBUFFER HalAllocateCommonBuffer;

// HalFreeCommonBuffer

typedef VOID (*HALFREECOMMONBUFFER) (
  PDMA_ADAPTER_OBJECT DmaAdapter,
  ULONG Length,
  PHYSICAL_ADDRESS LogicalAddress,
  PVOID VirtualAddress,
  BOOLEAN CacheEnabled
);
extern HALFREECOMMONBUFFER HalFreeCommonBuffer;

typedef BOOL (*VIRTUALCOPY) (LPVOID lpvDest, LPVOID lpvSrc, DWORD cbSize, DWORD fdwProtect);
extern VIRTUALCOPY VirtualCopy;

typedef LPVOID (*ALLOCPHYSMEM) (
  DWORD cbSize,
  DWORD fdwProtect,
  DWORD dwAlignmentMask,
  DWORD dwFlags,
  PULONG pPhysicalAddress
);
extern ALLOCPHYSMEM AllocPhysMem;

typedef BOOL (*FREEPHYSMEM)(LPVOID lpvAddress);
extern FREEPHYSMEM FreePhysMem;

typedef PVOID (*MMMAPIOSPACE)( 
  PHYSICAL_ADDRESS PhysicalAddress, 
  ULONG NumberOfBytes, 
  BOOLEAN CacheEnable 
);
extern MMMAPIOSPACE MmMapIoSpace;

typedef VOID (*MMUNMAPIOSPACE)( 
  PVOID BaseAddress, 
  ULONG NumberOfBytes 
);
extern MMUNMAPIOSPACE MmUnmapIoSpace;

typedef BOOL (*VIRTUALSETATTRIBUTES)(
  LPVOID lpvAddress,
  DWORD cbSize,
  DWORD dwNewFlags,
  DWORD dwMask,
  LPDWORD lpdwOldFlags
);
extern VIRTUALSETATTRIBUTES VirtualSetAttributes;

typedef void (*CACHERANGEFLUSH)(
	 LPVOID pAddr,
	 DWORD dwLength,
	 DWORD dwFlags 
	 );
extern CACHERANGEFLUSH CacheRangeFlush;


BOOL OpenCEDDK();
void CloseCEDDK();

PVOID MapPhysicalMemory(ULONG physAddr, int size);
void UnmapPhysicalMemory(PVOID virtAddr, int size);

typedef HANDLE (*LOADKERNELLIBRARY)(LPCWSTR lpszFileName);
extern LOADKERNELLIBRARY LoadKernelLibrary;

#if	(_WIN32_WCE >= 600)	

typedef LPVOID (*VIRTUALALLOCCOPYEX)(
    HANDLE hSrcProc,
    HANDLE hDstProc,
    LPVOID pAddr,
    DWORD cbSize,
    DWORD dwProtect
);
extern VIRTUALALLOCCOPYEX VirtualAllocCopyEx;

typedef DWORD (*GETDIRECTCALLERPROCESSID)(void);
extern GETDIRECTCALLERPROCESSID GetDirectCallerProcessId;

#endif //(_WIN32_WCE >= 600)	

// Part of pkfuncs.h

#if defined(_ARM_)
#define PUserKData ((LPBYTE)0xFFFFC800)
#else
#define PUserKData ((LPBYTE)0x00005800)
#endif
#define KINFO_OFFSET     0x300

#define UserKInfo  ((long *)(PUserKData+KINFO_OFFSET))

/** Flags for LockPages: */
#define LOCKFLAG_WRITE      0x001   // write access required
#define LOCKFLAG_QUERY_ONLY 0x002   // query only, page in but don't lock
#define LOCKFLAG_READ       0x004   // read access required (as opposed to page present but PAGE_NOACCESS)

#define KINX_PROCARRAY  0   /* address of process array */
#define KINX_PAGESIZE   1   /* system page size */
#define KINX_PFN_SHIFT  2   /* shift for page # in PTE */
#define KINX_PFN_MASK   3   /* mask for page # in PTE */
#define KINX_PAGEFREE   4   /* # of free physical pages */
#define KINX_SYSPAGES   5   /* # of pages used by kernel */
#define KINX_KHEAP      6   /* ptr to kernel heap array */
#define KINX_SECTIONS   7   /* ptr to SectionTable array */
#define KINX_MEMINFO    8   /* ptr to system MemoryInfo struct */
#define KINX_MODULES    9   /* ptr to module list */
#define KINX_DLL_LOW   10   /* lower bound of DLL shared space */
#define KINX_NUMPAGES  11   /* total # of RAM pages */
#define KINX_PTOC      12   /* ptr to ROM table of contents */
#define KINX_KDATA_ADDR 13  /* kernel mode version of KData */
#define KINX_GWESHEAPINFO 14 /* Current amount of gwes heap in use */
#define KINX_TIMEZONEBIAS 15 /* Fast timezone bias info */
#define KINX_PENDEVENTS 16  /* bit mask for pending interrupt events */
#define KINX_KERNRESERVE 17 /* number of kernel reserved pages */
#define KINX_API_MASK 18    /* bit mask for registered api sets */
#define KINX_NLS_CP 19      /* hiword OEM code page, loword ANSI code page */
#define KINX_NLS_SYSLOC 20  /* Default System locale */
#define KINX_NLS_USERLOC 21 /* Default User locale */
#define KINX_HEAP_WASTE 22  /* Kernel heap wasted space */
#define KINX_DEBUGGER 23    /* For use by debugger for protocol communication */
#define KINX_APISETS 24     /* APIset pointers */
#define KINX_MINPAGEFREE 25 /* water mark of the minimum number of free pages */
#define KINX_CELOGSTATUS 26 /* CeLog status flags */
#define KINX_NKSECTION  27  /* Address of NKSection */
#define KINX_PTR_CURTOKEN    28  /* Events to be set after power on */
#define KINX_TIMECHANGECOUNT 29    /* # of times time changed */



typedef BOOL (*LOCKPAGES)( 
  LPVOID lpvAddress, 
  DWORD cbSize, 
  PDWORD pPFNs, 
  int fOptions 
);
extern LOCKPAGES LockPages;

typedef BOOL (*UNLOCKPAGES)( 
  LPVOID lpvAddress, 
  DWORD cbSize 
);
extern UNLOCKPAGES UnlockPages;