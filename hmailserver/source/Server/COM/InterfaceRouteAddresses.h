// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "../common/bo/RouteAddresses.h"

class ATL_NO_VTABLE InterfaceRouteAddresses : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRouteAddresses, &CLSID_RouteAddresses>,
	public IDispatchImpl<IInterfaceRouteAddresses, &IID_IInterfaceRouteAddresses, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceRouteAddresses()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEROUTEADDRESSES)


BEGIN_COM_MAP(InterfaceRouteAddresses)
	COM_INTERFACE_ENTRY(IInterfaceRouteAddresses)
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

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceRouteAddress **pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long lDBID, /*[out, retval]*/ IInterfaceRouteAddress **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);

   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(DeleteByAddress)(/*[in]*/ BSTR sAddress);
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceRouteAddress **pVal);

   void Attach(std::shared_ptr<HM::RouteAddresses> pRouteAddresses);

public:

   std::shared_ptr<HM::RouteAddresses> route_addresses_;

};

OBJECT_ENTRY_AUTO(__uuidof(RouteAddresses), InterfaceRouteAddresses)
