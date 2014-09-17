// Copyright (c) 2013  Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "WhiteListCache.h"
#include "../../Common/BO/WhiteListAddresses.h"

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

namespace HM
{
   boost::shared_mutex whitelistAccessMutex_;

   std::vector<std::shared_ptr<WhiteListAddress>> WhiteListCache::whitelistItems_;
   bool WhiteListCache::needRefresh_ = true;

   WhiteListCache::WhiteListCache(void)
   {
      
   }

   void
   WhiteListCache::SetNeedRefresh()
   {
      needRefresh_ = true;
   }


   bool 
   WhiteListCache::IsWhitelisted(const String &fromAddress, const IPAddress &address)
   {
      // Create a lock for shared operations
      boost::upgrade_lock< boost::shared_mutex > lock(whitelistAccessMutex_);

      if (needRefresh_)
      {
         // We need exclusive access to be able to upade the cache
         boost::upgrade_to_unique_lock< boost::shared_mutex > uniqueLock(lock);

         Refresh();
      }

      std::vector<std::shared_ptr<WhiteListAddress> >::iterator iter = whitelistItems_.begin();
      std::vector<std::shared_ptr<WhiteListAddress> >::iterator iterEnd = whitelistItems_.end();

      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<WhiteListAddress> pWhiteAddress = (*iter);

         IPAddress iLowerIP = pWhiteAddress->GetLowerIPAddress();
         IPAddress iUpperIP = pWhiteAddress->GetUpperIPAddress();

         if (address.WithinRange(iLowerIP, iUpperIP))
         {
            String sWhiteEmailAddr = pWhiteAddress->GetEmailAddress();

            if (sWhiteEmailAddr.IsEmpty() || sWhiteEmailAddr == _T("*"))
            {
               // White listed
               return true;
            }

            // Check if the senders email address matches
            if (StringParser::WildcardMatchNoCase(sWhiteEmailAddr, fromAddress))
            {
               // White listed
               return true;
            }
         }
      }

      return false;

   }


   void
   WhiteListCache::Refresh()
   {

      WhiteListAddresses addresses;
      addresses.Refresh();

      std::vector<std::shared_ptr<WhiteListAddress> > vec = addresses.GetConstVector();

      std::vector<std::shared_ptr<WhiteListAddress> >::iterator iter = vec.begin();
      std::vector<std::shared_ptr<WhiteListAddress> >::iterator iterEnd = vec.end();

      std::vector<std::shared_ptr<WhiteListAddress>> items;
      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<WhiteListAddress> whiteAddress = (*iter);

         items.push_back(whiteAddress);
      }

      whitelistItems_ = items;
   }
}