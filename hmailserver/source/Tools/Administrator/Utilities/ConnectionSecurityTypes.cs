using System;
using System.Collections.Generic;
using System.Text;

namespace hMailServer.Administrator.Utilities
{
   class ConnectionSecurityTypes
   {
      public static Dictionary<string, object> Get(bool includeOptionalStartTls)
      {
         var connectionSecurityTypes = new Dictionary<string, object>();

         connectionSecurityTypes.Add("None", eConnectionSecurity.eCSNone);

         if (includeOptionalStartTls)
            connectionSecurityTypes.Add("STARTTLS (Optional)", eConnectionSecurity.eCSSTARTTLSOptional);

         connectionSecurityTypes.Add("STARTTLS (Required)", eConnectionSecurity.eCSSTARTTLSRequired);
         connectionSecurityTypes.Add("SSL/TLS", eConnectionSecurity.eCSTLS);

         return connectionSecurityTypes;
      }
   }
}
