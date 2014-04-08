using System.Runtime.InteropServices;

namespace Builder.Common
{
   internal class BuildStepWriteINI : BuildStep
   {
      private readonly string _fileName;
      private readonly string _key;
      private readonly string _section;
      private readonly string _value;

      public BuildStepWriteINI(Builder builder, string fileName, string section, string key, string value)
      {
         _builder = builder;

         _fileName = fileName;
         _section = section;
         _key = key;
         _value = value;
      }


      public override string Name
      {
         get { return "Write to inifile " + _fileName; }
      }

      [DllImport("kernel32")]
      private static extern long WritePrivateProfileString(string section,
         string key, string val, string filePath);

      public override void Run()
      {
         _builder.Log("Writing to inifile " + _fileName + "...\r\n", true);

         WritePrivateProfileString(_section, _key, ExpandMacros(_value), ExpandMacros(_fileName));
      }
   }
}