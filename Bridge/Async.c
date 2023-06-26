#include "Defs.h"
#include "Tools.h"
#include "Async.h"

VOID
NTAPI
AsyncKernelRoutine(
	__in PKAPC Apc,
	__in PKNORMAL_ROUTINE* NormalRoutine,
	__in PVOID* NormalContext,
	__in PVOID* SystemArgument1,
	__in PVOID* SystemArgument2
)
{
	PAPC_BLOCK ApcBlock = NULL;
	ApcBlock = CONTAINING_RECORD(Apc, APC_BLOCK, Apc);

	RinFreePool(ApcBlock);
}

BOOLEAN
NTAPI
AsyncCall(
	__inout PVOID NormalRoutine,
	__in_opt PVOID NormalContext,
	__in KPROCESSOR_MODE ProcessorMode
)
{
	PAPC_BLOCK ApcBlock;

	ApcBlock = RinAllocatePool(NonPagedPool, sizeof(APC_BLOCK));

	if (NULL != ApcBlock) {
		RtlZeroMemory(ApcBlock, sizeof(APC_BLOCK));

		KeInitializeApc(
			&ApcBlock->Apc,
			KeGetCurrentThread(),
			OriginalApcEnvironment,
			AsyncKernelRoutine,
			NULL,
			NormalRoutine,
			ProcessorMode,
			NormalContext);

		if (FALSE != KeInsertQueueApc(
			&ApcBlock->Apc,
			NULL,
			NULL,
			LOW_PRIORITY)) {
			return TRUE;
		}
	}

	return FALSE;
}