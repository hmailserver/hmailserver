// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.IO;

namespace Builder.Common
{
   internal class BuildStepWriteFile : BuildStep
   {
      private readonly string _contents;
      private readonly string _fileName;

      public BuildStepWriteFile(Builder builder, string fileName, string contents)
      {
         _builder = builder;

         _fileName = fileName;
         _contents = contents;
      }


      public override string Name
      {
         get { return "Write to file " + _fileName; }
      }

      public override void Run()
      {
         _builder.Log("Writing to file " + ExpandMacros(_fileName) + "...\r\n", true);
         File.WriteAllText(ExpandMacros(_fileName), ExpandMacros(_contents));
      }
   }
}