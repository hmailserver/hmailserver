// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

namespace Builder.Common
{
   public abstract class BuildStep
   {
      protected Builder _builder;

      public abstract string Name { get; }
      public abstract void Run();

      protected string ExpandMacros(string sInput)
      {
         return _builder.ExpandMacros(sInput);
      }
   }
}