// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

class DateTime;
////////////////////////////////////////////////////////////////////////////

// DateTime class helpers

#define AFX_OLE_DATETIME_ERROR (-1)


////////////////////////////////////////////////////////////////////////////

// DateTime class

namespace HM
{
   class DateTimeSpan;

   class DateTime
   {
       // Constructors
   public:
       static DateTime PASCAL GetCurrentTime();
    
       DateTime();
    
       DateTime(const DateTime& dateSrc);
       DateTime(const VARIANT& varSrc);
       DateTime(DATE dtSrc);
    
       DateTime(time_t& timeSrc);
       DateTime(const SYSTEMTIME& systimeSrc);
       DateTime(const FILETIME& filetimeSrc);
    
       DateTime(int nYear, int nMonth, int nDay,
                 int nHour, int nMin, int nSec);
       DateTime(WORD wDosDate, WORD wDosTime);
    
       // Attributes
   public:
       enum DateTimeStatus
       {
             valid = 0,
             invalid = 1,    // Invalid date (out of range, etc.)
             null = 2,       // Literally has no value
       };
    
       DATE dt_;
       DateTimeStatus status_;
    
       void SetStatus(DateTimeStatus status) { status_=status;}
       DateTimeStatus GetStatus() const { return status_;}
    
       int GetYear() const;
       int GetMonth() const;       // month of year (1 = Jan)
       int GetDay() const;         // day of month (0-31)
       int GetHour() const;        // hour in day (0-23)
       int GetMinute() const;      // minute in hour (0-59)
       int GetSecond() const;      // second in minute (0-59)
       int GetDayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat
       int GetDayOfYear() const;   // days since start of year, Jan 1 = 1

       VARIANT GetVariant();
    
       BOOL operator<(const DateTime& date) const;
       BOOL operator>(const DateTime& date) const;
       BOOL operator<=(const DateTime& date) const;
       BOOL operator>=(const DateTime& date) const;

       // DateTime math
       DateTime operator+(const DateTimeSpan& dateSpan) const;
       DateTime operator-(const DateTimeSpan& dateSpan) const;
       const DateTime& operator+=(const DateTimeSpan dateSpan);
       const DateTime& operator-=(const DateTimeSpan dateSpan);

       // DateTimeSpan math
       DateTimeSpan operator-(const DateTime& date) const;

       unsigned int ToInt() const;

       // Operations
   public:
       const DateTime& operator=(const DateTime& dateSrc);
       const DateTime& operator=(const VARIANT& varSrc);
       const DateTime& operator=(DATE dtSrc);
    
       const DateTime& operator=(const time_t& timeSrc);
       const DateTime& operator=(const SYSTEMTIME& systimeSrc);
       const DateTime& operator=(const FILETIME& filetimeSrc);
  
       operator DATE() const;
       int SetDateTime(int nYear, int nMonth, int nDay,
             int nHour, int nMin, int nSec);
       int SetDate(int nYear, int nMonth, int nDay);
       BOOL ParseDateTime(LPCTSTR lpszDate, DWORD dwFlags = 0,
             LCID lcid = LANG_USER_DEFAULT);
    
    
       // Implementation
   protected:
      void CheckRange();

   };


   ////////////////////////////////////////////////////////////////////////////

   // DateTimeSpan class
   class DateTimeSpan
   {
      // Constructors
   public:
      DateTimeSpan();

      DateTimeSpan(double dblSpanSrc);
      DateTimeSpan(const DateTimeSpan& dateSpanSrc);

      // Attributes
   public:
      enum DateTimeSpanStatus
      {
         valid = 0,
         invalid = 1,    // Invalid span (out of range, etc.)
         null = 2,       // Literally has no value
      };

      double span_;
      DateTimeSpanStatus status_;

      void SetStatus(DateTimeSpanStatus status) { status_=status;}
      DateTimeSpanStatus GetStatus() const { return status_;}


      double GetNumberOfDays() const;
      double GetNumberOfSeconds() const;
      double GetNumberOfHours() const;

      // Operations
   public:
      const DateTimeSpan& operator=(double dblSpanSrc);
      const DateTimeSpan& operator=(const DateTimeSpan& dateSpanSrc);

      BOOL operator==(const DateTimeSpan& dateSpan) const;
      BOOL operator!=(const DateTimeSpan& dateSpan) const;
      BOOL operator<(const DateTimeSpan& dateSpan) const;
      BOOL operator>(const DateTimeSpan& dateSpan) const;
      BOOL operator<=(const DateTimeSpan& dateSpan) const;
      BOOL operator>=(const DateTimeSpan& dateSpan) const;

      // DateTimeSpan math
      DateTimeSpan operator+(const DateTimeSpan& dateSpan) const;
      DateTimeSpan operator-(const DateTimeSpan& dateSpan) const;
      const DateTimeSpan& operator+=(const DateTimeSpan dateSpan);
      const DateTimeSpan& operator-=(const DateTimeSpan dateSpan);
      DateTimeSpan operator-() const;

      operator double() const;

      void SetDateTimeSpan(long lDays, int nHours, int nMins, int nSecs);

      // formatting
      _bstr_t Format(LPCTSTR pFormat) const;
      //CComString Format(UINT nID) const;

      // Implementation
   public:
      void CheckRange();
      friend DateTime;
   };

   class DateTimeTests
   {
   public:
      void Test();

   };


}
