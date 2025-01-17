#include "Defs.h"
#include "Async.h"
#include "Tools.h"
#include "Inject.h"
#include "Routine.h"

extern PWCHAR InjectProcessName;
extern PWCHAR InjectLoadedAfter;
extern PWCHAR InjectDllPath32;
extern PWCHAR InjectDllPath64;

VOID NTAPI
AsyncKernelNormalRoutine(
    __in PVOID NormalContext,
    __in PVOID SystemArgument1,
    __in PVOID SystemArgument2
)
{
    NTSTATUS Status;

    PUCHAR BaseAddress = NULL;
    SIZE_T RegionSize = PAGE_SIZE;

    PASYNC_USER_BLOCK64 SyncUser64;
    PASYNC_USER_BLOCK32 SyncUser32;

    if (NULL == NormalContext) {
        if (NULL != InjectDllPath64) {
            Status = ZwAllocateVirtualMemory(ZwCurrentProcess(),
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

                    LogSyncDbgPrint("x64 LdrLoadDll:%016llx\n", SyncUser64->LdrLoadDll);
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
                MEM_COMMIT | MEM_RESERVE,
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
                    LogSyncDbgPrint("x86 LdrLoadDll:%016llx\n", SyncUser32->LdrLoadDll);
                    AsyncCall(BaseAddress, SyncUser32, UserMode);
                }
            }
        }
    }
}

VOID NTAPI
LoadImageNotifyRoutine(
    __in_opt PUNICODE_STRING FullImageName,
    __in HANDLE ProcessId,
    __in PIMAGE_INFO ImageInfo
)
{
    NTSTATUS Status;
    PUNICODE_STRING ImageNameString;
    UNICODE_STRING DestinationString;
    UNICODE_STRING ProcessString;
    UNICODE_STRING AfterString;

    Status = SeLocateProcessImageName(IoGetCurrentProcess(), &ImageNameString);

    if (FALSE != NT_SUCCESS(Status)) {
        Status = RtlDowncaseUnicodeString(&DestinationString, ImageNameString, TRUE);

        if (FALSE != NT_SUCCESS(Status)) {
            RtlInitUnicodeString(&ProcessString, InjectProcessName);

            if (FALSE != FsRtlIsNameInExpression(&ProcessString, &DestinationString, FALSE, NULL)) {
                RtlFreeUnicodeString(&DestinationString);

                Status = RtlDowncaseUnicodeString(&DestinationString, FullImageName, TRUE);

                if (FALSE != NT_SUCCESS(Status)) {
                    RtlInitUnicodeString(&AfterString, InjectLoadedAfter);

                    if (FALSE != FsRtlIsNameInExpression(&AfterString, &DestinationString, FALSE, NULL)) {
                        AsyncCall(AsyncKernelNormalRoutine, PsGetProcessWow64Process(IoGetCurrentProcess()), KernelMode);
                    }

                    RtlFreeUnicodeString(&DestinationString);
                }
            }
            else {
                RtlFreeUnicodeString(&DestinationString);
            }
        }

        ExFreePool(ImageNameString);
    }
}