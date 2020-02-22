// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Macro;

   class IMacroExpander  
   {
   public:

      virtual bool ProcessMacro(std::shared_ptr<DALConnection> connection, const Macro &macro, String &sErrorMessage) = 0;

   private:

   };
}
