// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPCommandRangeActionTester.h"

#include "IMAPCommandRangeAction.h"
#include "IMAPFolderView.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/Messages.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   std::shared_ptr<IMAPFolderView>
   IMAPCommandRangeActionTester::CreateView_(const std::vector<unsigned int> &uids)
   {
      // The view only reads ids and UIDs, so no database is needed.
      std::shared_ptr<Messages> messages = std::shared_ptr<Messages>(new Messages(0, 0));

      for (unsigned int uid : uids)
      {
         std::shared_ptr<Message> message = std::shared_ptr<Message>(new Message(false));
         message->SetID(1000 + uid);
         message->SetUID(uid);
         messages->AddItem(message);
      }

      std::shared_ptr<IMAPFolderView> view = std::shared_ptr<IMAPFolderView>(new IMAPFolderView(0, 0));
      view->Initialize(messages);

      return view;
   }

   void
   IMAPCommandRangeActionTester::Check_(std::shared_ptr<IMAPFolderView> view, const String &set, bool isUID, const AnsiString &expected_uids)
   {
      const char *kind = isUID ? "UID" : "Sequence";

      std::vector<std::pair<int, IMAPViewEntry>> targets;

      if (!IMAPCommandRangeAction::ResolveTargets(view, set, isUID, targets))
      {
         failures_.push_back(Formatter::FormatAsAnsi("{0} set \"{1}\" was rejected", kind, set));
         return;
      }

      AnsiString uids;

      for (const auto &target : targets)
      {
         if (!uids.IsEmpty())
            uids += " ";

         uids += Formatter::FormatAsAnsi("{0}", target.second.uid);
      }

      if (uids != expected_uids)
         failures_.push_back(Formatter::FormatAsAnsi("{0} set \"{1}\" gave \"{2}\", expected \"{3}\"", kind, set, uids, expected_uids));
   }

   void
   IMAPCommandRangeActionTester::CheckMalformed_(std::shared_ptr<IMAPFolderView> view, const String &set, bool isUID)
   {
      std::vector<std::pair<int, IMAPViewEntry>> targets;

      if (IMAPCommandRangeAction::ResolveTargets(view, set, isUID, targets))
         failures_.push_back(Formatter::FormatAsAnsi("{0} set \"{1}\" was accepted", isUID ? "UID" : "Sequence", set));
   }

   void
   IMAPCommandRangeActionTester::TestUIDSets_()
   {
      // Gaps between the UIDs, as after an expunge.
      auto view = CreateView_({ 2, 5, 9 });

      Check_(view, _T("5"), true, "5");
      Check_(view, _T("3"), true, "");
      Check_(view, _T("2,9"), true, "2 9");
      Check_(view, _T("1:3"), true, "2");
      Check_(view, _T("3:4"), true, "");
      Check_(view, _T("4294967295"), true, "");

      // * is the highest UID in the folder.
      Check_(view, _T("*"), true, "9");
      Check_(view, _T("2,*"), true, "2 9");
      Check_(view, _T("5:*"), true, "5 9");
      Check_(view, _T("*:5"), true, "5 9");

      // N:* includes the last message even when N is above every UID.
      Check_(view, _T("100:*"), true, "9");

      // A range may be given in either order.
      Check_(view, _T("9:2"), true, "2 5 9");
      Check_(view, _T("6:3"), true, "5");

      // A message named more than once is included once, where it first appears.
      Check_(view, _T("5,2,5"), true, "5 2");
      Check_(view, _T("2:5,5:9,*"), true, "2 5 9");
   }

   void
   IMAPCommandRangeActionTester::TestSequenceSets_()
   {
      auto view = CreateView_({ 2, 5, 9 });

      Check_(view, _T("2"), false, "5");
      Check_(view, _T("4"), false, "");
      Check_(view, _T("1,3"), false, "2 9");
      Check_(view, _T("1:2"), false, "2 5");
      Check_(view, _T("2:10"), false, "5 9");

      // * is the number of messages in the folder.
      Check_(view, _T("*"), false, "9");
      Check_(view, _T("2:*"), false, "5 9");
      Check_(view, _T("*:2"), false, "5 9");
      Check_(view, _T("10:*"), false, "9");

      // A range may be given in either order.
      Check_(view, _T("3:2"), false, "5 9");

      Check_(view, _T("1:2,2:3"), false, "2 5 9");
   }

   void
   IMAPCommandRangeActionTester::TestEmptyFolder_()
   {
      auto view = CreateView_({});

      Check_(view, _T("*"), true, "");
      Check_(view, _T("1:*"), true, "");
      Check_(view, _T("*"), false, "");
      Check_(view, _T("1:*"), false, "");
   }

   void
   IMAPCommandRangeActionTester::TestMalformedSets_()
   {
      auto view = CreateView_({ 2, 5, 9 });

      const std::vector<String> sets =
      {
         _T(""), _T("abc"), _T("1.2"), _T("-1"), _T(" 1"), _T("1 "),
         _T("1,,2"), _T(",1"), _T("1,"), _T(","),
         _T(":2"), _T("1:"), _T(":"), _T("1:2:3"),
         _T("*1"), _T("1*"), _T("**"),
         _T("4294967296"), _T("12345678901"),

         // Sequence numbers and UIDs are nz-number (RFC 3501 9).
         _T("0"), _T("0:1"), _T("1:0"), _T("1,0")
      };

      for (const String &set : sets)
      {
         CheckMalformed_(view, set, true);
         CheckMalformed_(view, set, false);
      }
   }

   std::vector<AnsiString>
   IMAPCommandRangeActionTester::Run()
   {
      failures_.clear();

      TestUIDSets_();
      TestSequenceSets_();
      TestEmptyFolder_();
      TestMalformedSets_();

      return failures_;
   }
}
