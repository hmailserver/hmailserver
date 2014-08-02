// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ClassTester  
   {
   public:
	   ClassTester();
	   virtual ~ClassTester();

      void DoTests();

   private:

      void _LoadSettings();

      void _TestBackup();

      String mime_data_path_;
   };

}
