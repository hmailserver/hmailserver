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

   protected:

      /*
         Message sequence numbers are only meaningful as long as the client and the
         server agree on the content of the selected folder. If another client has
         expunged a message, and we have not yet been able to send an EXPUNGE response
         to this client (which we're not allowed to do while responding to FETCH, STORE,
         SEARCH or SORT), the client and the server disagree.

         Rather than operating on another message than the client intended, we fail the
         command and let the client resynchronize. This is the behavior described in
         RFC 2180, 4.1.2.
      */
      static bool HasUnsentExpunge(std::shared_ptr<HM::IMAPConnection> pConnection);
      static bool IsMessageSequenceNumberStale(std::shared_ptr<HM::IMAPConnection> pConnection, int messageIndex);
      static IMAPResult GetExpungeIssuedResult();

   };


}