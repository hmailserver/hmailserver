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

      String GetName() const {return m_sWildcard; }

      String GetWildcard() const  {return m_sWildcard; }
      void SetWildcard(const String &sNewVal) {m_sWildcard = sNewVal;}

      String GetDescription() const  {return m_sDescription; }
      void SetDescription(const String &sNewVal) {m_sDescription = sNewVal;}


      bool XMLStore(XNode *pNode, int iOptions);
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems (XNode *pNode, int iOptions) {return true;};

   private:

      String m_sWildcard;
      String m_sDescription;
      
   };
}