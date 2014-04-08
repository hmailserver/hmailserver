// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class CriticalSectionScope;

   class CriticalSection
   {
   public:
      CriticalSection(void);
      ~CriticalSection(void);

   private:

      bool Enter() const;
      void Leave() const;

      mutable CRITICAL_SECTION m_CriticalSection; 
      bool _initialized;

      friend class CriticalSectionScope;
   };


   class CriticalSectionScope
   {
   public:
      CriticalSectionScope(const CriticalSection &sec);
      ~CriticalSectionScope(void);
   private:
      const CriticalSection &m_Section;
   };
}