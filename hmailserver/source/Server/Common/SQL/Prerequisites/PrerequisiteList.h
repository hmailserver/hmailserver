// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


namespace HM
{
   class IPrerequisite;

   class PrerequisiteList
   {
   public:
      PrerequisiteList(void);
      ~PrerequisiteList(void);

      bool Ensure(std::shared_ptr<DALConnection> connection, int scriptDatabaseVersion, String &sErrorMessage);

   private:

      void Initialize_();

      std::vector<std::pair<int, std::shared_ptr<IPrerequisite > > > prerequisites_;
      
   };
}