// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace hMailServer.Shared
{
   public class CommandLineParser
   {
      private static Dictionary<string, string> _argumentMap = null;

      public static void Parse()
      {
         _argumentMap = new Dictionary<string, string>();

         bool firstArgument = true;
         
         string[] arguments = Environment.GetCommandLineArgs();

         foreach (string argument in arguments)
         {
            if (firstArgument)
            {
               // first argument is the executable path. 
               firstArgument = false;
               continue;
            }

            if (argument.IndexOf(":") > 0)
            {
               string name = argument.Substring(0, argument.IndexOf(":"));
               string value = argument.Substring(argument.IndexOf(":") + 1);

               _argumentMap[name] = value;
            }
            else
            {
               _argumentMap[argument] = string.Empty;
            }
         }
      }

      public static bool ContainsArgument(string argument)
      {
         if (_argumentMap.ContainsKey(argument))
            return true;
         else
            return false;
      }

      public static bool IsSilent()
      {
         if (ContainsArgument("/silent"))
            return true;

         return false;
      }

      public static Dictionary<string, string> GetArguments()
      {
         return _argumentMap;
      }

      public static string GetArgument(string name)
      {
         return _argumentMap[name];
      }

   }
}
