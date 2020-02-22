// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ExceptionHandler.h"
#include "../Util/ExceptionLogger.h"

#include <boost/thread/thread.hpp>


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ExceptionHandler::ExceptionHandler(void)
   {

   }

   LONG WINAPI ExceptionFilterWithLogging(EXCEPTION_POINTERS* pExp, DWORD dwExpCode)
   {
      // if an error occurs when shutting down, we want to log it completely before
      // the shut down completes.
      boost::this_thread::disable_interruption shutdown_temporarily_disabled;

      LOG_DEBUG("Logging exception..");

      ExceptionLogger::Log(dwExpCode, pExp);

      LOG_DEBUG("Completed logging of exception...");

      return EXCEPTION_EXECUTE_HANDLER;
   }
   
   
   bool
   ExceptionHandler::Run(const String &descriptive_name, boost::function<void()>& func)
   {
      __try
      {
         RunWithStandardExceptions(descriptive_name, func);
         return true;
      }
      __except (ExceptionFilterWithLogging(GetExceptionInformation(), GetExceptionCode()))
      {
         // this has been logged in the exception filter.
         return false;
      }   
   }

   void
   ExceptionHandler::RunWithStandardExceptions(const String &descriptive_name, boost::function<void()>& func)
   {
      try
      {
         func();
      }
      catch (boost::thread_interrupted&)
      {
         // shutting down
      }
      catch (boost::system::system_error& error)
      {
         try
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "ExceptionHandler::Run", GetExceptionText(descriptive_name), error);
         }
         catch (...)
         {
            // Don't swallow the original exception.
         }

         throw;
      }
      catch (std::exception& error)
      {
         String sErrorMessage = 
            Formatter::Format("An error occured while executing '{0}'", descriptive_name);

         try
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "ExceptionHandler::Run", GetExceptionText(descriptive_name), error);
         }
         catch (...)
         {
            // Don't swallow the origial exception
         }
         
         throw;
      }
      catch (...)
      {
         String sErrorMessage = 
            Formatter::Format("An error occured while executing '{0}'", descriptive_name);

         try
         {
            ErrorManager::Instance()->ReportError(ErrorManager::High, 4208, "ExceptionHandler::Run", GetExceptionText(descriptive_name));
         }
         catch (...)
         {
            // Don't swallow the original exception
         }

         throw;
      }
   }



   String 
   ExceptionHandler::GetExceptionText(const String &descriptive_name)
   {
      String sErrorMessage = 
         Formatter::Format("An error occured while executing '{0}'", descriptive_name);

      return sErrorMessage;

   }

   
} 