// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"


#include "../Common/bo/SecurityRange.h"

namespace HM
{  
   class SecurityRange;
   class SecurityRanges;
}

#include "COMCollection.h"

class ATL_NO_VTABLE InterfaceSecurityRange : 
   public COMCollectionItem<HM::SecurityRange, HM::SecurityRanges>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceSecurityRange, &CLSID_SecurityRange>,
	public IDispatchImpl<IInterfaceSecurityRange, &IID_IInterfaceSecurityRange, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public ISupportErrorInfo,
   public HM::COMAuthenticator
{
public:
	InterfaceSecurityRange()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACESECURITYRANGE)


BEGIN_COM_MAP(InterfaceSecurityRange)
	COM_INTERFACE_ENTRY(IInterfaceSecurityRange)
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

   STDMETHOD(Save)();

   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_LowerIP)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_LowerIP)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_UpperIP)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_UpperIP)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Priority)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_Priority)(/*[in]*/ long newVal);

   STDMETHOD(get_AllowPOP3Connections)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllowPOP3Connections)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_AllowSMTPConnections)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllowSMTPConnections)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_AllowIMAPConnections)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllowIMAPConnections)(/*[in]*/ VARIANT_BOOL newVal);


   STDMETHOD(get_RequireAuthForDeliveryToLocal)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_RequireAuthForDeliveryToLocal)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_RequireAuthForDeliveryToRemote)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_RequireAuthForDeliveryToRemote)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_RequireSMTPAuthLocalToLocal)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_RequireSMTPAuthLocalToLocal)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_RequireSMTPAuthLocalToExternal)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_RequireSMTPAuthLocalToExternal)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_RequireSMTPAuthExternalToLocal)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_RequireSMTPAuthExternalToLocal)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_RequireSMTPAuthExternalToExternal)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_RequireSMTPAuthExternalToExternal)(/*[in]*/ VARIANT_BOOL newVal);

   // Relaying
   STDMETHOD(get_AllowDeliveryFromLocalToLocal)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllowDeliveryFromLocalToLocal)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_AllowDeliveryFromLocalToRemote)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllowDeliveryFromLocalToRemote)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_AllowDeliveryFromRemoteToLocal)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllowDeliveryFromRemoteToLocal)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_AllowDeliveryFromRemoteToRemote)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllowDeliveryFromRemoteToRemote)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_EnableSpamProtection)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_EnableSpamProtection)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_IsForwardingRelay)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_IsForwardingRelay)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_EnableAntiVirus)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_EnableAntiVirus)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_RequireSSLTLSForAuth)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_RequireSSLTLSForAuth)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_Expires)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Expires)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_ExpiresTime)(/*[out, retval]*/ VARIANT *pVal);
   STDMETHOD(put_ExpiresTime)(/*[in]*/ VARIANT newVal);

   STDMETHOD(Delete)();
private:

};

OBJECT_ENTRY_AUTO(__uuidof(SecurityRange), InterfaceSecurityRange)
