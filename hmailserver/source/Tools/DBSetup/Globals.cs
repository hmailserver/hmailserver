// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace DBSetup
{
   class Globals
   {
      private static hMailServer.Application _application;

      public static void SetApp(hMailServer.Application application)
      {
         _application = application;
      }

      public static hMailServer.Application GetApp()
      {
         return _application;
      }

      public static hMailServer.eDBtype GetDatabaseType(string type)
      {
         switch (type)
         {
            case "MSSQL":
               return hMailServer.eDBtype.hDBTypeMSSQL;
            case "MySQL":
               return hMailServer.eDBtype.hDBTypeMySQL;
            case "PGSQL":
               return hMailServer.eDBtype.hDBTypePostgreSQL;
            default:
               throw new Exception("Unknown database type");

         }
      }

      
   }
}
