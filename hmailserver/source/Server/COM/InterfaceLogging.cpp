

#include "stdafx.h"
#include "COMError.h"

#include "InterfaceLogging.h"

InterfaceLogging::InterfaceLogging() :
   config_(nullptr),
   ini_file_settings_(nullptr)
{

}

bool 
InterfaceLogging::LoadSettings()
{
   if (!GetIsServerAdmin())
      return false;

   config_ = HM::Configuration::Instance();
   ini_file_settings_ = HM::IniFileSettings::Instance();

   return true;
}


STDMETHODIMP
InterfaceLogging::get_Enabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetUseLogging() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::put_Enabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetUseLogging(newVal == VARIANT_TRUE);
      
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::get_LogSMTP(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (config_->GetLogSMTPConversations() )
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::put_LogSMTP(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetLogSMTPConversations(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::get_LogPOP3(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (config_->GetLogPOP3Conversations() )
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::put_LogPOP3(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetLogPOP3Conversations(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::get_LogIMAP(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
   
      if (config_->GetLogIMAPConversations() )
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::put_LogIMAP(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetLogIMAPConversations(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::get_AWStatsEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetAWStatsEnabled() ? VARIANT_TRUE : VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::put_AWStatsEnabled(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetAWStatsEnabled(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::get_LogTCPIP(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (config_->GetLogTCPIP() )
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::put_LogTCPIP(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetLogTCPIP(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::get_LogApplication(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
   
      if (config_->GetLogApplication())
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP
InterfaceLogging::put_LogApplication(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetLogApplication(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceLogging::EnableLiveLogging(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      HM::Logger::Instance()->EnableLiveLogging(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_Device(eLogDevice *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      *pVal = INTLogDevice2COMLogDevice_(config_->GetLogDevice());
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::put_Device(eLogDevice newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();
   
      config_->SetLogDevice (COMLogDevice2INTLogDevice_((newVal)));
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

int
InterfaceLogging::COMLogDevice2INTLogDevice_(eLogDevice newVal)
{
   switch (newVal)
   {
   case hLogDeviceUnknown:
      return 0;
      break;
   case hLogDeviceSQL:
      return 1;
      break;
   case hLogDeviceFile:
      return 2;
      break;
   default:
      return 0;
      break;
   }
}
   
eLogDevice
InterfaceLogging::INTLogDevice2COMLogDevice_(int RelayMode)
{
   switch (RelayMode)
   {
   case 0:
      return hLogDeviceUnknown;
      break;
   case 1:
      return hLogDeviceSQL;
      break;
   case 2:
      return hLogDeviceFile;
      break;
   default:
      return hLogDeviceUnknown;
      break;
   }
}

STDMETHODIMP InterfaceLogging::get_LogFormat(eLogOutputFormat *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

   
      *pVal = IntLogFormat2ComLogFormat_(config_->GetLogFormat());
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::put_LogFormat(eLogOutputFormat newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetLogFormat(COMLogFormat2IntLogFormat_(newVal));
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

int
InterfaceLogging::COMLogFormat2IntLogFormat_(eLogOutputFormat newVal)
{
   switch (newVal)
   {
   case hLogFormatDefault:
      return 0;
      break;
   case hLogFormatCSA:
      return 1;
      break;
   default:
      return 0;
      break;
   }
}
   
eLogOutputFormat
InterfaceLogging::IntLogFormat2ComLogFormat_(int RelayMode)
{
   switch (RelayMode)
   {
   case 0:
      return hLogFormatDefault;
      break;
   case 1:
      return hLogFormatCSA;
      break;

   default:
      return hLogFormatDefault;
      break;
   }
}

STDMETHODIMP InterfaceLogging::get_LogDebug(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      if (config_->GetLogDebug() )
         *pVal = VARIANT_TRUE;
      else
         *pVal = VARIANT_FALSE;
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::put_LogDebug(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetLogDebug(newVal == VARIANT_TRUE);
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_Directory(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = ini_file_settings_->GetLogDirectory().AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_LiveLog(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = HM::Logger::Instance()->GetLiveLog().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_LiveLoggingEnabled(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = HM::Logger::Instance()->GetLiveLogEnabled();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_MaskPasswordsInLog(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      // OBSOLETE
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::put_MaskPasswordsInLog(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      // OBSOLETE
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_KeepFilesOpen(VARIANT_BOOL *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = config_->GetKeepFilesOpen() ? VARIANT_TRUE : VARIANT_FALSE;
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::put_KeepFilesOpen(VARIANT_BOOL newVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      config_->SetKeepFilesOpen(newVal == VARIANT_TRUE);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_CurrentEventLog(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = HM::Logger::Instance()->GetCurrentLogFileName(HM::Logger::Events).AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_CurrentErrorLog(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = HM::Logger::Instance()->GetCurrentLogFileName(HM::Logger::Error).AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_CurrentAwstatsLog(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = HM::Logger::Instance()->GetCurrentLogFileName(HM::Logger::AWStats).AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceLogging::get_CurrentDefaultLog(BSTR *pVal)
{
   try
   {
      if (!config_)
         return GetAccessDenied();

      *pVal = HM::Logger::Instance()->GetCurrentLogFileName(HM::Logger::Normal).AllocSysString();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


