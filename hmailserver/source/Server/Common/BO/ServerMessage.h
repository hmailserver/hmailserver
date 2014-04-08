// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ServerMessage : public BusinessObject<ServerMessage>
   {
   public:
      ServerMessage(void);
      ~ServerMessage(void);

      String GetName() const  {return m_sName; }
      void SetName(const String &sNewVal) {m_sName = sNewVal;}

      String GetText() const  {return m_sText; }
      void SetText(const String &sNewVal) {m_sText = sNewVal;}


      bool XMLStore(XNode *pNode);
      bool XMLLoad(XNode *pNode);

   private:

      String m_sName;
      String m_sText;
      
   };
}