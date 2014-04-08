// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


namespace HM
{
   class DistributionList;

   class PersistentDistributionList
   {
   public:
      PersistentDistributionList(void);
      ~PersistentDistributionList(void);

      static bool DeleteObject(shared_ptr<DistributionList> pDistList);
      
      static bool SaveObject(shared_ptr<DistributionList> pDistList);
      static bool SaveObject(shared_ptr<DistributionList> pDistList, String &sErrorMessage);

      static bool ReadObject(shared_ptr<DistributionList> pDistList, const String &sAddress);
      static bool ReadObject(shared_ptr<DistributionList> pDistList, __int64 ObjectID);
      static bool ReadObject(shared_ptr<DistributionList> pDistList, const SQLCommand &command);
      static bool ReadObject(shared_ptr<DistributionList> pDistList, shared_ptr<DALRecordset> pRS);

      static bool DeleteMembers(shared_ptr<DistributionList> pDistList);


   private:

      
   };
}
