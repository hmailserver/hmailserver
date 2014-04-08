// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

namespace HM
{
   class RouteAddress;
   class RouteAddresses;
}

#include "COMCollection.h"

class ATL_NO_VTABLE InterfaceRouteAddress : 
   public COMCollectionItem<HM::RouteAddress, HM::RouteAddresses>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRouteAddress, &CLSID_RouteAddress>,
	public IDispatchImpl<IInterfaceRouteAddress, &IID_IInterfaceRouteAddress, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceRouteAddress()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEROUTEADDRESS)


BEGIN_COM_MAP(InterfaceRouteAddress)
	COM_INTERFACE_ENTRY(IInterfaceRouteAddress)
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

   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_Address)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Address)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_RouteID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_RouteID)(/*[in]*/ long newVal);
   STDMETHOD(Save)();
   STDMETHOD(Delete)();

private:

};

OBJECT_ENTRY_AUTO(__uuidof(RouteAddress), InterfaceRouteAddress)
