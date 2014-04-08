// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MessageRecipient;

   class MessageRecipients
   {
   public:
      MessageRecipients(void);
      ~MessageRecipients(void);

      void Clear();
      void Add(shared_ptr<MessageRecipient> pRecipient);
      std::vector<shared_ptr<MessageRecipient> > & GetVector() {return _recipients; }
      int GetCount() {return (int) _recipients.size(); }
      String GetCommaSeperatedRecipientList();

      void RemoveNonLocalAccounts();
      void RemoveExternal();

   private:

      std::vector<shared_ptr<MessageRecipient> > _recipients;

   };
}