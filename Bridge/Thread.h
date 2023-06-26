#ifndef _THREAD_H_
#define _THREAD_H_

#include "Bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OffsetKProcessThreadListHead 0x30
#define OffsetKThreadThreadListEntry 0x2F8

	PKTRAP_FRAME
		NTAPI
		GetTrapFrameThread(
			__in PETHREAD Thread
		);

	typedef enum _KTHREAD_STATE {
		Initialized,
		Ready,
		Running,
		Standby,
		Terminated,
		Waiting,
		Transition,
		DeferredReady,
		GateWait
	} KTHREAD_STATE;

	KTHREAD_STATE
		NTAPI
		GetThreadState(
			__in PKTHREAD Thread
		);

	PLIST_ENTRY
		NTAPI
		GetProcessThreadListHead(
			__in PEPROCESS Process
		);

	PETHREAD
		NTAPI
		GetProcessFirstThread(
			__in PEPROCESS Process
		);

	PLIST_ENTRY
		NTAPI
		GetThreadListEntry(
			__in PETHREAD Thread
		);

	PETHREAD
		NTAPI
		GetNexThread(
			__in PETHREAD Thread
		);

	typedef struct _RESUME_FRAME64 {
		ULONG64 StackPointer;
		ULONG64 ProgramCounter;
		ULONG64 SegCs;
		ULONG64 EFlags;
	}RESUME_FRAME64, * PRESUME_FRAME64;

	typedef struct _CALL_FRAME64 {
		CONTEXT ContextFrame;
		ULONG64 RundownRoutine;
		ULONG64 Parameter;
		RESUME_FRAME64 ResumeFrame;
	}CALL_FRAME64, * PCALL_FRAME64;

#define KGDT64_NULL (0 * 16)            // NULL descriptor
#define KGDT64_R0_CODE (1 * 16)         // kernel mode 64-bit code
#define KGDT64_R0_DATA (1 * 16) + 8     // kernel mode 64-bit data (stack)
#define KGDT64_R3_CMCODE (2 * 16)       // user mode 32-bit code
#define KGDT64_R3_DATA (2 * 16) + 8     // user mode 32-bit data
#define KGDT64_R3_CODE (3 * 16)         // user mode 64-bit code
#define KGDT64_SYS_TSS (4 * 16)         // kernel mode system task state
#define KGDT64_R3_CMTEB (5 * 16)        // user mode 32-bit TEB
#define KGDT64_R0_CMCODE (6 * 16)       // kernel mode 32-bit code
#define KGDT64_LAST (7 * 16)            // last entry

#define MODE_BIT 0
#define MODE_MASK 1
#define RPL_MASK 3

#ifdef __cplusplus
}
#endif

#endif
