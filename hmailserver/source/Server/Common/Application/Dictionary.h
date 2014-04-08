// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Dictionary
   {
   public:
	   Dictionary();
      ~Dictionary();

      static String GetWindowsErrorDescription(int iErrorCode);
   
   };
}
