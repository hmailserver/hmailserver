// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// The part of the precompiled header which carries no Windows dependency.
//
// stdafx.h includes this file, so the server build is unaffected by its
// existence - it sees the same declarations, in the same order, as before.
// The reason it is a file of its own is that a compiler other than MSVC can
// include it without dragging in WinSock2.h, Windows.h, ATL and the ADO type
// library, which is what the rest of stdafx.h does. That makes it possible to
// build the parts of the code which are pure logic - string handling, record
// parsing, protocol grammar - on another toolchain, and so to run them under
// the address and undefined-behaviour sanitizers, and to fuzz them.
//
// The rule for this file: nothing in here, or in anything it includes, may
// depend on the Windows SDK. Declarations that do belong in stdafx.h instead.

#pragma once

// MSVC extensions and Windows SDK types used by the headers below. Under MSVC
// these come from the compiler and from the Windows headers, which stdafx.h has
// already included by the time it reaches this file, so the block is skipped.
#if !defined(_MSC_VER)

   #include <cstdarg>
   #include <cstdio>
   #include <cwchar>

   // StringParser declares overloads on __int64.
   typedef long long __int64;

   // CStdStr::AllocSysString hands a string to COM. Nothing outside the COM
   // layer calls it, so a declaration is enough to compile the header; a
   // definition is deliberately not provided, and a caller would fail to link.
   typedef wchar_t *BSTR;
   BSTR SysAllocString(const wchar_t *string);

   inline int vsprintf_s(char *buffer, size_t size, const char *format, va_list arguments)
   {
      return vsnprintf(buffer, size, format, arguments);
   }

   inline int _vstprintf(wchar_t *buffer, size_t size, const wchar_t *format, va_list arguments)
   {
      return vswprintf(buffer, size, format, arguments);
   }

   inline int _vsctprintf(const wchar_t *format, va_list arguments)
   {
      return vswprintf(0, 0, format, arguments);
   }

#endif // !defined(_MSC_VER)

//
// STL INCLUDES
//
// Also included by stdafx.h, ahead of this file and in its original position,
// so that the order the server build sees is unchanged. Repeated here because
// the headers below need them when this file is included on its own.
//
#include <map>
#include <vector>
#include <set>
#include <list>
#include <queue>
#include <functional>
#include <memory>
#include <string>

//
// PORTABLE COMMON FILES
//
#include "Util/StdString.h"
#include "Util/Parsing/StringParser.h"
