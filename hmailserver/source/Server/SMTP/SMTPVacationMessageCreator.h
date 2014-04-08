// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


namespace HM
{
   class Message;

   class SMTPVacationMessageCreator : public Singleton<SMTPVacationMessageCreator> 
   {
   public:
	   
      SMTPVacationMessageCreator();
	   virtual ~SMTPVacationMessageCreator();

      void CreateVacationMessage(shared_ptr<const Account> recipientAccount,
                                  const String &sToAddress, 
                                  const String &sVacationSubject, 
                                  const String &sVacationMessage,
                                  const shared_ptr<Message> pOriginalMessage);

      

      void VacationMessageTurnedOff(const String &sUserAddress);

   private:
      
      bool _CanSendVacationMessage(const String &sFrom, const String &sTo);

      multimap<String, String> mapVacationMessageRecipients;
      CriticalSection m_oCriticalSection;
   };
}
