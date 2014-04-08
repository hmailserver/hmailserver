// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;


namespace VMwareIntegration
{
   class Program
   {
      static void Main()
      {
        
         Application.EnableVisualStyles();
         Application.SetCompatibleTextRenderingDefault(false);

         formConfigurations conf = new formConfigurations();

         Application.Run(conf);
      }


      

   }
}
