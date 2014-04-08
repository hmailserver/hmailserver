// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Cacheable
   {
   public:
      Cacheable(void);
      virtual ~Cacheable(void);

      int Seconds();

   private:

      int m_iCreationTime;
   };
}