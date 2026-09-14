// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace DataDirectorySynchronizer
{
   class Globals
   {
      public enum ModeType
      {
         Import = 1,
         Delete = 2
      };

      public const string AllDomains = "All domains";

      // Messages in public folders aren't connected to an account, and the public IMAP folder
      // they belong to isn't part of the path on disk. Messages which aren't in the database
      // are therefore placed in this public IMAP folder.
      public const string DefaultPublicFolderImportPath = "Imported";

      public static ModeType Mode { get; set; }
      public static List<string> SelectedDomains { get; set; }
      public static bool SynchronizePublicFolders { get; set; }
      public static string PublicFolderImportPath { get; set; }

      private static hMailServer.Application _application;

      static Globals()
      {
         SelectedDomains = new List<string>();
         SynchronizePublicFolders = true;
         PublicFolderImportPath = DefaultPublicFolderImportPath;
      }

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
