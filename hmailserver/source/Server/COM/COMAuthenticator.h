// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class COMAuthenticator
   {
   public:

      COMAuthenticator()
      {
         // Create a dummy object so that it always exists.
         m_pAuthentication = shared_ptr<HM::COMAuthentication>(new COMAuthentication);

         m_pAuthentication->AttempAnonymousAuthentication();
      }
      
      void SetAuthentication(shared_ptr<HM::COMAuthentication> pAuthentication)
      {
         m_pAuthentication = pAuthentication; 
      }

   protected:

      bool GetIsServerAdmin()
      {
         if (!m_pAuthentication)
            return false;

         if (!m_pAuthentication->GetIsServerAdmin())
            return false;

         return true;
      }

      int GetAccessDenied()
      {
         if (!m_pAuthentication)
            return -1;

         return m_pAuthentication->GetAccessDenied();
      }

      shared_ptr<COMAuthentication> m_pAuthentication;

   private:

   };

}