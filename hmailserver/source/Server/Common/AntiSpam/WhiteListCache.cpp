// Copyright (c) 2013  Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "WhiteListCache.h"
#include "../../Common/BO/WhiteListAddresses.h"

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

namespace HM
{
   boost::shared_mutex _whitelistAccessMutex;

   std::vector<shared_ptr<WhiteListAddress>> WhiteListCache::_whitelistItems;
   bool WhiteListCache::_needRefresh = true;

   WhiteListCache::WhiteListCache(void)
   {
      
   }

   void
   WhiteListCache::SetNeedRefresh()
   {
      _needRefresh = true;
   }


   bool 
   WhiteListCache::IsWhitelisted(const String &fromAddress, const IPAddress &address)
   {
      // Create a lock for shared operations
      boost::upgrade_lock< boost::shared_mutex > lock(_whitelistAccessMutex);

      if (_needRefresh)
      {
         // We need exclusive access to be able to upade the cache
         boost::upgrade_to_unique_lock< boost::shared_mutex > uniqueLock(lock);

         Refresh();
      }

      vector<shared_ptr<WhiteListAddress> >::iterator iter = _whitelistItems.begin();
      vector<shared_ptr<WhiteListAddress> >::iterator iterEnd = _whitelistItems.end();

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<WhiteListAddress> pWhiteAddress = (*iter);

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

      vector<shared_ptr<WhiteListAddress> > vec = addresses.GetConstVector();

      vector<shared_ptr<WhiteListAddress> >::iterator iter = vec.begin();
      vector<shared_ptr<WhiteListAddress> >::iterator iterEnd = vec.end();

      std::vector<shared_ptr<WhiteListAddress>> items;
      for (; iter != iterEnd; iter++)
      {
         shared_ptr<WhiteListAddress> whiteAddress = (*iter);

         items.push_back(whiteAddress);
      }

      _whitelistItems = items;
   }
}