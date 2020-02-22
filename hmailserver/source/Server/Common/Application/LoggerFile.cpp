#include "stdafx.h"
#include "loggerfile.h"
#include "../Util/Time.h"

HIS_LoggerFile::HIS_LoggerFile(int iLogSetting)
{
   log_setting_ = iLogSetting;
}

HIS_LoggerFile::~HIS_LoggerFile(void)
{
}

void
HIS_LoggerFile::AddToLog(int LogType, const HIS_String &sRemoteHost, const HIS_String &sLogText)
{

   try
   {
      if (!(log_setting_ & 1))
         return; // logging not enabled.

      if (!(log_setting_ & LogType))
         return; // not intressted in this...

      HIS_String sTime = GetCurrentTime();

      HIS_String sLogTextTemp  = sLogText;
      sLogTextTemp.Replace("\r\n", "");

      HIS_String sFilename;
      HIS_String theTime = HIS_Time::GetCurrentDate();
      HIS_String sIPTemp = sRemoteHost;
      sFilename.Format("%s\\%s_%s.log", log_dir_, "hmailserver", theTime );

      HIS_String sData;
      sData.Format("%d\t%d\t\"%s\"\t\"%s\"\t\"%s\"\r\n", _GetThreadID(), sTime, sIPTemp, sLogTextTemp);
      
      
      WriteData_(sFilename, sData);

   }
   catch (...)
   {
   }

   return;  
}

void
HIS_LoggerFile::AddToLog(HIS_String sFilename, HIS_String sMessage)
{
   if (sFilename.IsEmpty()) 
      return;

   WriteData_(sFilename, sMessage);
}

bool
HIS_LoggerFile::WriteData_(HIS_String sFilename, HIS_String sData)
{
   HANDLE handleFile;
   // --- Open the file for writing.
   handleFile = CreateFile(sFilename, 
                     GENERIC_WRITE, 
                     FILE_SHARE_READ, 
                     NULL, // LPSECURITY_ATTRIBUTES
                     OPEN_ALWAYS, // -- open or create.
                     FILE_ATTRIBUTE_NORMAL, // attributes
                     NULL // file template
                     );

   if (handleFile == INVALID_HANDLE_VALUE) 
   { 
      assert(0);
   } 
   
   int iErrNo = GetLastError() ;
   if (iErrNo == ERROR_ALREADY_EXISTS)
   {
      // bug? we asked to append file.
      SetLastError(0);
   }

   // --- Go to the end of the file.
   SetFilePointer(handleFile,0,0,FILE_END);


   DWORD pNoOfBytesWritten = 0;

   BOOL bResult = WriteFile(handleFile,sData,sData.GetLength(), &pNoOfBytesWritten, NULL);
   // --- close the file
   CloseHandle(handleFile);
  

   return true;
}
