// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DatabaseSettings;

   class DALConnectionFactory  
   {
   public:
	   DALConnectionFactory();
	   virtual ~DALConnectionFactory();


      static std::shared_ptr<DALConnection> CreateConnection(std::shared_ptr<DatabaseSettings> pSettings);

   };
}
