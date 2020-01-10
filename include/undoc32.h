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
#ifndef _UNDOC32_H
#define _UNDOC32_H

typedef struct _KIDTENTRY
{
  /* 0x0000 */ unsigned short Offset;
  /* 0x0002 */ unsigned short Selector;
  /* 0x0004 */ unsigned short Access;
  /* 0x0006 */ unsigned short ExtendedOffset;
} KIDTENTRY, *PKIDTENTRY; /* size: 0x0008 */

typedef struct _KPCR
{
  union
  {
    /* 0x0000 */ struct _NT_TIB NtTib;
    struct
    {
      /* 0x0000 */ void* Used_ExceptionList;
      /* 0x0004 */ void* Used_StackBase;
      /* 0x0008 */ unsigned long MxCsr;
      /* 0x000c */ void* TssCopy;
      /* 0x0010 */ unsigned long ContextSwitches;
      /* 0x0014 */ unsigned long SetMemberCopy;
      /* 0x0018 */ void* Used_Self;
    }; /* size: 0x001c */
  }; /* size: 0x001c */
  /* 0x001c */ struct _KPCR* SelfPcr;
  /* 0x0020 */ void* Prcb;
  /* 0x0024 */ unsigned char Irql;
  /* 0x0028 */ unsigned long IRR;
  /* 0x002c */ unsigned long IrrActive;
  /* 0x0030 */ unsigned long IDR;
  /* 0x0034 */ void* KdVersionBlock;
  /* 0x0038 */ struct _KIDTENTRY* IDT;
  /* 0x003c */ void* GDT;
  /* 0x0040 */ void* TSS;
  /* 0x0044 */ unsigned short MajorVersion;
  /* 0x0046 */ unsigned short MinorVersion;
  /* 0x0048 */ unsigned long SetMember;
  /* 0x004c */ unsigned long StallScaleFactor;
  /* 0x0050 */ unsigned char SpareUnused;
  /* 0x0051 */ unsigned char Number;
  /* 0x0052 */ unsigned char Spare0;
  /* 0x0053 */ unsigned char SecondLevelCacheAssociativity;
  /* 0x0054 */ unsigned long VdmAlert;
  /* 0x0058 */ unsigned long KernelReserved[14];
  /* 0x0090 */ unsigned long SecondLevelCacheSize;
  /* 0x0094 */ unsigned long HalReserved[16];
  /* 0x00d4 */ unsigned long InterruptMode;
  /* 0x00d8 */ unsigned char Spare1;
  /* 0x00dc */ unsigned long KernelReserved2[17];
} KPCR, *PKPCR; /* size: 0x6020 */

typedef struct _KTRAP_FRAME
{
  /* 0x0000 */ unsigned long DbgEbp;
  /* 0x0004 */ unsigned long DbgEip;
  /* 0x0008 */ unsigned long DbgArgMark;
  /* 0x000c */ unsigned short TempSegCs;
  /* 0x000e */ unsigned char Logging;
  /* 0x000f */ unsigned char FrameType;
  /* 0x0010 */ unsigned long TempEsp;
  /* 0x0014 */ unsigned long Dr0;
  /* 0x0018 */ unsigned long Dr1;
  /* 0x001c */ unsigned long Dr2;
  /* 0x0020 */ unsigned long Dr3;
  /* 0x0024 */ unsigned long Dr6;
  /* 0x0028 */ unsigned long Dr7;
  /* 0x002c */ unsigned long SegGs;
  /* 0x0030 */ unsigned long SegEs;
  /* 0x0034 */ unsigned long SegDs;
  /* 0x0038 */ unsigned long Edx;
  /* 0x003c */ unsigned long Ecx;
  /* 0x0040 */ unsigned long Eax;
  /* 0x0044 */ unsigned char PreviousPreviousMode;
  /* 0x0045 */ unsigned char EntropyQueueDpc;
  union
  {
    /* 0x0046 */ unsigned char NmiMsrIbrs;
    /* 0x0046 */ unsigned char Reserved1;
  }; /* size: 0x0001 */
  /* 0x0047 */ unsigned char PreviousIrql;
  /* 0x0048 */ unsigned long MxCsr;
  /* 0x004c */ struct _EXCEPTION_REGISTRATION_RECORD* ExceptionList;
  /* 0x0050 */ unsigned long SegFs;
  /* 0x0054 */ unsigned long Edi;
  /* 0x0058 */ unsigned long Esi;
  /* 0x005c */ unsigned long Ebx;
  /* 0x0060 */ unsigned long Ebp;
  /* 0x0064 */ unsigned long ErrCode;
  /* 0x0068 */ unsigned long Eip;
  /* 0x006c */ unsigned long SegCs;
  /* 0x0070 */ unsigned long EFlags;
  /* 0x0074 */ unsigned long HardwareEsp;
  /* 0x0078 */ unsigned long HardwareSegSs;
  /* 0x007c */ unsigned long V86Es;
  /* 0x0080 */ unsigned long V86Ds;
  /* 0x0084 */ unsigned long V86Fs;
  /* 0x0088 */ unsigned long V86Gs;
} KTRAP_FRAME, *PKTRAP_FRAME; /* size: 0x008c */

#endif
