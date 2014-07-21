// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "../Common/BO/Route.h"

#include "COMCollection.h"

namespace HM 
{ 
   class Routes; 
}


class ATL_NO_VTABLE InterfaceRoute : 
   public COMCollectionItem<HM::Route, HM::Routes>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRoute, &CLSID_Route>,
	public IDispatchImpl<IInterfaceRoute, &IID_IInterfaceRoute, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo
{
public:
	InterfaceRoute()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEROUTE)


BEGIN_COM_MAP(InterfaceRoute)
	COM_INTERFACE_ENTRY(IInterfaceRoute)
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

   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_DomainName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_DomainName)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Description)(/*[in]*/ BSTR newVal);


   STDMETHOD(get_TargetSMTPHost)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_TargetSMTPHost)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_TargetSMTPPort)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_TargetSMTPPort)(/*[in]*/ long newVal);

   STDMETHOD(get_NumberOfTries)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_NumberOfTries)(/*[in]*/ long newVal);

   STDMETHOD(get_MinutesBetweenTry)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MinutesBetweenTry)(/*[in]*/ long newVal);

   STDMETHOD(get_AllAddresses)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllAddresses)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_RelayerRequiresAuth)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_RelayerRequiresAuth)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_RelayerAuthUsername)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_RelayerAuthUsername)(/*[in]*/ BSTR newVal);

   STDMETHOD(SetRelayerAuthPassword)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_TreatSecurityAsLocalDomain)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_TreatSecurityAsLocalDomain)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_TreatSenderAsLocalDomain)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_TreatSenderAsLocalDomain)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_TreatRecipientAsLocalDomain)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_TreatRecipientAsLocalDomain)(/*[in]*/ VARIANT_BOOL newVal);


   STDMETHOD(get_Addresses)(/*[out, retval]*/ IInterfaceRouteAddresses** pVal);

   STDMETHOD(get_UseSSL)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_UseSSL)(/*[in]*/ VARIANT_BOOL newVal);
   
   STDMETHOD(get_ConnectionSecurity)(/*[out, retval]*/ eConnectionSecurity *pVal);
   STDMETHOD(put_ConnectionSecurity)(/*[in]*/ eConnectionSecurity newVal);

   STDMETHOD(Save)();
   STDMETHOD(Delete)();



private:

};

OBJECT_ENTRY_AUTO(__uuidof(Route), InterfaceRoute)
