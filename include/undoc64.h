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
#ifndef _UNDOC64_H
#define _UNDOC64_H

typedef union _KIDTENTRY64
{
  union
  {
    struct
    {
      /* 0x0000 */ unsigned short OffsetLow;
      /* 0x0002 */ unsigned short Selector;
      struct /* bitfield */
      {
        /* 0x0004 */ unsigned short IstIndex : 3; /* bit position: 0 */
        /* 0x0004 */ unsigned short Reserved0 : 5; /* bit position: 3 */
        /* 0x0004 */ unsigned short Type : 5; /* bit position: 8 */
        /* 0x0004 */ unsigned short Dpl : 2; /* bit position: 13 */
        /* 0x0004 */ unsigned short Present : 1; /* bit position: 15 */
      }; /* bitfield */
      /* 0x0006 */ unsigned short OffsetMiddle;
      /* 0x0008 */ unsigned long OffsetHigh;
      /* 0x000c */ unsigned long Reserved1;
    }; /* size: 0x0010 */
    /* 0x0000 */ unsigned __int64 Alignment;
  }; /* size: 0x0010 */
} KIDTENTRY64, *PKIDTENTRY64; /* size: 0x0010 */

typedef struct _KPCR
{
  union
  {
    /* 0x0000 */ struct _NT_TIB NtTib;
    struct
    {
      /* 0x0000 */ void* GdtBase;
      /* 0x0008 */ void* TssBase;
      /* 0x0010 */ unsigned __int64 UserRsp;
      /* 0x0018 */ struct _KPCR* SelfPcr;
      /* 0x0020 */ void* CurrentPrcb;
      /* 0x0028 */ void* LockArray;
      /* 0x0030 */ void* Used_Self;
    }; /* size: 0x0038 */
  }; /* size: 0x0038 */
  /* 0x0038 */ union _KIDTENTRY64* IdtBase;
  /* 0x0040 */ unsigned __int64 Unused[2];
  /* 0x0050 */ unsigned char Irql;
  /* 0x0051 */ unsigned char SecondLevelCacheAssociativity;
  /* 0x0052 */ unsigned char ObsoleteNumber;
  /* 0x0053 */ unsigned char Fill0;
  /* 0x0054 */ unsigned long Unused0[3];
  /* 0x0060 */ unsigned short MajorVersion;
  /* 0x0062 */ unsigned short MinorVersion;
  /* 0x0064 */ unsigned long StallScaleFactor;
  /* 0x0068 */ void* Unused1[3];
  /* 0x0080 */ unsigned long KernelReserved[15];
  /* 0x00bc */ unsigned long SecondLevelCacheSize;
  /* 0x00c0 */ unsigned long HalReserved[16];
  /* 0x0100 */ unsigned long Unused2;
  /* 0x0108 */ void* KdVersionBlock;
  /* 0x0110 */ void* Unused3;
  /* 0x0118 */ unsigned long PcrAlign1[24];
} KPCR, *PKPCR; /* size: 0x0178 */

typedef struct _KTRAP_FRAME
{
  /* 0x0000 */ unsigned __int64 P1Home;
  /* 0x0008 */ unsigned __int64 P2Home;
  /* 0x0010 */ unsigned __int64 P3Home;
  /* 0x0018 */ unsigned __int64 P4Home;
  /* 0x0020 */ unsigned __int64 P5;
  union
  {
    /* 0x0028 */ char PreviousMode;
    /* 0x0028 */ unsigned char InterruptRetpolineState;
  }; /* size: 0x0001 */
  /* 0x0029 */ unsigned char PreviousIrql;
  union
  {
    /* 0x002a */ unsigned char FaultIndicator;
    /* 0x002a */ unsigned char NmiMsrIbrs;
  }; /* size: 0x0001 */
  /* 0x002b */ unsigned char ExceptionActive;
  /* 0x002c */ unsigned long MxCsr;
  /* 0x0030 */ unsigned __int64 Rax;
  /* 0x0038 */ unsigned __int64 Rcx;
  /* 0x0040 */ unsigned __int64 Rdx;
  /* 0x0048 */ unsigned __int64 R8;
  /* 0x0050 */ unsigned __int64 R9;
  /* 0x0058 */ unsigned __int64 R10;
  /* 0x0060 */ unsigned __int64 R11;
  union
  {
    /* 0x0068 */ unsigned __int64 GsBase;
    /* 0x0068 */ unsigned __int64 GsSwap;
  }; /* size: 0x0008 */
  /* 0x0070 */ struct _M128A Xmm0;
  /* 0x0080 */ struct _M128A Xmm1;
  /* 0x0090 */ struct _M128A Xmm2;
  /* 0x00a0 */ struct _M128A Xmm3;
  /* 0x00b0 */ struct _M128A Xmm4;
  /* 0x00c0 */ struct _M128A Xmm5;
  union
  {
    /* 0x00d0 */ unsigned __int64 FaultAddress;
    /* 0x00d0 */ unsigned __int64 ContextRecord;
  }; /* size: 0x0008 */
  /* 0x00d8 */ unsigned __int64 Dr0;
  /* 0x00e0 */ unsigned __int64 Dr1;
  /* 0x00e8 */ unsigned __int64 Dr2;
  /* 0x00f0 */ unsigned __int64 Dr3;
  /* 0x00f8 */ unsigned __int64 Dr6;
  /* 0x0100 */ unsigned __int64 Dr7;
  /* 0x0108 */ unsigned __int64 DebugControl;
  /* 0x0110 */ unsigned __int64 LastBranchToRip;
  /* 0x0118 */ unsigned __int64 LastBranchFromRip;
  /* 0x0120 */ unsigned __int64 LastExceptionToRip;
  /* 0x0128 */ unsigned __int64 LastExceptionFromRip;
  /* 0x0130 */ unsigned short SegDs;
  /* 0x0132 */ unsigned short SegEs;
  /* 0x0134 */ unsigned short SegFs;
  /* 0x0136 */ unsigned short SegGs;
  /* 0x0138 */ unsigned __int64 TrapFrame;
  /* 0x0140 */ unsigned __int64 Rbx;
  /* 0x0148 */ unsigned __int64 Rdi;
  /* 0x0150 */ unsigned __int64 Rsi;
  /* 0x0158 */ unsigned __int64 Rbp;
  union
  {
    /* 0x0160 */ unsigned __int64 ErrorCode;
    /* 0x0160 */ unsigned __int64 ExceptionFrame;
  }; /* size: 0x0008 */
  /* 0x0168 */ unsigned __int64 Rip;
  /* 0x0170 */ unsigned short SegCs;
  /* 0x0172 */ unsigned char Fill0;
  /* 0x0173 */ unsigned char Logging;
  /* 0x0174 */ unsigned short Fill1[2];
  /* 0x0178 */ unsigned long EFlags;
  /* 0x017c */ unsigned long Fill2;
  /* 0x0180 */ unsigned __int64 Rsp;
  /* 0x0188 */ unsigned short SegSs;
  /* 0x018a */ unsigned short Fill3;
  /* 0x018c */ unsigned long Fill4;
} KTRAP_FRAME, *PKTRAP_FRAME; /* size: 0x0190 */

#endif
