#ifndef _DEFS_H_
#define _DEFS_H_

#define _CRT_SECURE_NO_WARNINGS
#define _NO_CRT_STDIO_INLINE

#define DRIVER_NAME "[Bridge]"

#include <ntifs.h>
#include <windef.h>
#include <ntimage.h>

#include "Private.h"

#pragma warning(disable:4133)
#pragma warning(disable:4047)

#ifdef __cplusplus
extern "C" {
#endif

	NTSYSAPI
		PCHAR
		PsGetProcessImageFileName(
			__in PEPROCESS Process
		);

#ifdef __cplusplus
}
#endif

#endif
