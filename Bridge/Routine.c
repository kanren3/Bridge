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
    SIZE_T RegionSize = 0;
    ULONG FileSize;
    PVOID FileBuffer;

    PASYNC_USER_BLOCK64 SyncUser64;
    PASYNC_USER_BLOCK32 SyncUser32;

    if (NULL == NormalContext) {
        if (NULL != InjectDllPath64) {
            FileBuffer = ReadFile(InjectDllPath64, &FileSize);

            if (NULL != FileBuffer) {
                RegionSize += sizeof(DispatchrCode64);
                RegionSize += sizeof(LoaderCode64);
                RegionSize += sizeof(ASYNC_USER_BLOCK64);
                RegionSize += FileSize;

                Status = ZwAllocateVirtualMemory(ZwCurrentProcess(),
                                                 &BaseAddress,
                                                 0,
                                                 &RegionSize,
                                                 MEM_COMMIT | MEM_RESERVE,
                                                 PAGE_EXECUTE_READWRITE);

                if (FALSE != NT_SUCCESS(Status)) {
                    RtlZeroMemory(BaseAddress, RegionSize);
                    SyncUser64 = BaseAddress + sizeof(DispatchrCode64) + sizeof(LoaderCode64);
                    SyncUser64->DispatchrBase = BaseAddress;
                    SyncUser64->Loader = BaseAddress + sizeof(DispatchrCode64);
                    SyncUser64->ImageBase = BaseAddress + sizeof(DispatchrCode64) + sizeof(LoaderCode64) + sizeof(ASYNC_USER_BLOCK64);
                    SyncUser64->CleanBase = SyncUser64->Loader;
                    SyncUser64->CleanSize = FileSize - sizeof(DispatchrCode64);

                    RtlCopyMemory(
                        (PVOID)SyncUser64->DispatchrBase,
                        DispatchrCode64,
                        sizeof(DispatchrCode64));

                    RtlCopyMemory(
                        (PVOID)SyncUser64->Loader,
                        LoaderCode64,
                        sizeof(LoaderCode64));

                    RtlCopyMemory(
                        (PVOID)SyncUser64->ImageBase,
                        FileBuffer,
                        FileSize);

                    LogSyncDbgPrint("Loader64:%p ImageBase64:%p\n", (PVOID)SyncUser64->Loader, (PVOID)SyncUser64->ImageBase);
                    AsyncCall(BaseAddress, SyncUser64, UserMode);
                }
            }
        }
    }
    else {
        if (NULL != InjectDllPath32) {
            FileBuffer = ReadFile(InjectDllPath32, &FileSize);

            if (NULL != FileBuffer) {
                RegionSize += sizeof(DispatchrCode32);
                RegionSize += sizeof(LoaderCode32);
                RegionSize += sizeof(ASYNC_USER_BLOCK32);
                RegionSize += FileSize;

                Status = ZwAllocateVirtualMemory(ZwCurrentProcess(),
                                                 &BaseAddress,
                                                 0,
                                                 &RegionSize,
                                                 MEM_COMMIT | MEM_RESERVE,
                                                 PAGE_EXECUTE_READWRITE);

                if (FALSE != NT_SUCCESS(Status)) {
                    RtlZeroMemory(BaseAddress, RegionSize);
                    SyncUser32 = BaseAddress + sizeof(DispatchrCode32) + sizeof(LoaderCode32);
                    SyncUser32->DispatchrBase = BaseAddress;
                    SyncUser32->Loader = BaseAddress + sizeof(DispatchrCode32);
                    SyncUser32->ImageBase = BaseAddress + sizeof(DispatchrCode32) + sizeof(LoaderCode32) + sizeof(ASYNC_USER_BLOCK32);
                    SyncUser32->CleanBase = SyncUser32->Loader;
                    SyncUser32->CleanSize = FileSize - sizeof(DispatchrCode32);

                    RtlCopyMemory(
                        (PVOID)SyncUser32->DispatchrBase,
                        DispatchrCode32,
                        sizeof(DispatchrCode32));

                    RtlCopyMemory(
                        (PVOID)SyncUser32->Loader,
                        LoaderCode32,
                        sizeof(LoaderCode32));

                    RtlCopyMemory(
                        (PVOID)SyncUser32->ImageBase,
                        FileBuffer,
                        FileSize);

                    LogSyncDbgPrint("Loader32:%p ImageBase32:%p\n", (PVOID)SyncUser32->Loader, (PVOID)SyncUser32->ImageBase);

                    PsWrapApcWow64Thread(&SyncUser32, &BaseAddress);
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