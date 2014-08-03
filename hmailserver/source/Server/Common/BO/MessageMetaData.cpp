// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "MessageMetaData.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MessageMetaData::MessageMetaData() :
      account_id_(0),
      folder_id_(0),
      message_id_(0)
   {

   }

   MessageMetaData::MetaDataField 
   MessageMetaData::GetMetaDataField(const String &fieldName)
   {
      AnsiString sHeaderField = fieldName;
      sHeaderField.MakeUpper();

      MetaDataField metaDataField;

      if (sHeaderField == "SUBJECT")
         metaDataField = Subject;
      else if (sHeaderField == "CC")
         metaDataField = CC;
      else if (sHeaderField == "DATE")
         metaDataField = Date;
      else if (sHeaderField == "FROM")
         metaDataField = From;
      else if (sHeaderField == "TO")
         metaDataField = To;
      else
         metaDataField = Unknown;

      return metaDataField;
   }
}
