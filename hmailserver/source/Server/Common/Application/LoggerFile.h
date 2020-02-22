#pragma once

class HIS_LoggerFile :
   public HIS_Logger
{
public:
   HIS_LoggerFile(int iLogSetting);
   ~HIS_LoggerFile(void);

   virtual void AddToLog(int LogType, const HIS_String &sRemoteHost, const HIS_String &sLogText);
   
   void AddToLog(HIS_String sFilename, HIS_String sMessage);

   void SetLogDir(HIS_String sDir) {log_dir_ = sDir;}
private:

   bool WriteData_(HIS_String sFilename, HIS_String sData);


   HIS_String log_dir_;

};
