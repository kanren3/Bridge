#ifndef _TOOLS_H_
#define _TOOLS_H_

#ifdef __cplusplus
extern "C" {
#endif

	VOID
		NTAPI
		LogSyncDbgPrint(
			__in PCSTR Format,
			__in ...
		);

#pragma pack(4)

	typedef struct _PEB32
	{
		UCHAR InheritedAddressSpace;
		UCHAR ReadImageFileExecOptions;
		UCHAR BeingDebugged;
		UCHAR BitField;
		ULONG Mutant;
		ULONG ImageBaseAddress;
		ULONG Ldr;
		ULONG ProcessParameters;
		ULONG SubSystemData;
		ULONG ProcessHeap;
		ULONG FastPebLock;
		ULONG AtlThunkSListPtr;
		ULONG IFEOKey;
		ULONG CrossProcessFlags;
		ULONG UserSharedInfoPtr;
		ULONG SystemReserved;
		ULONG AtlThunkSListPtr32;
		ULONG ApiSetMap;
	} PEB32, * PPEB32;

	typedef struct _PEB_LDR_DATA32
	{
		ULONG Length;
		BOOLEAN Initialized;
		ULONG SsHandle;
		LIST_ENTRY32 InLoadOrderModuleList;
		LIST_ENTRY32 InMemoryOrderModuleList;
		LIST_ENTRY32 InInitializationOrderModuleList;
		ULONG EntryInProgress;
	} PEB_LDR_DATA32, * PPEB_LDR_DATA32;

	typedef struct _LDR_DATA_TABLE_ENTRY32
	{
		LIST_ENTRY32 InLoadOrderLinks;
		LIST_ENTRY32 InMemoryOrderModuleList;
		LIST_ENTRY32 InInitializationOrderModuleList;
		ULONG DllBase;
		ULONG EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING32 FullDllName;
		UNICODE_STRING32 BaseDllName;
		ULONG Flags;
		USHORT LoadCount;
		USHORT TlsIndex;
		union
		{
			LIST_ENTRY32 HashLinks;
			ULONG SectionPointer;
		};
		ULONG CheckSum;
		union
		{
			ULONG TimeDateStamp;
			ULONG LoadedImports;
		};
		ULONG EntryPointActivationContext;
		ULONG PatchInformation;
	} LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;

#pragma pack()

#pragma pack(8)

	typedef struct _PROCESS_BASIC_INFORMATION64 {
		ULONG64 ExitStatus;
		ULONG64 PebBaseAddress;
		ULONG64 AffinityMask;
		ULONG64 BasePriority;
		ULONG64 UniqueProcessId;
		ULONG64 InheritedFromUniqueProcessId;
	} PROCESS_BASIC_INFORMATION64, * PPROCESS_BASIC_INFORMATION64;

	typedef struct _PEB64
	{
		UCHAR InheritedAddressSpace;
		UCHAR ReadImageFileExecOptions;
		UCHAR BeingDebugged;
		UCHAR BitField;
		ULONG64 Mutant;
		ULONG64 ImageBaseAddress;
		ULONG64 Ldr;
		ULONG64 ProcessParameters;
		ULONG64 SubSystemData;
		ULONG64 ProcessHeap;
		ULONG64 FastPebLock;
		ULONG64 AtlThunkSListPtr;
		ULONG64 IFEOKey;
		ULONG64 CrossProcessFlags;
		ULONG64 UserSharedInfoPtr;
		ULONG SystemReserved;
		ULONG AtlThunkSListPtr32;
		ULONG64 ApiSetMap;
	} PEB64, * PPEB64;

	typedef struct _PEB_LDR_DATA64
	{
		ULONG Length;
		BOOLEAN Initialized;
		ULONG64 SsHandle;
		LIST_ENTRY64 InLoadOrderModuleList;
		LIST_ENTRY64 InMemoryOrderModuleList;
		LIST_ENTRY64 InInitializationOrderModuleList;
		ULONG64 EntryInProgress;
	} PEB_LDR_DATA64, * PPEB_LDR_DATA64;

	typedef struct _LDR_DATA_TABLE_ENTRY64
	{
		LIST_ENTRY64 InLoadOrderLinks;
		LIST_ENTRY64 InMemoryOrderModuleList;
		LIST_ENTRY64 InInitializationOrderModuleList;
		ULONG64 DllBase;
		ULONG64 EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING64 FullDllName;
		UNICODE_STRING64 BaseDllName;
		ULONG Flags;
		USHORT LoadCount;
		USHORT TlsIndex;
		union
		{
			LIST_ENTRY64 HashLinks;
			ULONG64 SectionPointer;
		};
		ULONG CheckSum;
		union
		{
			ULONG TimeDateStamp;
			ULONG64 LoadedImports;
		};
		ULONG64 EntryPointActivationContext;
		ULONG64 PatchInformation;
	} LDR_DATA_TABLE_ENTRY64, * PLDR_DATA_TABLE_ENTRY64;

#pragma pack()

	NTKERNELAPI
		PPEB64
		PsGetProcessPeb(
			__in PEPROCESS Process
		);

	NTKERNELAPI
		PPEB32
		PsGetProcessWow64Process(
			__in PEPROCESS Process
		);

	ULONG
		NTAPI
		GetUserModuleHandle32(
			__in PEPROCESS Process,
			__in LPCWSTR DllName
		);

	ULONGLONG
		NTAPI
		GetUserModuleHandle64(
			__in PEPROCESS Process,
			__in LPCWSTR DllName
		);

	ULONG
		NTAPI
		GetExportProcAddress32(
			__in PEPROCESS Process,
			__in ULONG ImageBase,
			__in LPCSTR FunctionName
		);

	ULONGLONG
		NTAPI
		GetExportProcAddress64(
			__in PEPROCESS Process,
			__in ULONG64 ImageBase,
			__in LPCSTR FunctionName
		);

#ifdef __cplusplus
}
#endif

#endif
