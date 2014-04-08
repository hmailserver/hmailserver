// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections;

namespace Builder.Common
{
   public class Builder
   {
      public delegate void MessageLogDelegate(bool timestamp, string message);

      private readonly ArrayList _buildSteps;
      private readonly ArrayList _macros;

      private int _stepStart;

      public Builder()
      {
         _buildSteps = new ArrayList();
         _macros = new ArrayList();

         _stepStart = -1;
         StepEnd = -1;
      }

      public bool RunAllSteps
      {
         get { return _stepStart == -1; }
      }

      public int StepStart
      {
         get { return _stepStart; }
         set { _stepStart = value; }
      }

      public int StepEnd { get; set; }


      public string ParameterSourcePath { get; set; }

      public string ParameterVS8Path { get; set; }

      public string ParameterInnoSetupPath { get; set; }

      public string ParameterGitPath { get; set; }

      public ArrayList Macros
      {
         get { return _macros; }
      }

      public int Count
      {
         get { return _buildSteps.Count; }
      }

      public event MessageLogDelegate MessageLog;


      public void LoadSettings(Settings settings)
      {
         ParameterSourcePath = settings.SourcePath;
         ParameterVS8Path = settings.VSPath;
         ParameterInnoSetupPath = settings.InnoSetupPath;
         ParameterGitPath = settings.GitPath;
      }

      public void Log(string sMessage, bool timestamp)
      {
         if (MessageLog != null)
            MessageLog(timestamp, sMessage);
      }

      public void Add(BuildStep bs)
      {
         _buildSteps.Add(bs);
      }

      private void AddMacro(string sName, string sValue)
      {
         var oMacro = new Macro(sName, sValue);
         _macros.Add(oMacro);
      }

      public void LoadMacros(string sourceDir, string version, string build)
      {
         Macros.Clear();
         AddMacro("%BUILD_NUMBER%", build);
         AddMacro("%PATH_SOURCE%", sourceDir);
         AddMacro("%HMAILSERVER_VERSION%", version);
         AddMacro("%PROGRAM_FILESX86%", BuildLoader.ProgramFilesx86());
         AddMacro("%PROGRAM_FILES%", BuildLoader.ProgramFiles());
      }

      public string ExpandMacros(string input)
      {
         string sExpanded = input;
         bool bFound = true;

         ArrayList macros = Macros;

         while (bFound)
         {
            bFound = false;

            foreach (Macro macro in macros)
            {
               if (sExpanded.IndexOf(macro.Name, System.StringComparison.Ordinal) >= 0)
               {
                  bFound = true;
                  sExpanded = sExpanded.Replace(macro.Name, macro.Value);
               }
            }
         }

         return sExpanded;
      }

      public BuildStep Get(int index)
      {
         return (BuildStep) _buildSteps[index];
      }
   }
}