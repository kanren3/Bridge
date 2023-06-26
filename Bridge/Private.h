#ifndef _PRIVATE_H_
#define _PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RinAllocatePool(a,b)(ExAllocatePoolWithTag(a, b, 'Rin'))
#define RinFreePool(a)(ExFreePoolWithTag(a, 'Rin'))

#ifdef __cplusplus
}
#endif

#endif
