// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "COMCollection.h"

#include "../Common/BO/ARCTrustedSealer.h"
#include "../Common/BO/ARCTrustedSealers.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// InterfaceARCTrustedSealer

class ATL_NO_VTABLE InterfaceARCTrustedSealer :
   public COMCollectionItem<HM::ARCTrustedSealer, HM::ARCTrustedSealers>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceARCTrustedSealer, &CLSID_ARCTrustedSealer>,
	public IDispatchImpl<IInterfaceARCTrustedSealer, &IID_IInterfaceARCTrustedSealer, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceARCTrustedSealer()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEARCTRUSTEDSEALER)


BEGIN_COM_MAP(InterfaceARCTrustedSealer)
	COM_INTERFACE_ENTRY(IInterfaceARCTrustedSealer)
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

   STDMETHOD(get_Domain)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Domain)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Description)(/*[in]*/ BSTR newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(ARCTrustedSealer), InterfaceARCTrustedSealer)
