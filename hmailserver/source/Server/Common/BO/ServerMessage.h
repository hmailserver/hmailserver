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

      String GetName() const  {return name_; }
      void SetName(const String &sNewVal) {name_ = sNewVal;}

      String GetText() const  {return text_; }
      void SetText(const String &sNewVal) {text_ = sNewVal;}


      bool XMLStore(XNode *pNode);
      bool XMLLoad(XNode *pNode);

   private:

      String name_;
      String text_;
      
   };
}