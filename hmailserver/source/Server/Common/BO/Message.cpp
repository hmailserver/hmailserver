// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "Message.h"


#include "../Util/File.h"
#include "../Util/GUIDCreator.h"

#include "Attachments.h"
#include "Attachment.h"

#include "MessageRecipients.h"
#include "../Mime/Mime.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Message::Message(bool generateFileName) 
   {
      Initialize(generateFileName);
   }

   Message::Message() 
   {
      Initialize(true);
   }

   Message::~Message()
   {

   }

   Message::Message(const Message& other) 
   {
      id_ = other.id_;

      message_size_ = other.message_size_;
      create_time_ = other.create_time_;
      filename_ = other.filename_;
      from_address_ = other.from_address_;

      message_account_id_ = other.message_account_id_;
      message_folder_id_ = other.message_folder_id_;

      message_state_ = other.message_state_;
      no_of_retries_ = other.no_of_retries_;
      flags_ = other.flags_;

      _uid = other._uid;
   }


   void
   Message::Initialize(bool generateFileName)
   {
      message_account_id_ = 0;
      message_state_ = Created;
      message_folder_id_ = 0;
      message_size_ = 0;
      no_of_retries_ = 0;
      _uid = 0;

      if (generateFileName)
      {
         filename_ = GenerateFileName();
      }

      // Message flags.
      flags_ = 0;
   }

   shared_ptr<MessageRecipients>
   Message::GetRecipients()
   {
      if (!recipients_)
         recipients_ = shared_ptr<MessageRecipients>(new MessageRecipients);

      return recipients_;
   }

   bool
   Message::_GetFlag(int iFlag) const
   {
      return (flags_ & iFlag) > 0;
   }

   void
   Message::_SetFlag(int iFlag, bool bSet)
   {
      if (bSet)
         flags_ = flags_ | iFlag;
      else
         flags_ = flags_ & ~iFlag;
   }

   bool 
   Message::GetFlagSeen() const
   {
      return _GetFlag(FlagSeen);
   }

   void 
   Message::SetFlagSeen(bool bNewVal)
   {
      _SetFlag(FlagSeen, bNewVal);
   }

   bool 
   Message::GetFlagDeleted() const
   {
      return _GetFlag(FlagDeleted);
   }

   void 
   Message::SetFlagDeleted(bool bNewVal)
   {
      _SetFlag(FlagDeleted, bNewVal);
   }

   bool 
   Message::GetFlagDraft() const
   {
      return _GetFlag(FlagDraft);
   }

   void 
   Message::SetFlagDraft(bool bNewVal)
   {
      _SetFlag(FlagDraft, bNewVal);
   }


   bool 
   Message::GetFlagAnswered() const
   {
      return _GetFlag(FlagAnswered);
   }

   void 
   Message::SetFlagAnswered(bool bNewVal)
   {
      _SetFlag(FlagAnswered, bNewVal);
   }

   bool 
   Message::GetFlagFlagged() const
   {
      return _GetFlag(FlagFlagged);
   }

   void 
   Message::SetFlagFlagged(bool bNewVal)
   {
      _SetFlag(FlagFlagged, bNewVal);
   }

   bool 
   Message::GetFlagRecent() const
   {
      return _GetFlag(FlagRecent);
   }

   void 
   Message::SetFlagRecent(bool bNewVal)
   {
      _SetFlag(FlagRecent, bNewVal);
   }

   bool 
   Message::GetFlagVirusScan() const
   {
      return _GetFlag(FlagVirusScan);
   }

   void 
   Message::SetFlagVirusScan(bool bNewVal)
   {
      _SetFlag(FlagVirusScan, bNewVal);
   }


   bool 
   Message::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Message"));

      String sFilename = filename_;
      sFilename.Replace(IniFileSettings::Instance()->GetDataDirectory() + _T("\\"), _T(""));

      pNode->AppendAttr(_T("CreateTime"), String(create_time_));
      pNode->AppendAttr(_T("Filename"), FileUtilities::GetFileNameFromFullPath(sFilename));
      pNode->AppendAttr(_T("FromAddress"), String(from_address_));
      pNode->AppendAttr(_T("State"), StringParser::IntToString(message_state_));
      pNode->AppendAttr(_T("Size"), StringParser::IntToString(message_size_));
      pNode->AppendAttr(_T("NoOfRetries"), StringParser::IntToString(no_of_retries_));
      pNode->AppendAttr(_T("Flags"), StringParser::IntToString(flags_));
      pNode->AppendAttr(_T("ID"), StringParser::IntToString(id_));
      pNode->AppendAttr(_T("UID"), StringParser::IntToString(_uid));

      return true;
   }

   bool
   Message::XMLLoad(XNode *pNode, int iOptions)
   {
      create_time_ = pNode->GetAttrValue(_T("CreateTime"));
      filename_ = pNode->GetAttrValue(_T("Filename"));
      from_address_ = pNode->GetAttrValue(_T("FromAddress"));
      message_state_ = (State) _ttoi(pNode->GetAttrValue(_T("State")));
      message_size_ = _ttoi(pNode->GetAttrValue(_T("Size")));
      no_of_retries_ = _ttoi(pNode->GetAttrValue(_T("NoOfRetries")));
      flags_ = _ttoi(pNode->GetAttrValue(_T("Flags")));
      _uid = _ttoi(pNode->GetAttrValue(_T("UID")));

      return true;
   }

   String
   Message::GetPartialFileName() const
   {
      return filename_;
   }

   void
   Message::SetPartialFileName(const String &sFileName) 
   {
      filename_ = sFileName;
   }

   String 
   Message::GenerateFileName()
   {
      return GUIDCreator::GetGUID() + ".eml";
   }
}
