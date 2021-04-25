// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DKIMParameters.h"
#include "../../MIME/MimeCode.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   void
   DKIMParameters::Load(const AnsiString &parameters)
   {
      std::vector<AnsiString> result = StringParser::SplitString(parameters, ";");

   
      for(AnsiString parameter : result)
      {
         int equalsPos = parameter.Find("=");
         if (equalsPos < 0)
            continue;

         AnsiString paramName = parameter.Mid(0, equalsPos);
         AnsiString paramValue = parameter.Mid(equalsPos+1);

         paramName.Trim();
         paramValue.Trim();

         if (paramName == "h")
            paramValue.Replace(" ", "");
         else if (paramName == "i")
            paramValue = DKIMQPDecode_(paramValue);
         else if (paramName == "b")
            paramValue.Replace(" ", "");

         parameters_[paramName] = paramValue;
      }
   }

   bool 
   DKIMParameters::GetIsSet(const AnsiString &paramName) const
   {
      std::map<AnsiString, AnsiString>::const_iterator iter = parameters_.find(paramName);

      return iter != parameters_.end();
   }

   AnsiString 
   DKIMParameters::GetValue(const AnsiString &paramName) const
   {
      std::map<AnsiString, AnsiString>::const_iterator iter = parameters_.find(paramName);

      if (iter == parameters_.end())
         return "";

      return (*iter).second;
   }

   size_t
   DKIMParameters::GetParamCount() const
   {
      return parameters_.size();
   }

   AnsiString 
   DKIMParameters::DKIMQPDecode_(AnsiString input) const
   {
      // Whitespace in the encoded text is ignored.
      input.Replace(" ", "");
      input.Replace("\r\n", "");
      input.Replace("\t", "");

      // qp-decode...
      MimeCodeQP decoder;
      decoder.SetInput((const char*) input.GetBuffer(), input.GetLength(), false);
      
      AnsiString signature;
      decoder.GetOutput(signature);

      return signature;

   }
}