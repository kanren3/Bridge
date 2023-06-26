#include "Defs.h"
#include "Thread.h"

PKTRAP_FRAME
NTAPI
GetTrapFrameThread(
	__in PETHREAD Thread
)
{
	PKTRAP_FRAME TrapFrame = NULL;
	ULONG64 InitialStack = 0;

	InitialStack = *(ULONG64*)((PUCHAR)Thread +
		GlobalBlock.KdDebuggerBlock.OffsetKThreadInitialStack);

	TrapFrame = (PKTRAP_FRAME)(InitialStack - sizeof(KTRAP_FRAME));

	return TrapFrame;
}

KTHREAD_STATE
NTAPI
GetThreadState(
	__in PKTHREAD Thread
)
{
	return *(PCCHAR)((PUCHAR)Thread +
		GlobalBlock.KdDebuggerBlock.OffsetKThreadState);
}

PLIST_ENTRY
NTAPI
GetProcessThreadListHead(
	__in PEPROCESS Process
)
{
	return (PLIST_ENTRY)((PUCHAR)Process + OffsetKProcessThreadListHead);
}

PETHREAD
NTAPI
GetProcessFirstThread(
	__in PEPROCESS Process
)
{
	return (PETHREAD)
		((PUCHAR)GetProcessThreadListHead(Process)->Flink - OffsetKThreadThreadListEntry);
}

PLIST_ENTRY
NTAPI
GetThreadListEntry(
	__in PETHREAD Thread
)
{
	return (PLIST_ENTRY)((PUCHAR)Thread + OffsetKThreadThreadListEntry);
}

PETHREAD
NTAPI
GetNexThread(
	__in PETHREAD Thread
)
{
	return (PETHREAD)
		((PUCHAR)(GetThreadListEntry(Thread)->Flink) - OffsetKThreadThreadListEntry);
}