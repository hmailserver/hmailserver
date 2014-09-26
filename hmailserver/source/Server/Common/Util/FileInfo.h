// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2006-03-25
//

#pragma once

#include "VariantDateTime.h"

namespace HM
{
   

   class FileInfo
   {
   public:
	   FileInfo(const String &name, const DateTime &created);
      FileInfo();

      String GetName() {return name_;}
      DateTime GetCreateTime() {return created_;}

   private:
      
      String name_;
      DateTime created_;
   };

}
