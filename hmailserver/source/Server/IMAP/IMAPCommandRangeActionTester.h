// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IMAPFolderView;

   // Message sets (RFC 3501 9), as ResolveTargets turns them into a session's messages.
   class IMAPCommandRangeActionTester
   {
   public:

      std::vector<AnsiString> Run();

   private:

      static std::shared_ptr<IMAPFolderView> CreateView_(const std::vector<unsigned int> &uids);

      // Reports the set unless it resolves to the expected UIDs, space separated.
      void Check_(std::shared_ptr<IMAPFolderView> view, const String &set, bool isUID, const AnsiString &expected_uids);
      void CheckMalformed_(std::shared_ptr<IMAPFolderView> view, const String &set, bool isUID);

      void TestUIDSets_();
      void TestSequenceSets_();
      void TestEmptyFolder_();
      void TestMalformedSets_();

      std::vector<AnsiString> failures_;
   };
}
