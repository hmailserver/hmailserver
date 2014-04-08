// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SingletonCreator
   {
   public:
      SingletonCreator(void);
      ~SingletonCreator(void);

      // Creates an instance of all hMailServer
      // singletons. Since all of the hMailServer
      // singletons are used almost all the time,
      // this is no waste of memory.

      static void Create();
      static void Delete();

   private:

 
   };
}