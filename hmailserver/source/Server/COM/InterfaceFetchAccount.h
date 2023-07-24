// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"
#include "../hMailServer/hMailServer.h"
#include "../Common/BO/FetchAccount.h"

#include "COMCollection.h"

namespace HM 
{ 
   class FetchAccount; 
   class FetchAccounts; 
}

class ATL_NO_VTABLE InterfaceFetchAccount : 
   public COMCollectionItem<HM::FetchAccount, HM::FetchAccounts>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceFetchAccount, &CLSID_FetchAccount>,
	public IDispatchImpl<IInterfaceFetchAccount, &IID_IInterfaceFetchAccount, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:

   InterfaceFetchAccount();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEFETCHACCOUNT)


BEGIN_COM_MAP(InterfaceFetchAccount)
	COM_INTERFACE_ENTRY(IInterfaceFetchAccount)
	COM_INTERFACE_ENTRY(IDispatch)
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

   STDMETHOD(get_ID)(LONG* pVal);
   STDMETHOD(get_Name)(BSTR* pVal);
   STDMETHOD(put_Name)(BSTR newVal);
   STDMETHOD(get_ServerAddress)(BSTR* pVal);
   STDMETHOD(put_ServerAddress)(BSTR newVal);
   STDMETHOD(get_Port)(LONG* pVal);
   STDMETHOD(put_Port)(LONG newVal);
   STDMETHOD(get_AccountID)(LONG* pVal);
   STDMETHOD(put_AccountID)(LONG newVal);
   STDMETHOD(get_ServerType)(LONG* pVal);
   STDMETHOD(put_ServerType)(LONG newVal);
   STDMETHOD(get_Username)(BSTR* pVal);
   STDMETHOD(put_Username)(BSTR newVal);
   STDMETHOD(get_Password)(BSTR* pVal);
   STDMETHOD(put_Password)(BSTR newVal);
   STDMETHOD(get_MinutesBetweenFetch)(LONG* pVal);
   STDMETHOD(put_MinutesBetweenFetch)(LONG newVal);
   STDMETHOD(get_DaysToKeepMessages)(LONG* pVal);
   STDMETHOD(put_DaysToKeepMessages)(LONG newVal);
   STDMETHOD(get_Enabled)(VARIANT_BOOL* pVal);
   STDMETHOD(put_Enabled)(VARIANT_BOOL newVal);
   STDMETHOD(Save)(void);
   STDMETHOD(DownloadNow)(void);
   STDMETHOD(get_MIMERecipientHeaders)(BSTR* pVal);
   STDMETHOD(put_MIMERecipientHeaders)(BSTR newVal);
   STDMETHOD(get_ProcessMIMERecipients)(VARIANT_BOOL* pVal);
   STDMETHOD(put_ProcessMIMERecipients)(VARIANT_BOOL newVal);
   STDMETHOD(get_ProcessMIMEDate)(VARIANT_BOOL* pVal);
   STDMETHOD(put_ProcessMIMEDate)(VARIANT_BOOL newVal);
   STDMETHOD(get_UseSSL)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_UseSSL)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(Delete)();
   STDMETHOD(get_NextDownloadTime)(BSTR* pVal);
	STDMETHOD(get_IsLocked)(VARIANT_BOOL* pVal);
   STDMETHOD(get_UseAntiSpam)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_UseAntiSpam)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_UseAntiVirus)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_UseAntiVirus)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_EnableRouteRecipients)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_EnableRouteRecipients)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_ConnectionSecurity)(/*[out, retval]*/ eConnectionSecurity *pVal);
   STDMETHOD(put_ConnectionSecurity)(/*[in]*/ eConnectionSecurity newVal);
private:

};

OBJECT_ENTRY_AUTO(__uuidof(FetchAccount), InterfaceFetchAccount)
