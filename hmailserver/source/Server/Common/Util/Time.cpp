// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Time.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Time::Time()
   {

   }

   Time::~Time()
   {

   }

   String
   Time::GetCurrentDateTime() 
   {
      SYSTEMTIME pTime;

      GetLocalTime(&pTime);
   
      String sValue;
      sValue.Format(_T("%d-%.02d-%.02d %.02d:%.02d:%.02d"), pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute, pTime.wSecond );
      return sValue;
   }

   String
   Time::GetCurrentDateTimeWithMilliseconds() 
   {
      SYSTEMTIME pTime;

      GetLocalTime(&pTime);
   
      String sValue;
      sValue.Format(_T("%d-%.02d-%.02d %.02d:%.02d:%.02d.%.03d"), pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds );
      return sValue;
   }

   String
   Time::GetCurrentDate() 
   {
      SYSTEMTIME pTime;

      GetLocalTime(&pTime);
   
      String sValue;
      sValue.Format(_T("%d-%.02d-%.02d"), pTime.wYear, pTime.wMonth, pTime.wDay);

      return sValue;
   }

   String
   Time::GetTimeStampFromDateTime(const DateTime &dt) 
   {
      if (dt.GetStatus() == DateTime::invalid)
         return "";

      String sValue;
      sValue.Format(_T("%d-%.02d-%.02d %.02d:%.02d:%.02d"), dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond() );
      return sValue;
   }

   bool
   Time::SeemsToBeValidYear(const String &sInternalDate)
   {
      String sYear = sInternalDate.Mid(0,4);
      long lYear = _ttoi(sYear);

      return (lYear > 1980);
   }

   int
   Time::GetUTCRelationMinutes()
   {
      // GetTimeZoneInformation returns UTC's
      // relation to us. But we need to produce
      // our relation to UTC. Therefore the *-1 below.

      long lMinutes = 0;

      TIME_ZONE_INFORMATION tzi;
      switch (GetTimeZoneInformation(&tzi))
      {
      case TIME_ZONE_ID_STANDARD:
      case TIME_ZONE_ID_UNKNOWN:
         lMinutes = tzi.Bias + tzi.StandardBias;
         break;
      case TIME_ZONE_ID_DAYLIGHT:
         lMinutes = tzi.Bias + tzi.DaylightBias;
      }

      lMinutes *= -1;

      return lMinutes;
   }

   String
   Time::GetUTCRelation()
   {
     long totalMinutes = GetUTCRelationMinutes();
     
     long hours = abs(totalMinutes / 60);
     long remainingMinutes = abs(totalMinutes % 60);

     String sRetVal;
     
     if (totalMinutes < 0)
        sRetVal.Format(_T("-%0.2d%0.2d"), hours, remainingMinutes);
     else
        sRetVal.Format(_T("+%0.2d%0.2d"), hours, remainingMinutes);
     
     return sRetVal;

   }

   bool 
   Time::GetTimeAdjustForTimezone(const String &sTimeZone, int &iHours, int &iMinutes)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the time modification for a timezone. The number of
   // hours from UTC.
   //
   //---------------------------------------------------------------------------()   
   {
      if (sTimeZone.IsEmpty())
      {
         // No timezone
         return false;
      }

      double dRetVal = 0;
      wchar_t s = sTimeZone.GetAt(0);
      
      if (s == '+' || s == '-' || isdigit(s))
      {
         bool bPositive = true;
         String sValue;
         if (s == '+')
            sValue = sTimeZone.Mid(1);
         else if (isdigit(s))
            sValue = sTimeZone;
         else if (s == '-')
         {
            bPositive = false;
            sValue = sTimeZone.Mid(1);
         }

         if (sValue.GetLength() != 4)
            return false;

         iHours = _ttoi(sValue.Mid(0,2));
         iMinutes = _ttoi(sValue.Mid(2,2));

         if (bPositive)
         {
            iHours *= -1;
            iMinutes *= -1;
         }
      }
      else if (sTimeZone == _T("UT") || sTimeZone == _T("GMT"))
         iHours = 0;
      else if (sTimeZone == _T("EST"))
         iHours = 5;
      else if (sTimeZone ==_T( "EDT"))
         iHours = 4;
      else if (sTimeZone == _T("CST"))
         iHours = 6;
      else if (sTimeZone == _T("CDT"))
         iHours = 5;
      else if (sTimeZone == _T("MST"))
         iHours = 7;
      else if (sTimeZone == _T("MDT"))
         iHours = 6;
      else if (sTimeZone == _T("PST"))
         iHours = 8;
      else if (sTimeZone == _T("CET"))
         iHours = -1;
      else if (sTimeZone == _T("MET"))
         iHours = -1;
      else if (sTimeZone == _T("PDT"))
         iHours = 7;
      else 
      {
         return false;
      }

      return true;
   }


   String
   Time::GetCurrentMimeDate()
   {
      
      // Returns a Date: header that has the following format:
      // Tue, 14 Sep 2004 21:53:33 +0200

      SYSTEMTIME pTime;

      GetLocalTime(&pTime);

      String sMonth = GetMonthShortName(pTime.wMonth);
      String sUTC = GetUTCRelation();
      String sDayOfWeek = GetCurrentDayName(pTime.wDayOfWeek);

      String sResult;
      sResult.Format(_T("%s, %d %s %d %.02d:%.02d:%.02d %s"),
         sDayOfWeek.c_str(), pTime.wDay, sMonth.c_str(), pTime.wYear, pTime.wHour, pTime.wMinute, pTime.wSecond, sUTC.c_str());

      return sResult;
   }
   
   String
   Time::GetIMAPDateFromMimeHeader(const String &sInDate)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Convert a date from MIME header format to IMAP date.
   // 
   // IN: Wed, 22 Dec 2004 21:08:21 +0100
   // OUT: 22-Dec-2004
   //
   //---------------------------------------------------------------------------()   
   {
      std::vector<String> vecParts = StringParser::SplitString(sInDate, " ");

      if (vecParts.size() < 5)
         return "";

      String sDay = vecParts[1];
      String sMonth = vecParts[2];
      String sYear = vecParts[3];

      String sResult;
      sResult.Format(_T("%s-%s-%s"), sDay.c_str(), sMonth.c_str(), sYear.c_str());

      return sResult;
   }

   DateTime
   Time::GetDateFromMimeHeader(const String &sInDate)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Convert a date from MIME header format to date
   // 
   // IN: Wed, 22 Dec 2004 21:08:21 +0100
   // OUT: DateTime class
   //
   //---------------------------------------------------------------------------()   
   {
      DateTime dtTmp = GetDateTimeFromMimeHeader(sInDate);

      // Copy the date (not the time)
      DateTime dt;
      dt.SetDate(dtTmp.GetYear(), dtTmp.GetMonth(), dtTmp.GetDay());
      
      return dt;
   }

   DateTime
   Time::GetDateTimeFromMimeHeader(const String &sInDate)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Convert a date from MIME header format to date. This function is very
   // flexible when it comes to accepting incorrectly formatted dates. For example
   // it will accept a date even if it's missing timestamp, zone specifier etc.
   // 
   // IN: Wed, 22 Dec 2004 21:08:21 +0100
   // OUT: DateTime class
   //
   //---------------------------------------------------------------------------()   
   {
      // The string may contain double spaces. If so, remove these now.
      String sDate = sInDate;
      sDate.Replace(_T("  "), _T(" "));
      std::vector<String> vecParts = StringParser::SplitString(sDate, " ");

      size_t iNumberOfParts = vecParts.size();

      DateTime dt;
      if (iNumberOfParts < 3)
      {
         // A date header should contain at least 4 parts
         return dt;
      }

      unsigned int iFieldIdx = 0;
      String sFirst = vecParts[iFieldIdx];
      if (sFirst.GetLength() > 3)
      {
         // Day-of-week is included in the header.
         iFieldIdx++;
      }

      String sDay, sMonth, sYear, sTime, sTimeZone;
      
      if (iFieldIdx < iNumberOfParts)
         sDay = vecParts[iFieldIdx++];
      
      if (iFieldIdx < iNumberOfParts)
         sMonth = vecParts[iFieldIdx++];

      if (iFieldIdx < iNumberOfParts)
         sYear = vecParts[iFieldIdx++];

      if (iFieldIdx < iNumberOfParts)
         sTime = vecParts[iFieldIdx++];
      
      if (iFieldIdx < iNumberOfParts)
         sTimeZone = vecParts[iFieldIdx++];

      // Convert to numerics
      int iDayIdx = _ttoi(sDay);
      int iMonthIdx = GetMonthIndex(sMonth);
      int iYearIdx = _ttoi(sYear);

      if (iYearIdx >= 50 && iYearIdx <= 99 )
         iYearIdx += 1900;
      else if (iYearIdx >= 0 && iYearIdx <= 49)
         iYearIdx += 2000;

      dt.SetDate(iYearIdx, iMonthIdx, iDayIdx);

      if (sTime.GetLength() < 5)
         return dt;

      // Parse out the time components
      std::vector<String> vecTime = StringParser::SplitString(sTime, ":");
      if (vecTime.size() < 3)
         return dt;

      String sHour = vecTime[0];
      String sMinute = vecTime[1];
      String sSecond = vecTime[2];

      int iHour = _ttoi(sHour);
      int iMinute = _ttoi(sMinute);
      int iSecond = _ttoi(sSecond);

      dt.SetDateTime(iYearIdx, iMonthIdx, iDayIdx, iHour, iMinute, iSecond);

      int iHoursDiff = 0;
      int iMinutesDiff = 0;

      if (!GetTimeAdjustForTimezone(sTimeZone, iHoursDiff, iMinutesDiff))
         return dt;

      // Append time
      DateTimeSpan dtSpan;
      dtSpan.SetDateTimeSpan(0, iHoursDiff, iMinutesDiff, 0);
      
      dt = dt + dtSpan;     

      return dt;
   }


   DateTime
   Time::GetDateFromIMAP(const String &sInDate)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Convert a date from IMAP date format to date
   // 
   // IN: 22-Dec-2004
   // OUT: DateTime class
   //
   //---------------------------------------------------------------------------()   
   {
      std::vector<String> vecParts = StringParser::SplitString(sInDate, "-");

      DateTime dt;
      if (vecParts.size() < 3)
         return dt;

      String sDay = vecParts[0];
      String sMonth = vecParts[1];
      String sYear = vecParts[2];

      int iDayIdx = _ttoi(sDay);
      int iMonthIdx = GetMonthIndex(sMonth);
      int iYearIdx = _ttoi(sYear);

      dt.SetDate(iYearIdx, iMonthIdx, iDayIdx);
      
      return dt;
   }

   DateTime
   Time::GetDateFromSystemDate(const String &sInDate)
      //---------------------------------------------------------------------------()
      // DESCRIPTION:
      // Convert a date from a hMailServer system date format to date
      // 
      // IN: 2004-12-01 12:13:25
      // OUT: DateTime class
      //
      //---------------------------------------------------------------------------()   
   {

      DateTime dt;

      String sYear = sInDate.Mid(0,4);
      String sMonth = sInDate.Mid(5,2);
      String sDay = sInDate.Mid(8,2);

      String sHour = sInDate.Mid(11,2);
      String sMinute = sInDate.Mid(14,2);
      String sSecond = sInDate.Mid(17,2);

      int iYear = _ttoi(sYear);
      int iMonth = _ttoi(sMonth);
      int iDay = _ttoi(sDay);

      int iHour = _ttoi(sHour);
      int iMinute = _ttoi(sMinute);
      int iSecond = _ttoi(sSecond);

      dt.SetDateTime(iYear, iMonth, iDay, iHour, iMinute, iSecond);

      return dt;
   }

   bool 
   Time::IsValidSystemDate(const String &dateString)
   {
      String sYear = dateString.Mid(0,4);
      String sMonth = dateString.Mid(5,2);
      String sDay = dateString.Mid(8,2);

      DateTime dt;
      dt.SetDateTime(_ttoi(sYear), _ttoi(sMonth), _ttoi(sDay), 0, 0, 0);

      if (dt.GetStatus() == dt.valid)
         return true;
      else
         return false;
   }

   String
   Time::GetIMAPInternalDate(const String &sInternalDate) 
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Convert a date from internal date format to mime format.
   // Internal date format: 2004-03-01 12:00:00
   //
   // Mime date format:
   //
   //       <daytime> ::= <SP> <date> <SP> <time>
   //
   //       <date> ::= <dd> <SP> <mon> <SP> <yy>
   // 
   //       <time> ::= <hh> ":" <mm> ":" <ss> <SP> <zone>
   //
   //       <dd> ::= the one or two decimal integer day of the month in
   //                  the range 1 to 31.
   //
   //       <mon> ::= "JAN" | "FEB" | "MAR" | "APR" | "MAY" | "JUN" |
   //                 "JUL" | "AUG" | "SEP" | "OCT" | "NOV" | "DEC"
   //
   //       <yy> ::= the two decimal integer year of the century in the
   //                 range 00 to 99.
   //
   //---------------------------------------------------------------------------()   
   {
      // First extract everything...
      String sYear = sInternalDate.Mid(0,4);
      String sMonth = sInternalDate.Mid(5,2);
      String sDay = sInternalDate.Mid(8,2);

      String sTime = sInternalDate.Mid(11);

      // If the day consists of one character,
      // it should be prepended with a space.
      if (sDay.Left(1) == _T("0"))
         sDay = " " + sDay.Mid(1);

      long lMonth = _ttoi(sMonth);
      sMonth = GetMonthShortName(lMonth);
      
      String sUTC = GetUTCRelation();

      String sResult;
      sResult.Format(_T("%s-%s-%s %s %s"), 
         sDay.c_str(), sMonth.c_str(), sYear.c_str(), sTime.c_str(), sUTC.c_str());
     
      return sResult;
   }

   String 
   Time::GetInternalDateFromIMAPInternalDate(const String &sMIMEDate)
   {
      // 30-Apr-2004 17:38:48 +0200

      std::vector<String> vecParts = StringParser::SplitString(sMIMEDate, " ");
      auto iterPart = vecParts.begin();

      String sResult = "";

      if (vecParts.size() < 2)
         return "";

      // Parse date:
      String sPart1 = vecParts[0];
      std::vector<String> vecDateParts = StringParser::SplitString(sPart1, "-");
      if (vecDateParts.size() != 3 )
         return "";
      
      long lDay = _ttoi(vecDateParts[0]);
      long lMonth = GetMonthIndex(vecDateParts[1]);
      long lYear = _ttoi(vecDateParts[2]);

      // Parse time
      String sPart2 = vecParts[1];
      std::vector<String> vecTimeParts = StringParser::SplitString(sPart2, ":");
      if (vecDateParts.size() != 3 )
         return "";
      
      long lHour = _ttoi(vecTimeParts[0]);
      long lMinute = _ttoi(vecTimeParts[1]);
      long lSecond = _ttoi(vecTimeParts[2]);

      sResult.Format(_T("%04d-%.02d-%.02d %.02d:%.02d:%.02d"), 
            lYear, lMonth, lDay, lHour, lMinute, lSecond);

      return sResult;
   }
   

   String 
   Time::GetMonthShortName(long lMonth)
   {
      switch (lMonth)
      {
      case 1:
         return "Jan";
      case 2:
         return "Feb";
      case 3:
         return "Mar";
      case 4:
         return "Apr";
      case 5:
         return "May";
      case 6:
         return "Jun";
      case 7:
         return "Jul";
      case 8:
         return "Aug";
      case 9:
         return "Sep";
      case 10:
         return "Oct";
      case 11:
         return "Nov";
      case 12:
         return "Dec";
      }

      return "NUL";
   }

   long 
   Time::GetMonthIndex(const String &sMonth)
   {
      if (sMonth.CompareNoCase(_T("JAN")) == 0)
         return 1;
      if (sMonth.CompareNoCase(_T("FEB")) == 0)
         return 2;
      if (sMonth.CompareNoCase(_T("MAR")) == 0)
         return 3;
      if (sMonth.CompareNoCase(_T("APR")) == 0)
         return 4;
      if (sMonth.CompareNoCase(_T("MAY")) == 0)
         return 5;
      if (sMonth.CompareNoCase(_T("JUN")) == 0)
         return 6;
      if (sMonth.CompareNoCase(_T("JUL")) == 0)
         return 7;
      if (sMonth.CompareNoCase(_T("AUG")) == 0)
         return 8;
      if (sMonth.CompareNoCase(_T("SEP")) == 0)
         return 9;
      if (sMonth.CompareNoCase(_T("OCT")) == 0)
         return 10;
      if (sMonth.CompareNoCase(_T("NOV")) == 0)
         return 11;
      if (sMonth.CompareNoCase(_T("DEC")) == 0)
         return 12;

      if (sMonth.CompareNoCase(_T("January")) == 0)
         return 1;
      if (sMonth.CompareNoCase(_T("February")) == 0)
         return 2;
      if (sMonth.CompareNoCase(_T("March")) == 0)
         return 3;
      if (sMonth.CompareNoCase(_T("April")) == 0)
         return 4;
      if (sMonth.CompareNoCase(_T("May")) == 0)
         return 5;
      if (sMonth.CompareNoCase(_T("June")) == 0)
         return 6;
      if (sMonth.CompareNoCase(_T("July")) == 0)
         return 7;
      if (sMonth.CompareNoCase(_T("August")) == 0)
         return 8;
      if (sMonth.CompareNoCase(_T("September")) == 0)
         return 9;
      if (sMonth.CompareNoCase(_T("October")) == 0)
         return 10;
      if (sMonth.CompareNoCase(_T("November")) == 0)
         return 11;
      if (sMonth.CompareNoCase(_T("December")) == 0)
         return 12;

      return 0;
   }

   String 
   Time::GetCurrentDayName(int Day)
   {
      switch (Day)
      {
      case 0:
         return "Sun";
      case 1:
         return "Mon";
      case 2:
         return "Tue";
      case 3:
         return "Wed";
      case 4:
         return "Thu";
      case 5:
         return "Fri";
      case 6:
         return "Sat";
      }

      return "";
   }

   void TimeTester::Test()
   {
      // Test parsing of MIME dates
      String sDate1 = "Mon, 5 Apr 2004 15:42:41 -0400 (EDT)";
      DateTime dt1 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt1.GetYear() != 2004 || dt1.GetMonth() != 4 || dt1.GetDay() != 5 ||
          dt1.GetHour() != 19 || dt1.GetMinute() != 42 || dt1.GetSecond() != 41)
          throw;

      sDate1 = "Tue, 19 Nov 2002 08:54:18 +0000";
      DateTime dt2 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt2.GetYear() != 2002 || dt2.GetMonth() != 11 || dt2.GetDay() != 19 ||
         dt2.GetHour() != 8 || dt2.GetMinute() != 54 || dt2.GetSecond() != 18)
         throw;

      sDate1 = "06 Oct 2005 08:10:26 GMT";
      DateTime dt3 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt3.GetYear() != 2005 || dt3.GetMonth() != 10 || dt3.GetDay() != 6 ||
         dt3.GetHour() != 8 || dt3.GetMinute() != 10 || dt3.GetSecond() != 26)
         throw;

      sDate1 = "Mon, 1 Sep 2003 21:58:23 +0200 (CEST)";
      DateTime dt4 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt4.GetYear() != 2003 || dt4.GetMonth() != 9 || dt4.GetDay() != 1 ||
         dt4.GetHour() != 19 || dt4.GetMinute() != 58 || dt4.GetSecond() != 23)
         throw;

      sDate1 = "Fri, 11 Feb 2005 10:47:55 +0100";
      DateTime dt5 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt5.GetYear() != 2005 || dt5.GetMonth() != 2 || dt5.GetDay() != 11 ||
         dt5.GetHour() != 9 || dt5.GetMinute() != 47 || dt5.GetSecond() != 55)
         throw;

      sDate1 = "Mon, 11 Jul 2005 19:10:32 +0200";
      DateTime dt6 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt6.GetYear() != 2005 || dt6.GetMonth() != 7 || dt6.GetDay() != 11 ||
         dt6.GetHour() != 17 || dt6.GetMinute() != 10 || dt6.GetSecond() != 32)
         throw;

      sDate1 = "11 Oct 2005 21:54:12 -0000";
      DateTime dt7 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt7.GetYear() != 2005 || dt7.GetMonth() != 10 || dt7.GetDay() != 11 ||
         dt7.GetHour() != 21 || dt7.GetMinute() != 54 || dt7.GetSecond() != 12)
         throw;

      sDate1 = "Thu, 8 Oct 1998 6:54:12 +0100";
      DateTime dt8 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt8.GetYear() != 1998 || dt8.GetMonth() != 10 || dt8.GetDay() != 8 ||
         dt8.GetHour() != 5 || dt8.GetMinute() != 54 || dt8.GetSecond() != 12)
         throw;

      sDate1 = "Mon, 23 Mar 1998 16:34:40 +0100";
      DateTime dt10 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt10.GetYear() != 1998 || dt10.GetMonth() != 3 || dt10.GetDay() != 23 ||
         dt10.GetHour() != 15 || dt10.GetMinute() != 34 || dt10.GetSecond() != 40)
         throw;

      sDate1 = "11 Oct 2005 21:54:12 -0000";
      DateTime dt11 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt11.GetYear() != 2005 || dt11.GetMonth() != 10 || dt11.GetDay() != 11 ||
         dt11.GetHour() != 21 || dt11.GetMinute() != 54 || dt11.GetSecond() != 12)
         throw;

      sDate1 = "Mon, 23 Mar 1998 16:34:40";
      DateTime dt12 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt12.GetYear() != 1998 || dt12.GetMonth() != 3 || dt12.GetDay() != 23 ||
         dt12.GetHour() != 16 || dt12.GetMinute() != 34 || dt12.GetSecond() != 40)
         throw;

      sDate1 = "23 Mar 1998 16:34:40";
      DateTime dt13 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt13.GetYear() != 1998 || dt13.GetMonth() != 3 || dt13.GetDay() != 23 ||
         dt13.GetHour() != 16 || dt13.GetMinute() != 34 || dt13.GetSecond() != 40)
         throw;

      sDate1 = "Fri, 1 Oct 99 08:01:24 +0300";
      DateTime dt14 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt14.GetYear() != 1999 || dt14.GetMonth() != 10 || dt14.GetDay() != 1 ||
         dt14.GetHour() != 5 || dt14.GetMinute() != 1 || dt14.GetSecond() != 24)
         throw;

      sDate1 = "Fri, 1 Oct 99 07:58:23 +0300";
      DateTime dt15 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt15.GetYear() != 1999 || dt15.GetMonth() != 10 || dt15.GetDay() != 1 ||
         dt15.GetHour() != 4 || dt15.GetMinute() != 58 || dt15.GetSecond() != 23)
         throw;

      sDate1 = "Mon, 25 Aug 03 18:31:06 +0200";
      DateTime dt16 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt16.GetYear() != 2003 || dt16.GetMonth() != 8 || dt16.GetDay() != 25 ||
         dt16.GetHour() != 16 || dt16.GetMinute() != 31 || dt16.GetSecond() != 06)
         throw;

      sDate1 = "Wed, 10 Jun 98 11:20:39 +0300";
      DateTime dt17 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt17.GetYear() != 1998 || dt17.GetMonth() != 6 || dt17.GetDay() != 10 ||
         dt17.GetHour() != 8 || dt17.GetMinute() != 20 || dt17.GetSecond() != 39)
         throw;

      sDate1 = "28 Oct 02 13:59:18 CET";
      DateTime dt18 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt18.GetYear() != 2002 || dt18.GetMonth() != 10 || dt18.GetDay() != 28 ||
         dt18.GetHour() != 12 || dt18.GetMinute() != 59 || dt18.GetSecond() != 18)
         throw;

      sDate1 = "Wed, 10 Jun 99 11:20:39 +0300";
      DateTime dt19 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt19.GetYear() != 1999 || dt19.GetMonth() != 6 || dt19.GetDay() != 10 ||
         dt19.GetHour() != 8 || dt19.GetMinute() != 20 || dt19.GetSecond() != 39)
         throw;

      sDate1 = "Wed, 10 Jun 00 11:20:39 +0300";
      DateTime dt20 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt20.GetYear() != 2000 || dt20.GetMonth() != 6 || dt20.GetDay() != 10 ||
         dt20.GetHour() != 8 || dt20.GetMinute() != 20 || dt20.GetSecond() != 39)
         throw;

      sDate1 = "Wed, 10 Jun 01 11:20:39 +0300";
      DateTime dt21 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt21.GetYear() != 2001 || dt21.GetMonth() != 6 || dt21.GetDay() != 10 ||
         dt21.GetHour() != 8 || dt21.GetMinute() != 20 || dt21.GetSecond() != 39)
         throw;

      sDate1 = "07 March 2005";
      DateTime dt22 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt22.GetYear() != 2005 || dt22.GetMonth() != 3 || dt22.GetDay() != 7 ||
         dt22.GetHour() != 0 || dt22.GetMinute() != 0 || dt22.GetSecond() != 0)
         throw;

      sDate1 = "07 Marcha 2005";
      DateTime dt23 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (dt23.status_ == DateTime::valid)
         throw;


      if (dt6 < dt5) // 2005-07 < 2005-02
         throw;
      if (dt5 > dt6 ) // 2005-02 > 2005-07 
         throw;
      if (dt1 < dt2 ) // 2004-04 > 2002-11
         throw;
      if (dt11 <  dt10) // 2005-10 < 1998-23
         throw;

      // Test some time stamps
      sDate1 = "11 Oct 2005 21:54:12 -0000";
      DateTime d1 = Time::GetDateFromMimeHeader(sDate1);
      if (d1.GetYear() != 2005 || d1.GetMonth() != 10 || d1.GetDay() != 11)
         throw;

      sDate1 = "Mon, 23 Mar 1998 16:34:40 +0100";
      DateTime d2 = Time::GetDateTimeFromMimeHeader(sDate1);
      if (d2.GetYear() != 1998 || d2.GetMonth() != 3 || d2.GetDay() != 23)
         throw;


   }

}
