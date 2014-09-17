// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "../common/bo/SURBLServers.h"

// InterfaceSURBLServers

class ATL_NO_VTABLE InterfaceSURBLServers :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceSURBLServers, &CLSID_SURBLServers>,
	public IDispatchImpl<IInterfaceSURBLServers, &IID_IInterfaceSURBLServers, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceSURBLServers()
	{
	}
   
   bool LoadSettings();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACESURBLSERVERS)


BEGIN_COM_MAP(InterfaceSURBLServers)
	COM_INTERFACE_ENTRY(IInterfaceSURBLServers)
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

   STDMETHOD(Refresh)();

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceSURBLServer **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long lDBID, /*[out, retval]*/ IInterfaceSURBLServer** pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceSURBLServer **pVal);
   STDMETHOD(get_ItemByDNSHost)(BSTR ItemName, IInterfaceSURBLServer **pVal);

public:

   std::shared_ptr<HM::SURBLServers> surbl_servers_;

};

OBJECT_ENTRY_AUTO(__uuidof(SURBLServers), InterfaceSURBLServers)
