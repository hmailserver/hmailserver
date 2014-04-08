// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Cache/Cacheable.h"

namespace HM
{
   class DALRecordset;
   class Property;
      
   class PropertySet : public Cacheable
   {
   public:
      PropertySet(void);
      ~PropertySet(void);

      void Refresh();


      long GetLong(const String &sPropertyName);
      bool GetBool(const String &sPropertyName);
      String GetString(const String &sPropertyName);

      void SetLong(const String &sPropertyName, long lValue);
      void SetBool(const String &sPropertyName, bool lValue);
      void SetString(const String &sPropertyName, const String &lValue);

      bool XMLStore(XNode *pBackupNode);
      bool XMLLoad(XNode *pBackupNode);

   private:

      void _OnPropertyChanged(shared_ptr<Property> pProperty);

      shared_ptr<Property> _GetProperty(const String &sPropertyName);

      bool _IsCryptedProperty(const String &sPropertyName);
      std::map<String, shared_ptr<Property> > m_mapItems;
   };
}
