// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Xml;

namespace Builder.Common
{
   public class BuildLoader
   {
      public Builder Load(string s)
      {
         var oBuilder = new Builder();

         var doc = new XmlDocument();
         doc.LoadXml(s);

         XmlNode buildNode = doc.ChildNodes[0];

         for (int i = 0; i < buildNode.ChildNodes.Count; i++)
         {
            XmlNode actionNode = buildNode.ChildNodes[i];

            string sType = actionNode.Attributes["type"].Value;

            if (sType == "writefile")
               AddActionWritefile(oBuilder, actionNode);
            else if (sType == "writeini")
               AddActionWriteIni(oBuilder, actionNode);
            else if (sType == "runapplication")
               AddActionRunApplication(oBuilder, actionNode);
            else if (sType == "compilevs2005")
               AddActionCompileVS2005(oBuilder, actionNode);
            else if (sType == "compilevs2008")
               AddActionCompileVS2008(oBuilder, actionNode);
            else if (sType == "copyfile")
               AddActionCopyFile(oBuilder, actionNode);
            else if (sType == "compileinnosetup")
               AddActionCompileInnoSetup(oBuilder, actionNode);
            else if (sType == "git")
               AddActionGit(oBuilder, actionNode);
            else if (sType == "cleardirectory")
               AddActionClearDirectory(oBuilder, actionNode);
            else
            {
               throw new Exception("Unknown build type " + sType);
            }
         }
         return oBuilder;
      }

      private void AddActionClearDirectory(Builder builder, XmlNode action)
      {
         string directory = action.Attributes["directory"].Value;

         builder.Add(new BuildStepClearDirectory(builder, directory));
      }

      private void AddActionWritefile(Builder builder, XmlNode action)
      {
         string sFile = action.Attributes["filename"].Value;
         string sValue = action.Attributes["value"].Value;
         sValue = sValue.Replace("\\r\\n", Environment.NewLine);

         builder.Add(new BuildStepWriteFile(builder, sFile, sValue));
      }

      private void AddActionWriteIni(Builder builder, XmlNode action)
      {
         string sFile = action.Attributes["filename"].Value;
         string sSection = action.Attributes["section"].Value;
         string sKey = action.Attributes["key"].Value;
         string sValue = action.Attributes["value"].Value;

         builder.Add(new BuildStepWriteINI(builder, sFile, sSection, sKey, sValue));
      }

      private void AddActionCompileVS2005(Builder builder, XmlNode action)
      {
         string sFile = action.Attributes["filename"].Value;
         string sConfiguration = action.Attributes["configuration"].Value;

         builder.Add(new BuildStepCompileVSNet(builder,
            sFile, sConfiguration));
      }

      private void AddActionCompileVS2008(Builder builder, XmlNode action)
      {
         string sFile = action.Attributes["filename"].Value;
         string sConfiguration = action.Attributes["configuration"].Value;

         builder.Add(new BuildStepCompileVSNet(builder,
            sFile, sConfiguration));
      }


      private void AddActionRunApplication(Builder builder, XmlNode action)
      {
         string sFile = action.Attributes["filename"].Value;
         string sParameters = action.Attributes["parameters"].Value;
         builder.Add(new BuildStepRunApplication(builder, sFile, sParameters));
      }


      private void AddActionCopyFile(Builder builder, XmlNode action)
      {
         string sFrom = action.Attributes["from"].Value;
         string sTo = action.Attributes["to"].Value;

         bool bOverwrite = false;
         XmlAttribute oAttr = action.Attributes["overwrite"];

         if (oAttr != null)
            bOverwrite = oAttr.Value == "true";

         builder.Add(new BuildStepCopyFile(builder, sFrom, sTo, bOverwrite));
      }

      private void AddActionCompileInnoSetup(Builder builder, XmlNode action)
      {
         string sFilename = action.Attributes["filename"].Value;

         builder.Add(new BuildStepInnoSetup(builder, sFilename));
      }

      private void AddActionGit(Builder builder, XmlNode action)
      {
         string sAction = action.Attributes["action"].Value;
         string sDirectory = action.Attributes["directory"].Value;

         BuildStepGit.GITAction gitaction;

         if (sAction == "revertlocalchanges")
            gitaction = BuildStepGit.GITAction.RevertLocalChanges;
         else if (sAction == "pull")
            gitaction = BuildStepGit.GITAction.Pull;
         else
            throw new Exception("Incorrect Git configuration");

         builder.Add(new BuildStepGit(builder, gitaction, sDirectory));
      }

      public static string ProgramFilesx86()
      {
         if (8 == IntPtr.Size
             || (!String.IsNullOrEmpty(Environment.GetEnvironmentVariable("PROCESSOR_ARCHITEW6432"))))
         {
            return Environment.GetEnvironmentVariable("ProgramFiles(x86)");
         }

         return Environment.GetEnvironmentVariable("ProgramFiles");
      }

      public static string ProgramFiles()
      {
         return Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
      }
   }
}