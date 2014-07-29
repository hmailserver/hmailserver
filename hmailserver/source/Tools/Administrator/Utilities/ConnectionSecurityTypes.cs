using System;
using System.Collections.Generic;
using System.Text;

namespace hMailServer.Administrator.Utilities
{
   class ConnectionSecurityTypes
   {
      public static Dictionary<string, object> Get()
      {
         return new Dictionary<string, object>
         {
            {"None", eConnectionSecurity.eCSNone},
            {"STARTTLS (Optional)", eConnectionSecurity.eCSSTARTTLSOptional},
            {"STARTTLS (Required)", eConnectionSecurity.eCSSTARTTLSRequired},
            {"SSL/TLS", eConnectionSecurity.eCSTLS}
         };
      }
   }
}
