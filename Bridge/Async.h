#ifndef _ASYNC_H_
#define _ASYNC_H_

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum _KAPC_ENVIRONMENT {
		OriginalApcEnvironment,
		AttachedApcEnvironment,
		CurrentApcEnvironment,
		InsertApcEnvironment
	} KAPC_ENVIRONMENT;

	typedef
		VOID
		(*PKNORMAL_ROUTINE) (
			IN PVOID NormalContext,
			IN PVOID SystemArgument1,
			IN PVOID SystemArgument2
			);

	typedef
		VOID
		(*PKKERNEL_ROUTINE) (
			IN struct _KAPC* Apc,
			IN OUT PKNORMAL_ROUTINE* NormalRoutine,
			IN OUT PVOID* NormalContext,
			IN OUT PVOID* SystemArgument1,
			IN OUT PVOID* SystemArgument2
			);

	typedef
		VOID
		(*PKRUNDOWN_ROUTINE) (
			IN struct _KAPC* Apc
			);

	NTKERNELAPI
		VOID
		KeInitializeApc(
			__out PRKAPC Apc,
			__in PRKTHREAD Thread,
			__in KAPC_ENVIRONMENT Environment,
			__in PKKERNEL_ROUTINE KernelRoutine,
			__in_opt PKRUNDOWN_ROUTINE RundownRoutine,
			__in_opt PKNORMAL_ROUTINE NormalRoutine,
			__in_opt KPROCESSOR_MODE ProcessorMode,
			__in_opt PVOID NormalContext
		);

	NTKERNELAPI
		BOOLEAN
		KeInsertQueueApc(
			__inout PRKAPC Apc,
			__in_opt PVOID SystemArgument1,
			__in_opt PVOID SystemArgument2,
			__in KPRIORITY Increment
		);

	typedef struct _APC_BLOCK {
		KAPC Apc;
		KEVENT Notify;
	}APC_BLOCK, * PAPC_BLOCK;

	BOOLEAN
		NTAPI
		AsyncCall(
			__inout PVOID NormalRoutine,
			__in_opt PVOID NormalContext,
			__in KPROCESSOR_MODE ProcessorMode
		);

#ifdef __cplusplus
}
#endif

#endif
