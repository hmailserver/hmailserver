/* Copyright (c) Martin Knafve / The hMailServer Community Developers (HCD) hMailServer.com */

#include <stdafx.h>
#include "DiagnosticResult.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DiagnosticResult::DiagnosticResult(String name, String description, bool success, String details) :
      name_(name), description_(description), success_(success), details_(details)
   {
      
   }

   DiagnosticResult::DiagnosticResult()
   {

   }
   
      
}
