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
      m_iID = other.m_iID;

      m_iMessageSize = other.m_iMessageSize;
      m_sCreateTime = other.m_sCreateTime;
      m_sFilename = other.m_sFilename;
      m_sFromAddress = other.m_sFromAddress;

      m_iMessageAccountID = other.m_iMessageAccountID;
      m_iMessageFolderID = other.m_iMessageFolderID;

      m_iMessageState = other.m_iMessageState;
      m_iNoOfRetries = other.m_iNoOfRetries;
      m_iFlags = other.m_iFlags;

      _uid = other._uid;
   }


   void
   Message::Initialize(bool generateFileName)
   {
      m_iMessageAccountID = 0;
      m_iMessageState = Created;
      m_iMessageFolderID = 0;
      m_iMessageSize = 0;
      m_iNoOfRetries = 0;
      _uid = 0;

      if (generateFileName)
      {
         m_sFilename = GenerateFileName();
      }

      // Message flags.
      m_iFlags = 0;
   }

   shared_ptr<MessageRecipients>
   Message::GetRecipients()
   {
      if (!m_pRecipients)
         m_pRecipients = shared_ptr<MessageRecipients>(new MessageRecipients);

      return m_pRecipients;
   }

   bool
   Message::_GetFlag(int iFlag) const
   {
      return (m_iFlags & iFlag) > 0;
   }

   void
   Message::_SetFlag(int iFlag, bool bSet)
   {
      if (bSet)
         m_iFlags = m_iFlags | iFlag;
      else
         m_iFlags = m_iFlags & ~iFlag;
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

      String sFilename = m_sFilename;
      sFilename.Replace(IniFileSettings::Instance()->GetDataDirectory() + _T("\\"), _T(""));

      pNode->AppendAttr(_T("CreateTime"), String(m_sCreateTime));
      pNode->AppendAttr(_T("Filename"), FileUtilities::GetFileNameFromFullPath(sFilename));
      pNode->AppendAttr(_T("FromAddress"), String(m_sFromAddress));
      pNode->AppendAttr(_T("State"), StringParser::IntToString(m_iMessageState));
      pNode->AppendAttr(_T("Size"), StringParser::IntToString(m_iMessageSize));
      pNode->AppendAttr(_T("NoOfRetries"), StringParser::IntToString(m_iNoOfRetries));
      pNode->AppendAttr(_T("Flags"), StringParser::IntToString(m_iFlags));
      pNode->AppendAttr(_T("ID"), StringParser::IntToString(m_iID));
      pNode->AppendAttr(_T("UID"), StringParser::IntToString(_uid));

      return true;
   }

   bool
   Message::XMLLoad(XNode *pNode, int iOptions)
   {
      m_sCreateTime = pNode->GetAttrValue(_T("CreateTime"));
      m_sFilename = pNode->GetAttrValue(_T("Filename"));
      m_sFromAddress = pNode->GetAttrValue(_T("FromAddress"));
      m_iMessageState = (State) _ttoi(pNode->GetAttrValue(_T("State")));
      m_iMessageSize = _ttoi(pNode->GetAttrValue(_T("Size")));
      m_iNoOfRetries = _ttoi(pNode->GetAttrValue(_T("NoOfRetries")));
      m_iFlags = _ttoi(pNode->GetAttrValue(_T("Flags")));
      _uid = _ttoi(pNode->GetAttrValue(_T("UID")));

      return true;
   }

   String
   Message::GetPartialFileName() const
   {
      return m_sFilename;
   }

   void
   Message::SetPartialFileName(const String &sFileName) 
   {
      m_sFilename = sFileName;
   }

   String 
   Message::GenerateFileName()
   {
      return GUIDCreator::GetGUID() + ".eml";
   }
}
