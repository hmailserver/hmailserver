// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{


   class HTTPClient  
   {
   public:
	   HTTPClient();
	   virtual ~HTTPClient();
      
      bool ExecuteScript(const String &sServer, const String &sPage, AnsiString &output) const;

   private:


   };

}