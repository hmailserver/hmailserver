// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

namespace hMailServer.Shared
{
   /// <summary>
   /// Process exit codes shared by the database setup tools. The installer maps
   /// these to an error message, so they must be kept in sync with the .iss script.
   /// </summary>
   public static class ExitCodes
   {
      public const int Success = 0;
      public const int UnexpectedError = 1;
      public const int AuthenticationFailed = 2;
      public const int UpgradeFailed = 3;
   }
}
