// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "FetchAccountUIDList.h"
#include "../Common/BO/FetchAccountUID.h"
#include "../Common/Persistence/PersistentFetchAccount.h"
#include "../Common/Util/Time.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   FetchAccountUIDList::FetchAccountUIDList(void)
   {
   }

   FetchAccountUIDList::~FetchAccountUIDList(void)
   {
   }
   
   void 
   FetchAccountUIDList::Refresh(__int64 iFAID)
   {
      m_iFAID = iFAID;

      SQLCommand command("select * from hm_fetchaccounts_uids where uidfaid = @UIDFAID");
      command.AddParameter("@UIDFAID", m_iFAID);

      shared_ptr<DALRecordset> pUIDRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pUIDRS)
         return;

      while (!pUIDRS->IsEOF())
      {
         int iUIDID = pUIDRS->GetLongValue("uidid");
         String sUIDValue = pUIDRS->GetStringValue("uidvalue");
         String sUIDTime = pUIDRS->GetStringValue("uidtime");

         shared_ptr<FetchAccountUID> pUID = shared_ptr<FetchAccountUID>(new FetchAccountUID(iUIDID, m_iFAID, sUIDValue, sUIDTime));

         _fetchedUIDs.insert(std::make_pair(sUIDValue, pUID));

         pUIDRS->MoveNext();     
      }

   }

   void
   FetchAccountUIDList::AddUID(const String &sUIDValue)
   {
      // Check so that it does not already exist first.
      if (IsUIDInList(sUIDValue))
         return;

      // Save current time as create time
      String sCreateTime = Time::GetCurrentDateTime();

      // Add to the database.
      __int64 iUIDID = PersistentFetchAccountUID::AddUID(m_iFAID, sUIDValue);

      shared_ptr<FetchAccountUID> pUID = shared_ptr<FetchAccountUID>(new FetchAccountUID(iUIDID, m_iFAID, sUIDValue, sCreateTime));

      _fetchedUIDs.insert(std::make_pair(sUIDValue, pUID));
   }

   bool 
   FetchAccountUIDList::IsUIDInList(const String&sUID) const
   {
      std::map<String, shared_ptr<FetchAccountUID> >::const_iterator iter = _fetchedUIDs.find(sUID);
      if (iter == _fetchedUIDs.end())
         return false;
      
      return true;
   }

   void 
   FetchAccountUIDList::DeleteUID(const String &sUID)
   {
      std::map<String, shared_ptr<FetchAccountUID> >::iterator iter = _fetchedUIDs.find(sUID);
      if (iter == _fetchedUIDs.end())
         return;

      shared_ptr<FetchAccountUID> pUID = (*iter).second;

      // Delete from the database
      PersistentFetchAccountUID::DeleteUID(pUID->GetID());

      // Delete from vector in memory.
      _fetchedUIDs.erase(iter);
   }

   void 
   FetchAccountUIDList::DeleteUIDsNotInSet(set<String> &setUIDs)
   {
      std::map<String, shared_ptr<FetchAccountUID> >::iterator iterFA = _fetchedUIDs.begin();
      std::map<String, shared_ptr<FetchAccountUID> >::iterator iterEnd = _fetchedUIDs.end();
      while (iterFA != iterEnd)
      {
         shared_ptr<FetchAccountUID> pUID = (*iterFA).second;

         if (setUIDs.find(pUID->GetUID()) == setUIDs.end())
         {
            // No, the UID stored in the database is not 
            // in the vector over valid UID's.

            // Delete from the database
            PersistentFetchAccountUID::DeleteUID(pUID->GetID());

            // Delete from vector in memory.
            iterFA = _fetchedUIDs.erase(iterFA);
         }
         else
            iterFA++;
      }
   }


   shared_ptr<FetchAccountUID> 
   FetchAccountUIDList::GetUID(const String &sUID)
   {
      std::map<String, shared_ptr<FetchAccountUID> >::const_iterator iter = _fetchedUIDs.find(sUID);
      if (iter == _fetchedUIDs.end())
      {

         shared_ptr<FetchAccountUID> pEmpty;
         return pEmpty;
      }

      return (*iter).second;
   }

}