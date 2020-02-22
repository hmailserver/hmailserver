// Copyright (c) 2014 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DisconnectedException : public std::exception
   {
   public:

      virtual const char* what() const
      {
         return "The client has been disconnected.";
      }


   private:

   };
}