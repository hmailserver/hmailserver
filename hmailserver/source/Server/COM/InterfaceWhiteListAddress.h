// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "COMCollection.h"

#include "..\Common\BO\WhiteListAddress.h"
#include "..\Common\BO\WhiteListAddresses.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// InterfaceWhiteListAddress

class ATL_NO_VTABLE InterfaceWhiteListAddress :
   public COMCollectionItem<HM::WhiteListAddress, HM::WhiteListAddresses>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceWhiteListAddress, &CLSID_WhiteListAddress>,
	public IDispatchImpl<IInterfaceWhiteListAddress, &IID_IInterfaceWhiteListAddress, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceWhiteListAddress()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEWHITELISTADDRESS)


BEGIN_COM_MAP(InterfaceWhiteListAddress)
	COM_INTERFACE_ENTRY(IInterfaceWhiteListAddress)
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
   STDMETHOD(Save)();
   STDMETHOD(Delete)();

   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_LowerIPAddress)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_LowerIPAddress)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_UpperIPAddress)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_UpperIPAddress)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_EmailAddress)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_EmailAddress)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Description)(/*[in]*/ BSTR newVal);

};

OBJECT_ENTRY_AUTO(__uuidof(WhiteListAddress), InterfaceWhiteListAddress)
