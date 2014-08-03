// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include <Winsvc.h>

namespace HM
{
   class ServiceManager  
   {
   public:
	   ServiceManager();
	   virtual ~ServiceManager();

      bool RegisterService(const String &ServiceName, const String &ServiceCaption);
      bool UnregisterService(const String &ServiceName);
      void MakeDependentOn(const String &ServiceName);

      bool StartServiceOnLocalComputer(const String &ServiceName);
      bool StopServiceOnLocalComputer(const String &ServiceName);

      SERVICE_STATUS GetServiceStatus(const String &ServiceName);

      bool UserControlService(const String &ServiceName, DWORD OpCode);
      bool DoesServiceExist(const String &ServiceName);

   private:

      bool ReconfigureService_(SC_HANDLE hSCMManager, const String &ServiceName);
   };
}
