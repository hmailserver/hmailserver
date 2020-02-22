// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Alias : public BusinessObject<Alias>
   {
   public:
	   Alias();
	   virtual ~Alias();

      __int64 GetDomainID() const { return domain_id_; }
      String GetName() const { return name_; }
      String GetValue() const { return value_; }
      bool GetIsActive() const { return active_; }

      void SetDomainID(__int64 newVal) { domain_id_ = newVal; }
      void SetName(const String & newVal) {  name_ = newVal; }
      void SetValue(const String & newVal) { value_ = newVal; }
      void SetIsActive(bool newVal) { active_ = newVal; }

      bool XMLStore(XNode *pParentNode, int iOptions);     
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true; }

      size_t GetEstimatedCachingSize();

   protected:

      __int64 domain_id_;
      String name_;
      String value_;
      bool active_;


   };
}
