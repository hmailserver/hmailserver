// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace hMailServer.Administrator.Utilities.Localization
{
   class InternalNames
   {
      public static string GetPortName(hMailServer.TCPIPPort port)
      {
         string ipAddress = port.Address;
         string portNumber = port.PortNumber.ToString();
         string protocolName = "";
         switch (port.Protocol)
         {
            case eSessionType.eSTIMAP:
               protocolName = "IMAP";
               break;
            case eSessionType.eSTPOP3:
               protocolName = "POP3";
               break;
            case eSessionType.eSTSMTP:
               protocolName = "SMTP";
               break;
         }


         return ipAddress + " / " + portNumber + " / " + protocolName;
      }

      public static string GetAdministrationLevelName(hMailServer.eAdminLevel level)
      {
         switch (level)
         {
            case eAdminLevel.hAdminLevelNormal:
               return Strings.Localize("User");
            case eAdminLevel.hAdminLevelDomainAdmin:
               return Strings.Localize("Domain");
            case eAdminLevel.hAdminLevelServerAdmin:
               return Strings.Localize("Server");
         }

         throw new Exception("Unknown administration level");
         
      }

      public static string GetDatabaseServerTypeName(hMailServer.eDBtype dbType)
      {
         switch (dbType)
         {
            case eDBtype.hDBTypeMSSQL:
               return "MSSQL";
            case eDBtype.hDBTypeMSSQLCE:
               return "MSSQL CE";
            case eDBtype.hDBTypeMySQL:
               return "MySQL";
            case eDBtype.hDBTypePostgreSQL:
               return "PostgreSQL";
            case eDBtype.hDBTypeUnknown:
               return "Unknown";
         }

         throw new Exception("Unknown administration level");

      }
   }
}
