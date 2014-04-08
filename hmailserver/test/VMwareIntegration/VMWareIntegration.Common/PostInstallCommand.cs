// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace VMwareIntegration.Common
{
   public class PostInstallCommand
   {
      private string _executable;
      private string _parameters;

      public PostInstallCommand(string executable, string parameters)
      {
         _executable = executable;
         _parameters = parameters;
      }

      public string Executable
      {
         get
         {
            return _executable;
         }
      }

      public string Parameters
      {
         get {return _parameters; }
      }
   }
}
