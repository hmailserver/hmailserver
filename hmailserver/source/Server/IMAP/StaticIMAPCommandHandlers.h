// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "IMAPConnection.h"
#include "IMAPCommand.h"


namespace HM
{
   class IMAPCommand;

   class StaticIMAPCommandHandlers : public Singleton<StaticIMAPCommandHandlers>
   {

   public:
	   StaticIMAPCommandHandlers();
      static std::map<IMAPConnection::eIMAPCommandType, std::shared_ptr<IMAPCommand> > &GetStaticHandlers() {return mapCommandHandlers; }

   private:
      

      static std::map<IMAPConnection::eIMAPCommandType, std::shared_ptr<IMAPCommand> > mapCommandHandlers;
   };

   class IMAPCommandUNKNOWN : public IMAPCommand
   {
      virtual IMAPResult ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);

   };

   class IMAPCommandNOOP : public IMAPCommand
   {
      virtual IMAPResult ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument);
   };



}