#include "Defs.h"
#include "Async.h"
#include "Tools.h"
#include "Inject.h"
#include "Routine.h"

extern PWCHAR InjectProcessName;
extern PWCHAR InjectDllPath32;
extern PWCHAR InjectDllPath64;

VOID
NTAPI
AsyncKernelNormalRoutine(
    __in PVOID NormalContext,
    __in PVOID SystemArgument1,
    __in PVOID SystemArgument2
)
{
    NTSTATUS Status = STATUS_SUCCESS;

    PUCHAR BaseAddress = NULL;
    SIZE_T RegionSize = PAGE_SIZE;

    PASYNC_USER_BLOCK64 SyncUser64 = NULL;
    PASYNC_USER_BLOCK32 SyncUser32 = NULL;

    if (NULL == NormalContext) {

        if (NULL != InjectDllPath64) {
            Status = ZwAllocateVirtualMemory(
                ZwCurrentProcess(),
                &BaseAddress,
                0,
                &RegionSize,
                MEM_COMMIT,
                PAGE_EXECUTE_READWRITE);

            if (FALSE != NT_SUCCESS(Status)) {
                RtlZeroMemory(BaseAddress, RegionSize);

                SyncUser64 = BaseAddress + sizeof(LoadShell64);
                SyncUser64->DllName.Buffer = BaseAddress + sizeof(LoadShell64) + sizeof(ASYNC_USER_BLOCK64);
                SyncUser64->DllName.Length = wcslen(InjectDllPath64) * 2;
                SyncUser64->DllName.MaximumLength = wcslen(InjectDllPath64) * 2;
                SyncUser64->LdrLoadDll = GetExportProcAddress64(
                    IoGetCurrentProcess(),
                    GetUserModuleHandle64(IoGetCurrentProcess(), L"ntdll.dll"),
                    "LdrLoadDll");

                if (NULL != SyncUser64->LdrLoadDll) {
                    RtlCopyMemory(
                        BaseAddress,
                        LoadShell64,
                        sizeof(LoadShell64));

                    RtlCopyMemory(
                        BaseAddress + sizeof(LoadShell64) + sizeof(ASYNC_USER_BLOCK64),
                        InjectDllPath64,
                        wcslen(InjectDllPath64) * 2);

                    RinDbgPrint("LoadShell64:%p BaseAddress:%p\n", LoadShell64, BaseAddress);

                    AsyncCall(BaseAddress, SyncUser64, UserMode);
                }
            }
        }
    }
    else {
        if (NULL != InjectDllPath32) {
            Status = ZwAllocateVirtualMemory(
                ZwCurrentProcess(),
                &BaseAddress,
                0,
                &RegionSize,
                MEM_COMMIT,
                PAGE_EXECUTE_READWRITE);

            if (FALSE != NT_SUCCESS(Status)) {
                RtlZeroMemory(BaseAddress, RegionSize);

                SyncUser32 = BaseAddress + sizeof(LoadShell32);
                SyncUser32->DllName.Buffer = BaseAddress + sizeof(LoadShell32) + sizeof(ASYNC_USER_BLOCK32);
                SyncUser32->DllName.Length = wcslen(InjectDllPath32) * 2;
                SyncUser32->DllName.MaximumLength = wcslen(InjectDllPath32) * 2;
                SyncUser32->LdrLoadDll = GetExportProcAddress32(
                    IoGetCurrentProcess(),
                    GetUserModuleHandle32(IoGetCurrentProcess(), L"ntdll.dll"),
                    "LdrLoadDll");

                if (NULL != SyncUser32->LdrLoadDll) {
                    RtlCopyMemory(
                        BaseAddress,
                        LoadShell32,
                        sizeof(LoadShell32));

                    RtlCopyMemory(
                        BaseAddress + sizeof(LoadShell32) + sizeof(ASYNC_USER_BLOCK32),
                        InjectDllPath32,
                        wcslen(InjectDllPath32) * 2);

                    PsWrapApcWow64Thread(&SyncUser32, &BaseAddress);

                    RinDbgPrint("LoadShell32:%p BaseAddress:%p\n", LoadShell32, BaseAddress);

                    AsyncCall(BaseAddress, SyncUser32, UserMode);
                }
            }
        }
    }
}

VOID
NTAPI
LoadImageNotifyRoutine(
    __in_opt PUNICODE_STRING FullImageName,
    __in HANDLE ProcessId,
    __in PIMAGE_INFO ImageInfo
)
{
    NTSTATUS Status;
    PWSTR LoadImagePath;
    PWSTR ImageFilePath;
    PUNICODE_STRING SourceString;
    UNICODE_STRING DestinationString;

    Status = SeLocateProcessImageName(IoGetCurrentProcess(), &SourceString);

    if (FALSE != NT_SUCCESS(Status)) {
        Status = RtlDowncaseUnicodeString(&DestinationString, SourceString, TRUE);

        if (FALSE != NT_SUCCESS(Status)) {
            ImageFilePath = RinAllocatePool(NonPagedPool, DestinationString.Length + 2);

            if (NULL != ImageFilePath) {
                RtlZeroMemory(ImageFilePath, DestinationString.Length + 2);
                RtlCopyMemory(ImageFilePath, DestinationString.Buffer, DestinationString.Length);

                if (0 != wcsstr(ImageFilePath, InjectProcessName)) {

                    if (NULL != FullImageName) {
                        LoadImagePath = RinAllocatePool(NonPagedPool, FullImageName->Length + 2);

                        if (NULL != LoadImagePath) {
                            RtlZeroMemory(LoadImagePath, FullImageName->Length + 2);
                            RtlCopyMemory(LoadImagePath, FullImageName->Buffer, FullImageName->Length);

                            if (NULL != wcsstr(LoadImagePath, L"System32\\kernel32.dll") ||
                                NULL != wcsstr(LoadImagePath, L"SysWOW64\\kernel32.dll")) {

                                AsyncCall(AsyncKernelNormalRoutine, PsGetProcessWow64Process(IoGetCurrentProcess()), KernelMode);
                            }

                            RinFreePool(LoadImagePath);
                        }
                    }
                }

                RinFreePool(ImageFilePath);
            }

            RtlFreeUnicodeString(&DestinationString);
        }

        ExFreePool(SourceString);
    }
}