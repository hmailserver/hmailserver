// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\fetchaccountuids.h"
#include "../Persistence/PersistentFetchAccount.h"
#include "../Util/Time.h"

#include "FetchAccountUID.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   FetchAccountUIDs::FetchAccountUIDs(void) :
      faid_(0)
   {
   }

   FetchAccountUIDs::~FetchAccountUIDs(void)
   {
   }
   
   void 
   FetchAccountUIDs::Refresh(__int64 iFAID)
   {
      faid_ = iFAID;

      SQLCommand command("select * from hm_fetchaccounts_uids where uidfaid = @UIDFAID");
      command.AddParameter("@UIDFAID", faid_);

      std::shared_ptr<DALRecordset> pUIDRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pUIDRS)
         return;

      while (!pUIDRS->IsEOF())
      {
         int iUIDID = pUIDRS->GetLongValue("uidid");
         String sUIDValue = pUIDRS->GetStringValue("uidvalue");
         String sUIDTime = pUIDRS->GetStringValue("uidtime");

         std::shared_ptr<FetchAccountUID> pUID = std::shared_ptr<FetchAccountUID>(new FetchAccountUID(iUIDID, faid_, sUIDValue, sUIDTime));

         vecObjects.push_back(pUID);

         pUIDRS->MoveNext();     
      }

   }

   bool
   FetchAccountUIDs::PreSaveObject(std::shared_ptr<FetchAccountUID> pUID, XNode *node)
   {
      pUID->SetAccountID(faid_);
      return true;
   }
}