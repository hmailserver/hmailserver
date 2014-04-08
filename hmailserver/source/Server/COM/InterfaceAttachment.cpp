// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "COMError.h"
#include "InterfaceAttachment.h"

void 
InterfaceAttachment::Attach(shared_ptr<HM::Attachment> pAttachment) 
{ 

   m_pAttachment = pAttachment; 
}

STDMETHODIMP InterfaceAttachment::get_Filename(BSTR *pVal)
{
   try
   {
      if (!m_pAttachment)
         return GetAccessDenied();

      *pVal = m_pAttachment->GetFileName().AllocSysString();
   
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAttachment::get_Size(long *pVal)
{
   try
   {
      if (!m_pAttachment)
         return GetAccessDenied();

      *pVal = m_pAttachment->GetSize();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP InterfaceAttachment::SaveAs(BSTR sName)
{
   try
   {
      if (!m_pAttachment)
         return GetAccessDenied();

      m_pAttachment->SaveAs(sName);
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}

STDMETHODIMP 
InterfaceAttachment::Delete()
{
   try
   {
      if (!m_pAttachment)
         return GetAccessDenied();

      m_pAttachment->Delete();
      return S_OK;
   }
   catch (...)
   {
      return COMError::GenerateGenericMessage();
   }
}


