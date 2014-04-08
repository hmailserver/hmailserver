// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.IO;

namespace Builder.Common
{
   internal class BuildStepCopyFile : BuildStep
   {
      private readonly string _from;
      private readonly bool _overwrite;
      private readonly string _to;

      public BuildStepCopyFile(Builder builder, string from, string to, bool overwrite)
      {
         _builder = builder;

         _from = from;
         _to = to;
         _overwrite = overwrite;
      }

      public override string Name
      {
         get { return "Copy file " + _from; }
      }

      public override void Run()
      {
         _builder.Log("Copying file " + ExpandMacros(_from) + "...\r\n", true);
         File.Copy(ExpandMacros(_from), ExpandMacros(_to), _overwrite);
      }
   }
}