/*
 * Copyright (c) 2020 Austin Hudson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIEDi
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _WINAPI_H
#define _WINAPI_H

#include "payload_util.h"

#define SystemKiSystemCall64Shadow 0x652b6a00 

typedef enum _POOL_TYPE
{
  NonPagedPool = 0,
  PagedPool = 1,
  NonPagedPoolMustSucceed = 2,
  DontUseThisType = 3,
  NonPagedPoolCacheAligned = 4,
  PagedPoolCacheAligned = 5,
  NonPagedPoolCacheAlignedMustS = 6,
  MaxPoolType = 7,
  NonPagedPoolSession = 32,
  PagedPoolSession = 33,
  NonPagedPoolMustSucceedSession = 34,
  DontUseThisTypeSession = 35,
  NonPagedPoolCacheAlignedSession = 36,
  PagedPoolCacheAlignedSession = 37,
  NonPagedPoolCacheAlignedMustSSession = 38,
} POOL_TYPE, *PPOOL_TYPE;

typedef struct _SYSTEM_MODULE_ENTRY
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
} SYSTEM_MODULE_ENTRY, * PSYSTEM_MODULE_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Count;
	SYSTEM_MODULE_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

//
// ntoskrnl.exe!ZwQuerySystemInformation
//
typedef NTSTATUS (NTAPI *ZwQuerySystemInformation_t)(
  INT SystemInformationClass,
  PVOID SystemInformation,
  ULONG SystemInformationLength,
  PULONG ReturnLength
);

//
// ntoskrnl.exe!ExAllocatePool
//
typedef PVOID (NTAPI *ExAllocatePool_t)(
  POOL_TYPE PoolType,
  SIZE_T NumberOfBytes
);

//
// ntoskrnl.exe!ExFreePool
//
typedef VOID (NTAPI *ExFreePool_t)(
  PVOID Pointer
);

//
// ntoskrnl.exe!KeGetCurrentIrql
//
typedef INT (NTAPI *KeGetCurrentIrql_t)(
  VOID
);

//
// ntoskrnl.exe!KeLowerIrql
//
typedef VOID (NTAPI *KeLowerIrql_t)(
  UINT NewIrql
);

//
// msr!KiSystemCall64
//
typedef BOOLEAN (NTAPI *KiSystemCall64_t)(
  KTRAP_FRAME * Frame
);

//
// srv!SrvTransactionNotImplemented
//
typedef NTSTATUS (NTAPI * SrvTransactionNotImplemented_t)(
  PVOID WorkContext
);

struct Functions {
  ZwQuerySystemInformation_t ZwQuerySystemInformation;
  ExAllocatePool_t           ExAllocatePool;
  ExFreePool_t               ExFreePool;
  KeGetCurrentIrql_t         KeGetCurrentIrql;
  KeLowerIrql_t              KeLowerIrql;
};

struct Drivers {
  PVOID NtosKrnlBase;
  PVOID SrvSmbv1Base;
};

#endif
