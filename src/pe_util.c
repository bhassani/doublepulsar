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
#include <ntstatus.h>
#include <winternl.h>
#include "payload_util.h"
#include "hs_util.h"
#include "pe_util.h"
#include "hashes.h"
#include "winapi.h"

PVOID GetPeBase(DWORD DrvHsh)
{
#if defined(_M_X64) || defined(_WIN64)
  PKPCR PcrPtr = (PKPCR)__readgsqword(FIELD_OFFSET(KPCR, SelfPcr));

  unsigned short InterruptEntryLow = 0;
  unsigned short InterruptEntryMid = 0;
  unsigned long  InterruptEntryTop = 0;
  void          *InterruptEntryPtr = 0;

  InterruptEntryLow = PcrPtr->IdtBase->OffsetLow;
  InterruptEntryMid = PcrPtr->IdtBase->OffsetMiddle;
  InterruptEntryTop = PcrPtr->IdtBase->OffsetHigh;

  InterruptEntryPtr  = (PVOID)(
    ((ULONG64)InterruptEntryTop << 32) +
    ((ULONG32)InterruptEntryMid << 16) +
    InterruptEntryLow);
#else
  PKPCR PcrPtr = (PKPCR)__readfsdword(FIELD_OFFSET(KPCR, SelfPcr));

  unsigned short InterruptEntryLow = 0;
  unsigned short InterruptEntryTop = 0;
  void          *InterruptEntryPtr = 0;

  InterruptEntryLow = PcrPtr->IDT->Offset;
  InterruptEntryTop = PcrPtr->IDT->ExtendedOffset;

  InterruptEntryPtr = (PVOID)(
    ((ULONG32)InterruptEntryTop << 16) +
    InterruptEntryLow);
#endif
  struct Functions           FuncTable   = { 0 };
  ULONG                      StructSize  = 0;
  DWORD                      StringHsh   = 0;
  PSYSTEM_MODULE_INFORMATION SystemInf   = 0;
  PSYSTEM_MODULE_ENTRY       SystemEnt   = 0;
  ULONG64                    ModuleBase  = 0;

  InterruptEntryPtr = (PVOID)(((ULONG_PTR)InterruptEntryPtr) &~ 0xfff);
  while ( (*(UINT16 *)InterruptEntryPtr) != IMAGE_DOS_SIGNATURE )
  {
    InterruptEntryPtr = (PVOID)(((ULONG_PTR)InterruptEntryPtr) - 0x1000);
  };

  if ( DrvHsh == HASH_NTOSKRNL )
    return InterruptEntryPtr;

  /**
   * For kernel-mode functionality to work properly,
   * we have to search for the following functions
   * using GetPeFunc():
   *
   *  1) ZwQuerySystemInformation();
   *  2) ExAllocatePool();
   *  3) ExFreePool();
   *
   * Furthermore - InterruptEntryPtr is actuall the 
   * base address of ntoskrnl.exe - so ... yeah. 
  **/
  FuncTable.ZwQuerySystemInformation = 
    GetPeFunc(InterruptEntryPtr, HASH_ZWQUERYSYSTEMINFORMATION);
  FuncTable.ExAllocatePool = 
    GetPeFunc(InterruptEntryPtr, HASH_EXALLOCATEPOOL);
  FuncTable.ExFreePool =
    GetPeFunc(InterruptEntryPtr, HASH_EXFREEPOOL);

  FuncTable.ZwQuerySystemInformation(0xb, &StructSize, 0, &StructSize);
  SystemInf = FuncTable.ExAllocatePool(PagedPool, StructSize);
  FuncTable.ZwQuerySystemInformation(0xb, SystemInf, StructSize, 0);

  SystemEnt = SystemInf->Module;
  for ( int i = 0 ; i < SystemInf->Count ; i++ )
  {
    StringHsh = HashStringDjb2((PCHAR)(
      SystemEnt[i].FullPathName + SystemEnt[i].OffsetToFileName), 0);
    if ( StringHsh == DrvHsh )
#if defined(_M_X64) 
      ModuleBase = (ULONG64)SystemEnt[i].ImageBase;
#else
      ModuleBase = (ULONG32)SystemEnt[i].ImageBase;
#endif
  };

  SystemEnt = NULL;
  if ( SystemInf != NULL )
    FuncTable.ExFreePool(SystemInf);

  FuncTable.ZwQuerySystemInformation = NULL;
  FuncTable.ExAllocatePool           = NULL;
  FuncTable.ExFreePool               = NULL;

  return (void *)ModuleBase;
};

PVOID GetPeFunc(PVOID ModPtr, DWORD FunHsh)
{
  PIMAGE_DOS_HEADER       DosHdr = 0;
  PIMAGE_NT_HEADERS       NtsHdr = 0;
  PIMAGE_EXPORT_DIRECTORY ExpHdr = 0;
  DWORD                   StrHsh = 0;
  PDWORD                  StrOff = 0;
  PDWORD                  FunOff = 0;
  PCHAR                   StrPln = 0;
  PUSHORT                 OrdOff = 0;

  DosHdr = (PIMAGE_DOS_HEADER)ModPtr;
  NtsHdr = (PIMAGE_NT_HEADERS)(((ULONG_PTR)ModPtr) + DosHdr->e_lfanew);
  ExpHdr = (PIMAGE_EXPORT_DIRECTORY)
  (((ULONG_PTR)ModPtr) + NtsHdr->OptionalHeader.DataDirectory[0].VirtualAddress);

  StrOff = (PDWORD)(((ULONG_PTR)ModPtr) + ExpHdr->AddressOfNames);
  FunOff = (PDWORD)(((ULONG_PTR)ModPtr) + ExpHdr->AddressOfFunctions);
  OrdOff = (PUSHORT)(((ULONG_PTR)ModPtr) + ExpHdr->AddressOfNameOrdinals);
  for ( int i = 0 ; i < ExpHdr->NumberOfNames ; i++ )
  {
    /*
     * We parse each individual export name until we 
     * reach the one we requested.
     *
     * if we find the requested module, we return it
     * back to be parsed by the requestee.
    */
    StrPln = (PCHAR)(((ULONG_PTR)ModPtr) + StrOff[i]);
    StrHsh = HashStringDjb2(StrPln, 0);
    if ( StrHsh == FunHsh )
      return (PVOID)(((ULONG_PTR)ModPtr) + FunOff[OrdOff[i]]);
  };
  return NULL;
};
