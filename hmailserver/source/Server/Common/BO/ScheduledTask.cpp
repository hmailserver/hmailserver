// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "ScheduledTask.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ScheduledTask::ScheduledTask(void) :
      m_iMinutesBetweenRun(0),
      m_eReoccurance(RunOnce)
   {

   }

   ScheduledTask::~ScheduledTask(void)
   {
   }

   int
   ScheduledTask::GetMinutesBetweenRun() const
   {
      return m_iMinutesBetweenRun;
   }

   void 
   ScheduledTask::SetMinutesBetweenRun(int iNewVal)
   {
      m_iMinutesBetweenRun = iNewVal;

      SetNextRunTime();
   }

   ScheduledTask::Reoccurance 
   ScheduledTask::GetReoccurance() const
   {
      return m_eReoccurance;
   }

   void 
   ScheduledTask::SetReoccurance(Reoccurance ro)
   {
      m_eReoccurance = ro;
   }

   DateTime 
   ScheduledTask::GetNextRunTime() const
   {
      return m_dtNextRunTime;

   }

   void 
   ScheduledTask::SetNextRunTime()
   {
      DateTime dtNow = DateTime::GetCurrentTime();
      
      DateTimeSpan dts;
      dts.SetDateTimeSpan(0,0, m_iMinutesBetweenRun, 0);

      m_dtNextRunTime = dtNow + dts;
   }

}