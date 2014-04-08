// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "COMCollection.h"

#include "..\Common\BO\GreyListingWhiteAddress.h"
#include "..\Common\BO\GreyListingWhiteAddresses.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// InterfaceGreyListingWhiteAddress

class ATL_NO_VTABLE InterfaceGreyListingWhiteAddress :
   public COMCollectionItem<HM::GreyListingWhiteAddress, HM::GreyListingWhiteAddresses>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceGreyListingWhiteAddress, &CLSID_GreyListingWhiteAddress>,
	public IDispatchImpl<IInterfaceGreyListingWhiteAddress, &IID_IInterfaceGreyListingWhiteAddress, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceGreyListingWhiteAddress()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEGREYLISTINGWHITEADDRESS)


BEGIN_COM_MAP(InterfaceGreyListingWhiteAddress)
	COM_INTERFACE_ENTRY(IInterfaceGreyListingWhiteAddress)
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

   STDMETHOD(get_IPAddress)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_IPAddress)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Description)(/*[in]*/ BSTR newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(GreyListingWhiteAddress), InterfaceGreyListingWhiteAddress)
