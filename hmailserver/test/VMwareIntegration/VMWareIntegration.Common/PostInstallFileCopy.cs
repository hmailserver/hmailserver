// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace VMwareIntegration.Common
{
   public class PostInstallFileCopy
   {
      private string _from;
      private string _to;

      public PostInstallFileCopy(string fromHost, string toGuest)
      {
         _from = fromHost;
         _to = toGuest;
      }

      public string From
      {
         get
         {
            return _from;
         }
      }

      public string To
      {
         get
         {
            return _to;
         }
      }
   }
}
