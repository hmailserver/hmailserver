// DateTime.cpp: implementation of the DateTime class.
//
// Copyright Mark A. Richman 
// April 11, 1999 
// Credits to CodeGuru
// http://www.codeguru.com/Cpp/Cpp/cpp_mfc/article.php/c765/
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VariantDateTime.h"

#include <math.h>
#include "Time.h"

////////////////////////////////////////////////////////////////////////////

// DateTime class HELPER definitions

// Verifies will fail if the needed buffer size is too large
#define MIN_DATE                (-657434L)  // about year 100
#define MAX_DATE                2958465L    // about year 9999
#define MAX_TIME_BUFFER_SIZE    128         // matches that in



// Half a second, expressed in days
#define HALF_SECOND  (1.0/172800.0)

// One-based array of days in year at month start
static int rgMonthDays[13] =
{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   

   static BOOL OleDateFromTm(WORD wYear, WORD wMonth, WORD wDay,
                                     WORD wHour, WORD wMinute, WORD wSecond, DATE& dtDest);


   static BOOL TmFromOleDate(DATE dtSrc, struct tm& tmDest);

   static double DoubleFromDate(DATE dt);

   static void TmConvertToStandardFormat(struct tm& tmSrc);
   static DATE DateFromDouble(double dbl);


   ////////////////////////////////////////////////////////////////////////////

   // DateTime class
   DateTime::DateTime()
   {
       dt_=0;
       SetStatus(invalid);
   }

   DateTime::DateTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec)
   {
      SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
   }

   DateTime::DateTime(const DateTime& dateSrc)
   {
       status_=dateSrc.status_;
       dt_=dateSrc.dt_;
   }

   DateTime::DateTime(const VARIANT& varSrc)
   {
       if (varSrc.vt == VT_BSTR)
       {
           String inputDate = varSrc.bstrVal;
           DateTime copy = Time::GetDateFromSystemDate(varSrc.bstrVal);

             dt_ = copy.dt_;
             status_ = copy.status_;
       }
       else
       {
             dt_ = varSrc.date;
             SetStatus(valid);
       }
   }

   DateTime::DateTime(DATE dtSrc)
   {
       dt_ = dtSrc;
       SetStatus(valid);
   }

   DateTime::DateTime(time_t& timeSrc)
   {
       // Convert time_t to struct tm
       struct tm ptm;
       int result = localtime_s(&ptm, &timeSrc);
    
       if (result == 0)
       {
             status_ = OleDateFromTm((WORD)(ptm.tm_year + 1900),
                  (WORD)(ptm.tm_mon + 1), (WORD)ptm.tm_mday,
                  (WORD)ptm.tm_hour, (WORD)ptm.tm_min,
                  (WORD)ptm.tm_sec, dt_) ? valid : invalid;
       }
       else
       {
             // Local time must have failed (timsSrc before 1/1/70 12am)
             dt_=0;
             SetStatus(invalid);
       }
    
    
   }

   DateTime::DateTime(const SYSTEMTIME& systimeSrc)
   {
       status_ = OleDateFromTm(systimeSrc.wYear, systimeSrc.wMonth,
             systimeSrc.wDay, systimeSrc.wHour, systimeSrc.wMinute,
             systimeSrc.wSecond, dt_) ? valid : invalid;
    
    
   }

   DateTime::DateTime(const FILETIME& filetimeSrc)
   {
       // Assume UTC FILETIME, so convert to LOCALTIME
       FILETIME filetimeLocal;
       if (!FileTimeToLocalFileTime( &filetimeSrc, &filetimeLocal))
       {
             status_ = invalid;
       }
       else
       {
             // Take advantage of SYSTEMTIME -> FILETIME conversion
             SYSTEMTIME systime;
             status_ = FileTimeToSystemTime(&filetimeLocal, &systime) ? valid : invalid;
        
             // At this point systime should always be valid, but...
             if (GetStatus() == valid)
             {
                  status_ = OleDateFromTm(systime.wYear, systime.wMonth,
                       systime.wDay, systime.wHour, systime.wMinute,
                       systime.wSecond, dt_) ? valid : invalid;
             }
       }
    
    
   }

   DateTime DateTime::operator+(const DateTimeSpan& dateSpan) const
   {
      DateTime dateResult;    // Initializes status_ to valid

      // If either operand NULL, result NULL
      if (GetStatus() == null || dateSpan.GetStatus() == null)
      {
         dateResult.SetStatus(null);
         return dateResult;
      }

      // If either operand invalid, result invalid
      if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
      {
         dateResult.SetStatus(invalid);
         return dateResult;
      }

      // Compute the actual date difference by adding underlying dates
      dateResult = DateFromDouble(DoubleFromDate(dt_) + dateSpan.span_);

      // Validate within range
      dateResult.CheckRange();

      return dateResult;
   }

   DateTime DateTime::operator-(const DateTimeSpan& dateSpan) const
   {
      DateTime dateResult;    // Initializes status_ to valid

      // If either operand NULL, result NULL
      if (GetStatus() == null || dateSpan.GetStatus() == null)
      {
         dateResult.SetStatus(null);
         return dateResult;
      }

      // If either operand invalid, result invalid
      if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
      {
         dateResult.SetStatus(invalid);
         return dateResult;
      }

      // Compute the actual date difference by subtracting underlying dates
      dateResult = DateFromDouble(DoubleFromDate(dt_) - dateSpan.span_);

      // Validate within range
      dateResult.CheckRange();

      return dateResult;
   }

   DateTimeSpan DateTime::operator-(const DateTime& date) const
   {
      DateTimeSpan spanResult;

      // If either operand NULL, result NULL
      if (GetStatus() == null || date.GetStatus() == null)
      {
         spanResult.SetStatus(DateTimeSpan::null);
         return spanResult;
      }

      // If either operand invalid, result invalid
      if (GetStatus() == invalid || date.GetStatus() == invalid)
      {
         spanResult.SetStatus(DateTimeSpan::invalid);
         return spanResult;
      }

      // Return result (span can't be invalid, so don't check range)
      return DoubleFromDate(dt_) - DoubleFromDate(date.dt_);
   }


   /////

   DateTime PASCAL DateTime::GetCurrentTime()
   {
      time_t t = ::time(0);
      return DateTime(t);
   }

   unsigned int
   DateTime::ToInt() const
   {
      struct tm tmTemp;

      if (GetStatus() == valid && TmFromOleDate(dt_, tmTemp))
      {
         tmTemp.tm_year -= 1900;
         tmTemp.tm_mon -= 1;

         time_t rawTime = _mkgmtime64(&tmTemp);

         int err = GetLastError();
         if (rawTime == -1)
            return 0;

         return (unsigned int) rawTime;
      }
      else
      {
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5160, "DateTime::ToInt()", "Tried to convert non-date to integral representation.");
         return 0;
      }

   }

   int DateTime::GetYear() const
   {
       struct tm tmTemp;
    
       if (GetStatus() == valid && TmFromOleDate(dt_, tmTemp))
             return tmTemp.tm_year;
       else
             return AFX_OLE_DATETIME_ERROR;
   }

   int DateTime::GetMonth() const
   {
       struct tm tmTemp;
    
       if (GetStatus() == valid && TmFromOleDate(dt_, tmTemp))
             return tmTemp.tm_mon;
       else
             return AFX_OLE_DATETIME_ERROR;
   }

   int DateTime::GetDay() const
   {
       struct tm tmTemp;
    
       if (GetStatus() == valid && TmFromOleDate(dt_, tmTemp))
             return tmTemp.tm_mday;
       else
             return AFX_OLE_DATETIME_ERROR;
   }

   int DateTime::GetHour() const
   {
       struct tm tmTemp;
    
       if (GetStatus() == valid && TmFromOleDate(dt_, tmTemp))
             return tmTemp.tm_hour;
    
       else
             return AFX_OLE_DATETIME_ERROR;
   }

   int DateTime::GetMinute() const
   {
       struct tm tmTemp;
    
       if (GetStatus() == valid && TmFromOleDate(dt_, tmTemp))
             return tmTemp.tm_min;
       else
             return AFX_OLE_DATETIME_ERROR;
   }

   int DateTime::GetSecond() const
   {
       struct tm tmTemp;
    
       if (GetStatus() == valid && TmFromOleDate(dt_, tmTemp))
             return tmTemp.tm_sec;
       else
             return AFX_OLE_DATETIME_ERROR;
   }

   int DateTime::GetDayOfWeek() const
   {
       struct tm tmTemp;
    
       if (GetStatus() == valid && TmFromOleDate(dt_, tmTemp))
             return tmTemp.tm_wday;
       else
             return AFX_OLE_DATETIME_ERROR;
   }

   int DateTime::GetDayOfYear() const
   {
       struct tm tmTemp;
    
       if (GetStatus() == valid && TmFromOleDate(dt_, tmTemp))
             return tmTemp.tm_yday;
       else
             return AFX_OLE_DATETIME_ERROR;
   }

   VARIANT 
   DateTime::GetVariant()
   {
      _variant_t vt;
      vt.ChangeType(VT_DATE);
      vt.date = dt_;

      return vt;
   }

   const DateTime& DateTime::operator=(const VARIANT& varSrc)
   {
       if (varSrc.vt != VT_DATE)
       {
        
             _variant_t varTemp(varSrc);
             varTemp.ChangeType(VT_DATE);
             dt_ = varTemp.date;
       }
       else
       {
             dt_ = varSrc.date;
             SetStatus(valid);
       }
    
       return *this;
   }

   const DateTime& DateTime::operator=(DATE dtSrc)
   {
       dt_ = dtSrc;
       SetStatus(valid);
    
       return *this;
   }

   const DateTime& DateTime::operator=(const time_t& timeSrc)
   {
       // Convert time_t to struct tm
       struct tm ptm;
       int result = localtime_s(&ptm, &timeSrc);
    
       if (result == 0)
       {
             status_ = OleDateFromTm((WORD)(ptm.tm_year + 1900),
                  (WORD)(ptm.tm_mon + 1), (WORD)ptm.tm_mday,
                  (WORD)ptm.tm_hour, (WORD)ptm.tm_min,
                  (WORD)ptm.tm_sec, dt_) ? valid : invalid;
       }
       else
       {
             // Local time must have failed (timsSrc before 1/1/70 12am)
       }
    
       return *this;
   }

   const DateTime& DateTime::operator=(const SYSTEMTIME& systimeSrc)
   {
       status_ = OleDateFromTm(systimeSrc.wYear, systimeSrc.wMonth,
             systimeSrc.wDay, systimeSrc.wHour, systimeSrc.wMinute,
             systimeSrc.wSecond, dt_) ? valid : invalid;
    
       return *this;
   }

   const DateTime& DateTime::operator=(const DateTime& dateSrc)
   {
      dt_ = dateSrc.dt_;
      status_ = dateSrc.status_;

      return *this;
   }

   const DateTime& DateTime::operator=(const FILETIME& filetimeSrc)
   {
       // Assume UTC FILETIME, so convert to LOCALTIME
       FILETIME filetimeLocal;
       if (!FileTimeToLocalFileTime( &filetimeSrc, &filetimeLocal))
       {
             status_ = invalid;
       }
       else
       {
             // Take advantage of SYSTEMTIME -> FILETIME conversion
             SYSTEMTIME systime;
             status_ = FileTimeToSystemTime(&filetimeLocal, &systime) ? valid : invalid;
        
             // At this point systime should always be valid, but...
             if (GetStatus() == valid)
             {
                  status_ = OleDateFromTm(systime.wYear, systime.wMonth,
                       systime.wDay, systime.wHour, systime.wMinute,
                       systime.wSecond, dt_) ? valid : invalid;
             }
       }
    
       return *this;
   }

   int DateTime::SetDateTime(int nYear, int nMonth, int nDay,
                                       int nHour, int nMin, int nSec)
   {
       return status_ = OleDateFromTm((WORD)nYear, (WORD)nMonth,
             (WORD)nDay, (WORD)nHour, (WORD)nMin, (WORD)nSec, dt_) ?
         valid : invalid;
   }

   int DateTime::SetDate(int nYear, int nMonth, int nDay)
   {
       return status_ = OleDateFromTm((WORD)nYear, (WORD)nMonth,
             (WORD)nDay, (WORD)0, (WORD)0, (WORD)0, dt_) ?
         valid : invalid;
   }

   void DateTime::CheckRange()
   {
      if (dt_ > MAX_DATE || dt_ < MIN_DATE) // about year 100 to about 9999

         SetStatus(invalid);
   }

   BOOL DateTime::ParseDateTime(LPCTSTR lpszDate, DWORD dwFlags, LCID lcid)
   {
       USES_CONVERSION;
       //CString strDate = lpszDate;
    
       SCODE sc;
       if (FAILED(sc = VarDateFromStr((LPOLESTR)T2COLE(lpszDate), lcid,
             dwFlags, &dt_)))
       {
             if (sc == DISP_E_TYPEMISMATCH)
             {
                  // Can't convert string to date, set 0 and invalidate
                  dt_ = 0;
                  SetStatus(invalid);
                  return FALSE;
             }
             else if (sc == DISP_E_OVERFLOW)
             {
                  // Can't convert string to date, set -1 and invalidate
                  dt_ = -1;
                  SetStatus(invalid);
                  return FALSE;
             }
             else
             {
                  // TRACE0("\nDateTime VarDateFromStr call failed.\n\t");
                  // if (sc == E_OUTOFMEMORY)
                  //  AfxThrowMemoryException();
                  // else
                  //  AfxThrowOleException(sc);
                  dt_=0;
                  SetStatus(invalid);
                  return FALSE;
             }
        
       }
    
       SetStatus(valid);
       return TRUE;
   }
   ////////////////////////////////////////////////////////////////////////////

   // DateTime class HELPERS - implementation

   BOOL OleDateFromTm(WORD wYear, WORD wMonth, WORD wDay,
                             WORD wHour, WORD wMinute, WORD wSecond, DATE& dtDest)
   {
       // Validate year and month (ignore day of week and milliseconds)
       if (wYear > 9999 || wMonth < 1 || wMonth > 12)
             return FALSE;
    
       //  Check for leap year and set the number of days in the month
       BOOL bLeapYear = ((wYear & 3) == 0) &&
             ((wYear % 100) != 0 || (wYear % 400) == 0);
    
       int nDaysInMonth =
             rgMonthDays[wMonth] - rgMonthDays[wMonth-1] +
             ((bLeapYear && wDay == 29 && wMonth == 2) ? 1 : 0);
    
       // Finish validating the date
       if (wDay < 1 || wDay > nDaysInMonth ||
             wHour > 23 || wMinute > 59 ||
             wSecond > 59)
       {
             return FALSE;
       }
    
       // Cache the date in days and time in fractional days
       long nDate;
       double dblTime;
    
       //It is a valid date; make Jan 1, 1AD be 1
       nDate = wYear*365L + wYear/4 - wYear/100 + wYear/400 +
        
             rgMonthDays[wMonth-1] + wDay;
    
       //  If leap year and it's before March, subtract 1:
       if (wMonth <= 2 && bLeapYear)
             --nDate;
    
       //  Offset so that 12/30/1899 is 0
       nDate -= 693959L;
    
       dblTime = (((long)wHour * 3600L) +  // hrs in seconds
             ((long)wMinute * 60L) +  // mins in seconds
             ((long)wSecond)) / 86400.;
    
       dtDest = (double) nDate + ((nDate >= 0) ? dblTime : -dblTime);
    
       return TRUE;
   }

   BOOL TmFromOleDate(DATE dtSrc, struct tm& tmDest)
   {
       // The legal range does not actually span year 0 to 9999.
       if (dtSrc > MAX_DATE || dtSrc < MIN_DATE) // about year 100 to about 9999
             return FALSE;
    
       long nDays;             // Number of days since Dec. 30, 1899
       long nDaysAbsolute;     // Number of days since 1/1/0
       long nSecsInDay;        // Time in seconds since midnight
       long nMinutesInDay;     // Minutes in day
    
       long n400Years;         // Number of 400 year increments since 1/1/0
       long n400Century;       // Century within 400 year block (0,1,2 or 3)
       long n4Years;           // Number of 4 year increments since 1/1/0
       long n4Day;             // Day within 4 year block
       //  (0 is 1/1/yr1, 1460 is 12/31/yr4)
       long n4Yr;              // Year within 4 year block (0,1,2 or 3)
       BOOL bLeap4 = TRUE;     // TRUE if 4 year block includes leap year
    
       double dblDate = dtSrc; // tempory serial date
    
       // If a valid date, then this conversion should not overflow
       nDays = (long)dblDate;
    
       // Round to the second
       dblDate += ((dtSrc > 0.0) ? HALF_SECOND : -HALF_SECOND);
    
       nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899
        
           dblDate = fabs(dblDate);
       nSecsInDay = (long)((dblDate - floor(dblDate)) * 86400.);
    
       // Calculate the day of week (sun=1, mon=2...)
       //   -1 because 1/1/0 is Sat.  +1 because we want 1-based
       tmDest.tm_wday = (int)((nDaysAbsolute - 1) % 7L) + 1;
    
       // Leap years every 4 yrs except centuries not multiples of 400.
       n400Years = (long)(nDaysAbsolute / 146097L);
    
       // Set nDaysAbsolute to day within 400-year block
       nDaysAbsolute %= 146097L;
    
       // -1 because first century has extra day
       n400Century = (long)((nDaysAbsolute - 1) / 36524L);
    
       // Non-leap century
       if (n400Century != 0)
       {
             // Set nDaysAbsolute to day within century
             nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;
        
             // +1 because 1st 4 year increment has 1460 days
             n4Years = (long)((nDaysAbsolute + 1) / 1461L);
        
             if (n4Years != 0)
                  n4Day = (long)((nDaysAbsolute + 1) % 1461L);
             else
             {
                  bLeap4 = FALSE;
                  n4Day = (long)nDaysAbsolute;
             }
       }
       else
       {
             // Leap century - not special case!
             n4Years = (long)(nDaysAbsolute / 1461L);
             n4Day = (long)(nDaysAbsolute % 1461L);
       }
    
       if (bLeap4)
       {
             // -1 because first year has 366 days
             n4Yr = (n4Day - 1) / 365;
        
             if (n4Yr != 0)
                  n4Day = (n4Day - 1) % 365;
       }
       else
       {
             n4Yr = n4Day / 365;
             n4Day %= 365;
       }
    
       // n4Day is now 0-based day of year. Save 1-based day of year, year number
           tmDest.tm_yday = (int)n4Day + 1;
       tmDest.tm_year = n400Years * 400 + n400Century * 100 + n4Years * 4 +
           n4Yr;
    
       // Handle leap year: before, on, and after Feb. 29.
       if (n4Yr == 0 && bLeap4)
       {
             // Leap Year
             if (n4Day == 59)
             {
                  /* Feb. 29 */
                  tmDest.tm_mon = 2;
                  tmDest.tm_mday = 29;
            
                  goto DoTime;
             }
        
             // Pretend it's not a leap year for month/day comp.
             if (n4Day >= 60)
                  --n4Day;
       }
    
       // Make n4DaY a 1-based day of non-leap year and compute
       //  month/day for everything but Feb. 29.
       ++n4Day;
    
       // Month number always >= n/32, so save some loop time */
       for (tmDest.tm_mon = (n4Day >> 5) + 1;
       n4Day > rgMonthDays[tmDest.tm_mon]; tmDest.tm_mon++);
    
       tmDest.tm_mday = (int)(n4Day - rgMonthDays[tmDest.tm_mon-1]);
    
   DoTime:
       if (nSecsInDay == 0)
             tmDest.tm_hour = tmDest.tm_min = tmDest.tm_sec = 0;
       else
       {
             tmDest.tm_sec = (int)nSecsInDay % 60L;
             nMinutesInDay = nSecsInDay / 60L;
             tmDest.tm_min = (int)nMinutesInDay % 60;
             tmDest.tm_hour = (int)nMinutesInDay / 60;
       }
    
       return TRUE;
   }

   BOOL DateTime::operator<(const DateTime& date) const
   {
      //ASSERT(GetStatus() == valid);
      //ASSERT(date.GetStatus() == valid);
       
      // Handle negative dates
      return DoubleFromDate(dt_) < DoubleFromDate(date.dt_);
   }

   BOOL DateTime::operator>(const DateTime& date) const
   {   //ASSERT(GetStatus() == valid);
      //ASSERT(date.GetStatus() == valid);
       
      // Handle negative dates
      return DoubleFromDate(dt_) > DoubleFromDate(date.dt_);
   }

   BOOL DateTime::operator<=(const DateTime& date) const
   {
      //ASSERT(GetStatus() == valid);
      //ASSERT(date.GetStatus() == valid);
       
      // Handle negative dates
      return DoubleFromDate(dt_) <= DoubleFromDate(date.dt_);
   }


   BOOL DateTime::operator>=(const DateTime& date) const
   {
      //ASSERT(GetStatus() == valid);
      //ASSERT(date.GetStatus() == valid);
       
      // Handle negative dates
      return DoubleFromDate(dt_) >= DoubleFromDate(date.dt_);
   }

   double DoubleFromDate(DATE dt)
   {
      // No problem if positive
      if (dt >= 0)
            return dt;
       
      // If negative, must convert since negative dates not continuous
      // (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
      double temp = ceil(dt);
      return temp - (dt - temp);
   }

   ////////////////////////////////////////////////////////////////////////////

   // DateTimeSpan class helpers

#define MAX_DAYS_IN_SPAN    3615897L

   ////////////////////////////////////////////////////////////////////////////

   // DateTimeSpan class

   DateTimeSpan::DateTimeSpan()
   {
      span_=0;
      SetStatus(invalid);
   }

   DateTimeSpan::DateTimeSpan(double dblSpanSrc)
   {
      span_ = dblSpanSrc;
      SetStatus(valid);
   }

   DateTimeSpan::DateTimeSpan(const DateTimeSpan& dateSpanSrc)
   {
      span_ = dateSpanSrc.span_;
      status_ = dateSpanSrc.status_;
   }


   double DateTimeSpan::GetNumberOfDays() const
   {
      if (status_ == valid)
         return span_;
      else
         return 0;
   }

   double 
   DateTimeSpan::GetNumberOfSeconds() const
   {
      if (status_ == valid)
         return span_ * 24 * 3600;
      else
         return 0;
   }

   double 
   DateTimeSpan::GetNumberOfHours() const
   {
      if (status_ == valid)
         return span_ * 24;
      else
         return 0;
   }


   const DateTimeSpan& DateTimeSpan::operator=(double dblSpanSrc)
   {
      span_ = dblSpanSrc;
      SetStatus(valid);
      return *this;
   }

   const DateTimeSpan& DateTimeSpan::operator=(const DateTimeSpan&
      dateSpanSrc)
   {
      span_ = dateSpanSrc.span_;
      status_ = dateSpanSrc.status_;
      return *this;
   }

   DateTimeSpan DateTimeSpan::operator+(const DateTimeSpan& dateSpan)
      const
   {
      DateTimeSpan dateSpanTemp;

      // If either operand Null, result Null
      if (GetStatus() == null || dateSpan.GetStatus() == null)
      {

         dateSpanTemp.SetStatus(null);
         return dateSpanTemp;
      }

      // If either operand Invalid, result Invalid
      if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
      {
         dateSpanTemp.SetStatus(invalid);
         return dateSpanTemp;
      }

      // Add spans and validate within legal range
      dateSpanTemp.span_ = span_ + dateSpan.span_;
      dateSpanTemp.CheckRange();

      return dateSpanTemp;
   }

   DateTimeSpan DateTimeSpan::operator-(const DateTimeSpan& dateSpan)
      const
   {
      DateTimeSpan dateSpanTemp;

      // If either operand Null, result Null
      if (GetStatus() == null || dateSpan.GetStatus() == null)
      {
         dateSpanTemp.SetStatus(null);
         return dateSpanTemp;
      }

      // If either operand Invalid, result Invalid
      if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
      {
         dateSpanTemp.SetStatus(invalid);
         return dateSpanTemp;
      }

      // Subtract spans and validate within legal range
      dateSpanTemp.span_ = span_ - dateSpan.span_;
      dateSpanTemp.CheckRange();

      return dateSpanTemp;
   }

   void DateTimeSpan::SetDateTimeSpan(
      long lDays, int nHours, int nMins, int nSecs)
   {
      // Set date span by breaking into fractional days (all input ranges valid)
      
      span_ = lDays + ((double)nHours)/24 + ((double)nMins)/(24*60) +
         ((double)nSecs)/(24*60*60);

      SetStatus(valid);
   }

   _bstr_t DateTimeSpan::Format(LPCTSTR pFormat) const
   {
      _bstr_t strSpan;
      struct tm tmTemp;

      // If null, return empty string
      if (GetStatus() == null)
         return strSpan;

      // If invalid, return DateTimeSpan resource string
      if (GetStatus() == invalid || !TmFromOleDate(span_, tmTemp))
      {
         //VERIFY(strSpan.LoadString(AFX_IDS_INVALID_DATETIMESPAN));
         return strSpan;
      }

      // Convert tm from afx internal format to standard format
      TmConvertToStandardFormat(tmTemp);

      // Fill in the buffer, disregard return value as it's not necessary
      LPTSTR lpszTemp = new TCHAR[MAX_TIME_BUFFER_SIZE];
      _tcsftime(lpszTemp, MAX_TIME_BUFFER_SIZE, pFormat, &tmTemp);
      strSpan=lpszTemp;
      delete [] lpszTemp;

      return strSpan;
   }


   void TmConvertToStandardFormat(struct tm& tmSrc)
   {
      // Convert afx internal tm to format expected by runtimes (_tcsftime, etc)
      tmSrc.tm_year -= 1900;  // year is based on 1900
      tmSrc.tm_mon -= 1;      // month of year is 0-based
      tmSrc.tm_wday -= 1;     // day of week is 0-based
      tmSrc.tm_yday -= 1;     // day of year is 0-based
   }

   DATE DateFromDouble(double dbl)
   {
      // No problem if positive
      if (dbl >= 0)
         return dbl;

      // If negative, must convert since negative dates not continuous
      // (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
      double temp = floor(dbl); // dbl is now whole part
      return temp + (temp - dbl);
   }

   void DateTimeSpan::CheckRange()
   {
      if(span_ < -MAX_DAYS_IN_SPAN || span_ > MAX_DAYS_IN_SPAN)
         SetStatus(invalid);
   }


   void
   DateTimeTests::Test()
   {
      // Handles dates starting 1970.
      DateTime time1 (1960,01,01, 00, 00, 00);
      if (time1.ToInt() != 0)
         throw;

      DateTime time2 (1970,01,01, 00, 00, 00);
      if (time2.ToInt() != 0)
         throw;

      DateTime time3 (1970,01,01, 00, 00, 05);
      if (time3.ToInt() != 5)
         throw;

      DateTime time4 (1970,01,02, 00, 00, 00);
      if (time4.ToInt() != 86400) // seconds of a day.
         throw;
      
      DateTime time5 (1971,01,01, 00, 00, 00);
      if (time5.ToInt() != 365 * 24 * 3600) // seconds of a day.
         throw;

      DateTime time6 (2009,05,14, 22, 02, 00);
      if (time6.ToInt() != 1242338520)
         throw;

      DateTime time7 (2020,12,31, 23, 59, 00);
      if (time7.ToInt() != 1609459140 )
         throw;

      DateTime timea (2030,01,01, 00, 00, 00);
      if (timea.ToInt() == 0)
         throw;


   }
}
