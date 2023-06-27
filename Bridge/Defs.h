#ifndef _DEFS_H_
#define _DEFS_H_

#include <ntifs.h>
#include <windef.h>
#include <ntstrsafe.h>
#include <ntimage.h>
#include <intrin.h>

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
