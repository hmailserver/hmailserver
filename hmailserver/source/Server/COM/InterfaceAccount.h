// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "../common/BO/Account.h"
#include "COMCollection.h"

namespace HM 
{ 
   class Accounts; 
   class COMAuthentication;
}

class ATL_NO_VTABLE InterfaceAccount : 
   public COMCollectionItem<HM::Account, HM::Accounts>,
	public CComObjectRootEx<CComSingleThreadModel>,
   public ISupportErrorInfo,
	public CComCoClass<InterfaceAccount, &CLSID_Account>,
	public IDispatchImpl<IInterfaceAccount, &IID_IInterfaceAccount, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
   InterfaceAccount()
   {
#ifdef _DEBUG
      InterlockedIncrement(&counter);
#endif
   }

   ~InterfaceAccount()
   {
#ifdef _DEBUG
      InterlockedDecrement(&counter);
#endif
   }

   void SetAuthentication(std::shared_ptr<HM::COMAuthentication> pAuthentication);

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEACCOUNT)


BEGIN_COM_MAP(InterfaceAccount)
	COM_INTERFACE_ENTRY(IInterfaceAccount)
	COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

   HRESULT STDMETHODCALLTYPE get_ID(LONG* pVal)
   {
      *pVal = (int) object_->GetID();
      return S_OK;
   }

   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

   STDMETHOD(get_Messages)(/*[out, retval]*/ IInterfaceMessages* *pVal);
   STDMETHOD(get_FetchAccounts)(/*[out, retval]*/ IInterfaceFetchAccounts * *pVal);
   STDMETHOD(Delete)(); 

   STDMETHOD(DeleteMessages)();
   STDMETHOD(get_ADUsername)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_ADUsername)(/*[in]*/ BSTR newVal);
   STDMETHOD(Save)();
   STDMETHOD(UnlockMailbox)();
   STDMETHOD(ValidatePassword)(/*[in]*/ BSTR Password, /*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(get_Size)(/*[out, retval]*/ float *pVal);
   STDMETHOD(get_Password)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Password)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_IsAD)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_IsAD)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_DomainID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_DomainID)(LONG newVal);
   STDMETHOD(get_Address)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Address)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_ADDomain)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_ADDomain)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Active)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Active)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_MaxSize)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxSize)(/*[in]*/ long newVal);

   STDMETHOD(get_VacationMessageIsOn)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_VacationMessageIsOn)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_VacationMessage)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_VacationMessage)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_VacationSubject)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_VacationSubject)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_VacationMessageExpires)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_VacationMessageExpires)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_VacationMessageExpiresDate)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_VacationMessageExpiresDate)(/*[in]*/ BSTR newVal);



   STDMETHOD(get_AdminLevel)(/*[out, retval]*/ eAdminLevel *pVal);
   STDMETHOD(put_AdminLevel)(/*[in]*/ eAdminLevel newVal);

   STDMETHOD(get_Rules)(/*[out, retval]*/ IInterfaceRules **pVal);

   STDMETHOD(get_IMAPFolders)(/*[out, retval]*/ IInterfaceIMAPFolders **pVal);

   STDMETHOD(get_QuotaUsed)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_ForwardEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_ForwardEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_ForwardAddress)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_ForwardAddress)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_ForwardKeepOriginal)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_ForwardKeepOriginal)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_SignatureEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_SignatureEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_SignaturePlainText)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_SignaturePlainText)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_SignatureHTML)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_SignatureHTML)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_LastLogonTime)(/*[out, retval]*/ VARIANT *pVal);

   STDMETHOD(get_PersonFirstName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_PersonFirstName)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_PersonLastName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_PersonLastName)(/*[in]*/ BSTR newVal);


private:

   std::shared_ptr<HM::COMAuthentication> authentication_;

#ifdef _DEBUG
   static long counter;
#endif

};

OBJECT_ENTRY_AUTO(__uuidof(Account), InterfaceAccount)
