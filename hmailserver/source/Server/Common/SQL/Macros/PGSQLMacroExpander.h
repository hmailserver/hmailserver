// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMacroExpander.h"

namespace HM
{
   class Macro;

   class PGSQLMacroExpander : public IMacroExpander
   {
   public:

      bool ProcessMacro(std::shared_ptr<DALConnection> connection, const Macro &macro, String &sErrorMessage);

   private:
   };
}
