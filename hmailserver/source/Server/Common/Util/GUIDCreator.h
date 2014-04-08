// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class GUIDCreator  
   {
   public:
	   GUIDCreator();
	   virtual ~GUIDCreator();

      static String GetGUID();
   };

}
