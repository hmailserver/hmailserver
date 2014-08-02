/*
	Copyright (c) 2003, Jeff Lee
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
#include "StdAfx.h"
#include "MimeChar.h"

#ifdef _DEBUG
   #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
   #define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   const unsigned char CMimeChar::char_tbl_[128] =
   {
	   CONTROL,						// 00 (NUL)
	   CONTROL,						// 01 (SOH)
	   CONTROL,						// 02 (STX)
	   CONTROL,						// 03 (ETX)
	   CONTROL,						// 04 (EOT)
	   CONTROL,						// 05 (ENQ)
	   CONTROL,						// 06 (ACK)
	   CONTROL,						// 07 (BEL)
	   CONTROL,						// 08 (BS)
	   SPACE | CONTROL,				// 09 (HT)
	   SPACE | CONTROL,				// 0A (LF)
	   SPACE | CONTROL,				// 0B (VT)
	   SPACE | CONTROL,				// 0C (FF)
	   SPACE | CONTROL,				// 0D (CR)
	   CONTROL,						// 0E (SI)
	   CONTROL,						// 0F (SO)
	   CONTROL,						// 10 (DLE)
	   CONTROL,						// 11 (DC1)
	   CONTROL,						// 12 (DC2)
	   CONTROL,						// 13 (DC3)
	   CONTROL,						// 14 (DC4)
	   CONTROL,						// 15 (NAK)
	   CONTROL,						// 16 (SYN)
	   CONTROL,						// 17 (ETB)
	   CONTROL,						// 18 (CAN)
	   CONTROL,						// 19 (EM)
	   CONTROL,						// 1A (SUB)
	   CONTROL,						// 1B (ESC)
	   CONTROL,						// 1C (FS)
	   CONTROL,						// 1D (GS)
	   CONTROL,						// 1E (RS)
	   CONTROL,						// 1F (US)
	   SPACE,							// 20 SPACE
	   PUNCT,							// 21 !
	   PUNCT | SPECIAL,				// 22 "
	   PUNCT,							// 23 #
	   PUNCT,							// 24 $
	   PUNCT,							// 25 %
	   PUNCT,							// 26 &
	   PUNCT,							// 27 '
	   PUNCT | SPECIAL,				// 28 (
	   PUNCT | SPECIAL,				// 29 )
	   PUNCT,							// 2A *
	   PUNCT,							// 2B +
	   PUNCT | SPECIAL,				// 2C ,
	   PUNCT,							// 2D -
	   PUNCT | SPECIAL,				// 2E .
	   PUNCT,							// 2F /
	   PRINT | HEXDIGIT,				// 30 0
	   PRINT | HEXDIGIT,				// 31 1
	   PRINT | HEXDIGIT,				// 32 2
	   PRINT | HEXDIGIT,				// 33 3
	   PRINT | HEXDIGIT,				// 34 4
	   PRINT | HEXDIGIT,				// 35 5
	   PRINT | HEXDIGIT,				// 36 6
	   PRINT | HEXDIGIT,				// 37 7
	   PRINT | HEXDIGIT,				// 38 8
	   PRINT | HEXDIGIT,				// 39 9
	   PUNCT | SPECIAL,				// 3A :
	   PUNCT | SPECIAL,				// 3B ;
	   PUNCT | SPECIAL,				// 3C <
	   PUNCT | SPECIAL,				// 3D =
	   PUNCT | SPECIAL,				// 3E >
	   PUNCT,							// 3F ?
	   PUNCT | SPECIAL,				// 40 @
	   PRINT | HEXDIGIT,				// 41 A
	   PRINT | HEXDIGIT,				// 42 B
	   PRINT | HEXDIGIT,				// 43 C
	   PRINT | HEXDIGIT,				// 44 D
	   PRINT | HEXDIGIT,				// 45 E
	   PRINT | HEXDIGIT,				// 46 F
	   PRINT,							// 47 G
	   PRINT,							// 48 H
	   PRINT,							// 49 I
	   PRINT,							// 4A J
	   PRINT,							// 4B K
	   PRINT,							// 4C L
	   PRINT,							// 4D M
	   PRINT,							// 4E N
	   PRINT,							// 4F O
	   PRINT,							// 50 P
	   PRINT,							// 51 Q
	   PRINT,							// 52 R
	   PRINT,							// 53 S
	   PRINT,							// 54 T
	   PRINT,							// 55 U
	   PRINT,							// 56 V
	   PRINT,							// 57 W
	   PRINT,							// 58 X
	   PRINT,							// 59 Y
	   PRINT,							// 5A Z
	   PUNCT | SPECIAL,				// 5B [
	   PUNCT | SPECIAL,				// 5C '\'
	   PUNCT | SPECIAL,				// 5D ]
	   PUNCT,							// 5E ^
	   PUNCT,							// 5F 
	   PUNCT,							// 60 `
	   PRINT,							// 61 a
	   PRINT,							// 62 b
	   PRINT,							// 63 c
	   PRINT,							// 64 d
	   PRINT,							// 65 e
	   PRINT,							// 66 f
	   PRINT,							// 67 g
	   PRINT,							// 68 h
	   PRINT,							// 69 i
	   PRINT,							// 6A j
	   PRINT,							// 6B k
	   PRINT,							// 6C l
	   PRINT,							// 6D m
	   PRINT,							// 6E n
	   PRINT,							// 6F o
	   PRINT,							// 70 p
	   PRINT,							// 71 q
	   PRINT,							// 72 r
	   PRINT,							// 73 s
	   PRINT,							// 74 t
	   PRINT,							// 75 u
	   PRINT,							// 76 v
	   PRINT,							// 77 w
	   PRINT,							// 78 x
	   PRINT,							// 79 y
	   PRINT,							// 7A z
	   PUNCT,							// 7B {
	   PUNCT,							// 7C |
	   PUNCT,							// 7D }
	   PUNCT,							// 7E ~
	   CONTROL,						// 7F (DEL)
   };
}
