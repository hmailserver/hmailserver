// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IMAPListLookup
   {
   public:

      IMAPListLookup();
      virtual ~IMAPListLookup();

      static bool IsItemInList(std::vector<String> vecItems, int item);

   private:

   };


}