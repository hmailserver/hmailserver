using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace ConfigureInstallation
{
   class Ini
   {
      [DllImport("kernel32")]
      private static extern long WritePrivateProfileString(string section,
          string key, string val, string filePath);

      public static void Write(string file, string section, string key, string value)
      {
         WritePrivateProfileString(section, key, value, file);
      }

   }
}
