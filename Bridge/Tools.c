#include "Defs.h"
#include "Tools.h"

VOID NTAPI
LogSyncDbgPrint(
    __in PCSTR Format,
    __in ...
)
{
    va_list ArgList;

    va_start(ArgList, Format);

    vDbgPrintExWithPrefix(
        "[Bridge]\t",
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        Format,
        ArgList);

    va_end(ArgList);
}

ULONG NTAPI
GetUserModuleHandle32(
    __in PEPROCESS Process,
    __in LPCWSTR DllName
)
{
    KAPC_STATE ApcSate = { 0 };

    PPEB32 Peb = NULL;
    PPEB_LDR_DATA32 Ldr = NULL;
    PLDR_DATA_TABLE_ENTRY32 HeadEntry = NULL;
    PLDR_DATA_TABLE_ENTRY32 NextEntry = NULL;

    WCHAR ModuleName[MAX_PATH] = { 0 };
    ULONG ModuleHandle = 0;

    Peb = PsGetProcessWow64Process(Process);

    KeStackAttachProcess(Process, &ApcSate);

    __try {
        if (NULL != Peb) {
            Ldr = (PPEB_LDR_DATA32)Peb->Ldr;

            if (NULL != Ldr) {
                if (&Ldr->InLoadOrderModuleList != (PLIST_ENTRY32)Ldr->InLoadOrderModuleList.Flink) {
                    HeadEntry = CONTAINING_RECORD(
                        &Ldr->InLoadOrderModuleList,
                        LDR_DATA_TABLE_ENTRY32,
                        InLoadOrderLinks);

                    NextEntry = CONTAINING_RECORD(
                        HeadEntry->InLoadOrderLinks.Flink,
                        LDR_DATA_TABLE_ENTRY32,
                        InLoadOrderLinks);

                    while (NextEntry != HeadEntry) {
                        RtlZeroMemory(
                            ModuleName,
                            sizeof(ModuleName));

                        RtlCopyMemory(
                            ModuleName,
                            (PVOID)NextEntry->BaseDllName.Buffer,
                            NextEntry->BaseDllName.Length);

                        if (FALSE == _wcsicmp(ModuleName, DllName)) {
                            ModuleHandle = NextEntry->DllBase;
                            break;
                        }

                        NextEntry = CONTAINING_RECORD(
                            NextEntry->InLoadOrderLinks.Flink,
                            LDR_DATA_TABLE_ENTRY32,
                            InLoadOrderLinks);
                    }
                }
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {

    }

    KeUnstackDetachProcess(&ApcSate);

    return ModuleHandle;
}

ULONGLONG NTAPI
GetUserModuleHandle64(
    __in PEPROCESS Process,
    __in LPCWSTR DllName
)
{
    KAPC_STATE ApcSate = { 0 };

    PPEB64 Peb = NULL;
    PPEB_LDR_DATA64 Ldr = NULL;
    PLDR_DATA_TABLE_ENTRY64 HeadEntry = NULL;
    PLDR_DATA_TABLE_ENTRY64 NextEntry = NULL;

    WCHAR ModuleName[MAX_PATH] = { 0 };
    ULONGLONG ModuleHandle = 0;

    Peb = PsGetProcessPeb(Process);

    KeStackAttachProcess(Process, &ApcSate);

    __try {
        if (NULL != Peb) {
            Ldr = (PPEB_LDR_DATA64)Peb->Ldr;

            if (NULL != Ldr) {
                if (&Ldr->InLoadOrderModuleList != (PLIST_ENTRY64)Ldr->InLoadOrderModuleList.Flink) {
                    HeadEntry = CONTAINING_RECORD(
                        &Ldr->InLoadOrderModuleList,
                        LDR_DATA_TABLE_ENTRY64,
                        InLoadOrderLinks);

                    NextEntry = CONTAINING_RECORD(
                        HeadEntry->InLoadOrderLinks.Flink,
                        LDR_DATA_TABLE_ENTRY64,
                        InLoadOrderLinks);

                    while (NextEntry != HeadEntry) {
                        RtlZeroMemory(
                            ModuleName,
                            sizeof(ModuleName));

                        RtlCopyMemory(
                            ModuleName,
                            (PVOID)NextEntry->BaseDllName.Buffer,
                            NextEntry->BaseDllName.Length);

                        if (FALSE == _wcsicmp(ModuleName, DllName)) {
                            ModuleHandle = NextEntry->DllBase;
                            break;
                        }

                        NextEntry = CONTAINING_RECORD(
                            NextEntry->InLoadOrderLinks.Flink,
                            LDR_DATA_TABLE_ENTRY64,
                            InLoadOrderLinks);
                    }
                }
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {

    }

    KeUnstackDetachProcess(&ApcSate);

    return ModuleHandle;
}

ULONG NTAPI
GetExportProcAddress32(
    __in PEPROCESS Process,
    __in ULONG ImageBase,
    __in LPCSTR FunctionName
)
{
    KAPC_STATE ApcSate = { 0 };

    PIMAGE_DOS_HEADER DosHeader;
    PIMAGE_NT_HEADERS32 NtHeaders;

    PIMAGE_EXPORT_DIRECTORY ExportDirectory;

    PULONG NameTable = 0;
    PCHAR Name = 0;

    PUSHORT OrdinalTable = 0;
    USHORT Ordinal = 0;

    PULONG AddressTable = 0;
    ULONG Address = 0;

    USHORT FunctionIndex = 0;
    USHORT NameIndex = 0;

    ULONG NtIndex = 0;

    if (0 == ImageBase) {
        return 0;
    }

    KeStackAttachProcess(Process, &ApcSate);

    __try {
        DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
        NtHeaders = (PIMAGE_NT_HEADERS32)(ImageBase + DosHeader->e_lfanew);

        ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(NtHeaders->OptionalHeader.
            DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + ImageBase);

        if (ExportDirectory) {

            NameTable = (PULONG)
                (ImageBase + ExportDirectory->AddressOfNames);

            OrdinalTable = (PUSHORT)
                (ImageBase + ExportDirectory->AddressOfNameOrdinals);

            AddressTable = (PULONG)
                (ImageBase + ExportDirectory->AddressOfFunctions);

            if (ExportDirectory->AddressOfNames &&
                ExportDirectory->AddressOfNameOrdinals &&
                ExportDirectory->AddressOfFunctions) {

                for (FunctionIndex = 0;
                    FunctionIndex < ExportDirectory->NumberOfFunctions;
                    FunctionIndex++) {

                    Name = NULL;

                    for (NameIndex = 0;
                        NameIndex < ExportDirectory->NumberOfNames;
                        NameIndex++) {
                        Ordinal = OrdinalTable[NameIndex];

                        if (Ordinal == FunctionIndex) {
                            Name = (PCHAR)(ImageBase + NameTable[NameIndex]);
                            Address = ImageBase + AddressTable[Ordinal];
                            Ordinal += (USHORT)ExportDirectory->Base;
                            break;
                        }
                    }

                    if (Name) {
                        if (!_strnicmp(FunctionName, Name, MAX_PATH)) {
                            KeUnstackDetachProcess(&ApcSate);
                            return Address;
                        }
                    }
                }
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {

    }

    KeUnstackDetachProcess(&ApcSate);
    return 0;
}

ULONGLONG NTAPI
GetExportProcAddress64(
    __in PEPROCESS Process,
    __in ULONG64 ImageBase,
    __in LPCSTR FunctionName
)
{
    KAPC_STATE ApcSate = { 0 };

    PIMAGE_DOS_HEADER DosHeader;
    PIMAGE_NT_HEADERS64 NtHeaders;

    PIMAGE_EXPORT_DIRECTORY ExportDirectory;

    PULONG NameTable = 0;
    PCHAR Name = 0;

    PUSHORT OrdinalTable = 0;
    USHORT Ordinal = 0;

    PULONG AddressTable = 0;
    ULONGLONG Address = 0;

    USHORT FunctionIndex = 0;
    USHORT NameIndex = 0;

    ULONG NtIndex = 0;

    if (0 == ImageBase) {
        return 0;
    }

    KeStackAttachProcess(Process, &ApcSate);

    __try {
        DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
        NtHeaders = (PIMAGE_NT_HEADERS64)(ImageBase + DosHeader->e_lfanew);

        ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(NtHeaders->OptionalHeader.
            DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + ImageBase);

        if (ExportDirectory) {

            NameTable = (PULONG)
                (ImageBase + ExportDirectory->AddressOfNames);

            OrdinalTable = (PUSHORT)
                (ImageBase + ExportDirectory->AddressOfNameOrdinals);

            AddressTable = (PULONG)
                (ImageBase + ExportDirectory->AddressOfFunctions);

            if (ExportDirectory->AddressOfNames &&
                ExportDirectory->AddressOfNameOrdinals &&
                ExportDirectory->AddressOfFunctions) {

                for (FunctionIndex = 0;
                    FunctionIndex < ExportDirectory->NumberOfFunctions;
                    FunctionIndex++) {

                    Name = NULL;

                    for (NameIndex = 0;
                        NameIndex < ExportDirectory->NumberOfNames;
                        NameIndex++) {
                        Ordinal = OrdinalTable[NameIndex];

                        if (Ordinal == FunctionIndex) {
                            Name = (PCHAR)(ImageBase + NameTable[NameIndex]);
                            Address = ImageBase + AddressTable[Ordinal];
                            Ordinal += (USHORT)ExportDirectory->Base;
                            break;
                        }
                    }

                    if (Name) {
                        if (!_strnicmp(FunctionName, Name, MAX_PATH)) {
                            KeUnstackDetachProcess(&ApcSate);
                            return Address;
                        }
                    }
                }
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {

    }

    KeUnstackDetachProcess(&ApcSate);
    return 0;
}