// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class IMAPConnection;
   class Message;

   class IMAPCommandArgument
   {
   public:
      
      void Tag(const String & sNewVal) {m_sTag = sNewVal;}
      String Tag() {return m_sTag;}

      void Command(const String & sNewVal) {m_sCommand = sNewVal;}
      String Command() {return m_sCommand; }

      size_t LiteralCount() {return m_vecLiterals.size();}
      String Literal(unsigned int Index);
      void AddLiteral(const String &sLiteral){m_vecLiterals.push_back(sLiteral);}

      std::vector<String> Literals() {return m_vecLiterals; }
      void Literals(const std::vector<String> sInput) { m_vecLiterals = sInput; }

   private:
      String m_sTag;
      String m_sCommand;
      std::vector<String> m_vecLiterals;
   };

   class IMAPCommand
   {
   public:
      IMAPCommand();
      ~IMAPCommand();

      virtual IMAPResult ExecuteCommand(shared_ptr<HM::IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument) = 0;
      
   };


}