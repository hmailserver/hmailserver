// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class BlockedAttachment : public BusinessObject<BlockedAttachment>
   {
   public:
      BlockedAttachment(void);
      ~BlockedAttachment(void);

      String GetName() const {return wildcard_; }

      String GetWildcard() const  {return wildcard_; }
      void SetWildcard(const String &sNewVal) {wildcard_ = sNewVal;}

      String GetDescription() const  {return description_; }
      void SetDescription(const String &sNewVal) {description_ = sNewVal;}


      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true;};

   private:

      String wildcard_;
      String description_;
      
   };
}