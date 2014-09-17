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
      
      void Tag(const String & sNewVal) {tag_ = sNewVal;}
      String Tag() {return tag_;}

      void Command(const String & sNewVal) {command_ = sNewVal;}
      String Command() {return command_; }

      size_t LiteralCount() {return literals_.size();}
      String Literal(unsigned int Index);
      void AddLiteral(const String &sLiteral){literals_.push_back(sLiteral);}

      std::vector<String> Literals() {return literals_; }
      void Literals(const std::vector<String> sInput) { literals_ = sInput; }

   private:
      String tag_;
      String command_;
      std::vector<String> literals_;
   };

   class IMAPCommand
   {
   public:
      IMAPCommand();
      ~IMAPCommand();

      virtual IMAPResult ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument) = 0;
      
   };


}