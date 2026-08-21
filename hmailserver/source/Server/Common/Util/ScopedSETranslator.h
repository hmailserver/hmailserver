// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include <eh.h>
#include <stdexcept>
#include <cstdio>

#include "ExceptionLogger.h"

namespace HM
{
   // Converts a Windows structured exception (access violation, etc.) into a
   // C++ exception on the current thread for the lifetime of the object, so it
   // can be caught by a normal catch (std::exception) handler. The previous
   // translator is restored on destruction.
   //
   // Requires the module to be compiled with /EHa (Async exception handling),
   // which hMailServer already uses.
   //
   // Note: _set_se_translator is per-thread, so an instance only affects the
   // thread that constructed it.
   class ScopedSETranslator
   {
   public:
      ScopedSETranslator() : previous_(_set_se_translator(Translate)) {}
      ~ScopedSETranslator() { _set_se_translator(previous_); }

      ScopedSETranslator(const ScopedSETranslator&) = delete;
      ScopedSETranslator& operator=(const ScopedSETranslator&) = delete;

   private:
      static void Translate(unsigned int code, EXCEPTION_POINTERS *ep)
      {
         // Write a minidump (via hMailServer.minidump.exe) while the faulting
         // stack is still intact, so the crash can be opened in a debugger
         // afterwards. Guarded so a failure here can't mask the original fault.
         if (ep)
         {
            try
            {
               ExceptionLogger::Log(code, ep);
            }
            catch (...)
            {
               // Don't let dump generation swallow the real exception.
            }
         }

         char msg[128];
         sprintf_s(msg, sizeof(msg), "Structured exception 0x%08X at address 0x%p",
            code, ep ? ep->ExceptionRecord->ExceptionAddress : nullptr);
         throw std::runtime_error(msg);
      }

      _se_translator_function previous_;
   };
}
