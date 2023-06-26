#ifndef _ROUTINE_H_
#define _ROUTINE_H_

#ifdef __cplusplus
extern "C" {
#endif

	VOID
		NTAPI
		LoadImageNotifyRoutine(
			__in_opt PUNICODE_STRING FullImageName,
			__in HANDLE ProcessId,
			__in PIMAGE_INFO ImageInfo
		);

#ifdef __cplusplus
}
#endif

#endif
