// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class POP3Configuration
   {
   public:

      POP3Configuration();
	   virtual ~POP3Configuration();

      long GetMaxPOP3Connections() const;
      void SetMaxPOP3Connections(int newVal);

      String GetWelcomeMessage() const;
      void SetWelcomeMessage(const String &sMessage);

   private:
      std::shared_ptr<PropertySet> GetSettings_() const;

   };

}
