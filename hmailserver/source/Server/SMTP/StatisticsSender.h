// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


namespace HM
{
   class StatisticsSender
   {
   public:
	   StatisticsSender();
	   virtual ~StatisticsSender();

      bool SendStatistics(int iNoOfMessages) const;
   };
}

