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
#ifndef _PAYLOAD_UTIL_
#define _PAYLOAD_UTIL_

#include <windows.h>
#include <winternl.h>

#if defined(_UM_UTILS)

#define OFFSET_TO_STRUCT_WIN64 0x60
#define OFFSET_TO_STRUCT_WIN32 0x30

typedef struct _PEB_LDR_DATA_CUSTOM {
    ULONG      Length;
    BOOL       Initialized;
    PVOID      SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA_CUSTOM, *PPEB_LDR_DATA_CUSTOM;

typedef struct _LDR_DATA_TABLE_ENTRY_CUSTOM {
    LIST_ENTRY     InLoadOrderLinks;
    LIST_ENTRY     InMemoryOrderLinks;
    LIST_ENTRY     InInitializationOrderLinks;
    PVOID          DllBase;
    PVOID          EntryPoint;
    ULONG          SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY_CUSTOM, *PLDR_DATA_TABLE_ENTRY_CUSTOM;

#elif defined(_KM_UTILS)

#define OFFSET_TO_STRUCT_WIN64 FIELD_OFFSET(KPCR, SelfPcr)
#define OFFSET_TO_STRUCT_WIN32 FIELD_OFFSET(KPCR, SelfPcr)

/**
 * Since windows.h in mingw (understandably) doesnt have 
 * KPCR defined, we'll do so manually using some sub
 * headers (undoc64.h) & (undoc32.h)
**/
#if defined(_M_X64)
#include "undoc64.h"
#else
#include "undoc32.h"
#endif

#else
#error Please define either _KM_UTILS or _UM_UTILS
#endif

#endif
