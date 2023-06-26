#include "Defs.h"
#include "Bridge.h"
#include "Async.h"
#include "Routine.h"

PWCHAR InjectProcessName = NULL;
PWCHAR InjectDllPath32 = NULL;
PWCHAR InjectDllPath64 = NULL;

NTSTATUS
NTAPI
CreateKey(
    __out PHANDLE KeyHandle,
    __in ACCESS_MASK DesiredAccess,
    __in PCWSTR KeyList,
    __in ULONG CreateOptions
)
{
    NTSTATUS Status = STATUS_SUCCESS;
    OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
    UNICODE_STRING KeyPath = { 0 };

    RtlInitUnicodeString(&KeyPath, KeyList);

    InitializeObjectAttributes(
        &ObjectAttributes,
        &KeyPath,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);

    Status = ZwCreateKey(
        KeyHandle,
        DesiredAccess,
        &ObjectAttributes,
        0,
        NULL,
        CreateOptions,
        NULL);

    return Status;
}

NTSTATUS
NTAPI
OpenKey(
    __out PHANDLE KeyHandle,
    __in ACCESS_MASK DesiredAccess,
    __in PCWSTR KeyList
)
{
    NTSTATUS Status = STATUS_SUCCESS;
    OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
    UNICODE_STRING KeyPath = { 0 };

    RtlInitUnicodeString(&KeyPath, KeyList);

    InitializeObjectAttributes(
        &ObjectAttributes,
        &KeyPath,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);

    Status = ZwOpenKey(
        KeyHandle,
        DesiredAccess,
        &ObjectAttributes);

    return Status;
}

NTSTATUS
NTAPI
SetValueKey(
    __in HANDLE KeyHandle,
    __in PCWSTR ValueName,
    __in ULONG Type,
    __in PVOID Data,
    __in ULONG DataSize
)
{
    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING KeyValueName = { 0 };

    RtlInitUnicodeString(&KeyValueName, ValueName);

    Status = ZwSetValueKey(
        KeyHandle,
        &KeyValueName,
        0,
        Type,
        Data,
        DataSize);

    return Status;
}

PWSTR
NTAPI
RegQueryUnicodeString(
    __in PUNICODE_STRING RegistryPath,
    __in PCWSTR ValueName
)
{
    NTSTATUS Status;
    UNICODE_STRING ValueString;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE KeyHandle;
    PKEY_VALUE_PARTIAL_INFORMATION PartialInfo;
    ULONG ResultLength;
    UNICODE_STRING SourceString;
    UNICODE_STRING DestinationString;
    PWSTR ResultString = NULL;

    RtlInitUnicodeString(&ValueString, ValueName);

    InitializeObjectAttributes(
        &ObjectAttributes,
        RegistryPath,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);

    Status = ZwOpenKey(
        &KeyHandle,
        GENERIC_READ,
        &ObjectAttributes);

    if (FALSE != NT_SUCCESS(Status)) {
        Status = ZwQueryValueKey(KeyHandle,
                                 &ValueString,
                                 KeyValuePartialInformation,
                                 NULL,
                                 0,
                                 &ResultLength);

        if (STATUS_BUFFER_TOO_SMALL == Status) {
            PartialInfo = RinAllocatePool(NonPagedPool, ResultLength);

            if (NULL != PartialInfo) {
                Status = ZwQueryValueKey(KeyHandle,
                                         &ValueString,
                                         KeyValuePartialInformation,
                                         PartialInfo,
                                         ResultLength,
                                         &ResultLength);

                if (FALSE != NT_SUCCESS(Status)) {

                    if (REG_SZ == PartialInfo->Type) {
                        SourceString.Buffer = PartialInfo->Data;
                        SourceString.Length = (USHORT)PartialInfo->DataLength;
                        SourceString.MaximumLength = (USHORT)PartialInfo->DataLength;

                        Status = RtlDowncaseUnicodeString(&DestinationString, &SourceString, TRUE);

                        if (FALSE != NT_SUCCESS(Status)) {
                            ResultString = RinAllocatePool(NonPagedPool, DestinationString.Length + 2);

                            if (NULL != ResultString) {
                                RtlZeroMemory(ResultString, DestinationString.Length + 2);
                                RtlCopyMemory(ResultString, DestinationString.Buffer, DestinationString.Length);
                            }

                            RtlFreeUnicodeString(&DestinationString);
                        }
                    }
                }

                RinFreePool(PartialInfo);
            }
        }

        ZwClose(KeyHandle);
    }

    return ResultString;
}

VOID
NTAPI
DriverUnload(
    __in PDRIVER_OBJECT DriverObject
)
{
    LARGE_INTEGER Interval;

    PsRemoveLoadImageNotifyRoutine(LoadImageNotifyRoutine);

    Interval.QuadPart = Int32x32To64(500, -10 * 1000);

    KeDelayExecutionThread(KernelMode, FALSE, &Interval);

    if (NULL != InjectProcessName) {
        RinFreePool(InjectProcessName);
    }
    if (NULL != InjectDllPath32) {
        RinFreePool(InjectDllPath32);
    }
    if (NULL != InjectDllPath64) {
        RinFreePool(InjectDllPath64);
    }
}

NTSTATUS
NTAPI
DriverEntry(
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
)
{
    NTSTATUS Status = STATUS_SUCCESS;

    DriverObject->DriverUnload = DriverUnload;

    InjectProcessName = RegQueryUnicodeString(RegistryPath, L"ProcessName");

    if (NULL == InjectProcessName) {
        return STATUS_UNSUCCESSFUL;
    }

    InjectDllPath32 = RegQueryUnicodeString(RegistryPath, L"DllPath32");
    InjectDllPath64 = RegQueryUnicodeString(RegistryPath, L"DllPath64");

    PsSetLoadImageNotifyRoutine(LoadImageNotifyRoutine);

    return STATUS_SUCCESS;
}